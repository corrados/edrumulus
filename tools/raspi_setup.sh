#!/bin/bash

echo "This script prepares a Raspberry Pi for Edrumulus usage"

# install required packages
pkgs='alsamixergui build-essential qt5-default libasound2-dev cmake libglib2.0-dev autoconf automake libtool lv2-dev xorg-dev libsndfile1-dev libjack-dev libsmf-dev libasound2-dev gettext'
if ! dpkg -s $pkgs >/dev/null 2>&1; then
  read -p "Do you want to install missing packages? " -n 1 -r
  echo
  if [[ $REPLY =~ ^[Yy]$ ]]; then
    sudo apt-get install $pkgs -y
  fi
fi

