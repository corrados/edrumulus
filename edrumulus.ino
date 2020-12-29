/******************************************************************************\
 * Copyright (c) 2020-2020
 * Author: Volker Fischer
 ******************************************************************************
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
\******************************************************************************/

#define USE_MIDI

#include "edrumulus.h"

#ifdef USE_MIDI
#include <MIDI.h>                   // Hairless USB MIDI
MIDI_CREATE_DEFAULT_INSTANCE();     // Hairless USB MIDI
//#include <BLEMIDI_Transport.h>                 // BLE MIDI
//#include <hardware/BLEMIDI_ESP32.h>            // BLE MIDI
//BLEMIDI_CREATE_INSTANCE ( "Edrumulus", MIDI ); // BLE MIDI
#endif

Edrumulus edrumulus;


void setup()
{
#ifdef USE_MIDI
  MIDI.begin();
  Serial.begin ( 38400 );
#else
  Serial.begin ( 115200 );
#endif

  // analog pins are 34 and 35, we also want to use the on-board LED as an overload indicator
  //edrumulus.setup ( 34, 35, 2 );
  edrumulus.setup ( 34, -1, 2 ); // no rim shot
}


void loop()
{
  int  midi_velocity, midi_pos;
  bool is_rim_shot;

  if ( edrumulus.process ( midi_velocity, midi_pos, is_rim_shot ) )
  {
#ifdef USE_MIDI
    MIDI.sendControlChange ( 16, midi_pos,      10 );                    // positional sensing
    MIDI.sendNoteOn        ( is_rim_shot ? 40 : 38, midi_velocity, 10 ); // (note, velocity, channel)
    MIDI.sendNoteOff       ( 38, 0,             10 );
#endif
  }

/*
// For debugging: measure the sampling rate and optionally output it to the serial interface
static int           prev_micros_cnt = 0;
static unsigned long prev_micros     = micros();
if ( prev_micros_cnt >= 10000 )
{
  Serial.println ( 1.0f / ( micros() - prev_micros ) * 1e6f, 7 );
  prev_micros_cnt = 0;
  prev_micros     = micros();
}
prev_micros_cnt++;
*/
}
