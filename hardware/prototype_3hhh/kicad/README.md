# edrumulus 3hhh prototype with a Teensy 4.1

PCBA design of the [edrumulus circuit](https://github.com/corrados/edrumulus/blob/661487f7e5eb3b547a28747b91cd4156a5f1f353/hardware/prototype2/edrumulus.pdf) with [modifications by 3hhh](https://github.com/corrados/edrumulus/discussions/98) to be used with a [Teensy 4.1](https://www.pjrc.com/store/teensy41.html) with soldered [pins](https://www.pjrc.com/store/teensy41_pins.html).

## Features

- size: 12x7cm
- price: ~30 EUR plus shipping and tariffs (~10 EUR to EU) at https://jlcpcb.com/ for 5 units (2025)
- little soldering required:
  - female socket for the Teensy 4.1
  - SMD components intended for assembly at the manufacturer
- all 18 Teensy 4.1 analog inputs are connected to respective edrumulus circuits
- female header as "breakout" connector to the drum hardware via DuPont jumper wires: Standard 6,3mm Jack stereo cables can relatively easily be soldered to three male-male jumper wires to connect the board to the drum kit (search "solder wires" on youtube). It may make sense to use short cables for soldering and connect them to longer 6,3mm cables as needed.
- 3hhh modifications:
  - Rp=10k for higher sensitivity
  - some pins (A1-A5) with Rs=100k for low sensitivity needs (kick, ...)
  - some pins (A6-A17) with Rs=10k for high sensitivity needs (snare, hihat, ...)
  - 1N5819WS Schottky diode: cheaper and lower forward voltage than BAT85
- M2 screw holes
- also see [this image](./edrumulus_3hhh.jpg)

## JLCPCB manufacturing

Make sure to select:

- 2 layers
- Suface Finish: LeadFree HASL or ENIG
- Mark on PCB: Order Number(Specify Position)
- PCB Assembly

Manufacturing incl. assembly (PCBA) can be done elsewhere as well, but was optimized for JLCPCB.

## Used software

[KiCad 9](https://www.kicad.org/) with the [JLCPCB plugin](https://github.com/Bouni/kicad-jlcpcb-tools).

Useful presets: https://github.com/sethhillbrand/kicad_templates

Helpful KiCad tutorial: https://www.youtube.com/playlist?list=PLUOaI24LpvQPls1Ru_qECJrENwzD7XImd

## Known issues

- edrumulus currently maxes out at ~15 used analog inputs
- the used female headers tend to run out of stock and may have to be replaced with others
- GND needs to be expanded with a small breadboard
