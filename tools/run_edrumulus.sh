#!/bin/bash

# Edrumulus Linux start script for using Drumgizmo
#
# Optional: If your Raspberry Pi has a button for ON/OFF connected to gpio pin3, add the following in /boot/config.txt
#           dtoverlay=gpio-shutdown,gpio_pin=3, active_low=1,gpio_pull=up
# Optional: Give audio group rights to do realtime
#           sudo sh -c 'echo "@audio   -  rtprio   95" >> /etc/security/limits.conf'
#           sudo sh -c 'echo "@audio   -  memlock  unlimited" >> /etc/security/limits.conf'

echo "Edrumulus Linux start script for using Drumgizmo (including setup/installation)"


# get environment --------------------------------------------------------------
NCORES=$(nproc)
gui_mode="" # GUI is the default (empty mode means that GUI is used)

# check of Teensy USB MIDI
if aconnect -l|grep -q Edrumulus; then
  echo "-> Edrumulus with Teensy hardware detected"
  is_teensy=true
fi

# check if we are in Jamulus session mode
if [[ "$1" == jamulus ]]; then
  echo "-> Jamulus session mode enabled"
  is_jamulus=true
  gui_mode="non_block"
fi
if [[ "$1" == uartjamulus ]]; then
  echo "-> Jamulus session mode enabled"
  is_jamulus=true
  is_raspi=true # UART connection to ESP32 is only supported on Raspberry Pi
  is_uart=true
  gui_mode="non_block"
fi

# check if direct serial connection to ESP32 shall be used or the default jack audio
if [[ "$1" == serial ]]; then
  echo "-> direct serial connection mode enabled"
  is_serial=true
  gui_mode="serial"
else
  gui_mode="${gui_mode} jack"
fi

# special mode: UART connection with console GUI
if [[ "$1" == uartgui ]]; then
  echo "-> UART GUI mode enabled"
  is_raspi=true # UART connection to ESP32 is only supported on Raspberry Pi
  is_uart=true
fi

# check if the WebUI GUI mode shall be used
if [[ "$1" == webui ]]; then
  echo "-> WebUI GUI mode enabled"
  is_raspi=true # WebUI GUI is only supported on Raspberry Pi
  is_uart=true
  gui_mode="webui jack"
fi


# install required packages ----------------------------------------------------
pkgs='git htop vim alsamixergui build-essential libasound2-dev jackd2 cmake libglib2.0-dev autoconf automake libtool lv2-dev xorg-dev libsndfile1-dev libjack-jackd2-dev libsmf-dev gettext a2jmidid libncurses5-dev ardour-lv2-plugins liblilv-dev python3-jack-client python3-rtmidi python3-serial'
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


# download and compile ecasound ------------------------------------------------
if [ -d "ecasound" ]; then
  echo "The ecasound directory is present, we assume it is compiled and ready to use. If not, delete the ecasound directory and call this script again."
else
  git clone https://github.com/kaivehmanen/ecasound.git
  cd ecasound
  ./autogen-vc.sh
  export CXXFLAGS="-g -std=c++11"
  ./configure
  make -j${NCORES}
  cd ..
fi


# drum kit setup ---------------------------------------------------------------
echo We assume that you have created the edrumuluskit with edrumulus/tools/mixdown_kits.m or PearlMMX is present

# if Pearl MMX drum kit is present, use this
if [ -d "PearlMMX" ]; then
  KITXML="PearlMMX/PearlMMX.xml"
  KITMIDIMAPXML="PearlMMX/Midimap.xml"
  use_ecasound=true
else
  # otherwise the default is to use the custom edrumuluskit
  if [ -d "edrumuluskit" ]; then
    KITXML="edrumuluskit/edrumuluskit.xml"
    KITMIDIMAPXML="edrumuluskit/edrumuluskit_midimap.xml"
    KITJACKPORTLEFT=DrumGizmo:0-left_channel
    KITJACKPORTRIGHT=DrumGizmo:1-right_channel
  fi
fi


# taken from "Raspberry Pi and realtime, low-latency audio" homepage at wiki.linuxaudio.org
#sudo service triggerhappy stop
#sudo service dbus stop
#sudo mount -o remount,size=128M /dev/shm


# start jack deamon ------------------------------------------------------------
# get first USB audio sound card device
ADEVICE=$(aplay -l|grep "USB Audio"|tail -1|cut -d' ' -f3)
echo "Using USB audio device: ${ADEVICE}"

# start the jack deamon (exit once all clients are closed with -T)
jackd -R -T --silent -P70 -t2000 -d alsa -dhw:${ADEVICE} -p 128 -n 3 -r 48000 -s >/dev/null 2>&1 &
sleep 1


# preparations -----------------------------------------------------------------
if [[ -v is_teensy ]] || [[ -v is_serial ]]; then
  # connect ALSA MIDI to Jack Audio MIDI
  a2jmidid -e >/dev/null 2>&1 &
  sleep 1

  # get Edrumulus MIDI name
  MIDIJACKPORT=$(jack_lsp|grep "(capture): Edrumulus MIDI")
else
  # start MIDI tool to convert serial MIDI to Jack Audio MIDI
  # note that to get access to /dev/ttyUSB0 you need to be in group tty/dialout
  MIDIJACKPORT=ttymidi:MIDI_in

  if [[ -v is_uart ]]; then
    mod-ttymidi/ttymidi -s /dev/serial0 -b 115200 &
    # on prototype 5 the ESP32 has to be started by setting GPIO9 to high
    sudo systemctl start pigpiod
    sleep 1
    pigs modes 9 w
    pigs w 9 1
  else
    if [ -r "/dev/ttyACM0" ]; then
      mod-ttymidi/ttymidi -b 38400 -s /dev/ttyACM0 & # ESP32-S3
    else
      mod-ttymidi/ttymidi -b 38400 &                 # ESP32
    fi
  fi
fi


# run Edrumulus ----------------------------------------------------------------
# maybe use the following for only closed match: -p close=1.0,position=1.0,diverse=0.0,random=0.0
# maybe use the following for adjusting the defaults a bit: -p close=0.95,position=1.0,diverse=0.1,random=0.04
if [[ -v is_raspi ]]; then
  ./drumgizmo/drumgizmo/drumgizmo -l -L max=5,rampdown=0.02 -p close=1.0,position=1.0,diverse=0.0,random=0.0 -i jackmidi -I midimap=$KITMIDIMAPXML -o jackaudio $KITXML &
else
  ./drumgizmo/drumgizmo/drumgizmo -p close=1.0,position=1.0,diverse=0.0,random=0.0 -i jackmidi -I midimap=$KITMIDIMAPXML -o jackaudio $KITXML &
fi

# wait for Drumgizmo to be fully loaded and available (check for jack audio ports)
while [[ $(jack_lsp) != *"DrumGizmo"* ]]; do sleep 0.1; done

if [[ -v use_ecasound ]]; then
  ./ecasound/ecasound/ecasound --server -q -s settings/*.ecs &
else
  jack_connect $KITJACKPORTLEFT system:playback_1
  jack_connect $KITJACKPORTRIGHT system:playback_2
fi
jack_connect "$MIDIJACKPORT" DrumGizmo:drumgizmo_midiin

# Edrumulus GUI must always be called even if no GUI is selected to load/store settings
./edrumulus_gui.py ${gui_mode}

if [[ -v is_jamulus ]]; then
  if [ -z "$2" ]; then
    ./../../jamulus/Jamulus -n -i ../../jamulus/Jamulus.ini -c anygenre1.jamulus.io &
  else
    ./../../jamulus/Jamulus -n -i ../../jamulus/Jamulus.ini -c $2 &
  fi
  sleep 15
  if [[ -v use_ecasound ]]; then
    jack_disconnect ecasound:out_1 system:playback_1
    jack_disconnect ecasound:out_2 system:playback_2
    jack_connect ecasound:out_1 "Jamulus:input left"
    jack_connect ecasound:out_2 "Jamulus:input right"
  else
    jack_disconnect $KITJACKPORTLEFT system:playback_1
    jack_disconnect $KITJACKPORTRIGHT system:playback_2
    jack_connect $KITJACKPORTLEFT "Jamulus:input left"
    jack_connect $KITJACKPORTRIGHT "Jamulus:input right"
  fi
  echo "###---------- PRESS ANY KEY TO TERMINATE THE EDRUMULUS/JAMULUS SESSION ---------###"
  read -n 1 -s -r -p ""
fi


# clean up ---------------------------------------------------------------------
killall drumgizmo

if [[ -v is_jamulus ]]; then
  killall Jamulus
fi

if [[ -v is_teensy ]] || [[ -v is_serial ]]; then
  killall a2jmidid
else
  killall ttymidi
fi

if [[ -v is_uart ]]; then
  sudo systemctl stop pigpiod
fi

if [[ -v use_ecasound ]]; then
  killall ecasound
fi

