# Edrumulus User Manual

Edrumulus is a high quality open source e-drum trigger module software. To use Edrumulus, you need to have

- a supported micro controller deverloper board (see the README file for supported micro controllers),
- an analog front end circuit to connect the trigger pads to the micro controller (soldering skills are needed),
- the [Arduino IDE](https://www.arduino.cc/en/software) for flashing the micro controller firmware,
- some other software tools like [Hairless MIDI](https://projectgus.github.io/hairless-midiserial),
  [loopMIDI](https://www.tobias-erichsen.de/software/loopmidi.html), etc., for using the Edrumulus MIDI output signal,
- [Octave](https://www.gnu.org/software/octave) software to control Edrumulus trigger parameter in real-time.


## Analog front end circuit

If Edrumulus is installed on the micro controller and there is no analog front end circuit attached,
you will get a lot of false triggering (i.e. a lot of MIDI notes). For the Edrumulus system to work correctly,
all configured ADC inputs must have the analog front end circuit attached.

There is some debugging available in the Edrumulus firmware to check the analog front end. E.g., you can
undefine USE_MIDI and enable the code in Edrumulus::process() which is called "TEST check DC offset values".
If you flash that version of the Edrumulus firmware and open the Arduino serial plotter, you can see the
DC offset of all configured ADC inputs. They must all be at around half the available value range of the ADC.
This would be around 2048 for a 12 bit ADC. Make sure to revert your changes if you want to run Edrumulus
in normal mode again.


## Setup for ESP32 under Windows

Install the Arduino IDE and add the ESP32 Board Manager according to
[this linked description](https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html).
Now open the edrumulus.ino file in the Arduino IDE and customize the settings:

- number_pads defines the number of pad inputs you want to use.
- analog_pins and analog_pins_rimshot define the GPIO ports you use for your pad inputs. If you have
  a single piezo pad (mono pad), set the corresponding GPIO value to -1 to disable the second input
  for that pad.

Start the loopMIDI and Hairless MIDI tools. Configure the Hairless MIDI "MIDI Out" combo box to
loopMIDI to be able to use Edrumulus with your DAW.

