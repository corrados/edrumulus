# Edrumulus Road Map

The road map items are sorted by priority.

- [ ] **Use rim switch theshold in dB**

  Do not use a linear parameter as it is implemented right now.

- [ ] **Improve rim shot detection**

  Especially for pads like the PDA-120L where the piezos are located near the edge, the rim shot detection does perform poorly.
  The rim shot detection for non-rim-switch pads is highly influenced on the overload of the ADC. This has to be investigated
  (i.e., that loud strikes on the middle of the mesh head triggers a rim shot because the clipped head signal is lower than
  the true peak and therefore we could get over the rim shot threshold since the rim trigger is not overloaded).

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

- [ ] **For the ESP32 prototype, adjust the ADC_noise_peak_velocity_scaling in edrumulus.h correctly**

- [ ] **Support positional sensing for rim shots**

- [ ] **Should we consider pre-scan time high peaks for velocity estimation?**

- [ ] **Hot spot suppression on mesh pads with center piezo**

  Striking directly on the piezo results in detected velocity values which are much too high.

- [ ] **Documentation (user manual)**

  Create an Edrumulus manual which describes the hardware/software setup, parameter description, HOWTOs, etc.

- [ ] **Algorihm documentation**

  The algorithm description should be improved. Especially, improve retrigger cancellation section.

