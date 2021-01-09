# Edrumulus project log

- (12/22/2020) Just tested BLE MIDI (i.e. MIDI over bluetooth). I could successfully connect to GarageBand
  on an iPhone. Unfortunately, the bluetooth connection caused some interference in the audio input signal
  so that the threshold had to be increased and also we got a lot of false detections on low velocity hits
  at the edge of the pad. So for future hardware designs some shielding should be considered. Also, I started
  looking at the second piezo signal to support rim shot detection.

- (12/20/2020) The positional sensing algorithm is now also ported to the ESP32 micro controller. I have made a
  new Youtube video using the current implementation (Git commit c796369): https://youtu.be/naP-ODXl9Y0

- (12/18/2020) I have ported the Octave peak detection code to the ESP32 developer board (a DOIT ESP32 DEVKIT V1,
  no positional sensing yet) and connected it via my PC and Hairless MIDI to my Roland TD-20 module so that
  the snare sound was coming out of the TD-20. This time I could test the performance in real-time. The parameters
  were not yet optimized but still, the results were very promising. Without positional sensing, the ESP32 runs at
  about 56 kHz sampling rate when calculating the peak detection algorithm on one pad. Since I only need 8 kHz
  sampling rate (maybe even 4 kHz is sufficient), we have a lot of headroom for the positional sensing algorithm
  or to add rim shot support and support multiple pads.

- (12/13/2020) I am very pleased about the current algorithm performance. The algorithm is not yet fine-tuned but
  already performs pretty well. I have created a short Youtube video of the algorithm (Git commit c83743e) to show
  the current performance in action: https://youtu.be/6eQjCD-DFjo


# TODO list

- We sometime have double-triggers on hard hits or when the rim is hit. The mask time is already
  at 10 ms. So, the decay handling should be improved to suppress these double-triggers.

  There is also a problem if a press roll with low velocity is played at the edge of the mesh had.
  In this case we sometimes get incorrect detected hits. I just did a test with my TD-20 module by
  setting the velocity of the notes to a fixed value and played a low velocity press roll in the
  middle and at the edge of the mesh had. It seems in the middle of the mesh head it triggers very
  precise. But at the edge of the mesh head you can hear that some hits are not detected. So, it
  seems Roland has optimized their retrigger cancellation algorithm for reducing the detection
  probability of incorrect hits in favor of detecting very low level hits.

- If a rim shot is used, the positional sensing parameters must be adjusted to correctly estimate
  the position.

- The algorithm is optimized for Roland PD-120 pad only. Other pad types should be supported, too.

- The normalization of the positional sensing should be improved.
