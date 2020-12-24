# Edrumulus

![Homepage picture](algorithm/images/edrumulus.png)

The aim of the Edrumulus project is to create a high quality Open Source e-drum trigger module software.


## Project specifications

- Overall latency should be as small as possible. The goal is to get a latency < 10 ms.

- Positional sensing shall be supported.

- One goal would be to use a ESP32 microprocessor, similar to the [open e-drums](https://open-e-drums.com)
  project. It has shown that the ESP32 is powerful enough to fulfill the task of a drum trigger module.

  Many open drum trigger implementations only use one half of the signal (i.e., only the positive
  wave) or use a bridge rectifier to capture the analog signal. Since we want to implement more
  sophisticated algorithms, we want to capture the entire signal without non-linear analog
  preprocessing. Since microcontrollers usually only convert analog signals in the range of 0 to 3.3 V,
  we have to move the point of operation in the middle of the voltage range.

  A circuit diagram of my test setup is given in the following picture:
  <br/><img src="algorithm/images/edrumulus_testing.png" width="400">


## Algorithm development

The algorithm research is done using a regular audio card. The drum pad output signal is captured and
the signal processing and algorithm development is then done in Octave. The development is done in
three steps:

1. Create the algorithms using a captured test signal and analyze it as a whole. This is the
   easiest and fastest way to create and improve the algorithms (this is the "playground"). The
   corresponding source file is algorithm/drumtrigger.m.

2. If the algorithm works ok, it is ported to a sample-based processing. The goal is to make the
   Octave code as similar to the C++ micro controller implementation as possible. The corresponding
   source file is algorithm/edrumulus.m.

3. Port the sample-based processing code to C++ and test it in real-time on the hardware. To make
   sure the port was successful, we send test data to the micro controller, query the processed
   signal and compare it in Octave to the reverence code.

The algorithms are described in [this document](algorithm/README.md).


## Project log

- (12/22/2020) Just tested BLE MIDI (i.e. MIDI over bluetooth). I could successfully connect to GarageBand
  on an iPhone. Unfortunately, the bluetooth connection caused some interference in the audio input signal
  so that the threshold had to be increased and also we got a lot of false detections on low velocity hits
  at the edge of the pad. So for future hardware designs some shielding should be considered. Also, I started
  looking at the second piezo signal to support rim shot detection.

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


## TODO list

- The algorithm is optimized for Roland PD-120 pad only. Other pad types should be supported, too.

- The low velocity performance at the edge of the pad is not good enough. This should be improved.

- We sometime have double-triggers on hard hits or when the rim is hit. The mask time is already
  at 10 ms. So, the decay handling should be improved to suppress these double-triggers.

- The normalization of the positional sensing should be improved.
