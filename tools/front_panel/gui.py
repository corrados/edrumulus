#!/usr/bin/env python3

import jack
import binascii

client   = jack.Client('edrumulus_front_panel')
port_in  = client.midi_inports.register('input')
port_out = client.midi_outports.register('output')

@client.set_process_callback
def process(frames):
    for offset, data in port_in.incoming_midi_events():
        print('{}: 0x{}'.format(client.last_frame_time + offset,
                                binascii.hexlify(data).decode()))

with client:
    print('press Return to quit')
    port_in.connect('ttymidi:MIDI_in')
    port_out.connect('ttymidi:MIDI_out')
    port_out.write_midi_event(0, [185, 108, 0])
    input()

