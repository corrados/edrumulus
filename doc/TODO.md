


- improve rim shot detection (especially for pads like the PDA-120L where the piezos are located
  near the edge, the rim shot detection does perform poorly)

- use rim switch theshold in dB (not linear as it is implemented right now)

- Signal overload correction
  -> maybe based on low-pass filtering as suggested in https://github.com/corrados/edrumulus/discussions/17#discussion-3736264

- Attach second piezo on mesh pad and experiment with improved velocity/position detection using the second piezo signal
  -> maybe use PDA-120L pad since it already has three pizeo sensors installed
  -> 1. apply current positional sensing on each of the piezos and triangulate the position (easiest solution)
  -> 2. try out if a velocity-based algorithm gives any reasonable results
  -> 3. estimate the time difference between first peaks (at 8 kHz sampling rate the sample distance would be
        equivalent to approx. 8 cm which is not usable, some interpolation must be supported or the sampling rate
        must be enlarged)

- Improve Edrumulus/Drumgizmo usage on Raspberry Pi (Pi4, Pi 3+ and Pi Zero)
  -> maybe use ecasound to mix all the Drumgizmo audio channels in real-time to change the sound of the
     kit instantly without loading new samples in Drumgizmo

- support positional sensing for rim shots

- Hot spot suppression on mesh pads with center piezo
  -> striking directly on the piezo results in detected velocity values which are much too high

- fix ghost strikes: probably caused by electrical interferers or noise in the microcontroller developer
  board itself -> either find a software solution or changes in the analog front end are needed

- fix ### MARKER: ESP32 issue with read/write settings ###
  -> ESP32 storage seems to be unreliable (which is maybe caused by the way we are dealing with the threads), i.e.,
     some parameters are changed from time to time to a large value

- support hardware keyboard/display like Arduino LCD1602 Display Keypad Shield

- Improve dynamics for Drumgizmo (for official drum kits as well as self-recorded kit)
  MIDI file 1:10:127 -> reference -> record audio -> DGEdit -> check the estimated powers
  -> maybe Bent can help out here

- Documentation: Edrumulus manual which describes the hardware/software setup, parameter description, HOWTOs, etc.

- Should we consider pre-scan time high peaks for velocity estimation?

- simulate 16 bit with current hardware and check if thresholds are correct

- for the ESP32 prototype, adjust the ADC_noise_peak_velocity_scaling in edrumulus.h correctly

- possible memory issue on the ESP32, see 406c31fd66aeb1609f75e22672d5fa9280749f37

- introduce defines for debugging functionality instead of /* */

- Documentation: Algorithm description
  -> improve retrigger cancellation section

- Create Qt application for controlling Edrumulus via MIDI using the rtmidi library



