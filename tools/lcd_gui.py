#!/usr/bin/env python3

#*******************************************************************************
# Copyright (c) 2022-2022
# Author(s): Volker Fischer, Tobias Fischer
#*******************************************************************************
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; either version 2 of the License, or (at your option) any later
# version.
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
# details.
# You should have received a copy of the GNU General Public License along with
# this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
#*******************************************************************************

# required libraries: sudo pip install RPLCD

import time
import jack
import binascii
import RPi.GPIO as GPIO
from RPLCD.gpio import CharLCD

# tables
enum_curve_types = ["LINEAR", "EXP1", "EXP2", "LOG1", "LOG2"]
enum_pad_names   = ["snare", "kick", "hi-hat", "ctrl", "crash", "tom1", "ride", "tom2", "tom3"]
enum_pad_types   = ["PD120", "PD80R", "PD8", "FD8", "VH12", "VH12CTRL", "KD7", "TP80", "CY6", "CY8", "DIABOLO12",
                    "CY5", "HD1TOM", "PD6", "KD8", "PDX8", "KD120", "PD5", "PDA120LS", "PDX100", "KT10"]
settings_tab = [ # [settings name], [MIDI note], [settings range]
  ["type",      102,  20],
  ["thresh",    103,  31],
  ["sens",      104,  31],
  ["pos thres", 105,  31],
  ["pos sens",  106,  31],
  ["rim thres", 107,  31],
  ["curve",     109,   4],
  ["spike",     110,   4],
  ["rim/pos",   111,   3],
  ["note",      112, 127],
  ["note rim",  113, 127],
  ["cross",     114,  31] ]
button_pin  = {25: 0, 11: 1, 8: 2, 7: 3, 12: 4, 13: 5}
# button 0: back;  button 1: OK;  button 2: left;  button 3: down;  button 4: up;  button 5: right
button_name = {25: 'back', 11: 'OK', 8: 'left', 7: 'down', 12: 'up', 13: 'right'}

# general initializations
database           = [0] * 128
selected_menu_item = 0
selected_pad       = 0

# init jack
client                 = jack.Client('edrumulus_front_panel')
port_in                = client.midi_inports.register('input')
port_out               = client.midi_outports.register('output')
midi_send_cmd          = -1 # invalidate per default
midi_previous_send_cmd = -1
midi_send_val          = 0

# init 16x2 LCD
lcd = CharLCD(pin_rs = 27, pin_rw = None, pin_e = 17, pins_data = [22, 23, 24, 10],
              numbering_mode = GPIO.BCM, cols = 16, rows = 2, auto_linebreaks = False)


def button_handler(pin):
  if GPIO.input(pin) == 0: # note that button is inverted
    name       = button_name[pin] # current button name
    start_time = time.time()
    # auto press functionality for up/down/left/right buttons
    if (name == 'left') or (name == 'down') or (name == 'up') or (name == 'right'):
      on_button_pressed(name) # initial button press action
      auto_press_index = 0
      while GPIO.input(pin) == 0: # wait for the button up
        time.sleep(0.01)
        if time.time() - start_time - 0.7 - auto_press_index * 0.1 > 0: # after 0.7 s, auto press every 100 ms
          on_button_pressed(name)
          auto_press_index += 1
    else:
      while GPIO.input(pin) == 0: # wait for the button up
        time.sleep(0.01)
      if time.time() - start_time < 0.7:
        on_button_pressed(name) # on button up
      else:
        # TODO: implementation of going a menu level up...
        pass


def on_button_pressed(button_name):
  # TODO implement different menu levels here...
  # if we are in trigger settings menu level
  update_trigger_settings_menu(button_name)


def update_trigger_settings_menu(button_name):
  global selected_menu_item, selected_pad, lcd, database, midi_send_val, midi_send_cmd
  database_index = settings_tab[selected_menu_item][1]

  if button_name == 'up':
    if selected_menu_item == 0:
      selected_menu_item = 11
    else:
      selected_menu_item -= 1

  if button_name == 'down':
    if selected_menu_item == 11:
      selected_menu_item = 0
    else:
      selected_menu_item += 1

  if (button_name == 'right') and (database [database_index] < settings_tab [selected_menu_item][2]):
    database [database_index] += 1
    midi_send_val = database [database_index]; # send value to Edrumulus
    midi_send_cmd = database_index;            # send value to Edrumulus

  if (button_name == 'left') and (database [database_index] > 0):
    database [database_index] -= 1
    midi_send_val = database [database_index]; # send value to Edrumulus
    midi_send_cmd = database_index;            # send value to Edrumulus

  if (button_name == 'OK') and (selected_pad < 8):
    selected_pad += 1
    midi_send_val = selected_pad;
    midi_send_cmd = 108;

  if (button_name == 'back') and (selected_pad > 0):
    selected_pad -= 1
    midi_send_val = selected_pad;
    midi_send_cmd = 108;

  update_lcd()


def update_lcd():
  global lcd, settings_tab, selected_menu_item, database, selected_pad
  lcd.clear()
  lcd.cursor_pos = (0, 0)
  lcd.write_string("%s:%s" % (enum_pad_names [selected_pad], settings_tab[selected_menu_item][0]))

  if selected_menu_item == 0:   # enum_pad_types
    lcd.cursor_pos = (1, 3)
    lcd.write_string("<%s>" % enum_pad_types [database [settings_tab [selected_menu_item] [1]]])
  elif selected_menu_item == 6: # enum_curve_types
    lcd.cursor_pos = (1, 4)
    lcd.write_string("<%s>" % enum_curve_types [database [settings_tab [selected_menu_item] [1]]])
  else:                         # use a number
    lcd.cursor_pos = (1, 6)
    lcd.write_string("<%d>" % database [settings_tab [selected_menu_item][1]])


@client.set_process_callback
def process(frames):
  global client, port_in, port_out, database, midi_send_val, midi_send_cmd, midi_previous_send_cmd
  port_out.clear_buffer()
  for offset, data in port_in.incoming_midi_events():
    if len(data) == 3:
      if int.from_bytes(data[0], "big") == 0x80:
        key   = int.from_bytes(data[1], "big")
        value = int.from_bytes(data[2], "big")
        # do not update command which was just changed to avoid the value jumps back to old value
        if midi_previous_send_cmd != key:
          database[key] = value

  if midi_send_cmd >= 0:
    port_out.write_midi_event(0, (185, midi_send_cmd, midi_send_val))
    midi_previous_send_cmd = midi_send_cmd # store previous value
    midi_send_cmd          = -1 # invalidate current command to prepare for next command


with client:
  print('press Return to quit')

  # init buttons
  GPIO.setmode(GPIO.BCM)
  GPIO.setup(25, GPIO.IN, pull_up_down = GPIO.PUD_DOWN)
  GPIO.add_event_detect(25, GPIO.BOTH, callback = button_handler, bouncetime = 20)
  GPIO.setup(11, GPIO.IN, pull_up_down = GPIO.PUD_DOWN)
  GPIO.add_event_detect(11, GPIO.BOTH, callback = button_handler, bouncetime = 20)
  GPIO.setup(8, GPIO.IN, pull_up_down = GPIO.PUD_DOWN)
  GPIO.add_event_detect(8, GPIO.BOTH, callback = button_handler, bouncetime = 20)
  GPIO.setup(7, GPIO.IN, pull_up_down = GPIO.PUD_DOWN)
  GPIO.add_event_detect(7, GPIO.BOTH, callback = button_handler, bouncetime = 20)
  GPIO.setup(12, GPIO.IN, pull_up_down = GPIO.PUD_DOWN)
  GPIO.add_event_detect(12, GPIO.BOTH, callback = button_handler, bouncetime = 20)
  GPIO.setup(13, GPIO.IN, pull_up_down = GPIO.PUD_DOWN)
  GPIO.add_event_detect(13, GPIO.BOTH, callback = button_handler, bouncetime = 20)

  # startup message on LCD
  lcd.clear()
  lcd.cursor_pos = (0, 3)
  lcd.write_string('Edrumulus')
  lcd.cursor_pos = (1, 2)
  lcd.write_string('Prototype 5')

  port_in.connect('ttymidi:MIDI_in')
  port_out.connect('ttymidi:MIDI_out')
  port_out.write_midi_event(0, (185, 108, selected_pad))
  time.sleep(1)
  update_lcd()
  input()
  lcd.close()

