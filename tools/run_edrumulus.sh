#!/bin/bash

echo "This script prepares a Raspberry Pi for Edrumulus usage"


# install required packages ----------------------------------------------------
pkgs='git htop alsamixergui build-essential libasound2-dev jackd2 cmake libglib2.0-dev autoconf automake libtool lv2-dev xorg-dev libsndfile1-dev libjack-jackd2-dev libsmf-dev gettext'
if ! dpkg -s $pkgs >/dev/null 2>&1; then
  read -p "Do you want to install missing packages? " -n 1 -r
  echo
  if [[ $REPLY =~ ^[Yy]$ ]]; then
    sudo apt-get install $pkgs -y
  fi
fi


## download and compile ttymidi -------------------------------------------------
#if [ -d "ttymidi" ]; then
#  echo "The ttyMIDI directory is present, we assume it is compiled and ready to use. If not, delete the ttyMIDI #directory and call this script again."
#else
#  git clone https://github.com/cjbarnes18/ttymidi.git
#  cd ttymidi
#  make
#  cd ..
#fi


# download and compile mod-ttymidi ---------------------------------------------
if [ -d "mod-ttymidi" ]; then
  echo "The ttyMIDI directory is present, we assume it is compiled and ready to use. If not, delete the mod-ttymidi directory and call this script again."
else
  git clone https://github.com/moddevices/mod-ttymidi.git
  cd mod-ttymidi
  make
  cd ..
fi


# download and compile Drumgizmo -----------------------------------------------
if [ -d "drumgizmo" ]; then
  echo "The Drumgizmo directory is present, we assume it is compiled and ready to use. If not, delete the Drumgizmo directory and call this script again."
else
  git clone http://cgit.drumgizmo.org/drumgizmo.git
  cd drumgizmo
  git checkout develop
  git submodule update --init
  ./autogen.sh
  ./configure --prefix=$PWD/install --with-lv2dir=$HOME/.lv2 --enable-lv2
  make -j4
  cd ..
fi


# TODO download Drumgizmo drum kit into the drumgizmo directory, e.g., edrumulus/tools/drumgizmo/DRSKit/


# get first USB audio sound card device
ADEVICE=$(aplay -l|grep "USB Audio"|tail -1|cut -d' ' -f3)
echo "Using USB audio device: ${ADEVICE}"

# start the jack deamon
jackd -R -T --silent -P70 -t2000 -d alsa -dhw:${ADEVICE} -p 128 -n 3 -r 48000 -s &>/dev/null &
sleep 1

# note that to get access to /dev/ttyUSB0 we need to be in group tty/dialout
mod-ttymidi/ttymidi -b 38400 &

./drumgizmo/drumgizmo/drumgizmo -s -S limit=500M -i jackmidi -I midimap=drumgizmo/DRSKit/DRSKit_midimap_edrumulus.xml -o jackaudio drumgizmo/DRSKit/DRSKit_edrumulus.xml &
sleep 10

jack_connect ttymidi:MIDI_in DrumGizmo:drumgizmo_midiin
jack_connect DrumGizmo:0-AmbL system:playback_1
jack_connect DrumGizmo:1-AmbR system:playback_2

echo "###---------- PRESS ANY KEY TO TERMINATE THE EDRUMULUS SESSION ---------###"
read -n 1 -s -r -p ""
killall mod-ttymidi
killall drumgizmo
killall jackd

