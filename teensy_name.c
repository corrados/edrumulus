/******************************************************************************\
 * Copyright (c) 2020-2024
 * Author(s): Volker Fischer
 ******************************************************************************
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
\******************************************************************************/

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
