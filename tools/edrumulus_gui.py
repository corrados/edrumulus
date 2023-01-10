#!/usr/bin/env python3

#*******************************************************************************
# Copyright (c) 2022-2023
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

# Edrumulus simple terminal GUI

import sys
import jack
import time
no_gui      = len(sys.argv) > 1 and sys.argv[1] == "no_gui"    # no GUI but blocking (just settings management)
non_block   = len(sys.argv) > 1 and sys.argv[1] == "non_block" # no GUI and non-blocking (just settings management)
use_lcd     = len(sys.argv) > 1 and sys.argv[1] == "lcd"       # LCD GUI mode on Raspberry Pi
use_ncurses = not no_gui and not non_block and not use_lcd     # normal console GUI mode (default)
if use_lcd:
  import RPi.GPIO as GPIO
  from RPLCD.gpio import CharLCD
elif use_ncurses:
  import curses

# tables
max_num_pads = 9
midi_map     = {38: "snare", 40: "snare", 36: "kick", 22: "hi-hat", 26: "hi-hat", 44: "pedal", \
                49: "crash", 55: "crash", 51: "ride", 48: "tom1",   50: "tom1", \
                45: "tom2",  47: "tom2",  43: "tom3", 58: "tom3"}
curve_types  = ["LINEAR", "EXP1", "EXP2", "LOG1", "LOG2"]
pad_names    = ["snare", "kick", "hi-hat", "ctrl", "crash", "tom1", "ride", "tom2", "tom3"]
pad_types    = ["PD120", "PD80R",  "PD8", "FD8", "VH12", "VH12CTRL", "KD7", "TP80",     "CY6",    "CY8", "DIABOLO12", \
                "CY5",   "HD1TOM", "PD6", "KD8", "PDX8", "KD120",    "PD5", "PDA120LS", "PDX100", "KT10"]
cmd_names    = ["type", "thresh", "sens", "pos thres", "pos sens", "rim thres", "curve", "spike", "rim/pos", "note", "note rim", "cross"]
cmd_val      = [   102,      103,    104,         105,        106,         107,     109,     110,       111,    112,        113,     114]
cmd_val_rng  = [    20,       31,     31,          31,         31,          31,       4,       4,         3,    127,        127,      31]
database     = [0] * len(cmd_val)
hi_hat_ctrl  = 0  # current hi-hat control value
sel_pad                 = 0
sel_cmd                 = 0
version_major           = -1
version_minor           = -1
do_update_param_outputs = False
midi_send_cmd           = -1 # invalidate per default
midi_previous_send_cmd  = -1
midi_send_val           = -1
auto_pad_sel            = False; # no auto pad selection per default
is_load_settings        = False

# initialize jack audio for MIDI
client      = jack.Client("EdrumulusGUI")
input_port  = client.midi_inports.register("MIDI_in")
output_port = client.midi_outports.register("MIDI_out")


################################################################################
# ncurses GUI implementation ###################################################
################################################################################
col_start = 5  # start column of parameter display
row_start = 1  # start row of parameter display
box_len   = 17 # length of the output boxes


def ncurses_init():
  global mainwin, midiwin, midigwin, poswin, posgwin, ctrlwin
  mainwin  = curses.initscr()
  midiwin  = curses.newwin(box_len, 24, row_start + 5, col_start)
  midigwin = curses.newwin(box_len, 26, row_start + 5, col_start + 25)
  poswin   = curses.newwin(box_len, 7,  row_start + 5, col_start + 52)
  posgwin  = curses.newwin(box_len, 24, row_start + 5, col_start + 60)
  ctrlwin  = curses.newwin(box_len, 7,  row_start + 5, col_start + 85)
  curses.noecho()       # turn off key echoing
  mainwin.keypad(True)  # enable the keypad for non-char keys
  mainwin.nodelay(True) # we want a non-blocking getch()
  curses.curs_set(0)    # suppress cursor


def ncurses_cleanup():
  mainwin.keypad(False)
  curses.echo()
  curses.endwin()


def ncurses_update_param_outputs():
  if version_major >= 0 and version_minor >= 0:
    mainwin.addstr(row_start - 1, col_start, "Edrumulus v{0}.{1}".format(version_major, version_minor))
  mainwin.addstr(row_start, col_start, "Press a key (q:quit; s,S:sel pad; c,C:sel command; a,A: auto pad sel; up,down: change param; r: reset)")
  if auto_pad_sel:
    mainwin.addstr(row_start + 2, col_start, "Selected pad (auto):  {:2d} ({:s})      ".format(sel_pad, pad_names[sel_pad]))
  else:
    mainwin.addstr(row_start + 2, col_start, "Selected pad:         {:2d} ({:s})      ".format(sel_pad, pad_names[sel_pad]))

  mainwin.addstr(row_start + 3, col_start, "Parameter: {:>10s}: {:s}             ".format(cmd_names[sel_cmd], \
    pad_types[database[sel_cmd]] if sel_cmd == 0 else curve_types[database[sel_cmd]] if sel_cmd == 6 else str(database[sel_cmd])))
  mainwin.refresh()
  midiwin.box() # in this box the received note-on MIDI notes are shown
  midiwin.addstr(0, 8, "MIDI-IN")
  midiwin.addstr(1, 2, "note (name) | value")
  midiwin.refresh()
  midigwin.box() # in this box the received MIDI velocity graph is shown
  midigwin.addstr(0, 6, "VELOCITY-GRAPH")
  midigwin.refresh()
  poswin.box() # in this box the received positional sensing values are shown
  poswin.addstr(0, 2, "POS")
  poswin.refresh()
  posgwin.box() # in this box the received positional sensing graph is shown
  posgwin.addstr(0, 5, "POSITION-GRAPH")
  posgwin.refresh()
  ctrlwin.box() # in this box the hi-hat controller value/bar is shown
  ctrlwin.addstr(0, 1, "CTRL")
  ctrlwin.addstr(1, 1, "{:4d}".format(hi_hat_ctrl))
  ctrlwin.vline(2, 3, curses.ACS_BLOCK, box_len - 3) # for reversed hline
  ctrlwin.vline(2, 3, " ", int((127.0 - hi_hat_ctrl) / 127 * (box_len - 3)))
  ctrlwin.refresh()


def ncurses_update_midi_win(key, value, instrument_name):
  midiwin.move(2, 0)
  midiwin.insdelln(1)
  midiwin.addstr(2, 1, "{:3d} ({:<6s}) | {:3d}".format(key, instrument_name, value))
  midigwin.move(1, 0)
  midigwin.insdelln(1)
  midigwin.move(2, 1)
  midigwin.hline(curses.ACS_BLOCK, max(1, int(float(value) / 128 * 25)))


def ncurses_update_possense_win(value):
  poswin.move(1, 0)
  poswin.insdelln(1)
  poswin.addstr(1, 1, " {:3d}".format(value))
  posgwin.move(1, 0)
  posgwin.insdelln(1)
  posgwin.addstr(1, 1, "M--------------------E")
  posgwin.addch(1, 2 + int(float(value) / 128 * 20), curses.ACS_BLOCK)


def ncurses_input_loop():
  global sel_pad, sel_cmd, database, auto_pad_sel, do_update_param_outputs
  # loop until user presses q
  while (ch := mainwin.getch()) != ord("q"):
    if ch != -1:
      if ch == ord("s") or ch == ord("S"): # change selected pad #####################
        cur_sel_pad = sel_pad
        cur_sel_pad = cur_sel_pad + 1 if ch == ord("s") else cur_sel_pad - 1
        sel_pad     = max(0, min(max_num_pads - 1, cur_sel_pad))
        send_value_to_edrumulus(108, sel_pad)
      elif ch == ord("c") or ch == ord("C"): # change selected command ###############
        cur_sel_cmd = sel_cmd
        cur_sel_cmd = cur_sel_cmd + 1 if ch == ord("c") else cur_sel_cmd - 1
        sel_cmd     = max(0, min(len(cmd_val) - 1, cur_sel_cmd))
      elif ch == 258 or ch == 259: # change parameter value with up/down keys ########
        cur_sel_val       = database[sel_cmd]
        cur_sel_val       = cur_sel_val + 1 if ch == 259 else cur_sel_val - 1
        database[sel_cmd] = max(0, min(cmd_val_rng[sel_cmd], cur_sel_val))
        send_value_to_edrumulus(cmd_val[sel_cmd], database[sel_cmd])
      elif ch == ord("a") or ch == ord("A"): # enable/disable auto pad selection #####
        auto_pad_sel = (ch == ord("a")) # capital "A" disables auto pad selection
      elif ch == ord("r"): # reset all settings ######################################
        mainwin.addstr(row_start + 1, col_start, "DO YOU REALLY WANT TO RESET ALL EDRUMULUS PARAMETERS [y/n]?")
        mainwin.nodelay(False) # temporarily, use blocking getch()
        if mainwin.getch() == ord("y"):
          send_value_to_edrumulus(115, 0) # midi_send_val will be ignored by Edrumulus for this command
        mainwin.nodelay(True) # go back to unblocking getch()
        mainwin.addstr(row_start + 1, col_start, "                                                           ")
      do_update_param_outputs = True

    if do_update_param_outputs:
      ncurses_update_param_outputs()
      do_update_param_outputs = False
    time.sleep(0.01)


################################################################################
# LCD GUI implementation #######################################################
################################################################################
button_name        = {25: "back", 11: "OK", 8: "down", 7: "up", 12: "left", 13: "right"}
selected_menu_item = 0
selected_pad       = 0


def lcd_button_handler(pin):
  if GPIO.input(pin) == 0: # note that button is inverted
    name       = button_name[pin] # current button name
    start_time = time.time()
    # auto press functionality for up/down/left/right buttons
    if (name == "left") or (name == "down") or (name == "up") or (name == "right"):
      lcd_on_button_pressed(name) # initial button press action
      auto_press_index = 0
      while GPIO.input(pin) == 0: # wait for the button up
        time.sleep(0.01)
        if time.time() - start_time - 0.7 - auto_press_index * 0.1 > 0: # after 0.7 s, auto press every 100 ms
          lcd_on_button_pressed(name)
          auto_press_index += 1
    else:
      while GPIO.input(pin) == 0: # wait for the button up
        time.sleep(0.01)
      if time.time() - start_time < 0.7:
        lcd_on_button_pressed(name) # on button up
      else:
        # TODO: implementation of going a menu level up...
        pass


def lcd_on_button_pressed(button_name):
  # TODO implement different menu levels here...
  # if we are in trigger settings menu level
  lcd_update_trigger_settings_menu(button_name)


def lcd_update_trigger_settings_menu(button_name):
  global selected_menu_item, selected_pad, database
  database_index = selected_menu_item
  if button_name == "down" and selected_menu_item > 0:
    selected_menu_item -= 1
  elif button_name == "up" and selected_menu_item < len(cmd_val) - 1:
      selected_menu_item += 1
  elif button_name == "right" and database[database_index] < cmd_val_rng[selected_menu_item]:
    database[database_index] += 1
    send_value_to_edrumulus(database_index, database [database_index])
  elif button_name == "left" and database[database_index] > 0:
    database[database_index] -= 1
    send_value_to_edrumulus(database_index, database[database_index])
  elif button_name == "OK" and selected_pad < 8:
    selected_pad += 1
    send_value_to_edrumulus(108, selected_pad)
  elif button_name == "back" and selected_pad > 0:
    selected_pad -= 1
    send_value_to_edrumulus(108, selected_pad)
  lcd_update()


def lcd_update():
  lcd.clear()
  lcd.cursor_pos = (0, 0)
  lcd.write_string("%s:%s" % (pad_names[selected_pad], cmd_names[selected_menu_item]))
  if selected_menu_item == 0:   # pad_types
    lcd.cursor_pos = (1, 3)
    lcd.write_string("<%s>" % pad_types[database[selected_menu_item]])
  elif selected_menu_item == 6: # curve_types
    lcd.cursor_pos = (1, 4)
    lcd.write_string("<%s>" % curve_types[database[selected_menu_item]])
  else:                         # use a number
    lcd.cursor_pos = (1, 6)
    lcd.write_string("<%d>" % database[selected_menu_item])


def lcd_init():
  global lcd
  lcd = CharLCD(pin_rs = 27, pin_rw = None, pin_e = 17, pins_data = [22, 23, 24, 10],
                numbering_mode = GPIO.BCM, cols = 16, rows = 2, auto_linebreaks = False)
  # startup message on LCD
  lcd.clear()
  lcd.cursor_pos = (0, 3)
  lcd.write_string("Edrumulus")
  lcd.cursor_pos = (1, 2)
  lcd.write_string("Prototype 5")
  # buttons initialization
  GPIO.setmode(GPIO.BCM)
  for pin in list(button_name.keys()):
    GPIO.setup(pin, GPIO.IN, pull_up_down = GPIO.PUD_DOWN)
    GPIO.add_event_detect(pin, GPIO.BOTH, callback = lcd_button_handler, bouncetime = 20)


################################################################################
# Settings handling ############################################################
################################################################################
def store_settings():
  global database
  with open("settings/trigger_settings.txt", "w") as f:
    for (pad_index, pad) in enumerate(pad_names):
      database = [-1] * len(cmd_val) # set database to invalid values
      send_value_to_edrumulus(108, pad_index)
      while any(i < 0 for i in database): # check if all values are received
        time.sleep(0.001)
      for (idx, midi_id) in enumerate(cmd_val):
        f.write("%d,%d,%d\n" % (pad_index, midi_id, database[idx]))


def load_settings():
  global database, is_load_settings
  is_load_settings = True # to update database of current command
  with open("settings/trigger_settings.txt", "r") as f:
    cur_pad = -1 # initialize with illegal index
    while True:
      line = f.readline()
      if len(line) == 0:
        break
      (pad, command, value) = line.replace("\n", "").split(",")
      if int(command) in cmd_val:
        if cur_pad != int(pad):
          database = [-1] * len(cmd_val) # set database to invalid values
          cur_pad  = int(pad)
          send_value_to_edrumulus(108, cur_pad)
        send_value_to_edrumulus(int(command), int(value))
        cur_cmd = cmd_val.index(int(command))
        while database[cur_cmd] != int(value): # wait for parameter to be applied in Edrumulus
          time.sleep(0.001)
  is_load_settings = False # we are done now


################################################################################
# MIDI handling (via jack audio) ###############################################
################################################################################
def send_value_to_edrumulus(command, value):
  global midi_send_cmd, midi_send_val
  (midi_send_cmd, midi_send_val) = (command, value);
  while midi_send_cmd >= 0:
    time.sleep(0.001)


# jack audio callback function
@client.set_process_callback
def process(frames):
  global database, midi_send_val, midi_send_cmd, midi_previous_send_cmd, do_update_param_outputs, \
         version_major, version_minor, hi_hat_ctrl, sel_pad
  output_port.clear_buffer()
  for offset, data in input_port.incoming_midi_events():
    if len(data) == 3:
      status = int.from_bytes(data[0], "big")
      key    = int.from_bytes(data[1], "big")
      value  = int.from_bytes(data[2], "big")

      if status == 0x80: # act on control messages
        if key in cmd_val:
          cur_cmd = cmd_val.index(key)
          # do not update command which was just changed to avoid the value jumps back to old value
          if (midi_previous_send_cmd != key) or is_load_settings:
            database[cur_cmd]       = max(0, min(cmd_val_rng[cur_cmd], value));
            do_update_param_outputs = True;
        if key == 127: # check for major version number
          version_major = value
        if key == 126: # check for minor version number
          version_minor = value

      if (status & 0xF0) == 0x90: # display current note-on received value
        try:
          instrument_name = midi_map[key]
        except:
          instrument_name = "" # not all MIDI values have a defined instrument name
        if use_ncurses:
          ncurses_update_midi_win(key, value, instrument_name)
        do_update_param_outputs = True
        if auto_pad_sel and instrument_name and value > 10: # auto pad selection (velocity threshold of 10)
          try:
            pad_index = pad_names.index(instrument_name) # throws exception if pad was not found
            if sel_pad is not pad_index: # only change pad if it is different from current
              sel_pad       = pad_index
              midi_send_val = sel_pad # we cannot use send_value_to_edrumulus here
              midi_send_cmd = 108
          except:
            pass # pad not found, do nothing

      if (status & 0xF0) == 0xB0: # display current positional sensing received value
        if key == 16: # positional sensing
          if use_ncurses:
            ncurses_update_possense_win(value)
          do_update_param_outputs = True
        if key == 4: # hi-hat controller
          hi_hat_ctrl             = value
          do_update_param_outputs = True

  if midi_send_cmd >= 0:
    output_port.write_midi_event(0, (185, midi_send_cmd, midi_send_val))
    midi_previous_send_cmd = midi_send_cmd # store previous value
    midi_send_cmd          = -1 # invalidate current command to prepare for next command


################################################################################
# Main function ################################################################
################################################################################
# initialize GUI (16x2 LCD or ncurses GUI)
if use_lcd:
  lcd_init()
elif use_ncurses:
  ncurses_init()

if not use_ncurses and not non_block:
  print("press Return to quit")

with client:
  try:
    input_port.connect("ttymidi:MIDI_in")   # ESP32
    output_port.connect("ttymidi:MIDI_out") # ESP32
  except:
    try:
      teensy_out = jack.get_ports("Edrumulus ", is_midi=True, is_input=True)
      teensy_in  = jack.get_ports("Edrumulus ", is_midi=True, is_output=True)
      if teensy_in and teensy_out:
        input_port.connect(teensy_in[0])   # Teensy
        output_port.connect(teensy_out[0]) # Teensy
    except:
      pass # if no Edrumulus hardware was found, no jack is started

  # load settings from file
  load_settings()

  send_value_to_edrumulus(108, sel_pad) # to query all Edrumulus current parameters
  time.sleep(0.2)
  if use_lcd:
    lcd_update()
  elif use_ncurses:
    ncurses_update_param_outputs()

  # main loop (LCD is event driven and does not need a loop)
  if use_lcd or no_gui:
    input() # simply wait until a key is pressed to quit the application
  elif use_ncurses:
    ncurses_input_loop()

  # store settings in file
  if not no_gui and not non_block:
    store_settings()

  # clean up and exit
  if use_lcd:
    lcd.close() # just this single call is needed
  elif use_ncurses:
    ncurses_cleanup()

