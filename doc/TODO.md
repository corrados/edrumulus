# Edrumulus Road Map

The road map items are sorted by priority.

- [ ] **NoteOn instead of aftertrouch**

  See https://github.com/corrados/edrumulus/discussions/85.

- [ ] **Create a super simple default drum kit in the git repo so that run_edrumulus.sh runs successfully without any other dependencies**

  See https://github.com/corrados/edrumulus/discussions/94#discussioncomment-6994715.

- [ ] **Add support for USB OTG MIDI for the ESP32-S3**

  It seems we have to wait for the Github espressif/arduino-esp32 project to support the newest ESP-IDF
  so that #include "tinyusb.h" does not give a compiler error. See esp-idf/examples/peripherals/usb/device/tusb_midi
  for an example code.

- [ ] **Introduce a first peak detection reliability**

  This can be used to improve the positional sensing. E.g., if the reliability is low, we could
  use the position of the last detected peak if it is close to the current peak in time (e.g., if
  we have a fast roll situation).

- [ ] **Support positional sensing for rim shots**

- [ ] **Support direct rim strike**

  See https://github.com/corrados/edrumulus/discussions/84.

- [ ] **Improve rim shot detection**

  Especially for pads like the PDA-120L where the piezos are located near the edge, the rim shot detection does perform poorly.

- [ ] **Add support for rim boost setting for multiple head sensor pads**

  The rim boost setting is only supported for pads with just one head sensor.

- [ ] **Testing support for ESP32-S3 with real front-end**

  The initial implementation is done and ADC data from multiple inputs can be processed. Touching the
  ESP32-S3 developer board gives a lot of MIDI note outputs which makes me believe that everything works
  as expected. But to make sure it really works, it has to be tested with a real front-end circuit and
  edrum pads connected which is still outstanding.

- [ ] **For the ESP32 prototype, adjust the ADC_noise_peak_velocity_scaling in edrumulus.h correctly**

- [ ] **Should we consider pre-scan time high peaks for velocity estimation?**

- [ ] **Hot spot suppression on mesh pads with center piezo**

  Striking directly on the piezo results in detected velocity values which are much too high.

- [ ] **Documentation (user manual)**

  Create an Edrumulus manual which describes the hardware/software setup, parameter description, HOWTOs, etc.

- [ ] **Algorihm documentation**

  The algorithm description should be improved. Especially, improve retrigger cancellation section.

