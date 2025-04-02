# Edrumulus User Manual

Edrumulus is a high quality open source e-drum trigger module software. To use Edrumulus, you need to have:

- a supported micro controller developer board (see the README file for supported micro controllers),
- an analog front end circuit to connect the trigger pads to the micro controller (either a bread board is
  used or soldering skills are needed),
- the [Arduino](https://www.arduino.cc/en/software) or [PlatformIO](https://platformio.org/) IDE for flashing the micro controller firmware,
- [Python](https://www.python.org) to control Edrumulus trigger parameters in real-time.
  - Install the Python PIP packages python-rtmidi and windows-curses.

## Analog front end circuit

If Edrumulus is installed on the micro controller and there is no analog front end circuit attached,
you will get a lot of false triggering (i.e. a lot of MIDI notes). For the Edrumulus system to work correctly,
all configured ADC inputs must have the analog front end circuit attached.

If an analog front end for a configured input is missing or faulty, Edrumulus will report a DC offset error.

If you need higher sensitivity for low and medium velocity hits, you can try the MPS-750X circuit from below.

## Setup for ESP32

Install the Arduino IDE and add the ESP32 Board Manager according to
[this linked description](https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html).
Now open the edrumulus.ino file in the Arduino IDE and compile and upload.

## Use Edrumulus in your DAW

Go into edrumulus/tools directory and start edrumulus_gui.py. This tool will connect to Edrumulus and
creates (virtual) MIDI ports which can be selected in your DAW as a MIDI device. This tool works on
Windows, Linux and MacOS.

E.g., on Linux simply type `python3 edrumulus_gui.py`. If you have an ESP32-S3, this usually is on a
different serial port than the normal ESP32. E.g., for an ESP32-S3 on Windows, the following command
should work: `python edrumulus_gui.py serial COM4`.

## Hardware-specific comments

### How to support new pieces of hardware

1. Start with prototype 2 and optimize the analog circuit. Random testing or a simulation with software like LTSpice could be used.
3. Analyze the samples in the Octave model to identify the edrumulus parameters:
   - Attach the pad to a normal sound card and record the strikes with 8 kHz sampling rate and according to https://github.com/corrados/edrumulus/blob/main/doc/algorithm.md#test-signals.
   - Add the recording to https://github.com/corrados/edrumulus/tree/main/algorithm/signals.
   - Create a new type in https://github.com/corrados/edrumulus/blob/main/algorithm/signalsandsettings.m and use an
     initial parameter set from a similar pad as a starting point.
   - Call the script signalsandsettings.m in Octave which automatically uses the script drumtrigger.m.
   - Adjust the parameters so that the decay curve looks ok and try to get as little false triggers as possible.
   - The updated parameters are then used in https://github.com/corrados/edrumulus/blob/main/edrumulus_parameters.cpp.
   - To get it fully working, the new type has to be added at multiple places, e.g. in the GUI python script, too.
4. Test the parameters in practice. Modify the available real-time parameters for hopefully better results. Possibly go back to previous steps.

### Millenium MPS-750X

- Recommended circuit: [Rs=10k, Rp=10k, Cp=0](https://github.com/corrados/edrumulus/discussions/98) with low forward voltage diodes (e.g. Shottky BAT85) and Rs=100k for the kick drum.
  - Rs=10k versus Rs=100k increases the sensitivity for low and medium velocity hits at the expense of higher shorting risk for harder hits. Shorting too many times may eventually break your ADC and should therefore not be done for high duty pads with low sensitivity needs such as the kick drum. Values between Rs=10k and Rs=100k may be chosen as a compromise.
- TRS pins:
  - Toms: hit piezo = T, rim piezo = R, GND = S.
  - Cymbals including Hi-Hat: Bow piezo = S, Rim switch = R (none for Hi-Hat), GND = T.
- Ride and crash cymbals draw some current.
- The ride rim switch differs from the ride bell switch by a 5k resistor.
- Set all MPS750X potis to maximum output with a screw driver (sometimes the direction appears to be indicated incorrectly).
