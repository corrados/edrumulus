#!/usr/bin/env python3

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
  global selected_menu_item
  if GPIO.input(pin) == 1:
    lcd.clear()
    if button_name[pin] == 'up':
      if selected_menu_item == 0:
        selected_menu_item = 11
        lcd.curser_pos = (0, 6)
        lcd.write_string("%s" % settings_tab[11][0])
      elif selected_menu_item == 1:
        selected_menu_item = 0
        lcd.curser_pos = (0, 6)
        lcd.write_string("%s" % settings_tab[0][0])
      elif selected_menu_item == 2:
        selected_menu_item = 1
        lcd.curser_pos = (0, 5)
        lcd.write_string("%s" % settings_tab[1][0])
      elif selected_menu_item == 3:
        selected_menu_item = 2
        lcd.curser_pos = (0, 6)
        lcd.write_string("%s" % settings_tab[2][0])
      elif selected_menu_item == 4:
        selected_menu_item = 3
        lcd.curser_pos = (0, 4)
        lcd.write_string("%s" % settings_tab[3][0])
      elif selected_menu_item == 5:
        selected_menu_item = 4
        lcd.curser_pos = (0, 4)
        lcd.write_string("%s" % settings_tab[4][0])
      elif selected_menu_item == 6:
        selected_menu_item = 5
        lcd.curser_pos = (0, 4)
        lcd.write_string("%s" % settings_tab[5][0])
      elif selected_menu_item == 7:
        selected_menu_item = 6
        lcd.curser_pos = (0, 6)
        lcd.write_string("%s" % settings_tab[6][0])
      elif selected_menu_item == 8:
        selected_menu_item = 7
        lcd.curser_pos = (0, 6)
        lcd.write_string("%s" % settings_tab[7][0])
      elif selected_menu_item == 9:
        selected_menu_item = 8
        lcd.curser_pos = (0, 5)
        lcd.write_string("%s" % settings_tab[8][0]) 
      elif selected_menu_item == 10:
        selected_menu_item = 9
        lcd.curser_pos = (0, 6)
        lcd.write_string("%s" % settings_tab[9][0])       
      elif selected_menu_item == 11:
        selected_menu_item = 10
        lcd.curser_pos = (0, 4)
        lcd.write_string("%s" % settings_tab[10][0])
              
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

