

ecasound --server -i test.wav -o alsa \
-elv2:urn:ardour:a-eq,20,1,200,10,10,200,1,10,200,1,10,200,1,10,8000,1,-15,1,1,1,1,1,1,1 \
-elv2:urn:ardour:a-reverb,0.2,1,1 \
-elv2:urn:ardour:a-comp,10,80,0,4,-30,0

# echo -e "c-select default\r\nc-mute on\r\n" | nc -w1 localhost 2868
# echo -e "c-select default\r\nc-mute off\r\n" | nc -w1 localhost 2868

# a-eq parameters: ------------------------
# freql:   Frequency L  { 20, 20000} Hz
# gl:      Gain L       {-20,    20} dB
# freq1:   Frequency 1  { 20, 20000} Hz
# g1:      Gain 1       {-20,    20} dB
# bw1:     Bandwidth 1  {0.1,     4}
# freq2:   Frequency 2  { 20, 20000} Hz
# g2:      Gain 2       {-20,    20} dB
# bw2:     Bandwidth 2  {0.1,     4}
# freq3:   Frequency 3  { 20, 20000} Hz
# g3:      Gain 3       {-20,    20} dB
# bw3:     Bandwidth 3  {0.1,     4}
# freq4:   Frequency 4  { 20, 20000} Hz
# g4:      Gain 4       {-20,    20} dB
# bw4:     Bandwidth 4  {0.1,     4}
# freqh:   Frequency H  { 20, 20000} Hz
# gh:      Gain H       {-20,    20} dB
# master:  Master Gain  {-20,    20} dB
# filtogl: Filter L     {  0,     1} on/off
# filtog1: Filter 1     {  0,     1} on/off
# filtog2: Filter 2     {  0,     1} on/off
# filtog3: Filter 3     {  0,     1} on/off
# filtog4: Filter 4     {  0,     1} on/off
# filtogh: Filter H     {  0,     1} on/off
# enable:  Enable       {  0,     1} on/off

# a-comp parameters: ----------------------
# att:    Attack    {0.1,  100} ms
# rel:    Release   {  1, 2000} ms
# kn:     Knee      {  0,    8} dB
# rat:    Ratio     {  1,   20}
# thr:    Threshold {-60,    0} dB
# mak:    Makeup    {  0,   30} dB
# sidech: Sidechain {  0,    1} on/off
# enable: Enable    {  0,    1} on/off

# a-reverb parameters: --------------------
# blend:  Blend     {0.0, 1}
# roomsz: Room Size {0.5, 1}
# enable: Enable    {  0, 1} on/off

