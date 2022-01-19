#!/bin/bash

echo "This script prepares a Raspberry Pi for Edrumulus usage"


# get environment --------------------------------------------------------------
NCORES=$(nproc)

# check of Teensy USB MIDI
if aconnect -l|grep -q Edrumulus; then
  echo "Edrumulus with Teensy hardware detected"
  is_teensy=true
fi


# install required packages ----------------------------------------------------
pkgs='git htop alsamixergui build-essential libasound2-dev jackd2 cmake libglib2.0-dev autoconf automake libtool lv2-dev xorg-dev libsndfile1-dev libjack-jackd2-dev libsmf-dev gettext a2jmidid libncurses5-dev'
if ! dpkg -s $pkgs >/dev/null 2>&1; then
  read -p "Do you want to install missing packages? " -n 1 -r
  echo
  if [[ $REPLY =~ ^[Yy]$ ]]; then
    sudo apt-get install $pkgs -y
  fi
fi


# download and compile mod-ttymidi ---------------------------------------------
if [ -d "mod-ttymidi" ]; then
  echo "The ttyMIDI directory is present, we assume it is compiled and ready to use. If not, delete the mod-ttymidi directory and call this script again."
else
  git clone https://github.com/moddevices/mod-ttymidi.git
  cd mod-ttymidi
  make
  cd ..
fi


# download and compile SendMIDI ------------------------------------------------
if [ -d "SendMIDI" ]; then
  echo "The SendMIDI directory is present, we assume it is compiled and ready to use. If not, delete the SendMIDI directory and call this script again."
else
  git clone https://github.com/gbevin/SendMIDI.git
  cd SendMIDI
  git checkout 1.0.15
  cd Builds/LinuxMakefile
  make
  cd ../../..
fi


# download and compile Drumgizmo -----------------------------------------------
if [ -d "drumgizmo" ]; then
  echo "The Drumgizmo directory is present, we assume it is compiled and ready to use. If not, delete the Drumgizmo directory and call this script again."
else
  git clone git://git.drumgizmo.org/drumgizmo.git
  cd drumgizmo
  git checkout edrumulus
  git submodule update --init
  ./autogen.sh
  ./configure --prefix=$PWD/install --with-lv2dir=$HOME/.lv2 --enable-lv2
  make -j${NCORES}
  cd ..
fi


# compile EdrumulusGUI ---------------------------------------------------------
if [ ! -f EdrumulusGUI ]; then
  echo "Compile EdrumulusGUI"
  gcc edrumulus_gui.cpp -o EdrumulusGUI -lncurses -ljack -lstdc++
fi


# TODO automate the creation of the kit: download source kits and call mixdown_kits.m
echo We assume that you have created the edrumuluskit with edrumulus/tools/mixdown_kits.m

if [ -d "edrumuluskit" ]; then
  KITXML="edrumuluskit/edrumuluskit.xml"
  KITMIDIMAPXML="edrumuluskit/edrumuluskit_midimap.xml"
  KITJACKPORTLEFT=DrumGizmo:0-left_channel
  KITJACKPORTRIGHT=DrumGizmo:1-right_channel
fi


# taken from "Raspberry Pi and realtime, low-latency audio" homepage at wiki.linuxaudio.org
#sudo service triggerhappy stop
#sudo service dbus stop
#sudo mount -o remount,size=128M /dev/shm


# jack deamon ------------------------------------------------------------------
# get first USB audio sound card device
ADEVICE=$(aplay -l|grep "USB Audio"|tail -1|cut -d' ' -f3)
echo "Using USB audio device: ${ADEVICE}"

# start the jack deamon (exit once all clients are closed with -T)
jackd -R -T --silent -P70 -t2000 -d alsa -dhw:${ADEVICE} -p 128 -n 3 -r 48000 -s >/dev/null 2>&1 &
sleep 1


# write Edrumulus trigger configuration ----------------------------------------
if [[ -v is_teensy ]]; then
  # diable spike cancellation for Teensy 4.0 prototype
  #SendMIDI/Builds/LinuxMakefile/build/sendmidi dev "Edrumulus" ch 10 cc 110 0

  # snare
  SendMIDI/Builds/LinuxMakefile/build/sendmidi dev "Edrumulus" ch 10 cc 108 0 # pad 0
  #SendMIDI/Builds/LinuxMakefile/build/sendmidi dev "Edrumulus" ch 10 cc 102 2 # PD8
  #SendMIDI/Builds/LinuxMakefile/build/sendmidi dev "Edrumulus" ch 10 cc 103 1 # threshold
  #SendMIDI/Builds/LinuxMakefile/build/sendmidi dev "Edrumulus" ch 10 cc 104 8 # sensitivity
  #SendMIDI/Builds/LinuxMakefile/build/sendmidi dev "Edrumulus" ch 10 cc 107 16 # rim shot threshold
  #SendMIDI/Builds/LinuxMakefile/build/sendmidi dev "Edrumulus" ch 10 cc 105 26 # positional sensing threshold
  #SendMIDI/Builds/LinuxMakefile/build/sendmidi dev "Edrumulus" ch 10 cc 106 11 # positional sensing sensitivity
  #SendMIDI/Builds/LinuxMakefile/build/sendmidi dev "Edrumulus" ch 10 cc 111 3 # both, rim shot and positional sensing

  # kick
  SendMIDI/Builds/LinuxMakefile/build/sendmidi dev "Edrumulus" ch 10 cc 108 1 # pad 1
  #SendMIDI/Builds/LinuxMakefile/build/sendmidi dev "Edrumulus" ch 10 cc 102 6 # KD7
  #SendMIDI/Builds/LinuxMakefile/build/sendmidi dev "Edrumulus" ch 10 cc 103 9 # threshold
  #SendMIDI/Builds/LinuxMakefile/build/sendmidi dev "Edrumulus" ch 10 cc 104 9 # sensitivity

  # Hi-Hat
  SendMIDI/Builds/LinuxMakefile/build/sendmidi dev "Edrumulus" ch 10 cc 108 2 # pad 2
  #SendMIDI/Builds/LinuxMakefile/build/sendmidi dev "Edrumulus" ch 10 cc 102 2 # PD8
  #SendMIDI/Builds/LinuxMakefile/build/sendmidi dev "Edrumulus" ch 10 cc 103 2 # threshold
  #SendMIDI/Builds/LinuxMakefile/build/sendmidi dev "Edrumulus" ch 10 cc 104 8 # sensitivity
  #SendMIDI/Builds/LinuxMakefile/build/sendmidi dev "Edrumulus" ch 10 cc 111 1 # enable rim shot

  # Hi-Hat control
  SendMIDI/Builds/LinuxMakefile/build/sendmidi dev "Edrumulus" ch 10 cc 108 3 # pad 3
  #SendMIDI/Builds/LinuxMakefile/build/sendmidi dev "Edrumulus" ch 10 cc 102 3 # FD8
  #SendMIDI/Builds/LinuxMakefile/build/sendmidi dev "Edrumulus" ch 10 cc 103 5 # threshold
  #SendMIDI/Builds/LinuxMakefile/build/sendmidi dev "Edrumulus" ch 10 cc 104 0 # sensitivity

  # crash
  SendMIDI/Builds/LinuxMakefile/build/sendmidi dev "Edrumulus" ch 10 cc 108 4 # pad 4
  #SendMIDI/Builds/LinuxMakefile/build/sendmidi dev "Edrumulus" ch 10 cc 102 2 # PD8
  #SendMIDI/Builds/LinuxMakefile/build/sendmidi dev "Edrumulus" ch 10 cc 103 19 # threshold
  #SendMIDI/Builds/LinuxMakefile/build/sendmidi dev "Edrumulus" ch 10 cc 104 21 # sensitivity
  #SendMIDI/Builds/LinuxMakefile/build/sendmidi dev "Edrumulus" ch 10 cc 111 1 # enable rim shot

  # tom 1
  SendMIDI/Builds/LinuxMakefile/build/sendmidi dev "Edrumulus" ch 10 cc 108 5 # pad 5
  #SendMIDI/Builds/LinuxMakefile/build/sendmidi dev "Edrumulus" ch 10 cc 102 1 # PD80R
  #SendMIDI/Builds/LinuxMakefile/build/sendmidi dev "Edrumulus" ch 10 cc 103 9 # threshold
  #SendMIDI/Builds/LinuxMakefile/build/sendmidi dev "Edrumulus" ch 10 cc 104 0 # sensitivity
  #SendMIDI/Builds/LinuxMakefile/build/sendmidi dev "Edrumulus" ch 10 cc 111 1 # enable rim shot

  # ride
  SendMIDI/Builds/LinuxMakefile/build/sendmidi dev "Edrumulus" ch 10 cc 108 6 # pad 6
  #SendMIDI/Builds/LinuxMakefile/build/sendmidi dev "Edrumulus" ch 10 cc 102 2 # PD8
  #SendMIDI/Builds/LinuxMakefile/build/sendmidi dev "Edrumulus" ch 10 cc 103 18 # threshold
  #SendMIDI/Builds/LinuxMakefile/build/sendmidi dev "Edrumulus" ch 10 cc 104 21 # sensitivity
  #SendMIDI/Builds/LinuxMakefile/build/sendmidi dev "Edrumulus" ch 10 cc 111 1 # enable rim shot

  # tom 2
  SendMIDI/Builds/LinuxMakefile/build/sendmidi dev "Edrumulus" ch 10 cc 108 7 # pad 7
  #SendMIDI/Builds/LinuxMakefile/build/sendmidi dev "Edrumulus" ch 10 cc 102 1 # PD80R
  #SendMIDI/Builds/LinuxMakefile/build/sendmidi dev "Edrumulus" ch 10 cc 103 18 # threshold
  #SendMIDI/Builds/LinuxMakefile/build/sendmidi dev "Edrumulus" ch 10 cc 104 0 # sensitivity
  #SendMIDI/Builds/LinuxMakefile/build/sendmidi dev "Edrumulus" ch 10 cc 111 1 # enable rim shot

  # connect ALSA MIDI to Jack Audio MIDI
  a2jmidid -e >/dev/null 2>&1 &
  sleep 1

  # get Edrumulus MIDI name
  MIDIJACKPORT=$(jack_lsp|grep "(capture): Edrumulus MIDI")
else
  # prepare serial output interface for sending MIDI configuration messages
  stty 38400 -F /dev/ttyUSB0

  # snare
  echo -n -e '\xB9\x6C\x00' > /dev/ttyUSB0 # select pad: 0
  #echo -n -e '\xB9\x6D\x03' > /dev/ttyUSB0 # MIDI curve type: LOG1
  #echo -n -e '\xB9\x66\x02' > /dev/ttyUSB0 # pad type: PD8
  #echo -n -e '\xB9\x67\x05' > /dev/ttyUSB0 # threshold
  #echo -n -e '\xB9\x68\x08' > /dev/ttyUSB0 # sensitivity
  #echo -n -e '\xB9\x6B\x10' > /dev/ttyUSB0 # rim shot threshold
  #echo -n -e '\xB9\x69\x1A' > /dev/ttyUSB0 # positional sensing threshold
  #echo -n -e '\xB9\x6A\x0B' > /dev/ttyUSB0 # positional sensing sensitivity
  #echo -n -e '\xB9\x6F\x03' > /dev/ttyUSB0 # rim/pos: both, rim shot and positional sensing

  # kick
  echo -n -e '\xB9\x6C\x01' > /dev/ttyUSB0 # select pad: 1
  #echo -n -e '\xB9\x6D\x03' > /dev/ttyUSB0 # MIDI curve type: LOG1
  #echo -n -e '\xB9\x66\x06' > /dev/ttyUSB0 # pad type: KD7
  #echo -n -e '\xB9\x67\x0A' > /dev/ttyUSB0 # threshold
  echo -n -e '\xB9\x68\x09' > /dev/ttyUSB0 # sensitivity

  # Hi-Hat
  echo -n -e '\xB9\x6C\x02' > /dev/ttyUSB0 # select pad: 2
  #echo -n -e '\xB9\x6D\x03' > /dev/ttyUSB0 # MIDI curve type: LOG1
  #echo -n -e '\xB9\x66\x02' > /dev/ttyUSB0 # pad type: PD8
  #echo -n -e '\xB9\x67\x05' > /dev/ttyUSB0 # threshold
  #echo -n -e '\xB9\x68\x08' > /dev/ttyUSB0 # sensitivity
  #echo -n -e '\xB9\x6F\x01' > /dev/ttyUSB0 # rim/pos: enable rim shot

  # Hi-Hat control
  echo -n -e '\xB9\x6C\x03' > /dev/ttyUSB0 # select pad: 3
  #echo -n -e '\xB9\x66\x03' > /dev/ttyUSB0 # pad type: FD8
  echo -n -e '\xB9\x67\x0C' > /dev/ttyUSB0 # threshold
  echo -n -e '\xB9\x68\x1C' > /dev/ttyUSB0 # sensitivity

  # crash
  echo -n -e '\xB9\x6C\x04' > /dev/ttyUSB0 # select pad: 4
  #echo -n -e '\xB9\x6D\x03' > /dev/ttyUSB0 # MIDI curve type: LOG1
  #echo -n -e '\xB9\x66\x08' > /dev/ttyUSB0 # pad type: CY6
  #echo -n -e '\xB9\x67\x09' > /dev/ttyUSB0 # threshold
  #echo -n -e '\xB9\x68\x0B' > /dev/ttyUSB0 # sensitivity
  #echo -n -e '\xB9\x6F\x01' > /dev/ttyUSB0 # rim/pos: enable rim shot

  # tom 1
  echo -n -e '\xB9\x6C\x05' > /dev/ttyUSB0 # select pad: 5
  #echo -n -e '\xB9\x66\x01' > /dev/ttyUSB0 # pad type: PD80R
  #echo -n -e '\xB9\x67\x09' > /dev/ttyUSB0 # threshold
  #echo -n -e '\xB9\x68\x00' > /dev/ttyUSB0 # sensitivity

  # ride
  echo -n -e '\xB9\x6C\x06' > /dev/ttyUSB0 # select pad: 6
  #echo -n -e '\xB9\x66\x02' > /dev/ttyUSB0 # pad type: PD8
  #echo -n -e '\xB9\x67\x12' > /dev/ttyUSB0 # threshold
  #echo -n -e '\xB9\x68\x15' > /dev/ttyUSB0 # sensitivity
  #echo -n -e '\xB9\x6F\x01' > /dev/ttyUSB0 # rim/pos: enable rim shot

  # tom 2
  echo -n -e '\xB9\x6C\x07' > /dev/ttyUSB0 # select pad: 7
  #echo -n -e '\xB9\x66\x01' > /dev/ttyUSB0 # pad type: PD80R
  #echo -n -e '\xB9\x67\x12' > /dev/ttyUSB0 # threshold
  #echo -n -e '\xB9\x68\x00' > /dev/ttyUSB0 # sensitivity

  # start MIDI tool to convert serial MIDI to Jack Audio MIDI
  # note that to get access to /dev/ttyUSB0 we need to be in group tty/dialout
  mod-ttymidi/ttymidi -b 38400 &
  MIDIJACKPORT=ttymidi:MIDI_in
fi


# run Edrumulus ----------------------------------------------------------------
if [ $USER = "pi" ]; then
  ./drumgizmo/drumgizmo/drumgizmo -l -L max=2,rampdown=0.02 -i jackmidi -I midimap=$KITMIDIMAPXML -o jackaudio $KITXML &
  sleep 20
else
  ./drumgizmo/drumgizmo/drumgizmo -i jackmidi -I midimap=$KITMIDIMAPXML -o jackaudio $KITXML &
  sleep 5
fi

jack_connect $KITJACKPORTLEFT system:playback_1
jack_connect $KITJACKPORTRIGHT system:playback_2


# either use direct MIDI connection or through EdrumulusGUI
##jack_connect "$MIDIJACKPORT" DrumGizmo:drumgizmo_midiin
##echo "###---------- PRESS ANY KEY TO TERMINATE THE EDRUMULUS SESSION ---------###"
##read -n 1 -s -r -p ""
./EdrumulusGUI DrumGizmo:drumgizmo_midiin


killall drumgizmo

if [[ -v is_teensy ]]; then
  killall a2jmidid
else
  killall ttymidi
fi

