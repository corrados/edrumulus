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
  git clone git://git.drumgizmo.org/drumgizmo.git
  cd drumgizmo
  git checkout develop
  git submodule update --init
  ./autogen.sh
  ./configure --prefix=$PWD/install --with-lv2dir=$HOME/.lv2 --enable-lv2
  make -j4
  cd ..
fi


# TODO download Drumgizmo drum kit into the drumgizmo directory, e.g., edrumulus/tools/drumgizmo/DRSKit/
echo We assume that you have downloaded and unzipped the DRSKit in the drumgizmo directory.

# we now assume that the DRSKit was already downloaded and unzipped and
# we copy our special configuration files in that directory
if [[ -d "drumgizmo/DRSKit" && ! -f "drumgizmo/DRSKit/DRSKit_edrumulus.xml" && ! -f "drumgizmo/DRSKit/DRSKit_midimap_edrumulus.xml" ]]; then
  cp DRSKit_edrumulus.xml DRSKit_midimap_edrumulus.xml drumgizmo/DRSKit/
fi


# write Edrumulus trigger configuration ----------------------------------------
stty 38400 -F /dev/ttyUSB0

# snare
echo -n -e '\xB9\x6C\x00' > /dev/ttyUSB0 # select pad: 0
echo -n -e '\xB9\x66\x02' > /dev/ttyUSB0 # pad type: PD8
echo -n -e '\xB9\x67\x03' > /dev/ttyUSB0 # threshold
echo -n -e '\xB9\x68\x08' > /dev/ttyUSB0 # sensitivity
echo -n -e '\xB9\x6B\x10' > /dev/ttyUSB0 # rim shot threshold
echo -n -e '\xB9\x69\x1A' > /dev/ttyUSB0 # positional sensing threshold
echo -n -e '\xB9\x6A\x0B' > /dev/ttyUSB0 # positional sensing sensitivity
echo -n -e '\xB9\x6F\x03' > /dev/ttyUSB0 # rim/pos: both, rim shot and positional sensing

# kick
echo -n -e '\xB9\x6C\x01' > /dev/ttyUSB0 # select pad: 1
echo -n -e '\xB9\x66\x06' > /dev/ttyUSB0 # pad type: KD7
echo -n -e '\xB9\x67\x09' > /dev/ttyUSB0 # threshold
echo -n -e '\xB9\x68\x09' > /dev/ttyUSB0 # sensitivity

# Hi-Hat
echo -n -e '\xB9\x6C\x02' > /dev/ttyUSB0 # select pad: 2
echo -n -e '\xB9\x66\x02' > /dev/ttyUSB0 # pad type: PD8
echo -n -e '\xB9\x67\x04' > /dev/ttyUSB0 # threshold
echo -n -e '\xB9\x68\x08' > /dev/ttyUSB0 # sensitivity
echo -n -e '\xB9\x6F\x01' > /dev/ttyUSB0 # rim/pos: enable rim shot

# Hi-Hat control
echo -n -e '\xB9\x6C\x03' > /dev/ttyUSB0 # select pad: 3
echo -n -e '\xB9\x66\x03' > /dev/ttyUSB0 # pad type: FD8
echo -n -e '\xB9\x67\x05' > /dev/ttyUSB0 # threshold
echo -n -e '\xB9\x68\x00' > /dev/ttyUSB0 # sensitivity

# crash
echo -n -e '\xB9\x6C\x04' > /dev/ttyUSB0 # select pad: 4
echo -n -e '\xB9\x66\x02' > /dev/ttyUSB0 # pad type: PD8
echo -n -e '\xB9\x67\x13' > /dev/ttyUSB0 # threshold
echo -n -e '\xB9\x68\x15' > /dev/ttyUSB0 # sensitivity
echo -n -e '\xB9\x6F\x01' > /dev/ttyUSB0 # rim/pos: enable rim shot

# tom 1
echo -n -e '\xB9\x6C\x05' > /dev/ttyUSB0 # select pad: 5
echo -n -e '\xB9\x66\x01' > /dev/ttyUSB0 # pad type: PD80R
echo -n -e '\xB9\x67\x09' > /dev/ttyUSB0 # threshold
echo -n -e '\xB9\x68\x00' > /dev/ttyUSB0 # sensitivity

# ride
echo -n -e '\xB9\x6C\x06' > /dev/ttyUSB0 # select pad: 6
echo -n -e '\xB9\x66\x02' > /dev/ttyUSB0 # pad type: PD8
echo -n -e '\xB9\x67\x12' > /dev/ttyUSB0 # threshold
echo -n -e '\xB9\x68\x15' > /dev/ttyUSB0 # sensitivity
echo -n -e '\xB9\x6F\x01' > /dev/ttyUSB0 # rim/pos: enable rim shot

# tom 2
echo -n -e '\xB9\x6C\x07' > /dev/ttyUSB0 # select pad: 7
echo -n -e '\xB9\x66\x01' > /dev/ttyUSB0 # pad type: PD80R
echo -n -e '\xB9\x67\x12' > /dev/ttyUSB0 # threshold
echo -n -e '\xB9\x68\x00' > /dev/ttyUSB0 # sensitivity


# run Edrumulus ----------------------------------------------------------------
# get first USB audio sound card device
ADEVICE=$(aplay -l|grep "USB Audio"|tail -1|cut -d' ' -f3)
echo "Using USB audio device: ${ADEVICE}"

# start the jack deamon
jackd -R -T --silent -P70 -t2000 -d alsa -dhw:${ADEVICE} -p 128 -n 3 -r 48000 -s &>/dev/null &
sleep 1

# note that to get access to /dev/ttyUSB0 we need to be in group tty/dialout
mod-ttymidi/ttymidi -b 38400 &

./drumgizmo/drumgizmo/drumgizmo --async-load -s -S limit=500M -l -L max=3,rampdown=0.1 -i jackmidi -I midimap=drumgizmo/DRSKit/DRSKit_midimap_edrumulus.xml -o jackaudio drumgizmo/DRSKit/DRSKit_edrumulus.xml &
sleep 10

jack_connect ttymidi:MIDI_in DrumGizmo:drumgizmo_midiin
jack_connect DrumGizmo:0-AmbL system:playback_1
jack_connect DrumGizmo:1-AmbR system:playback_2

echo "###---------- PRESS ANY KEY TO TERMINATE THE EDRUMULUS SESSION ---------###"
read -n 1 -s -r -p ""
killall mod-ttymidi
killall drumgizmo
killall jackd

