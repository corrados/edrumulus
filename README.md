# Edrumulus

![Homepage picture](algorithm/images/edrumulus.png)

Open Source E-Drum Trigger Module Software


## Project specifications

- Research is done using a regular audio card, capture the drum pad output signal and develop
  the algorithms in Octave.

- Positional sensing shall be supported.

- Overall latency should be as small as possible. The goal is to get a latency < 10 ms.

- One goal would be to use a ESP32 microprocessor, similar to the [open e-drums](https://open-e-drums.com) project.
  It has shown that the ESP32 is powerful enough to fulfill the task of a drum trigger module.

  Many open drum trigger implementations only use one half of the signal (i.e., only the positive
  wave) or use a bridge rectifier to capture the analog signal. Since we want to implement more
  sophisticated algorithms, we want to capture the entire signal without non-linear analog
  preprocessing. Since microcontrollers usually only convert analog signals in the range of 0 to 3.3 V,
  we have to move the point of operation in the middle of the voltage range by using two equal resistors
  in series connection (one connected to the ground and one to the 3.3 V) and a capacitor to feed
  the piezo signal into the ADC of the microcontroller.


## Project log

- (12/20/2020) The positional sensing algorithm is now also ported to the ESP32 micro controller. I have made a
  new Youtube video using the current implementation (Git commit c796369): https://youtu.be/naP-ODXl9Y0

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


## Project TODO list

- The algorithm is optimized for Roland PD-120 pad only. Other pad types should be supported, too.

- The low velocity performance at the edge of the pad is not good enough. This should be improved.

- We sometime have double-triggers on hard hits or when the rim is hit. The mask time is already
  at 10 ms. So, the decay handling should be improved to suppress these double-triggers.

- The normalization of the positional sensing should be improved.

- Create a class for the edrumulus library.


## Algorithm

The algorithm is described in [this document](algorithm/README.md).

