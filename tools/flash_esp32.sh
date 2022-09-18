#!/bin/bash

echo "This script flashes the ESP32 micro controller installed on Edrumulus Prototype 5."

# check if we are running on a Raspberry Pi by checking if the user name is pi
if [ ! $USER = "pi" ]; then
  echo "Error: This script must be run on a Raspberry pi."
  exit 1
fi


