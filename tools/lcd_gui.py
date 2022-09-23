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
enum_pad_types = ["PD120", "PD80R", "PD8", "FD8", "VH12", "VH12CTRL", "KD7", "TP80", "CY6", "CY8",
  "DIABOLO12", "CY5", "HD1TOM", "PD6", "KD8", "PDX8", "KD120", "PD5", "PDA120LS", "PDX100", "KT10"]
enum_curve_types = ["LINEAR", "EXP1", "EXP2", "LOG1", "LOG2"]
enum_pad_names = ["snare", "kick", "hi-hat", "ctrl", "crash", "tom1", "ride", "tom2", "tom3"]
settings_tab = [ # [settings name], [MIDI note], [settings range], [cursor pos],
  ["type",      102,  20,   6],
  ["thresh",    103,  31,   5],
  ["sens",      104,  31,   6],
  ["pos thres", 105,  31,   4],
  ["pos sens",  106,  31,   4],
  ["rim thres", 107,  31,   4],
  ["curve",     109,   4,   5],
  ["spike",     110,   4,   5],
  ["rim/pos",   111,   3,   4],
  ["note",      112, 127,   6],
  ["note rim",  113, 127,   4],
  ["cross",     114,  31,   6] ]
button_pin = {25: 0, 11: 1, 8: 2, 7: 3, 12: 4, 13: 5}
button_name = {25: 'back', 11: 'OK', 8: 'left', 7: 'down', 12: 'up', 13: 'right'}

# button 0: back;  button 1: OK;  button 2: left;  button 3: down;  button 4: up;  button 5: right;

# general initializations
database = [0] * 128;
selected_menu_item = 0

# init jack
client   = jack.Client('edrumulus_front_panel')
port_in  = client.midi_inports.register('input')
port_out = client.midi_outports.register('output')

# init 16x2 LCD
lcd = CharLCD(pin_rs = 27, pin_rw = None, pin_e = 17, pins_data = [22, 23, 24, 10],
              numbering_mode = GPIO.BCM, cols = 16, rows = 2, auto_linebreaks = False)

def button_handler(pin):
  global selected_menu_item, lcd, database
  if GPIO.input(pin) == 1:
    lcd.clear()
    if button_name[pin] == 'up':
      if selected_menu_item == 0:
        selected_menu_item = 11
        lcd.cursor_pos = (0, settings_tab[selected_menu_item][3])
        lcd.write_string("%s" % settings_tab[selected_menu_item][0])
      else:
        selected_menu_item = selected_menu_item - 1
        lcd.cursor_pos = (0, settings_tab[selected_menu_item][3])
        lcd.write_string("%s" % settings_tab[selected_menu_item][0])

    if button_name[pin] == 'down':
      if selected_menu_item == 11:
        selected_menu_item = 0
        lcd.cursor_pos = (0, settings_tab[selected_menu_item][3])
        lcd.write_string("%s" % settings_tab[selected_menu_item][0])
      else:
        selected_menu_item = selected_menu_item + 1
        lcd.cursor_pos = (0, settings_tab[selected_menu_item][3])
        lcd.write_string("%s" % settings_tab[selected_menu_item][0])
    
    if button_name[pin] == 'right':
      database [selected_menu_item] = database [selected_menu_item] + 1
      if database [selected_menu_item] >= settings_tab [selected_menu_item][2]:
        database [selected_menu_item] = database [selected_menu_item] - 1
    
    if button_name[pin] == 'left':
      database [selected_menu_item] = database [selected_menu_item] - 1
      if database [selected_menu_item] <= 0:
        database [selected_menu_item] = database [selected_menu_item] + 1
            
    lcd.cursor_pos = (1, 6)
    lcd.write_string("<%d>" % database [settings_tab[selected_menu_item][1]])
  
  
    #lcd.write_string("%s" % settings_tab[selected_menu_item][0])

    # TEST2
    #lcd.write_string("%s" % button_name[pin])

@client.set_process_callback
def process(frames):
  port_out.clear_buffer()
  for offset, data in port_in.incoming_midi_events():
    if len(data) == 3:
      if int.from_bytes(data[0], "big") == 0x80:
        key   = int.from_bytes(data[1], "big")
        value = int.from_bytes(data[2], "big")
        database[key] = value

      # for debugging
      #print('{}, {}'.format(key, value))
      print('{}: 0x{}'.format(client.last_frame_time + offset,
                              binascii.hexlify(data).decode()))
      print(database)

with client:
  print('press Return to quit')

  # init buttons
  GPIO.setmode(GPIO.BCM)
  GPIO.setup(25, GPIO.IN, pull_up_down = GPIO.PUD_DOWN)
  GPIO.add_event_detect(25, GPIO.RISING, callback = button_handler, bouncetime = 20)
  GPIO.setup(11, GPIO.IN, pull_up_down = GPIO.PUD_DOWN)
  GPIO.add_event_detect(11, GPIO.RISING, callback = button_handler, bouncetime = 20)
  GPIO.setup(8, GPIO.IN, pull_up_down = GPIO.PUD_DOWN)
  GPIO.add_event_detect(8, GPIO.RISING, callback = button_handler, bouncetime = 20)
  GPIO.setup(7, GPIO.IN, pull_up_down = GPIO.PUD_DOWN)
  GPIO.add_event_detect(7, GPIO.RISING, callback = button_handler, bouncetime = 20)
  GPIO.setup(12, GPIO.IN, pull_up_down = GPIO.PUD_DOWN)
  GPIO.add_event_detect(12, GPIO.RISING, callback = button_handler, bouncetime = 20)
  GPIO.setup(13, GPIO.IN, pull_up_down = GPIO.PUD_DOWN)
  GPIO.add_event_detect(13, GPIO.RISING, callback = button_handler, bouncetime = 20)

  # testing LCD
  lcd.clear()
  lcd.cursor_pos = (0, 3)
  lcd.write_string('Edrumulus')
  lcd.cursor_pos = (1, 2)
  lcd.write_string('Prototype 5')
  time.sleep(1)
  lcd.clear()

  port_in.connect('ttymidi:MIDI_in')
  port_out.connect('ttymidi:MIDI_out')
  port_out.write_midi_event(0, (185, 108, 0))
  input()

