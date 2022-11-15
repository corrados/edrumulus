

ecasound --server -i test.wav -o alsa \
-elv2:urn:ardour:a-eq,20,1,200,10,10,200,1,10,200,1,10,200,1,10,8000,1,-15,1,1,1,1,1,1,1

# echo -e "c-select default\r\nc-mute on\r\n" | nc -w1 localhost 2868
# echo -e "c-select default\r\nc-mute off\r\n" | nc -w1 localhost 2868

# a-eq parameters: ------------------------
# freql:   Frequency L
# gl:      Gain L
# freq1:   Frequency 1
# g1:      Gain 1
# bw1:     Bandwidth 1
# freq2:   Frequency 2
# g2:      Gain 2
# bw2:     Bandwidth 2
# freq3:   Frequency 3
# g3:      Gain 3
# bw3:     Bandwidth 3
# freq4:   Frequency 4
# g4:      Gain 4
# bw4:     Bandwidth 4
# freqh:   Frequency H
# gh:      Gain H
# master:  Master Gain
# filtogl: Filter L
# filtog1: Filter 1
# filtog2: Filter 2
# filtog3: Filter 3
# filtog4: Filter 4
# filtogh: Filter H
# enable:  Enable

