#!/usr/bin/env python3

# required libraries: sudo pip install RPLCD

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

# general initializations
database = [0] * 128;

# init jack
client   = jack.Client('edrumulus_front_panel')
port_in  = client.midi_inports.register('input')
port_out = client.midi_outports.register('output')

# init 16x2 LCD
lcd = CharLCD(pin_rs = 27, pin_rw = None, pin_e = 17, pins_data = [22, 23, 24, 10],
              numbering_mode = GPIO.BCM, cols = 16, rows = 2, auto_linebreaks = False)

def button_handler(pin):
  if GPIO.input(pin) == 1:
    lcd.clear()

    # TEST show pressed button value on console
    print("pin %s's value is %s" % (pin, GPIO.input(pin)))

    # TEST show dummy outputs on a button press
    lcd.cursor_pos = (0, 0)
    lcd.write_string("%d: setting" % button_pin[pin])
    lcd.cursor_pos = (1, 0)
    lcd.write_string("%s" % settings_tab[button_pin[pin]])

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

  port_in.connect('ttymidi:MIDI_in')
  port_out.connect('ttymidi:MIDI_out')
  port_out.write_midi_event(0, (185, 108, 0))
  input()

