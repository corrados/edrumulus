// Copyright (c) 2020-2026 Volker Fischer
// SPDX-License-Identifier: GPL-2.0-or-later

#ifdef TEENSYDUINO

#  include <usb_names.h>

// define the name of the Teensy USB MIDI device showing up in the operating system
#  define MANUFACTURER_NAME                  \
    {                                        \
      'v', 'o', 'f', 'i', ' ', 't', 'e', 'c' \
    }
#  define MANUFACTURER_NAME_LEN 8
#  define PRODUCT_NAME                            \
    {                                             \
      'E', 'd', 'r', 'u', 'm', 'u', 'l', 'u', 's' \
    }
#  define PRODUCT_NAME_LEN 9

struct usb_string_descriptor_struct usb_string_manufacturer_name = {
    2 + MANUFACTURER_NAME_LEN * 2,
    3,
    MANUFACTURER_NAME};

struct usb_string_descriptor_struct usb_string_product_name = {
    2 + PRODUCT_NAME_LEN * 2,
    3,
    PRODUCT_NAME};

#endif
