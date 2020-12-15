# Edrumulus
Open Source E-Drum Trigger Module Software


## Project log

- (12/13/2020) I am very pleased about the current algorithm performance. The algorithm is not yet fine-tuned but
  already performs pretty well. I have created a short Youtube video of the algorithm (Git commit c83743e) to show
  the current performance in action: https://youtu.be/6eQjCD-DFjo


## Project specifications

- Research is done using a regular audio card, capture the drum pad output signal and develop
  the algorithms in Octave.

- One goal would be to use a Raspberry Pi Zero as a trigger module. So, it gets a sampled
  audio signal from the GIOP (some external hardware needed) and processes it using a C++
  software. It outputs a MIDI signal. Since the Raspberry Pi Zero has only a slow processor,
  it will not be possible to include the complete drum module.
  As an alternative an Arduino could be used, similar to the [open e-drums](https://open-e-drums.com) project.

- Positional sensing shall be supported.

- Overall latency should be as small as possible. The goal is to get a latency < 10 ms.


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
