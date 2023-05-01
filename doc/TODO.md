# Edrumulus Road Map

The road map items are sorted by priority.

- [ ] **Rubber pad curve**

  The curve type LINEAR was calibrated against the Roland module using a mesh head pad like the PD-80R.
  Using a rubber pad like the PD-8, this seems to need a different curve type, the LOG2 curve to match
  the MIDI notes at same strike velocity of the Roland module (where the reference pad is the Roland
  digital snare).<br/>
  The question is why we need a different curve for the rubber pads.

- [ ] **Improve overload compensation algorithm**

  The current overload compensation algorithms works on fix steps which leads to the fact that in the high velocity
  MIDI ranges there can only be certain values achieved (quantized). It would be better to have a continuous increase
  of MIDI velocity values even in case of an overload.

- [ ] **Add support for USB OTG MIDI for the ESP32-S3**

  It seems we have to wait for the Github espressif/arduino-esp32 project to support the newest ESP-IDF
  so that #include "tinyusb.h" does not give a compiler error. See esp-idf/examples/peripherals/usb/device/tusb_midi
  for an example code.

- [ ] **Support 3-zone pads like the Roland CY-15R**

  See [this discussion](https://github.com/corrados/edrumulus/discussions/73#discussioncomment-5623391).

- [ ] **Improve rim shot detection**

  Especially for pads like the PDA-120L where the piezos are located near the edge, the rim shot detection does perform poorly.

- [ ] **Testing support for ESP32-S3 with real front-end**

  The initial implementation is done and ADC data from multiple inputs can be processed. Touching the
  ESP32-S3 developer board gives a lot of MIDI note outputs which makes me believe that everything works
  as expected. But to make sure it really works, it has to be tested with a real front-end circuit and
  edrum pads connected which is still outstanding.

- [ ] **Improve dynamics for Drumgizmo**

  Could be done for official drum kits as well as self-recorded kit.

  To debug the dynamics:
  - Use MIDI file 1:10:127 as a reference.
  - Record audio.
  - Use DGEdit to estimate the powers.
  - Check the estimated powers.

  Maybe Bent can help out here.

- [ ] **Introduce a first peak detection reliability**

  This can be used to improve the positional sensing. E.g., if the reliability is low, we could
  use the position of the last detected peak if it is close to the current peak in time (e.g., if
  we have a fast roll situation).

- [ ] **Use rim switch theshold in dB**

  Do not use a linear parameter as it is implemented right now.

- [ ] **For the ESP32 prototype, adjust the ADC_noise_peak_velocity_scaling in edrumulus.h correctly**

- [ ] **Support positional sensing for rim shots**

- [ ] **Should we consider pre-scan time high peaks for velocity estimation?**

- [ ] **Hot spot suppression on mesh pads with center piezo**

  Striking directly on the piezo results in detected velocity values which are much too high.

- [ ] **Documentation (user manual)**

  Create an Edrumulus manual which describes the hardware/software setup, parameter description, HOWTOs, etc.

- [ ] **Algorihm documentation**

  The algorithm description should be improved. Especially, improve retrigger cancellation section.

