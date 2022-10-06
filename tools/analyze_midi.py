#!/usr/bin/env python3

#*******************************************************************************
# Copyright (c) 2022-2022
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

import time
import jack
import pygame


# initializations
client                 = jack.Client('edrumulus_analyze_midi')
port_in                = client.midi_inports.register('input')
port_out               = client.midi_outports.register('output')
midi_send_cmd          = -1 # invalidate per default
midi_previous_send_cmd = -1
midi_send_val          = 0
new_data               = False
key                    = 0
value                  = 0


@client.set_process_callback
def process(frames):
  global key, value, new_data
  for offset, data in port_in.incoming_midi_events():
    if len(data) == 3:
      if int.from_bytes(data[0], "big") & 0xF0 == 0x90:
        key   = int.from_bytes(data[1], "big")
        value = int.from_bytes(data[2], "big")
        new_data = True
        while new_data:
          time.sleep(0.001)


with client:
  print('press Return to quit')
  port_in.connect('ttymidi:MIDI_in')
  port_out.connect('ttymidi:MIDI_out')
  
  pygame.init()
  screen = pygame.display.set_mode((640, 480))
  pygame.display.set_caption("Edrumulus MIDI Analyzer")
  screen.fill((255, 255, 255))

  pygame.draw.line(screen, (0, 0, 0), (10, 10),  (10, 400))
  pygame.draw.line(screen, (0, 0, 0), (10, 400), (600, 400))
  pygame.display.flip()

  while True:
    if new_data:
      pygame.draw.circle(screen, (0, 0, 255), (100, 200), value * 10, 1)
      pygame.display.flip() 
      new_data = False
    time.sleep(0.01)

