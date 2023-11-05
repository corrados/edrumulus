# Edrumulus Road Map

The road map items are sorted by priority.


- [ ] **Support more control pedal settings in the GUI**

  Right now, the control_threshold is coupled with pad_settings.velocity_threshold and control_range is coupled with
  pad_settings.velocity_sensitivity. We have three parameters which are not yet controllable with the GUI: ctrl_history_len,
  ctrl_velocity_range_fact, ctrl_velocity_threshold.

  Since positional sensing does not make sense for the pedal, I'll soon implement the following:
  - couple control_threshold with pad_settings.pos_threshold
  - couple control_range with pad_settings.pos_sensitivity
  - couple ctrl_velocity_threshold with pad_settings.velocity_threshold
  - couple ctrl_velocity_range_fact with pad_settings.velocity_sensitivity

  This will be a little break of compatibility since now the fundamental pedal parameters are controlled with the
  "positional" GUI parameters. But this makes sense since we control how the position of the pedal is converted to
  MIDI signals. The velocity threshold/sensitivity now configures the pedal stomp sound which makes sense to total
  sense to me.

  The remaining parameter ctrl_history_len may be coupled with the GUI cancellation setting. I would like to re-use
  existing parameters since I want to avoid adding an additional GUI setting for this.

- [ ] **Support positional sensing for rim shots**

  A test signal *pd85rimshotpossense.wav* is available in the signal directory. Unfortunately, the
  current positional sensing algorithm does not work correctly. A deeper inspection of the first
  peaks of a strike should be done to find the source of the issue.

- [ ] **Cross talk cancellation doesn't work reliably (Github Issue [#109](https://github.com/corrados/edrumulus/issues/109))**

  User 3hhh observed that cross talk spikes may actually arrive before the spike of the pad he actually hit.

- [ ] **Introduce a first peak detection reliability**

  This can be used to improve the positional sensing. E.g., if the reliability is low, we could
  use the position of the last detected peak if it is close to the current peak in time (e.g., if
  we have a fast roll situation).

- [ ] **Support direct rim strike**

  See https://github.com/corrados/edrumulus/discussions/84.

- [ ] **Create a super simple default drum kit in the git repo so that run_edrumulus.sh runs successfully without any other dependencies**

  See https://github.com/corrados/edrumulus/discussions/94#discussioncomment-6994715.

- [ ] **Add support for USB OTG MIDI for the ESP32-S3**

  It seems we have to wait for the Github espressif/arduino-esp32 project to support the newest ESP-IDF
  so that #include "tinyusb.h" does not give a compiler error. See esp-idf/examples/peripherals/usb/device/tusb_midi
  for an example code.

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

