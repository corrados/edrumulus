
# Pearl MMX kit live mixing setup
./ecasound/ecasound/ecasound --server -q \
\
-a:KDrum -i jack_multi,DrumGizmo:0-KDrum \
     -elv2:urn:ardour:a-comp,10,80,0,4,-30,0 \
     -elv2:urn:ardour:a-eq,160,0,75,4.3,0.875,2436,0,1.2,256,-2,0.766,909,-1.3,10.62,8088,-2.2,-0.62,1,1,1,1,1,1,1 \
     -chcopy:1,2 -ea:100 -epp:50 \
     -elv2:urn:ardour:a-reverb,0.2,1,0 \
-a:Snare -i jack_multi,DrumGizmo:1-Snare \
     -elv2:urn:ardour:a-comp,10,80,0,4,-30,0 \
     -elv2:urn:ardour:a-eq,24,-20,86,5.6,1,194,-4.9,0.38,2500,0,1,734,5.4,0.61,2245,16.1,0.5,1,1,1,1,1,1,1 \
     -chcopy:1,2 -ea:100 -epp:50 \
     -elv2:urn:ardour:a-reverb,0.3,0.5,0 \
-a:Hihat -i jack_multi,DrumGizmo:2-Hihat \
     -elv2:urn:ardour:a-comp,10,80,0,4,-30,0 \
     -elv2:urn:ardour:a-eq,315,-20,300,0,1,1000,0,1,2500,0,1,6000,0,1,4112,10.7,-11.1,1,1,1,1,1,1,1 \
     -chcopy:1,2 -ea:100 -epp:30 \
     -elv2:urn:ardour:a-reverb,0.2,1,0 \
-a:Tom1 -i jack_multi,DrumGizmo:3-Tom1 \
     -elv2:urn:ardour:a-comp,10,80,0,4,-30,0 \
     -elv2:urn:ardour:a-eq,160,0,300,0,1,118,12,1,829,-20,2.26,5204,20,1,9000,0,-4,1,1,1,1,1,1,1 \
     -chcopy:1,2 -ea:100 -epp:40 \
     -elv2:urn:ardour:a-reverb,0.2,1,0 \
-a:Tom2 -i jack_multi,DrumGizmo:4-Tom2 \
     -elv2:urn:ardour:a-comp,10,80,0,4,-30,0 \
     -elv2:urn:ardour:a-eq,160,0,300,0,1,106,12,1,829,-20,2.26,2741,20,1,9000,0,-4,1,1,1,1,1,1,1 \
     -chcopy:1,2 -ea:100 -epp:60 \
     -elv2:urn:ardour:a-reverb,0.2,1,0 \
-a:Tom3 -i jack_multi,DrumGizmo:5-Tom3 \
     -elv2:urn:ardour:a-comp,10,80,0,4,-30,0 \
     -elv2:urn:ardour:a-eq,160,0,300,0,1,99,12,1,829,-9.5,2.26,2741,19.6,1.87,9000,0,-4,1,1,1,1,1,1,1 \
     -chcopy:1,2 -ea:100 -epp:70 \
     -elv2:urn:ardour:a-reverb,0.2,1,0 \
-a:OHLeft -i jack_multi,DrumGizmo:6-OHLeft \
     -elv2:urn:ardour:a-comp,10,80,0,4,-30,0 \
     -elv2:urn:ardour:a-eq,160,0,86,8.2,1,146,-8.9,0.36,2500,0,1,6000,0,1,5091,12.2,-6.28,1,1,1,1,1,1,1 \
     -chcopy:1,2 -ea:40 -epp:0 \
     -elv2:urn:ardour:a-reverb,0.2,1,0 \
-a:OHRight -i jack_multi,DrumGizmo:7-OHRight \
     -elv2:urn:ardour:a-comp,10,80,0,4,-30,0 \
     -elv2:urn:ardour:a-eq,160,0,86,8.2,1,146,-8.9,0.36,2500,0,1,6000,0,1,5091,12.2,-6.28,1,1,1,1,1,1,1 \
     -chcopy:1,2 -ea:40 -epp:100 \
     -elv2:urn:ardour:a-reverb,0.2,1,0 \
-a:all -o jack,system


# remote control examples: ----------------
# echo -e "c-select default\r\nc-mute on\r\n" | nc -w1 localhost 2868
# echo -e "c-select default\r\nc-mute off\r\n" | nc -w1 localhost 2868
# echo -e "cop-status\r\n" | nc -w1 localhost 2868
# echo -e "cop-set 1,17,0\r\n" | nc -w1 localhost 2868
# echo -e "cop-get 1,17\r\n" | nc -w1 localhost 2868
# echo -e "c-select Snare\r\ncop-set 2,1,100\r\n" | nc -w1 localhost 2868
# echo -e "cs-status\r\n" | nc -w1 localhost 2868


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




## CrocellKit live mixing setup
#ecasound --server \
#\
#-a:AmbLeft,AmbRight,OHLeft,OHRight,\
#OHCenter,Hihat,Ride,SnareTop,SnareBottom,\
#Tom1,Tom2,FTom1,FTom2,\
#KDrumInside,KDrumOutside \
#\
#-i jack_multi,DrumGizmo:0-AmbLeft,DrumGizmo:1-AmbRight,DrumGizmo:2-OHLeft,DrumGizmo:3-OHRight,\
#DrumGizmo:4-OHCenter,DrumGizmo:5-Hihat,DrumGizmo:6-Ride,DrumGizmo:7-SnareTop,DrumGizmo:8-SnareBottom,\
#DrumGizmo:9-Tom1,DrumGizmo:10-Tom2,DrumGizmo:11-FTom1,DrumGizmo:12-FTom2,\
#DrumGizmo:13-KDrumInside,DrumGizmo:14-KDrumOutside \
#\
#-a:AmbLeft -chcopy:1,2 -ea:100 -epp:0 \
#     -elv2:urn:ardour:a-comp,10,80,0,4,-30,0 \
#     -elv2:urn:ardour:a-eq,160,0,300,0,1,1000,0,1,2500,0,1,6000,0,1,9000,0,0,1,1,1,1,1,1,1 \
#     -elv2:urn:ardour:a-reverb,0.2,1,0 \
#-a:AmbRight -chcopy:1,2 -ea:100 -epp:100 \
#     -elv2:urn:ardour:a-comp,10,80,0,4,-30,0 \
#     -elv2:urn:ardour:a-eq,160,0,300,0,1,1000,0,1,2500,0,1,6000,0,1,9000,0,0,1,1,1,1,1,1,1 \
#     -elv2:urn:ardour:a-reverb,0.2,1,0 \
#-a:OHLeft -chcopy:1,2 -ea:100 -epp:0 \
#     -elv2:urn:ardour:a-comp,10,80,0,4,-30,0 \
#     -elv2:urn:ardour:a-eq,160,0,300,0,1,1000,0,1,2500,0,1,6000,0,1,9000,0,0,1,1,1,1,1,1,1 \
#     -elv2:urn:ardour:a-reverb,0.2,1,0 \
#-a:OHRight -chcopy:1,2 -ea:100 -epp:100 \
#     -elv2:urn:ardour:a-comp,10,80,0,4,-30,0 \
#     -elv2:urn:ardour:a-eq,160,0,300,0,1,1000,0,1,2500,0,1,6000,0,1,9000,0,0,1,1,1,1,1,1,1 \
#     -elv2:urn:ardour:a-reverb,0.2,1,0 \
#-a:OHCenter -chcopy:1,2 -ea:50 -epp:50 \
#     -elv2:urn:ardour:a-comp,10,80,0,4,-30,0 \
#     -elv2:urn:ardour:a-eq,160,0,300,0,1,1000,0,1,2500,0,1,6000,0,1,9000,0,0,1,1,1,1,1,1,1 \
#     -elv2:urn:ardour:a-reverb,0.2,1,0 \
#-a:Hihat -chcopy:1,2 -ea:50 -epp:50 \
#     -elv2:urn:ardour:a-comp,10,80,0,4,-30,0 \
#     -elv2:urn:ardour:a-eq,160,0,300,0,1,1000,0,1,2500,0,1,6000,0,1,9000,0,0,1,1,1,1,1,1,1 \
#     -elv2:urn:ardour:a-reverb,0.2,1,0 \
#-a:Ride -chcopy:1,2 -ea:50 -epp:50 \
#     -elv2:urn:ardour:a-comp,10,80,0,4,-30,0 \
#     -elv2:urn:ardour:a-eq,160,0,300,0,1,1000,0,1,2500,0,1,6000,0,1,9000,0,0,1,1,1,1,1,1,1 \
#     -elv2:urn:ardour:a-reverb,0.2,1,0 \
#-a:SnareTop -chcopy:1,2 -ea:50 -epp:50 \
#     -elv2:urn:ardour:a-comp,10,80,0,4,-30,0 \
#     -elv2:urn:ardour:a-eq,160,0,300,0,1,1000,0,1,2500,0,1,6000,0,1,9000,0,0,1,1,1,1,1,1,1 \
#     -elv2:urn:ardour:a-reverb,0.3,0.5,1 \
#-a:SnareBottom -chcopy:1,2 -ea:50 -epp:50 \
#     -elv2:urn:ardour:a-comp,10,80,0,4,-30,0 \
#     -elv2:urn:ardour:a-eq,160,0,300,0,1,1000,0,1,2500,0,1,6000,0,1,9000,0,0,1,1,1,1,1,1,1 \
#     -elv2:urn:ardour:a-reverb,0.2,1,0 \
#-a:Tom1 -chcopy:1,2 -ea:80 -epp:50 \
#     -elv2:urn:ardour:a-comp,10,80,0,4,-30,0 \
#     -elv2:urn:ardour:a-eq,160,0,300,0,1,1000,0,1,2500,0,1,6000,0,1,9000,0,0,1,1,1,1,1,1,1 \
#     -elv2:urn:ardour:a-reverb,0.2,1,1 \
#-a:Tom2 -chcopy:1,2 -ea:80 -epp:50 \
#     -elv2:urn:ardour:a-comp,10,80,0,4,-30,0 \
#     -elv2:urn:ardour:a-eq,160,0,300,0,1,1000,0,1,2500,0,1,6000,0,1,9000,0,0,1,1,1,1,1,1,1 \
#     -elv2:urn:ardour:a-reverb,0.2,1,1 \
#-a:FTom1 -chcopy:1,2 -ea:80 -epp:50 \
#     -elv2:urn:ardour:a-comp,10,80,0,4,-30,0 \
#     -elv2:urn:ardour:a-eq,160,0,300,0,1,1000,0,1,2500,0,1,6000,0,1,9000,0,0,1,1,1,1,1,1,1 \
#     -elv2:urn:ardour:a-reverb,0.2,1,1 \
#-a:FTom2 -chcopy:1,2 -ea:80 -epp:50 \
#     -elv2:urn:ardour:a-comp,10,80,0,4,-30,0 \
#     -elv2:urn:ardour:a-eq,160,0,300,0,1,1000,0,1,2500,0,1,6000,0,1,9000,0,0,1,1,1,1,1,1,1 \
#     -elv2:urn:ardour:a-reverb,0.2,1,1 \
#-a:KDrumInside -chcopy:1,2 -ea:50 -epp:50 \
#     -elv2:urn:ardour:a-comp,10,80,0,4,-30,0 \
#     -elv2:urn:ardour:a-eq,160,0,300,0,1,1000,0,1,2500,0,1,6000,0,1,9000,0,0,1,1,1,1,1,1,1 \
#     -elv2:urn:ardour:a-reverb,0.2,1,1 \
#-a:KDrumOutside -chcopy:1,2 -ea:50 -epp:50 \
#     -elv2:urn:ardour:a-comp,10,80,0,4,-30,0 \
#     -elv2:urn:ardour:a-eq,100,10,300,0,1,1000,0,1,2500,0,1,6000,0,1,9000,0,0,1,1,1,1,1,1,1 \
#     -elv2:urn:ardour:a-reverb,0.2,1,0 \
#-a:all -o jack,system

