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
if [[ "$1" == jamulus ]]; then
  echo "-> Jamulus session mode enabled"
  is_jamulus=true
fi

# check if the GUI mode shall be used
if [[ "$1" == gui ]]; then
  echo "-> GUI mode enabled"
  is_gui=true
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


# preparations -----------------------------------------------------------------
if [[ -v is_teensy ]]; then
  # connect ALSA MIDI to Jack Audio MIDI
  a2jmidid -e >/dev/null 2>&1 &
  sleep 1

  # get Edrumulus MIDI name
  MIDIJACKPORT=$(jack_lsp|grep "(capture): Edrumulus MIDI")
else
  # start MIDI tool to convert serial MIDI to Jack Audio MIDI
  # note that to get access to /dev/ttyUSB0 you need to be in group tty/dialout
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

