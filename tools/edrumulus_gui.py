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

import os
import sys
import signal
import socket
import time
import threading
import math
from pathlib import Path
use_rtmidi  = "rtmidi"    in sys.argv # use rtmidi instead of jack audio
no_gui      = "no_gui"    in sys.argv # no GUI but blocking (just settings management)
non_block   = "non_block" in sys.argv # no GUI and non-blocking (just settings management)
use_lcd     = "lcd"       in sys.argv # LCD GUI mode on Raspberry Pi
use_webui   = "webui"     in sys.argv # web UI GUI mode on Raspberry Pi
use_ncurses = not no_gui and not non_block and not use_lcd and not use_webui # normal console GUI mode (default)
if use_rtmidi:
  import rtmidi
  from rtmidi.midiutil import open_midiinput
  from rtmidi.midiutil import open_midioutput
else:
  import jack
if use_lcd:
  import RPi.GPIO as GPIO
  from RPLCD.gpio import CharLCD
elif use_ncurses:
  import curses
elif use_webui:
  import http.server

# tables
pad_types_dict = {"PDA120LS Roland Mesh Pad":18, "PDX100 Roland Mesh Pad":19, "PD120 Roland Mesh Pad":0, \
                    "PD85 Roland Mesh Pad":1, "PDX8 Roland Mesh Pad":15, "DIABOLO12 drum-tec Mesh Pad":10, \
                    "MPS-750X Millenium Mesh Tom":21, "MPS-750X Millenium Mesh Snare":22, \
                  "PD8 Roland Rubber Pad":2, "PD6 Roland Rubber Pad":13, "PD5 Roland Rubber Pad":17, \
                    "HD1TOM Roland Rubber Pad":12, "TP80 Yamaha Rubber Pad":7, \
                  "CY8 Roland Cymbal":9, "CY6 Roland Cymbal":8, "CY5 Roland Cymbal":11, "VH12 Roland Cymbal":4, \
                    "MPS-750X Millenium Ride":24, "MPS-750X Millenium Crash":25, "LEHHS12C Lemon Hi-Hat Cymbal":26, \
                  "KD120 Roland Mesh Kick Pad":16, "KD8 Roland Kick Pad":14, \
                    "KD7 Roland Kick Pad":6, "KT10 Roland Kick Pedal":20, "MPS-750X Millenium Kick Pad":23, \
                  "FD8 Roland Hi-Hat Pedal":3, "VH12CTRL Roland Hi-Hat Pedal":5, "LEHHS12CCTRL Lemon Hi-Hat Pedal":27}
pad_names    = ["snare", "kick", "hi-hat", "ctrl", "crash", "tom1", "ride", "tom2", "tom3"]
curve_types  = ["LINEAR", "EXP1", "EXP2", "LOG1", "LOG2"]
cmd_names    = [                 "type", "thresh", "sens", "pos thres", "pos sens", "rim thres", "mask",              "curve"]
cmd_val      = [                    102,      103,    104,         105,        106,         107,    118,                  109]
cmd_val_rng  = [len(pad_types_dict) - 1,       31,     31,          31,         31,          31,     63, len(curve_types) - 1]
cmd_names   += ["rim/pos", "rim boost", "cross", "note", "note rim", "note2", "note2 rim",         "coupling", "spike (GLOBAL)"]
cmd_val     += [      111,         119,     114,    112,        113,     116,         117,                120,              110]
cmd_val_rng += [        3,          31,      31,    127,        127,     127,         127, len(pad_names) - 1,                4]
midi_map     = {38: "snare", 40: "snare", 36: "kick", 22: "hi-hat", 26: "hi-hat", 44: "pedal", \
                49: "crash", 55: "crash", 51: "ride", 48: "tom1",   50: "tom1", \
                45: "tom2",  47: "tom2",  43: "tom3", 58: "tom3"}
database                = [0] * len(cmd_val)
pad_types_dict_list     = list(pad_types_dict)
pad_types               = list(dict(sorted(pad_types_dict.items(), key=lambda item: item[1]))) # sorted as in Edrumulus enumeration
hi_hat_ctrl             = 0  # current hi-hat control value
sel_pad                 = 0
sel_cmd                 = 0
version_major           = -1
version_minor           = -1
do_update_midi_in       = False
do_update_display       = False
SIGINT_received         = False
original_sigint_handler = []
midi_send_cmd           = -1 # invalidate per default
midi_previous_send_cmd  = -1
midi_send_val           = -1
auto_pad_sel            = False; # no auto pad selection per default
is_load_settings        = False
error_value             = 0
selected_kit            = ""
kit_vol_str             = ""

# initialize jack audio for MIDI
if not use_rtmidi:
  client      = jack.Client("EdrumulusGUI")
  input_port  = client.midi_inports.register("MIDI_in")
  output_port = client.midi_outports.register("MIDI_out")


################################################################################
# Common GUI functions #########################################################
################################################################################
def process_user_input(ch):
  global sel_pad, sel_cmd, database, auto_pad_sel
  if ch == "s" and sel_pad < len(pad_names) - 1:
    send_value_to_edrumulus(108, sel_pad := sel_pad + 1)
  elif ch == "S" and sel_pad > 0:
    send_value_to_edrumulus(108, sel_pad := sel_pad - 1)
  elif ch == "c" and sel_cmd < len(cmd_val) - 1:
    sel_cmd += 1
  elif ch == "C" and sel_cmd > 0:
    sel_cmd -= 1
  elif (ch == chr(259) or ch == "U"): # 259: up key
    if cmd_names[sel_cmd] == "type": # special order for types needed, derived from dictionary
      linear_idx = get_linear_pad_type_index(database[sel_cmd])
      if linear_idx < cmd_val_rng[sel_cmd]:
        linear_idx += 1
        database[sel_cmd] = pad_types_dict[pad_types_dict_list[linear_idx]]
        send_value_to_edrumulus(cmd_val[sel_cmd], database[sel_cmd])
    else:
      if database[sel_cmd] < cmd_val_rng[sel_cmd]:
        database[sel_cmd] += 1
        send_value_to_edrumulus(cmd_val[sel_cmd], database[sel_cmd])
  elif (ch == chr(258) or ch == "D"): # 258: down key
    if cmd_names[sel_cmd] == "type": # special order for types needed, derived from dictionary
      linear_idx = get_linear_pad_type_index(database[sel_cmd])
      if linear_idx > 0:
        linear_idx -= 1
        database[sel_cmd] = pad_types_dict[pad_types_dict_list[linear_idx]]
        send_value_to_edrumulus(cmd_val[sel_cmd], database[sel_cmd])
    else:
      if database[sel_cmd] > 0:
        database[sel_cmd] -= 1
        send_value_to_edrumulus(cmd_val[sel_cmd], database[sel_cmd])
  elif ch == "a" or ch == "A": # enable/disable auto pad selection
    auto_pad_sel = ch == "a" # capital "A" disables auto pad selection
  elif (ch == "k" or ch == "K") and not use_rtmidi: # kit selection (only for jack audio mode)
    ecasound_switch_chains(ch == "k")
  elif (ch == "v" or ch == "V") and not use_rtmidi: # kit volume (only for jack audio mode)
    ecasound_kit_volume(ch == "v")

def get_linear_pad_type_index(d):
  return pad_types_dict_list.index([k for k, v in pad_types_dict.items() if v == d][0])

def parse_cmd_param(cmd):
  # check for "pad type" and "curve type" special cases, otherwise convert integer in string
  return pad_types[database[cmd]] if cmd == 0 else curve_types[database[cmd]] if cmd == 7 else str(database[cmd])

def signal_handler(sig, frame):
  global SIGINT_received
  signal.signal(signal.SIGINT, original_sigint_handler) # we are done, restore original signal handler
  SIGINT_received = True


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
  mainwin.addstr(row_start, col_start, "Trying to connect to Edrumulus device...")
  mainwin.refresh()

def ncurses_cleanup():
  mainwin.keypad(False)
  curses.echo()
  curses.endwin()

def ncurses_update_param_outputs():
  mainwin.move(row_start - 1, col_start) # clear first line
  mainwin.clrtoeol()                     # clear first line
  if error_value > 63:
    mainwin.addstr(row_start + 4, col_start, "DC OFFSET ERROR ON PAD {:2d}/{:1d}".format((error_value % 64), math.floor((error_value - 64) / 64)))
  elif error_value > 0:
    mainwin.addstr(row_start + 4, col_start, "SAMPLING RATE TOO LOW WARNING")
  else:
    mainwin.addstr(row_start + 4, col_start, "                             ")
  if version_major >= 0 and version_minor >= 0:
    mainwin.addstr(row_start - 1, col_start, "Edrumulus v{0}.{1}".format(version_major, version_minor))
  if selected_kit:
    mainwin.addstr(row_start - 1, col_start + 30, selected_kit + ", Kit-Vol: " + kit_vol_str if kit_vol_str else selected_kit)
  mainwin.addstr(row_start, col_start, "Press a key (q:quit; s,S:sel pad; c,C:sel command; a,A: auto pad sel; up,down: change param; r: reset)")
  if auto_pad_sel:
    mainwin.addstr(row_start + 2, col_start, "Selected pad (auto):       {:2d} ({:s})      ".format(sel_pad, pad_names[sel_pad]))
  else:
    mainwin.addstr(row_start + 2, col_start, "Selected pad:              {:2d} ({:s})      ".format(sel_pad, pad_names[sel_pad]))
  mainwin.addstr(row_start + 3, col_start, "Parameter: {:>15s}: {:s}                       ".format(cmd_names[sel_cmd], parse_cmd_param(sel_cmd)))
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
  global sel_pad, sel_cmd, database, auto_pad_sel, do_update_display, do_update_midi_in
  # loop until user presses q
  while (ch := mainwin.getch()) != ord("q") and not SIGINT_received:
    if ch != -1:
      do_update_display = True
      if ch == ord("r"): # reset all settings
        mainwin.addstr(row_start + 1, col_start, "DO YOU REALLY WANT TO RESET ALL EDRUMULUS PARAMETERS [y/n]?")
        mainwin.nodelay(False) # temporarily, use blocking getch()
        if mainwin.getch() == ord("y"):
          send_value_to_edrumulus(115, 0) # midi_send_val will be ignored by Edrumulus for this command
        mainwin.nodelay(True) # go back to unblocking getch()
        mainwin.addstr(row_start + 1, col_start, "                                                           ")
      else:
        process_user_input(chr(ch))

    time.sleep(0.01) # moving sleep up before ncurses_update_param_outputs() fixes timing issue on MacOS
    if do_update_midi_in or do_update_display:
      ncurses_update_param_outputs()
      do_update_display = False
      do_update_midi_in = False


################################################################################
# LCD GUI implementation #######################################################
################################################################################
button_name          = {25: "back", 11: "OK", 8: "down", 7: "up", 12: "left", 13: "right"}
lcd_menu_id          = 0 # 0: main menu, 1: trigger menu
lcd_shutdown_confirm = False

def lcd_button_handler(pin):
  global lcd_menu_id
  if GPIO.input(pin) == 0: # note that button is inverted
    name       = button_name[pin] # current button name
    start_time = time.time()
    # auto press functionality for up/down/left/right buttons
    if (name == "left") or (name == "down") or (name == "up") or (name == "right"):
      lcd_on_button_pressed(name, False) # initial button press action
      auto_press_index = 0
      while GPIO.input(pin) == 0: # wait for the button up
        time.sleep(0.01)
        if time.time() - start_time - 0.7 - auto_press_index * 0.1 > 0: # after 0.7 s, auto press every 100 ms
          lcd_on_button_pressed(name, False)
          auto_press_index += 1
    else:
      while GPIO.input(pin) == 0 and time.time() - start_time < 0.7: # wait for the button up or time-out
        time.sleep(0.01)
      lcd_on_button_pressed(name, time.time() - start_time > 0.7)

def lcd_on_button_pressed(button_name, is_long_press):
  global lcd_menu_id, lcd_shutdown_confirm, auto_pad_sel
  if lcd_menu_id == 0: # main menu #####
    if button_name == "up":
      process_user_input("k") # change kit
    elif button_name == "down":
      process_user_input("K")
    elif button_name == "right":
      process_user_input("v") # change kit volume
    elif button_name == "left":
      process_user_input("V")
    elif button_name == "OK" and not is_long_press:
      if lcd_shutdown_confirm:
        lcd_shutdown()
      else:
        lcd_menu_id = 1 # go into trigger menu
    elif button_name == "back" and not is_long_press:
      lcd_shutdown_confirm = False # cancel shutdown procedure
    elif button_name == "back" and is_long_press:
      lcd_shutdown()
  elif lcd_menu_id == 1: # trigger menu #####
    if button_name == "OK" and not is_long_press:
      process_user_input("s") # select pad
    if button_name == "OK" and is_long_press:
      auto_pad_sel = not auto_pad_sel # toggle auto pad selection
    elif button_name == "back" and not is_long_press:
      process_user_input("S")
    elif button_name == "back" and is_long_press:
      lcd_menu_id = 0 # long press of "back" returns in main menu
    elif button_name == "up":
      process_user_input("c") # select trigger parameter
    elif button_name == "down":
      process_user_input("C")
    elif button_name == "right":
      process_user_input(chr(259)) # change trigger parameter
    elif button_name == "left":
      process_user_input(chr(258))
  lcd_update()

def lcd_shutdown():
  global lcd_shutdown_confirm
  if not lcd_shutdown_confirm:
    lcd.clear()
    lcd.cursor_pos = (0, 0)
    lcd.write_string("Really Shutdown?")
    lcd_shutdown_confirm = True
  else:
    lcd.clear()
    store_settings()
    os.system("sudo shutdown -h now")

def lcd_loop():
  global do_update_display
  while not SIGINT_received:
    if do_update_display:
      lcd_update()
      do_update_display = False
    time.sleep(0.1)

def lcd_update():
  if not lcd_shutdown_confirm: # do not overwrite shutdown question text
    lcd.clear()
    lcd.cursor_pos = (0, 0)
    if lcd_menu_id == 0: # main menu
      if selected_kit: # only show main menu if selected kit name is available
        lcd.write_string(selected_kit)
        if kit_vol_str: # only show kit volume if available
          lcd.cursor_pos = (1, 0)
          lcd.write_string("Vol: %s" % kit_vol_str)
    elif lcd_menu_id == 1: # trigger menu
      lcd.write_string(("A:" if auto_pad_sel else "") + "%s:%s" % (pad_names[sel_pad], cmd_names[sel_cmd]))
      lcd.cursor_pos = (1, 4)
      lcd.write_string("<%s>" % parse_cmd_param(sel_cmd).split(" ")[0]) # split to only show pad type short name

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
# Web UI GUI implementation ####################################################
################################################################################
if use_webui:
  class WebUI(http.server.BaseHTTPRequestHandler):
    def log_message(self, format, *args):
      pass # no logging
    def do_GET(self):
      self.send_response(200)
      self.send_header("Content-type", "text/html")
      self.end_headers()
      self.wfile.write(bytes("<body><form action='button'>", "utf-8"))
      self.wfile.write(bytes("<head><style>button{font-size:60px;}table{font-size:60px;width:100%}</style></head>", "utf-8"))
      if self.path.find("key=") != -1:
        key_value = self.path.split("key=")[1]
        if key_value == "askshutdown":
          self.wfile.write(bytes("<button type='submit' name='key' value='shutdown'>SHUTDOWN NOW!</button><br><br>", "utf-8"))
          self.wfile.write(bytes("<button type='submit' name='key' value=''>CANCEL</button><br>", "utf-8"))
        elif key_value == "shutdown":
          store_settings()
          os.system("sudo shutdown -h now")
        else:
          process_user_input(key_value)
          # quick hack fix for update problem: introduce delay on pad selection
          if key_value == "s" or key_value == "S":
            time.sleep(0.01)

      self.wfile.write(bytes("""
        <table><tr><td>Pad:</td><td><button type='submit' name='key' value='S'>DOWN</button></td>
                                <td><button type='submit' name='key' value='s'>UP</button></td></tr>
        <tr><td>Parameter:</td><td><button type='submit' name='key' value='C'>DOWN</button></td>
                               <td><button type='submit' name='key' value='c'>UP</button></td></tr>
        <tr><td>Value:</td><td><button type='submit' name='key' value='D'>DOWN</button></td>
                           <td><button type='submit' name='key' value='U'>UP</button></td></tr><br>
        </table><table><tr><td>""", "utf-8"))
      self.wfile.write(bytes("%s: %s: %s" % (pad_names[sel_pad], cmd_names[sel_cmd], parse_cmd_param(sel_cmd)), "utf-8"))
      self.wfile.write(bytes("</td></tr></table><br><br><br><button type='submit' name='key' value='askshutdown'>SHUTDOWN</button></form></body>", "utf-8"))


################################################################################
# Settings handling ############################################################
################################################################################
def store_settings():
  global database
  settings_file = Path(__file__).parent.joinpath("settings", "trigger_settings_current.txt")
  with settings_file.open("w") as f:
    for (pad_index, pad) in enumerate(pad_names):
      database = [-1] * len(cmd_val) # set database to invalid values
      send_value_to_edrumulus(108, pad_index)
      while any(i < 0 for i in database): # check if all values are received
        time.sleep(0.001)
      for (idx, midi_id) in enumerate(cmd_val):
        f.write("%d,%d,%d\n" % (pad_index, midi_id, database[idx]))
  settings_file.replace(Path(__file__).parent.joinpath("settings", "trigger_settings.txt")) # fixes Issue #108 (settings file empty)

def load_settings():
  global database, is_load_settings
  is_load_settings = True # to update database of current command
  settings_file = Path(__file__).parent.joinpath("settings", "trigger_settings.txt")
  with settings_file.open("r") as f:
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
# Ecasound handling (via socket) ###############################################
################################################################################
ecasound_socket          = []
chain_setups             = []
chain_index              = 0
kit_volume               = 0 # dB
ecasound_socket          = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
ecasound_connect_try_cnt = 20 # 20 * 0.5s = 10s

def ecasound_connection():
  global ecasound_socket, ecasound_connect_try_cnt, do_update_display, chain_setups
  ecasound_socket.settimeout(0.2)
  try:
    ecasound_socket.connect(('localhost', 2868))
    time.sleep(2.0) # give ecasound some time to load all .ecs files
    ecasound_socket.sendall("cs-list\r\n".encode("utf8")) # query chain names
    data         = ecasound_socket.recv(1024)
    chain_setups = str(data).split("\\r\\n")[1].split(",")
    ecasound_connect_try_cnt = 0 # 0 means socket connected successful and chain names received
    ecasound_switch_chains(True) # initial chain select
    do_update_display = True # update GUI
  except:
    ecasound_connect_try_cnt -= 1
    if ecasound_connect_try_cnt > 1: # will stop trying at ecasound_connect_try_cnt == 1
      threading.Timer(0.5, ecasound_connection).start()

def ecasound_switch_chains(do_increment):
  global chain_setups, chain_index, selected_kit, kit_vol_str
  if ecasound_connect_try_cnt == 0:
    chain_index = chain_index + 1 if do_increment else chain_index - 1
    chain_index = chain_index % len(chain_setups)
    selected_kit = chain_setups[chain_index]
    ecasound_socket.sendall("engine-halt\r\ncs-select {0}\r\ncs-connect {0}\r\nengine-launch\r\nstart\r\n".format(selected_kit).encode("utf8"))
    ecasound_apply_kit_volume()

def ecasound_kit_volume(do_increment):
  global kit_volume, kit_vol_str
  if ecasound_connect_try_cnt == 0:
    # now modify and apply new volume value
    kit_volume = kit_volume + 1 if do_increment else kit_volume - 1
    kit_volume = max(min(kit_volume, 30), -30)
    ecasound_apply_kit_volume()

def ecasound_apply_kit_volume():
  global kit_vol_str
  ecasound_socket.sendall("c-select Master\r\ncop-set 1,1,{0}\r\n".format(kit_volume).encode("utf8"))
  kit_vol_str = str(kit_volume) + " dB"


################################################################################
# MIDI implementation ##########################################################
################################################################################
def act_on_midi_in(status, key, value):
  global database, midi_send_val, midi_send_cmd, midi_previous_send_cmd, do_update_midi_in, \
         version_major, version_minor, hi_hat_ctrl, sel_pad, do_update_display, error_value

  if status == 0x80: # act on control messages (0x80: Note Off)
    if key in cmd_val:
      cur_cmd = cmd_val.index(key)
      # do not update command which was just changed to avoid the value jumps back to the old value
      if (midi_previous_send_cmd != key) or is_load_settings:
        database[cur_cmd] = max(0, min(cmd_val_rng[cur_cmd], value));
        do_update_midi_in = True;
    if key == 125: # check for error state
      error_value       = value
      do_update_display = True
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
    do_update_midi_in = True
    if auto_pad_sel and instrument_name and value > 10: # auto pad selection (velocity threshold of 10)
      try:
        pad_index = pad_names.index(instrument_name) # throws exception if pad was not found
        if sel_pad is not pad_index: # only change pad if it is different from current
          sel_pad           = pad_index
          midi_send_val     = sel_pad # we cannot use send_value_to_edrumulus here
          midi_send_cmd     = 108
          do_update_display = True
      except:
        pass # pad not found, do nothing

  if (status & 0xF0) == 0xB0: # display current positional sensing received value
    if key == 16: # positional sensing
      if use_ncurses:
        ncurses_update_possense_win(value)
      do_update_midi_in = True
    if key == 4: # hi-hat controller
      hi_hat_ctrl       = value
      do_update_midi_in = True


################################################################################
# MIDI handling (via jack audio) ###############################################
################################################################################
if not use_rtmidi:
  def send_value_to_edrumulus(command, value):
    global midi_send_cmd, midi_send_val
    (midi_send_cmd, midi_send_val) = (command, value);
    while midi_send_cmd >= 0:
      time.sleep(0.001)

  # jack audio callback function
  @client.set_process_callback
  def process(frames):
    global midi_send_cmd, midi_previous_send_cmd
    output_port.clear_buffer()
    for offset, data in input_port.incoming_midi_events():
      if len(data) == 3:
        act_on_midi_in(int.from_bytes(data[0], "big"), int.from_bytes(data[1], "big"), int.from_bytes(data[2], "big"))

    if midi_send_cmd >= 0:
      output_port.write_midi_event(0, (185, midi_send_cmd, midi_send_val))
      midi_previous_send_cmd = midi_send_cmd # store previous value
      midi_send_cmd          = -1 # invalidate current command to prepare for next command


################################################################################
# MIDI handling (via rtmidi) ###################################################
################################################################################
if use_rtmidi:
  def send_value_to_edrumulus(command, value):
    global midi_send_cmd, midi_send_val
    (midi_send_cmd, midi_send_val) = (command, value);
    midiout.send_message([185, midi_send_cmd, midi_send_val])
    midi_previous_send_cmd = midi_send_cmd # store previous value
    midi_send_cmd          = -1 # invalidate current command to prepare for next command

  class MidiInputHandler(object):
    def __init__(self, port):
      self.port = port
    def __call__(self, event, data=None):
      if len(event[0]) == 3:
        act_on_midi_in(event[0][0], event[0][1], event[0][2])


################################################################################
# Main function ################################################################
################################################################################
# initialize GUI (16x2 LCD or ncurses GUI)
if use_lcd:
  lcd_init()
elif use_ncurses:
  ncurses_init()
elif use_webui:
  web_server = http.server.HTTPServer(("", 8080), WebUI)
  web_server.timeout = 1

# ctrl+c quits the application
original_sigint_handler = signal.signal(signal.SIGINT, signal_handler)

# initialize MIDI
if use_rtmidi: # initialize rtmidi (only Teensy board supported)
  try:
    in_name  = "EdrumulusIn" if [s for s in rtmidi.MidiIn().get_ports() if "EdrumulusIn" in s] else "Edrumulus"
    out_name = "EdrumulusOut" if [s for s in rtmidi.MidiOut().get_ports() if "EdrumulusOut" in s] else "Edrumulus"
    midiin, port_name_in   = open_midiinput([s for s in rtmidi.MidiIn().get_ports() if in_name in s][0], client_name="EdrumulusGUI")
    midiout, port_name_out = open_midioutput([s for s in rtmidi.MidiOut().get_ports() if out_name in s][0], client_name="EdrumulusGUI")
    midiin.set_callback(MidiInputHandler(port_name_in))
  except:
    raise Exception("No native Edrumulus USB device (e.g., Teensy) nor loopMIDI driver found.")
else: # initialize jack midi
  client.activate()
  try:
    input_port.connect("ttymidi:MIDI_in")   # ESP32
    output_port.connect("ttymidi:MIDI_out") # ESP32
  except:
    try:
      teensy_out = client.get_ports("Edrumulus ", is_midi=True, is_input=True)
      teensy_in  = client.get_ports("Edrumulus ", is_midi=True, is_output=True)
      input_port.connect(teensy_in[0])   # Teensy
      output_port.connect(teensy_out[0]) # Teensy
    except:
      pass # if no Edrumulus hardware was found, no jack is started

# load settings from file
load_settings()

send_value_to_edrumulus(108, sel_pad) # to query all Edrumulus current parameters
time.sleep(0.2)
do_update_display = True

# it takes time for ecasound to start up -> we need a timer thread for socket connection
if not use_rtmidi: # ecasound is only supported for jack audio mode
  threading.Timer(0.0, ecasound_connection).start()

# main loop
if no_gui:
  print("press Return to quit")
  input() # wait until a key is pressed to quit the application
elif use_lcd:
  lcd_loop()
elif use_ncurses:
  ncurses_input_loop()
elif use_webui:
  while not SIGINT_received:
    web_server.handle_request()

# store settings in file
if not no_gui and not non_block:
  store_settings()

# clean up and exit
if use_lcd:
  lcd.close() # just this single call is needed
elif use_ncurses:
  ncurses_cleanup()
elif use_webui:
  web_server.server_close()
if use_rtmidi:
  midiin.delete()
  midiout.delete()
else:
  client.deactivate()
  client.close()

