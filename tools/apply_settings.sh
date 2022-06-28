#!/bin/bash

echo "This script applies preset settings to Edrumulus"


# get environment --------------------------------------------------------------
# check of Teensy USB MIDI
if aconnect -l|grep -q Edrumulus; then
  echo "-> Edrumulus with Teensy hardware detected"
  is_teensy=true
fi


# send settings to Edrumulus ---------------------------------------------------
if [[ -v is_teensy ]]; then

  # diable spike cancellation for Teensy 4.0 prototype
  #SendMIDI/Builds/LinuxMakefile/build/sendmidi dev "Edrumulus" ch 10 cc 110 0

  # snare
  SendMIDI/Builds/LinuxMakefile/build/sendmidi dev "Edrumulus" ch 10 cc 108 0 # pad 0
  #SendMIDI/Builds/LinuxMakefile/build/sendmidi dev "Edrumulus" ch 10 cc 102 2 # PD8
  #SendMIDI/Builds/LinuxMakefile/build/sendmidi dev "Edrumulus" ch 10 cc 103 1 # threshold
  #SendMIDI/Builds/LinuxMakefile/build/sendmidi dev "Edrumulus" ch 10 cc 104 8 # sensitivity
  #SendMIDI/Builds/LinuxMakefile/build/sendmidi dev "Edrumulus" ch 10 cc 107 16 # rim shot threshold
  #SendMIDI/Builds/LinuxMakefile/build/sendmidi dev "Edrumulus" ch 10 cc 105 26 # positional sensing threshold
  #SendMIDI/Builds/LinuxMakefile/build/sendmidi dev "Edrumulus" ch 10 cc 106 11 # positional sensing sensitivity
  #SendMIDI/Builds/LinuxMakefile/build/sendmidi dev "Edrumulus" ch 10 cc 111 3 # both, rim shot and positional sensing

else

  # prepare serial output interface for sending MIDI configuration messages
  stty 38400 -F /dev/ttyUSB0

  # general settings
  echo -n -e '\xB9\x6E\x04' > /dev/ttyUSB0 # spike cancellation: 4

  # snare
  echo -n -e '\xB9\x6C\x00' > /dev/ttyUSB0 # select pad:       0
  echo -n -e '\xB9\x66\x0F' > /dev/ttyUSB0 # pad type:         PDX8
  echo -n -e '\xB9\x6D\x00' > /dev/ttyUSB0 # MIDI curve type:  LINEAR
  echo -n -e '\xB9\x67\x06' > /dev/ttyUSB0 # threshold:        6
  echo -n -e '\xB9\x68\x04' > /dev/ttyUSB0 # sensitivity:      4
  echo -n -e '\xB9\x6B\x0E' > /dev/ttyUSB0 # rim threshold:    14
  echo -n -e '\xB9\x69\x15' > /dev/ttyUSB0 # pos threshold:    21
  echo -n -e '\xB9\x6A\x1B' > /dev/ttyUSB0 # pos sensitivity:  27
  echo -n -e '\xB9\x6F\x03' > /dev/ttyUSB0 # rim/pos:          3 (both)
  echo -n -e '\xB9\x70\x26' > /dev/ttyUSB0 # midi note:        38
  echo -n -e '\xB9\x71\x28' > /dev/ttyUSB0 # midi note rim:    40
  echo -n -e '\xB9\x72\x05' > /dev/ttyUSB0 # crosstalk cancel: 5

  # kick
  echo -n -e '\xB9\x6C\x01' > /dev/ttyUSB0 # select pad:       1
  echo -n -e '\xB9\x66\x06' > /dev/ttyUSB0 # pad type:         KD7
  echo -n -e '\xB9\x6D\x00' > /dev/ttyUSB0 # MIDI curve type:  LINEAR
  echo -n -e '\xB9\x67\x0E' > /dev/ttyUSB0 # threshold:        14
  echo -n -e '\xB9\x68\x07' > /dev/ttyUSB0 # sensitivity:      7
  echo -n -e '\xB9\x6F\x00' > /dev/ttyUSB0 # rim/pos:          0 (none)
  echo -n -e '\xB9\x70\x24' > /dev/ttyUSB0 # midi note:        36
  echo -n -e '\xB9\x72\x00' > /dev/ttyUSB0 # crosstalk cancel: 0

  # Hi-Hat
  echo -n -e '\xB9\x6C\x02' > /dev/ttyUSB0 # select pad:       2
  echo -n -e '\xB9\x66\x0B' > /dev/ttyUSB0 # pad type:         CY5
  echo -n -e '\xB9\x6D\x00' > /dev/ttyUSB0 # MIDI curve type:  LINEAR
  echo -n -e '\xB9\x67\x06' > /dev/ttyUSB0 # threshold:        6
  echo -n -e '\xB9\x68\x04' > /dev/ttyUSB0 # sensitivity:      4
  echo -n -e '\xB9\x6F\x00' > /dev/ttyUSB0 # rim/pos:          0 (none)
  echo -n -e '\xB9\x70\x16' > /dev/ttyUSB0 # midi note:        22
  echo -n -e '\xB9\x72\x08' > /dev/ttyUSB0 # crosstalk cancel: 8

  # Hi-Hat control
  echo -n -e '\xB9\x6C\x03' > /dev/ttyUSB0 # select pad:       3
  echo -n -e '\xB9\x66\x03' > /dev/ttyUSB0 # pad type:         FD8
  echo -n -e '\xB9\x67\x0C' > /dev/ttyUSB0 # threshold:        12
  echo -n -e '\xB9\x68\x1A' > /dev/ttyUSB0 # sensitivity:      26
  echo -n -e '\xB9\x70\x2C' > /dev/ttyUSB0 # midi note:        44

  # crash
  echo -n -e '\xB9\x6C\x04' > /dev/ttyUSB0 # select pad:       4
  echo -n -e '\xB9\x66\x0B' > /dev/ttyUSB0 # pad type:         CY5
  echo -n -e '\xB9\x6D\x00' > /dev/ttyUSB0 # MIDI curve type:  LINEAR
  echo -n -e '\xB9\x67\x06' > /dev/ttyUSB0 # threshold:        6
  echo -n -e '\xB9\x68\x04' > /dev/ttyUSB0 # sensitivity:      4
  echo -n -e '\xB9\x6F\x00' > /dev/ttyUSB0 # rim/pos:          0 (none)
  echo -n -e '\xB9\x70\x31' > /dev/ttyUSB0 # midi note:        49
  echo -n -e '\xB9\x72\x0E' > /dev/ttyUSB0 # crosstalk cancel: 14

  # tom 1
  echo -n -e '\xB9\x6C\x05' > /dev/ttyUSB0 # select pad:       5
  echo -n -e '\xB9\x66\x0C' > /dev/ttyUSB0 # pad type:         HD1TOM
  echo -n -e '\xB9\x6D\x00' > /dev/ttyUSB0 # MIDI curve type:  LINEAR
  echo -n -e '\xB9\x67\x08' > /dev/ttyUSB0 # threshold:        8
  echo -n -e '\xB9\x68\x05' > /dev/ttyUSB0 # sensitivity:      5
  echo -n -e '\xB9\x6F\x00' > /dev/ttyUSB0 # rim/pos:          0 (none)
  echo -n -e '\xB9\x70\x30' > /dev/ttyUSB0 # midi note:        48
  echo -n -e '\xB9\x72\x04' > /dev/ttyUSB0 # crosstalk cancel: 4

  # ride
  echo -n -e '\xB9\x6C\x06' > /dev/ttyUSB0 # select pad:       6
  echo -n -e '\xB9\x66\x0B' > /dev/ttyUSB0 # pad type:         CY5
  echo -n -e '\xB9\x6D\x00' > /dev/ttyUSB0 # MIDI curve type:  LINEAR
  echo -n -e '\xB9\x67\x06' > /dev/ttyUSB0 # threshold:        6
  echo -n -e '\xB9\x68\x04' > /dev/ttyUSB0 # sensitivity:      4
  echo -n -e '\xB9\x6F\x00' > /dev/ttyUSB0 # rim/pos:          0 (none)
  echo -n -e '\xB9\x70\x33' > /dev/ttyUSB0 # midi note:        51
  echo -n -e '\xB9\x72\x08' > /dev/ttyUSB0 # crosstalk cancel: 8

  # tom 2
  echo -n -e '\xB9\x6C\x07' > /dev/ttyUSB0 # select pad:       7
  echo -n -e '\xB9\x66\x0C' > /dev/ttyUSB0 # pad type:         HD1TOM
  echo -n -e '\xB9\x6D\x00' > /dev/ttyUSB0 # MIDI curve type:  LINEAR
  echo -n -e '\xB9\x67\x08' > /dev/ttyUSB0 # threshold:        8
  echo -n -e '\xB9\x68\x05' > /dev/ttyUSB0 # sensitivity:      5
  echo -n -e '\xB9\x6F\x00' > /dev/ttyUSB0 # rim/pos:          0 (none)
  echo -n -e '\xB9\x70\x2D' > /dev/ttyUSB0 # midi note:        45
  echo -n -e '\xB9\x72\x04' > /dev/ttyUSB0 # crosstalk cancel: 4

  # tom 3
  echo -n -e '\xB9\x6C\x08' > /dev/ttyUSB0 # select pad:       8
  echo -n -e '\xB9\x66\x0C' > /dev/ttyUSB0 # pad type:         HD1TOM
  echo -n -e '\xB9\x6D\x00' > /dev/ttyUSB0 # MIDI curve type:  LINEAR
  echo -n -e '\xB9\x67\x08' > /dev/ttyUSB0 # threshold:        8
  echo -n -e '\xB9\x68\x05' > /dev/ttyUSB0 # sensitivity:      5
  echo -n -e '\xB9\x6F\x00' > /dev/ttyUSB0 # rim/pos:          0 (none)
  echo -n -e '\xB9\x70\x2B' > /dev/ttyUSB0 # midi note:        43
  echo -n -e '\xB9\x72\x04' > /dev/ttyUSB0 # crosstalk cancel: 4

fi

