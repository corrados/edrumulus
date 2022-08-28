#!/usr/bin/env python3

import jack
import binascii

client       = jack.Client('edrumulus_front_panel')
port_in      = client.midi_inports.register('input')
port_out     = client.midi_outports.register('output')
port_through = client.midi_outports.register('through')

@client.set_process_callback
def process(frames):
  port_out.clear_buffer()
  port_through.clear_buffer()
  for offset, data in port_in.incoming_midi_events():
    port_through.write_midi_event(offset, data) # pass through

    # for debugging
    print('{}: 0x{}'.format(client.last_frame_time + offset,
                                binascii.hexlify(data).decode()))

with client:
  print('press Return to quit')
  port_in.connect('ttymidi:MIDI_in')
  port_out.connect('ttymidi:MIDI_out')
  port_out.write_midi_event(0, (185, 108, 0))
  input()

