
# CrocellKit live mixing setup
ecasound --server \
\
-a:AmbLeft,AmbRight,OHLeft,OHRight,\
OHCenter,Hihat,Ride,SnareTop,SnareBottom,\
Tom1,Tom2,FTom1,FTom2,\
KDrumInside,KDrumOutside \
\
-i jack_multi,DrumGizmo:0-AmbLeft,DrumGizmo:1-AmbRight,DrumGizmo:2-OHLeft,DrumGizmo:3-OHRight,\
DrumGizmo:4-OHCenter,DrumGizmo:5-Hihat,DrumGizmo:6-Ride,DrumGizmo:7-SnareTop,DrumGizmo:8-SnareBottom,\
DrumGizmo:9-Tom1,DrumGizmo:10-Tom2,DrumGizmo:11-FTom1,DrumGizmo:12-FTom2,\
DrumGizmo:13-KDrumInside,DrumGizmo:14-KDrumOutside \
\
-a:AmbLeft -ea:100 -epp:0 \
     -elv2:urn:ardour:a-comp,10,80,0,4,-30,0 \
     -elv2:urn:ardour:a-eq,160,0,300,0,1,1000,0,1,2500,0,1,6000,0,1,9000,0,0,1,1,1,1,1,1,1 \
     -elv2:urn:ardour:a-reverb,0.2,1,0 \
-a:AmbRight -ea:100 -epp:100 \
     -elv2:urn:ardour:a-comp,10,80,0,4,-30,0 \
     -elv2:urn:ardour:a-eq,160,0,300,0,1,1000,0,1,2500,0,1,6000,0,1,9000,0,0,1,1,1,1,1,1,1 \
     -elv2:urn:ardour:a-reverb,0.2,1,0 \
-a:OHLeft -ea:100 -epp:0 \
     -elv2:urn:ardour:a-comp,10,80,0,4,-30,0 \
     -elv2:urn:ardour:a-eq,160,0,300,0,1,1000,0,1,2500,0,1,6000,0,1,9000,0,0,1,1,1,1,1,1,1 \
     -elv2:urn:ardour:a-reverb,0.2,1,0 \
-a:OHRight -ea:100 -epp:100 \
     -elv2:urn:ardour:a-comp,10,80,0,4,-30,0 \
     -elv2:urn:ardour:a-eq,160,0,300,0,1,1000,0,1,2500,0,1,6000,0,1,9000,0,0,1,1,1,1,1,1,1 \
     -elv2:urn:ardour:a-reverb,0.2,1,0 \
-a:OHCenter -ea:50 -epp:50 \
     -elv2:urn:ardour:a-comp,10,80,0,4,-30,0 \
     -elv2:urn:ardour:a-eq,160,0,300,0,1,1000,0,1,2500,0,1,6000,0,1,9000,0,0,1,1,1,1,1,1,1 \
     -elv2:urn:ardour:a-reverb,0.2,1,0 \
-a:Hihat -ea:50 -epp:50 \
     -elv2:urn:ardour:a-comp,10,80,0,4,-30,0 \
     -elv2:urn:ardour:a-eq,160,0,300,0,1,1000,0,1,2500,0,1,6000,0,1,9000,0,0,1,1,1,1,1,1,1 \
     -elv2:urn:ardour:a-reverb,0.2,1,0 \
-a:Ride -ea:50 -epp:50 \
     -elv2:urn:ardour:a-comp,10,80,0,4,-30,0 \
     -elv2:urn:ardour:a-eq,160,0,300,0,1,1000,0,1,2500,0,1,6000,0,1,9000,0,0,1,1,1,1,1,1,1 \
     -elv2:urn:ardour:a-reverb,0.2,1,0 \
-a:SnareTop -ea:50 -epp:50 \
     -elv2:urn:ardour:a-comp,10,80,0,4,-30,0 \
     -elv2:urn:ardour:a-eq,160,0,300,0,1,1000,0,1,2500,0,1,6000,0,1,9000,0,0,1,1,1,1,1,1,1 \
     -elv2:urn:ardour:a-reverb,0.3,0.5,1 \
-a:SnareBottom -ea:50 -epp:50 \
     -elv2:urn:ardour:a-comp,10,80,0,4,-30,0 \
     -elv2:urn:ardour:a-eq,160,0,300,0,1,1000,0,1,2500,0,1,6000,0,1,9000,0,0,1,1,1,1,1,1,1 \
     -elv2:urn:ardour:a-reverb,0.2,1,0 \
-a:Tom1 -ea:80 -epp:50 \
     -elv2:urn:ardour:a-comp,10,80,0,4,-30,0 \
     -elv2:urn:ardour:a-eq,160,0,300,0,1,1000,0,1,2500,0,1,6000,0,1,9000,0,0,1,1,1,1,1,1,1 \
     -elv2:urn:ardour:a-reverb,0.2,1,1 \
-a:Tom2 -ea:80 -epp:50 \
     -elv2:urn:ardour:a-comp,10,80,0,4,-30,0 \
     -elv2:urn:ardour:a-eq,160,0,300,0,1,1000,0,1,2500,0,1,6000,0,1,9000,0,0,1,1,1,1,1,1,1 \
     -elv2:urn:ardour:a-reverb,0.2,1,1 \
-a:FTom1 -ea:80 -epp:50 \
     -elv2:urn:ardour:a-comp,10,80,0,4,-30,0 \
     -elv2:urn:ardour:a-eq,160,0,300,0,1,1000,0,1,2500,0,1,6000,0,1,9000,0,0,1,1,1,1,1,1,1 \
     -elv2:urn:ardour:a-reverb,0.2,1,1 \
-a:FTom2 -ea:80 -epp:50 \
     -elv2:urn:ardour:a-comp,10,80,0,4,-30,0 \
     -elv2:urn:ardour:a-eq,160,0,300,0,1,1000,0,1,2500,0,1,6000,0,1,9000,0,0,1,1,1,1,1,1,1 \
     -elv2:urn:ardour:a-reverb,0.2,1,1 \
-a:KDrumInside -ea:50 -epp:50 \
     -elv2:urn:ardour:a-comp,10,80,0,4,-30,0 \
     -elv2:urn:ardour:a-eq,160,0,300,0,1,1000,0,1,2500,0,1,6000,0,1,9000,0,0,1,1,1,1,1,1,1 \
     -elv2:urn:ardour:a-reverb,0.2,1,1 \
-a:KDrumOutside -ea:50 -epp:50 \
     -elv2:urn:ardour:a-comp,10,80,0,4,-30,0 \
     -elv2:urn:ardour:a-eq,100,10,300,0,1,1000,0,1,2500,0,1,6000,0,1,9000,0,0,1,1,1,1,1,1,1 \
     -elv2:urn:ardour:a-reverb,0.2,1,0 \
-a:all -o jack,system


# remote control examples: ----------------
# echo -e "c-select default\r\nc-mute on\r\n" | nc -w1 localhost 2868
# echo -e "c-select default\r\nc-mute off\r\n" | nc -w1 localhost 2868
# echo -e "cop-status\r\n" | nc -w1 localhost 2868
# echo -e "cop-set 1,17,0\r\n" | nc -w1 localhost 2868
# echo -e "cop-get 1,17\r\n" | nc -w1 localhost 2868


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

