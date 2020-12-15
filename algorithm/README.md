# Drum triggering algorithms

## Brainstorming

### Peak detection

  - If you calculate the power of the recorded real-valued audio signal, the resulting power curve has
    significant power drops caused by the nature of a sinusoidal signal. A filtering can smooth the
    curve. As a test I have used an Hilbert transform to convert the real-valued signal in a complex
    signal. As a result, the magnitude of that complex signal is much smoother already without having
    modified the actual spectrum of the signal (real-valued signals have mirror symmetric spectrum).

  - To improve the peak detection, we can make use of the known decay curve of the trigger pad in use.
    So, after successfully detecting a peak, we know that this peak causes a slowly decaying power
    curve which has a known shape and we can subtract that known curve from the signal to improve the
    detection of the next pad hit.

### Positional sensing

  - It has shown that if you hit the pad close to the edge, the resulting sound has less low frequencies
    and sounds more crisp. So, the idea is to low-pass filter the signal and at the detected peak position we
	calculate the power ratio of the low-pass filtered signal with the unfiltered signal. This is then
	the metric for the positional sensing.

### First results

  - The following plot shows how the current status of the algorithms performs. At the beginning there are
    some single hits. Then there follows a region with a snare drum roll. After that, there are single hits
    which start from the middle, move to the edge and go back to the middle of the pad where the hits are
    equally strong. As shown by the black markers, the positional sensing seems to work pretty well. Also,
    the peak detection and velocity estimation seems to be pretty good as well.
    ![First results plot](images/first_results.jpg)
