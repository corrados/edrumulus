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
import matplotlib
# ['GTK3Agg', 'GTK3Cairo', 'GTK4Agg', 'GTK4Cairo', 'MacOSX', 'nbAgg', 'QtAgg', 'QtCairo', 'Qt5Agg', 'Qt5Cairo', 'TkAgg', 
# 'TkCairo', 'WebAgg', 'WX', 'WXAgg', 'WXCairo', 'agg', 'cairo', 'pdf', 'pgf', 'ps', 'svg', 'template']

#matplotlib.use('QtAgg')
#from matplotlib import pyplot as plt
import matplotlib.pyplot as plt
import matplotlib.markers as mkr
import numpy as np
#%matplotlib notebook


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
midi_values            = [0] * 100


@client.set_process_callback
def process(frames):
  global key, value, new_data, midi_values
  for offset, data in port_in.incoming_midi_events():
    if len(data) == 3:
      if int.from_bytes(data[0], "big") & 0xF0 == 0x90:
        key   = int.from_bytes(data[1], "big")
        value = int.from_bytes(data[2], "big")
        #midi_values.put(value)

        midi_values.pop(0)
        midi_values.append(value)

        #new_data = True
        #while new_data:
        #  time.sleep(0.001)


with client:
  print('press Return to quit')
  port_in.connect('ttymidi:MIDI_in')
  port_out.connect('ttymidi:MIDI_out')

  #an = np.linspace(0, 2 * np.pi, 100)
  #plt.plot(3 * np.cos(an), 3 * np.sin(an))
  #plt.plot((1, 2), marker="o")
  #plt.scatter([1, 1], [1, -2])
  #plt.show(block=False)

  test = [1, 2, 3, 4]
  print(test)
  print(test.pop(0))
  print(test)
  test.append(22)
  print(test)

  for i in range(0, 50):
    print(midi_values)
    plt.ion()
    print(plt.isinteractive())
    print(plt.get_backend())
    plt.clf()
    plt.plot(midi_values)
    #plt.draw()
    plt.show()
    plt.pause(0.5)
#    time.sleep(1)

  #while True:
  #  if new_data:
  #    # TODO
  #    new_data = False
  #    #print(midi_values)
  #  time.sleep(0.01)

  # TEST
  #input()

