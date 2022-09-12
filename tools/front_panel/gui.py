#!/usr/bin/env python3

# required libraries: sudo pip install RPLCD

import jack
import binascii
import RPi.GPIO as GPIO
from RPLCD.gpio import CharLCD

# general initializations
database = [0] * 128;

# init jack
client   = jack.Client('edrumulus_front_panel')
port_in  = client.midi_inports.register('input')
port_out = client.midi_outports.register('output')

# init 16x2 LCD
lcd = CharLCD(pin_rs = 27, pin_rw = None, pin_e = 17, pins_data = [22, 23, 24, 10],
              numbering_mode = GPIO.BOARD, cols = 16, rows = 2)

# init buttons
GPIO.setmode(GPIO.BCM)
GPIO.setup(25, GPIO.IN, pull_up_down = GPIO.PUD_DOWN)
GPIO.add_event_detect(25, GPIO.RISING, callback = button_handler, bouncetime = 10)
GPIO.setup(11, GPIO.IN, pull_up_down = GPIO.PUD_DOWN)
GPIO.add_event_detect(11, GPIO.RISING, callback = button_handler, bouncetime = 10)
GPIO.setup(8, GPIO.IN, pull_up_down = GPIO.PUD_DOWN)
GPIO.add_event_detect(8, GPIO.RISING, callback = button_handler, bouncetime = 10)
GPIO.setup(7, GPIO.IN, pull_up_down = GPIO.PUD_DOWN)
GPIO.add_event_detect(7, GPIO.RISING, callback = button_handler, bouncetime = 10)
GPIO.setup(12, GPIO.IN, pull_up_down = GPIO.PUD_DOWN)
GPIO.add_event_detect(12, GPIO.RISING, callback = button_handler, bouncetime = 10)
GPIO.setup(13, GPIO.IN, pull_up_down = GPIO.PUD_DOWN)
GPIO.add_event_detect(13, GPIO.RISING, callback = button_handler, bouncetime = 10)

def button_handler(pin):
  print("pin %s's value is %s" % (pin, GPIO.input(pin)))

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

  # testing LCD
  lcd.clear()
  lcd.cursor_pos = (1, 4)
  lcd.write_string('Edrumulus')
  lcd.cursor_pos = (2, 0)
  lcd.write_string('press Return to quit')

  port_in.connect('ttymidi:MIDI_in')
  port_out.connect('ttymidi:MIDI_out')
  port_out.write_midi_event(0, (185, 108, 0))
  input()

