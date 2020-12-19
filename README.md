# Edrumulus

![Homepage picture](algorithm/images/edrumulus.png)

Open Source E-Drum Trigger Module Software


## Project TODO list

- The algorithm is optimized for Roland PD-120 pad only. Other pad types should be supported, too.

- The low velocity performance at the edge of the pad is not good enough. This should be improved.

- We sometime have double-triggers on hard hits or when the rim is hit. The mask time is already
  at 10 ms. So, the decay handling should be improved to suppress these double-triggers.

- The normalization of the positional sensing should be improved.

- Create a class for the edrumulus library.


## Project log

- (12/18/2020) I have ported the Octave peak detection code to the ESP32 developer board (no positional sensing
  yet) and connected it via my PC and Hairless MIDI to my Roland TD-20 module so that the snare sound was
  coming out of the TD-20. This time I could test the performance in real-time. The parameters were not yet
  optimized but still, the results were very promising. Without positional sensing, the ESP32 runs at about
  56 kHz sampling rate when calculating the peak detection algorithm on one pad. Since I only need 8 kHz
  sampling rate (maybe even 4 kHz is sufficient), we have a lot of headroom for the positional sensing algorithm
  or to add rim shot support and support multiple pads.

- (12/13/2020) I am very pleased about the current algorithm performance. The algorithm is not yet fine-tuned but
  already performs pretty well. I have created a short Youtube video of the algorithm (Git commit c83743e) to show
  the current performance in action: https://youtu.be/6eQjCD-DFjo


## Project specifications

- Research is done using a regular audio card, capture the drum pad output signal and develop
  the algorithms in Octave.

- Positional sensing shall be supported.

- Overall latency should be as small as possible. The goal is to get a latency < 10 ms.

- One goal would be to use a ESP32 microprocessor, similar to the [open e-drums](https://open-e-drums.com) project.
  It has shown that the ESP32 is powerful enough to fulfill the task of a drum trigger module.

  As an alternative a Raspberry Pi Zero could be used as a trigger module. It gets a sampled
  audio signal from the GIOP (some external hardware needed) and processes it using a C++
  software. It outputs a MIDI signal. Since the Raspberry Pi Zero has only a slow processor,
  it will not be possible to include the complete drum module.


## Commercial module latency

According to https://www.vdrums.com/forum/general/the-lounge/1182869-fastest-lowest-latency-drum-module-available and http://onyx3.com/EDLM, the drum modules have the following measured latencies:

- Roland TD-50\30: **3 ms**    (measured by Chris K)
- Roland TD11\15:  **3 ms**    (measured by Chris K)
- Roland TD15:     **3.15 ms** (measured by 30YearsLater)
- Roland TD12:     **3.60 ms** (measured by 30YearsLater)
- Roland TD-20:    **5.7 ms**  (measured by Chris K)
- Roland TD-4:     **3.8 ms**  (measured by onyx3.com)
- Roland TD-17:    **3.6 ms**  (measured by onyx3.com)
- MIMIC:           **4 ms**    (measured by Chris K)

I measured my Roland TDW-20 module by using a stereo splitter cable and connected the trigger output
of my PD-120 pad directly to one channel of my sound card input and also to the trigger input of the
TDW-20 module. Then I connected the analog audio output of the TDW-20 to the other stereo channel of
my sound card and recorded the signal with Audacity. I now measured the latency between the main
peak of the pad trigger to the first peak of the synthesized signal from the TDW-20. As seen in the
screen shot, there is a latency of about **7 ms**:
![Roland TDW-20 drum module measured latency](algorithm/images/roland_td20_latency.jpg)


## Algorithm

The algorithm is described in [this document](algorithm/README.md).
