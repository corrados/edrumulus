#!/usr/bin/env python3

#*******************************************************************************
# Copyright (c) 2022-2023
# Author(s): Volker Fischer
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

import jack
import curses
import time


# tables
max_num_pads = 9
midi_map     = {38: "snare", 40: "snare", 36: "kick", 22: "hi-hat", 26: "hi-hat", 44: "pedal", \
                49: "crash", 51: "ride",  48: "tom1", 45: "tom2",   43: "tom3"}
curve_types  = ["LINEAR", "EXP1", "EXP2", "LOG1", "LOG2"]
pad_names    = ["snare", "kick", "hi-hat", "ctrl", "crash", "tom1", "ride", "tom2", "tom3"]
pad_types    = ["PD120", "PD80R", "PD8", "FD8", "VH12", "VH12CTRL", "KD7", "TP80", "CY6", "CY8", "DIABOLO12", \
                "CY5", "HD1TOM", "PD6", "KD8", "PDX8", "KD120", "PD5", "PDA120LS", "PDX100", "KT10"]
cmd_names    = [ "type", "thresh", "sens", "pos thres", "pos sens", "rim thres", "curve", "spike", "rim/pos", "note", "note rim", "cross" ]
cmd_val      = [    102,      103,    104,         105,        106,         107,     109,     110,       111,    112,        113,     114 ]
cmd_val_rng  = [     20,       31,     31,          31,         31,          31,       4,       4,         3,    127,        127,      31 ]
database     = [0] * len(cmd_val)
hi_hat_ctrl  = 0  # current hi-hat control value
col_start    = 5  # start column of parameter display
row_start    = 1  # start row of parameter display
box_len      = 17 # length of the output boxes
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

# initialize GUI
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

# initialize jack audio for MIDI
client      = jack.Client('EdrumulusGUI')
input_port  = client.midi_inports.register("MIDI_in")
output_port = client.midi_outports.register("MIDI_out")


# parse command parameter
def parse_cmd_param(cmd):
  # check for "pad type" and "curve type" special cases, otherwise convert integer in string
  return pad_types[database[cmd]] if cmd == 0 else curve_types[database[cmd]] if cmd == 6 else str(database[cmd])


# update window parameter outputs
def update_param_outputs():
  if version_major >= 0 and version_minor >= 0:
    mainwin.addstr(row_start - 1, col_start, "Edrumulus v{0}.{1}".format(version_major, version_minor))
  mainwin.addstr(row_start, col_start, "Press a key (q:quit; s,S:sel pad; c,C:sel command; a,A: auto pad sel; up,down: change param; r: reset)")
  if auto_pad_sel:
    mainwin.addstr(row_start + 2, col_start, "Selected pad (auto):  {:2d} ({:s})      ".format(sel_pad, pad_names[sel_pad]))
  else:
    mainwin.addstr(row_start + 2, col_start, "Selected pad:         {:2d} ({:s})      ".format(sel_pad, pad_names[sel_pad]))

  mainwin.addstr(row_start + 3, col_start, "Parameter: {:>10s}: {:s}             ".format(cmd_names[sel_cmd], parse_cmd_param(sel_cmd)))
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
  ctrlwin.vline(2, 3, ' ', int((127.0 - hi_hat_ctrl) / 127 * (box_len - 3)))
  ctrlwin.refresh()


# update pad selection (for auto pad selection)
def update_pad_selection(midi_note_in, midi_note1, midi_note2, pad_index):
  global sel_pad, midi_send_val, midi_send_cmd
  if (midi_note_in == midi_note1 or midi_note_in == midi_note2) and sel_pad is not pad_index:
    sel_pad       = pad_index
    midi_send_val = sel_pad
    midi_send_cmd = 108


def store_settings():
  with open("settings/trigger_settings.txt", "w") as f:
    for (pad_index, pad) in enumerate(pad_names):
      send_value_to_edrumulus(108, pad_index)
      time.sleep(0.2) # should be enough time to transfer all pad parameters
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
      (pad, command, value) = line.replace('\n', '').split(',')
      if int(command) in cmd_val:
        if cur_pad != int(pad):
          database = [0] * 128 # reset database
          cur_pad  = int(pad)
          send_value_to_edrumulus(108, cur_pad)
        send_value_to_edrumulus(int(command), int(value))
        cur_cmd = cmd_val.index(int(command))
        while database[cur_cmd] != int(value): # wait for parameter to be applied in Edrumulus
          time.sleep(0.001)
  is_load_settings = False # we are done now


def send_value_to_edrumulus(command, value):
  global midi_send_cmd, midi_send_val
  (midi_send_cmd, midi_send_val) = (command, value);
  while midi_send_cmd >= 0:
    time.sleep(0.001)


# jack audio callback function
@client.set_process_callback
def process(frames):
  global database, midi_send_val, midi_send_cmd, midi_previous_send_cmd, do_update_param_outputs, \
         version_major, version_minor, hi_hat_ctrl
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
        midiwin.move(2, 0)
        midiwin.insdelln(1)
        try:
          instrument_name = midi_map[key]
        except:
          instrument_name = "" # not all MIDI values have a defined instrument name
        midiwin.addstr(2, 1, "{:3d} ({:<6s}) | {:3d}".format(key, instrument_name, value))
        midigwin.move(1, 0)
        midigwin.insdelln(1)
        midigwin.move(2, 1)
        midigwin.hline(curses.ACS_BLOCK, max(1, int(float(value) / 128 * 25)))
        do_update_param_outputs = True
        if auto_pad_sel and value > 10: # auto pad selection
          update_pad_selection(key, 38, 40, 0) # snare
          update_pad_selection(key, 36, 36, 1) # kick
          update_pad_selection(key, 22, 26, 2) # hi-hat
          update_pad_selection(key, 49, 55, 4) # crash
          update_pad_selection(key, 48, 50, 5) # tom1
          update_pad_selection(key, 51, 53, 6) # ride
          update_pad_selection(key, 45, 47, 7) # tom2
          update_pad_selection(key, 43, 58, 8) # tom3


      if (status & 0xF0) == 0xB0: # display current positional sensing received value
        if key == 16: # positional sensing
          poswin.move(1, 0)
          poswin.insdelln(1)
          poswin.addstr(1, 1, " {:3d}".format(value))
          posgwin.move(1, 0)
          posgwin.insdelln(1)
          posgwin.addstr(1, 1, "M--------------------E")
          posgwin.addch(1, 2 + int(float(value) / 128 * 20), curses.ACS_BLOCK)
          do_update_param_outputs = True

        if key == 4: # hi-hat controller
          hi_hat_ctrl             = value
          do_update_param_outputs = True

  if midi_send_cmd >= 0:
    output_port.write_midi_event(0, (185, midi_send_cmd, midi_send_val))
    midi_previous_send_cmd = midi_send_cmd # store previous value
    midi_send_cmd          = -1 # invalidate current command to prepare for next command


# main function
with client:
  try:
    input_port.connect('ttymidi:MIDI_in')   # ESP32
    output_port.connect('ttymidi:MIDI_out') # ESP32
  except:
    try:
      teensy_out = jack.get_ports("Edrumulus ", is_midi=True, is_input=True)
      teensy_in  = jack.get_ports("Edrumulus ", is_midi=True, is_output=True)
      if teensy_in and teensy_out:
        input_port.connect(teensy_in[0])   # Teensy
        output_port.connect(teensy_out[0]) # Teensy
    except:
      pass # if no Edrumulus hardware was found, no jack is started

  # TODO load settings takes way too long...
  #load_settings()

  # initial pad selection for retrieving Edrumulus parameters for current selected pad
  midi_send_val = sel_pad
  midi_send_cmd = 108
  update_param_outputs()

  # loop until user presses q
  while (ch := mainwin.getch()) != ord('q'):
    if ch != -1:
      if ch == ord('s') or ch == ord('S'): # change selected pad
        cur_sel_pad   = sel_pad
        cur_sel_pad   = cur_sel_pad + 1 if ch == ord('s') else cur_sel_pad - 1
        sel_pad       = max(0, min(max_num_pads - 1, cur_sel_pad))
        midi_send_val = sel_pad
        midi_send_cmd = 108
      elif ch == ord('c') or ch == ord('C'): # change selected command
        cur_sel_cmd = sel_cmd
        cur_sel_cmd = cur_sel_cmd + 1 if ch == ord('c') else cur_sel_cmd - 1
        sel_cmd     = max(0, min(len(cmd_val) - 1, cur_sel_cmd))
      elif ch == 258 or ch == 259: # change parameter value with up/down keys
        cur_sel_val       = database[sel_cmd]
        cur_sel_val       = cur_sel_val + 1 if ch == 259 else cur_sel_val - 1
        database[sel_cmd] = max(0, min(cmd_val_rng[sel_cmd], cur_sel_val))
        midi_send_val     = database[sel_cmd]
        midi_send_cmd     = cmd_val[sel_cmd]
      elif ch == ord('a') or ch == ord('A'): # enable/disable auto pad selection
        auto_pad_sel = (ch == ord('a')) # capital 'A' disables auto pad selection
      elif ch == ord('r'):
        mainwin.addstr(row_start + 1, col_start, "DO YOU REALLY WANT TO RESET ALL EDRUMULUS PARAMETERS [y/n]?")
        mainwin.nodelay(False) # temporarily, use blocking getch()
        if mainwin.getch() == ord('y'):
          midi_send_cmd = 115 # midi_send_val will be ignored by Edrumulus for this command
        mainwin.nodelay(True) # go back to unblocking getch()
        mainwin.addstr(row_start + 1, col_start, "                                                           ")
      do_update_param_outputs = True

    if do_update_param_outputs:
      update_param_outputs()
      do_update_param_outputs = False
    time.sleep(0.01)

  # clean up and exit
  mainwin.box()
  mainwin.addstr(row_start + 5, col_start, "Storing settings...")
  mainwin.refresh()
  store_settings()
  mainwin.keypad(False)
  curses.echo()
  curses.endwin()

