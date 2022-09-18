#!/bin/bash

echo "This script flashes the ESP32 micro controller installed on Edrumulus Prototype 5."

# check if we are running on a Raspberry Pi by checking if the user name is pi
if [ ! $USER = "pi" ]; then
  echo "Error: This script must be run on a Raspberry pi."
  exit 1
fi


# install required packages ----------------------------------------------------
pkgs='git python3-pip'
if ! dpkg -s $pkgs >/dev/null 2>&1; then
  read -p "Do you want to install missing packages? " -n 1 -r
  echo
  if [[ $REPLY =~ ^[Yy]$ ]]; then
    sudo apt-get install $pkgs -y
    sudo pip install esptool
  fi
fi


# download ESP32 IDF -----------------------------------------------------------
if [ -d "esp-idf" ]; then
  echo "The esp-idf directory is present. We assume the ESP32 IDF is correctly downloaded."
else
  git clone --recursive https://github.com/espressif/esp-idf.git
fi


# prepare the ESP32 for flashing -----------------------------------------------
# We assume that the following hardware wiring is used:
# PI-GPIO9 -> ESP32 EN
# PI-GPIO4 -> ESP32 IO0
sudo systemctl start pigpiod
pigs modes 9 w
pigs modes 4 w

# to flash, set IO0 to low and reset ESP32
pigs w 4 0
pigs w 9 0
sleep 1
pigs w 9 1

esptool.py -p /dev/serial0 flash_id

#esptool.py -p /dev/serial0 erase_flash

#esptool.py --chip esp32 --port /dev/serial0 write_flash -z --flash_mode dio --flash_freq 80m --flash_size detect 0xe000 boot_app0.bin 0x1000 bootloader_dio_80m.bin 0x10000 edrumulus.ino.bin 0x8000 edrumulus.ino.partitions.bin 

# set IO0 to non-flash mode and reboot ESP32
pigs w 4 1
pigs w 9 0
sleep 1
pigs w 9 1



