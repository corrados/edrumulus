#!/bin/bash

echo "This script prepares a Raspberry Pi for Edrumulus usage"


# install required packages ----------------------------------------------------
pkgs='git htop alsamixergui build-essential libasound2-dev cmake libglib2.0-dev autoconf automake libtool lv2-dev xorg-dev libsndfile1-dev libjack-jackd2-dev libsmf-dev gettext'
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


# TODO download Drumgizmo drum kit





# TODO

# to get access to /dev/ttyUSB0 we need to be in group tty
# TODO first check if we are already in group, otherwise add to the group
# TODO it seems that on the raspi the user pi already has dialout rights
#sudo usermod -a -G tty pi
#sudo usermod -a -G dialout pi

# ~/edrumulus/tools/ttymidi/ttymidi -s /dev/ttyUSB0 -b 38400 -v

#./tools/drumgizmo/drumgizmo --no-resampling -i alsamidi -I midimap=~/edrumulus/tools/DRSKit/Midimap_minimal.xml -o alsa -O frames=128,periods=2 ~/edrumulus/tools/DRSKit/DRSKit_minimal.xml

# get the MIDI channel numbers of aconnect
#TTYMIDI_NUM=$(aconnect -l|grep "ttymidi"|cut -d' ' -f2|cut -d':' -f1)





