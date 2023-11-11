# Edrumulus User Manual

Edrumulus is a high quality open source e-drum trigger module software. To use Edrumulus, you need to have:

- A supported micro controller developer board (see the README file for supported micro controllers)
- An analog front end circuit to connect the trigger pads to the micro controller (soldering skills are needed)
- The [Arduino](https://www.arduino.cc/en/software) or [PlatformIO](https://platformio.org/) IDE for flashing the micro controller firmware
- Some other software tools like [Hairless MIDI](https://projectgus.github.io/hairless-midiserial) and
  [loopMIDI](https://www.tobias-erichsen.de/software/loopmidi.html), etc., for using the Edrumulus MIDI output signal
- [Python](https://www.python.org) to control Edrumulus trigger parameters in real-time.
  - Install the Python PIP packages python-rtmidi and windows-curses.

## Analog front end circuit

If Edrumulus is installed on the micro controller and there is no analog front end circuit attached,
you will get a lot of false triggering (i.e. a lot of MIDI notes). For the Edrumulus system to work correctly,
all configured ADC inputs must have the analog front end circuit attached.

There is some debugging available in the Edrumulus firmware to check the analog front end. E.g., you can
undefine USE_MIDI and enable the code in `Edrumulus::process()` which is called "TEST check DC offset values".
If you flash that version of the Edrumulus firmware and open the Arduino serial plotter, you can see the
DC offset of all configured ADC inputs. They must all be at around half the available value range of the ADC.
This would be around 2048 for a 12 bit ADC. Make sure to revert your changes if you want to run Edrumulus
in normal mode again.


## Setup for ESP32 under Windows

Install the Arduino IDE and add the ESP32 Board Manager according to
[this linked description](https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html).
Now open the edrumulus.ino file in the Arduino IDE and compile and upload.

### Use Edrumulus in your DAW under Windows

- Start loopMIDI and create two loopback MIDI-ports:
  - `EdrumulusIn`
  - `EdrumulusOut`
- Start Hairless MIDI and set:
  - MIDI Out to `EdrumulusIn`
  - MIDI In to `EdrumulusOut`
- Start the Edrumulus user interface with:
  - `python edrumulus/tools/edrumulus_gui.py rtmidi`

Example:

![Screenshot of a typical Windows setup](/doc/images/win_setup.png)

## How to support new pieces of hardware

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

