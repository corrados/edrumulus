#!/bin/bash

# Edrumulus Linux start script for using Drumgizmo
#
# Optional: If your Raspberry Pi has a button for ON/OFF connected to gpio pin3, add the following in /boot/config.txt
#           dtoverlay=gpio-shutdown,gpio_pin=3, active_low=1,gpio_pull=up
# Optional: Give audio group rights to do realtime
#           sudo sh -c 'echo "@audio   -  rtprio   95" >> /etc/security/limits.conf'
#           sudo sh -c 'echo "@audio   -  memlock  unlimited" >> /etc/security/limits.conf'

echo "This script prepares a Linux/Raspberry Pi system for Edrumulus usage"


# get environment --------------------------------------------------------------
NCORES=$(nproc)

# check of Teensy USB MIDI
if aconnect -l|grep -q Edrumulus; then
  echo "-> Edrumulus with Teensy hardware detected"
  is_teensy=true
fi

# check if we are running on a Raspberry Pi by checking if the user name is pi
if [ $USER = "pi" ]; then
  echo "-> Running on a Raspberry pi"
  is_raspi=true
fi

# check if we are in Jamulus session mode
if [ "$1" == jamulus ] || [ "$2" == jamulus ]; then
  echo "-> Jamulus session mode enabled"
  is_jamulus=true
fi

# check if the GUI mode shall be used
if [ "$1" == gui ] || [ "$2" == gui ]; then
  echo "-> GUI mode enabled"
  is_gui=true
fi

# check if we are using custom settings (which overwrite the settings stored on the microcontroller)
if [ "$1" == set ] || [ "$2" == set ]; then
  echo "-> Using custom settings (overwrites settings on device)"
  is_settings=true
fi


# install required packages ----------------------------------------------------
pkgs='git htop vim alsamixergui build-essential libasound2-dev jackd2 cmake libglib2.0-dev autoconf automake libtool lv2-dev xorg-dev libsndfile1-dev libjack-jackd2-dev libsmf-dev gettext a2jmidid libncurses5-dev'
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
  if [[ -v is_settings ]]; then
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
  fi

  # connect ALSA MIDI to Jack Audio MIDI
  a2jmidid -e >/dev/null 2>&1 &
  sleep 1

  # get Edrumulus MIDI name
  MIDIJACKPORT=$(jack_lsp|grep "(capture): Edrumulus MIDI")
else
  # prepare serial output interface for sending MIDI configuration messages
  stty 38400 -F /dev/ttyUSB0

  if [[ -v is_settings ]]; then
    # general settings
    echo -n -e '\xB9\x6E\x04' > /dev/ttyUSB0 # spike cancellation: 4

    # snare
    echo -n -e '\xB9\x6C\x00' > /dev/ttyUSB0 # select pad:       0
    echo -n -e '\xB9\x6D\x00' > /dev/ttyUSB0 # MIDI curve type:  LINEAR
    echo -n -e '\xB9\x66\x0F' > /dev/ttyUSB0 # pad type:         PDX8
    echo -n -e '\xB9\x67\x07' > /dev/ttyUSB0 # threshold:        7
    echo -n -e '\xB9\x68\x07' > /dev/ttyUSB0 # sensitivity:      7
    echo -n -e '\xB9\x6B\x04' > /dev/ttyUSB0 # rim threshold:    4
    echo -n -e '\xB9\x69\x16' > /dev/ttyUSB0 # pos threshold:    22
    echo -n -e '\xB9\x6A\x1F' > /dev/ttyUSB0 # pos sensitivity:  31
    echo -n -e '\xB9\x6F\x03' > /dev/ttyUSB0 # rim/pos:          3 (both)
    echo -n -e '\xB9\x70\x26' > /dev/ttyUSB0 # midi note:        38
    echo -n -e '\xB9\x71\x28' > /dev/ttyUSB0 # midi note rim:    40
    echo -n -e '\xB9\x72\x05' > /dev/ttyUSB0 # crosstalk cancel: 5
    sleep 0.2

    # kick
    echo -n -e '\xB9\x6C\x01' > /dev/ttyUSB0 # select pad:       1
    echo -n -e '\xB9\x6D\x00' > /dev/ttyUSB0 # MIDI curve type:  LINEAR
    echo -n -e '\xB9\x66\x06' > /dev/ttyUSB0 # pad type:         KD7
    echo -n -e '\xB9\x67\x0E' > /dev/ttyUSB0 # threshold:        14
    echo -n -e '\xB9\x68\x09' > /dev/ttyUSB0 # sensitivity:      9
    echo -n -e '\xB9\x6F\x03' > /dev/ttyUSB0 # rim/pos:          0 (none)
    echo -n -e '\xB9\x70\x24' > /dev/ttyUSB0 # midi note:        36
    echo -n -e '\xB9\x72\x00' > /dev/ttyUSB0 # crosstalk cancel: 0
    sleep 0.2

    # Hi-Hat
    echo -n -e '\xB9\x6C\x02' > /dev/ttyUSB0 # select pad:       2
    echo -n -e '\xB9\x6D\x00' > /dev/ttyUSB0 # MIDI curve type:  LINEAR
    echo -n -e '\xB9\x66\x0B' > /dev/ttyUSB0 # pad type:         CY5
    echo -n -e '\xB9\x67\x06' > /dev/ttyUSB0 # threshold:        6
    echo -n -e '\xB9\x68\x06' > /dev/ttyUSB0 # sensitivity:      6
    echo -n -e '\xB9\x6F\x03' > /dev/ttyUSB0 # rim/pos:          0 (none)
    echo -n -e '\xB9\x70\x16' > /dev/ttyUSB0 # midi note:        22
    echo -n -e '\xB9\x72\x08' > /dev/ttyUSB0 # crosstalk cancel: 8
    sleep 0.2

    # Hi-Hat control
    echo -n -e '\xB9\x6C\x03' > /dev/ttyUSB0 # select pad:       3
    echo -n -e '\xB9\x66\x03' > /dev/ttyUSB0 # pad type:         FD8
    echo -n -e '\xB9\x67\x0C' > /dev/ttyUSB0 # threshold:        12
    echo -n -e '\xB9\x68\x1F' > /dev/ttyUSB0 # sensitivity:      31
    echo -n -e '\xB9\x70\x2C' > /dev/ttyUSB0 # midi note:        44
    sleep 0.2

    # crash
    echo -n -e '\xB9\x6C\x04' > /dev/ttyUSB0 # select pad:       4
    echo -n -e '\xB9\x6D\x00' > /dev/ttyUSB0 # MIDI curve type:  LINEAR
    echo -n -e '\xB9\x66\x0B' > /dev/ttyUSB0 # pad type:         CY5
    echo -n -e '\xB9\x67\x0D' > /dev/ttyUSB0 # threshold:        13
    echo -n -e '\xB9\x68\x09' > /dev/ttyUSB0 # sensitivity:      9
    echo -n -e '\xB9\x6F\x03' > /dev/ttyUSB0 # rim/pos:          0 (none)
    echo -n -e '\xB9\x70\x31' > /dev/ttyUSB0 # midi note:        49
    echo -n -e '\xB9\x72\x08' > /dev/ttyUSB0 # crosstalk cancel: 8
    sleep 0.2

    # tom 1
    echo -n -e '\xB9\x6C\x05' > /dev/ttyUSB0 # select pad:       5
    echo -n -e '\xB9\x6D\x00' > /dev/ttyUSB0 # MIDI curve type:  LINEAR
    echo -n -e '\xB9\x66\x0C' > /dev/ttyUSB0 # pad type:         HD1TOM
    echo -n -e '\xB9\x67\x08' > /dev/ttyUSB0 # threshold:        8
    echo -n -e '\xB9\x68\x06' > /dev/ttyUSB0 # sensitivity:      6
    echo -n -e '\xB9\x6F\x03' > /dev/ttyUSB0 # rim/pos:          0 (none)
    echo -n -e '\xB9\x70\x30' > /dev/ttyUSB0 # midi note:        48
    echo -n -e '\xB9\x72\x04' > /dev/ttyUSB0 # crosstalk cancel: 4
    sleep 0.2

    # ride
    echo -n -e '\xB9\x6C\x06' > /dev/ttyUSB0 # select pad:       6
    echo -n -e '\xB9\x6D\x00' > /dev/ttyUSB0 # MIDI curve type:  LINEAR
    echo -n -e '\xB9\x66\x0B' > /dev/ttyUSB0 # pad type:         CY5
    echo -n -e '\xB9\x67\x08' > /dev/ttyUSB0 # threshold:        8
    echo -n -e '\xB9\x68\x09' > /dev/ttyUSB0 # sensitivity:      9
    echo -n -e '\xB9\x6F\x03' > /dev/ttyUSB0 # rim/pos:          0 (none)
    echo -n -e '\xB9\x70\x33' > /dev/ttyUSB0 # midi note:        51
    echo -n -e '\xB9\x72\x00' > /dev/ttyUSB0 # crosstalk cancel: 0
    sleep 0.2

    # tom 2
    echo -n -e '\xB9\x6C\x07' > /dev/ttyUSB0 # select pad:       7
    echo -n -e '\xB9\x6D\x00' > /dev/ttyUSB0 # MIDI curve type:  LINEAR
    echo -n -e '\xB9\x66\x0C' > /dev/ttyUSB0 # pad type:         HD1TOM
    echo -n -e '\xB9\x67\x08' > /dev/ttyUSB0 # threshold:        8
    echo -n -e '\xB9\x68\x06' > /dev/ttyUSB0 # sensitivity:      6
    echo -n -e '\xB9\x6F\x03' > /dev/ttyUSB0 # rim/pos:          0 (none)
    echo -n -e '\xB9\x70\x2D' > /dev/ttyUSB0 # midi note:        45
    echo -n -e '\xB9\x72\x04' > /dev/ttyUSB0 # crosstalk cancel: 4
    sleep 0.2

    # tom 3
    echo -n -e '\xB9\x6C\x08' > /dev/ttyUSB0 # select pad:       8
    echo -n -e '\xB9\x6D\x00' > /dev/ttyUSB0 # MIDI curve type:  LINEAR
    echo -n -e '\xB9\x66\x0C' > /dev/ttyUSB0 # pad type:         HD1TOM
    echo -n -e '\xB9\x67\x08' > /dev/ttyUSB0 # threshold:        8
    echo -n -e '\xB9\x68\x06' > /dev/ttyUSB0 # sensitivity:      6
    echo -n -e '\xB9\x6F\x03' > /dev/ttyUSB0 # rim/pos:          0 (none)
    echo -n -e '\xB9\x70\x2B' > /dev/ttyUSB0 # midi note:        43
    echo -n -e '\xB9\x72\x04' > /dev/ttyUSB0 # crosstalk cancel: 4
  fi

  # start MIDI tool to convert serial MIDI to Jack Audio MIDI
  # note that to get access to /dev/ttyUSB0 we need to be in group tty/dialout
  mod-ttymidi/ttymidi -b 38400 &
  MIDIJACKPORT=ttymidi:MIDI_in
fi


# run Edrumulus ----------------------------------------------------------------
if [[ -v is_raspi ]]; then
  ./drumgizmo/drumgizmo/drumgizmo -l -L max=2,rampdown=0.02 -i jackmidi -I midimap=$KITMIDIMAPXML -o jackaudio $KITXML &
  sleep 20
else
  ./drumgizmo/drumgizmo/drumgizmo -i jackmidi -I midimap=$KITMIDIMAPXML -o jackaudio $KITXML &
  sleep 5
fi

jack_connect $KITJACKPORTLEFT system:playback_1
jack_connect $KITJACKPORTRIGHT system:playback_2


# either use direct MIDI connection or through EdrumulusGUI
if [[ -v is_gui ]]; then
  ./EdrumulusGUI DrumGizmo:drumgizmo_midiin
elif [[ -v is_jamulus ]]; then
  jack_connect "$MIDIJACKPORT" DrumGizmo:drumgizmo_midiin
  jack_disconnect $KITJACKPORTLEFT system:playback_1
  jack_disconnect $KITJACKPORTRIGHT system:playback_2
  if [ -z "$2" ]; then
    ./../../jamulus/Jamulus -n -i ../../jamulus/Jamulus.ini -c anygenre1.jamulus.io &
  else
    ./../../jamulus/Jamulus -n -i ../../jamulus/Jamulus.ini -c $2 &
  fi
  sleep 5
  jack_connect $KITJACKPORTLEFT "Jamulus:input left"
  jack_connect $KITJACKPORTRIGHT "Jamulus:input right"
  echo "###---------- PRESS ANY KEY TO TERMINATE THE EDRUMULUS/JAMULUS SESSION ---------###"
  read -n 1 -s -r -p ""
else
  jack_connect "$MIDIJACKPORT" DrumGizmo:drumgizmo_midiin
  echo "###---------- PRESS ANY KEY TO TERMINATE THE EDRUMULUS SESSION ---------###"
  read -n 1 -s -r -p ""
fi

killall drumgizmo

if [[ -v is_jamulus ]]; then
  killall Jamulus
fi

if [[ -v is_teensy ]]; then
  killall a2jmidid
else
  killall ttymidi
fi

