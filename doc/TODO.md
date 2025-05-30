# Edrumulus Road Map

The road map items are sorted by priority.

- [ ] **Introduce a first peak detection reliability**

  This can be used to improve the positional sensing. E.g., if the reliability is low, we could
  use the position of the last detected peak if it is close to the current peak in time (e.g., if
  we have a fast roll situation). But the last detected peak is maybe not a good idea to use since
  we usually have two sticks and the position of the last peak is most probably from the other
  stick which position is most probably away from the correct position of the current peak.

- [ ] **Support direct rim strike**

  See https://github.com/corrados/edrumulus/discussions/84.

- [ ] **Hot spot suppression on mesh pads with center piezo**

  Striking directly on the piezo results in detected velocity values which are much too high.

- [ ] **Change rim shot/positinal sensing counters**

  Taken from the code:
    - only use one counter instead of rim_shot_cnt and pos_sense_cnt
    - as long as counter is not finished, do check "hil_filt_new > threshold" again to see if we have a higher peak in that
      time window -> if yes, restart everything using the new detected peak

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

- [ ] **Cross talk cancellation doesn't work reliably (Github Issue [#109](https://github.com/corrados/edrumulus/issues/109))**

  User 3hhh observed that cross talk spikes may actually arrive before the spike of the pad he actually hit.
  -> Can be fixed with the xtalk programm created by 3hhh (see Git submodule in tools directory).

- [ ] **For the ESP32 prototype, adjust the ADC_noise_peak_velocity_scaling in edrumulus.h correctly**

- [ ] **Should we consider pre-scan time high peaks for velocity estimation?**

- [ ] **Documentation (user manual)**

  Create an Edrumulus manual which describes the hardware/software setup, parameter description, HOWTOs, etc.

- [ ] **Algorihm documentation**

  The algorithm description should be improved. Especially, improve retrigger cancellation section.

