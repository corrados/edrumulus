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
const int number_pads = 1;//5; // <- tested: with current code the ESP32 can only handle up to 5 pads


void setup()
{
#ifdef USE_MIDI
  MIDI.begin();
  Serial.begin ( 38400 );
#else
  Serial.begin ( 115200 );
#endif

  // analog pins are 34 and 35, we also want to use the on-board LED as an overload indicator
  const int analog_pins[]         = { 34, 35 };
  const int analog_pins_rimshot[] = { -1, -1 }; // no rim shot
  edrumulus.setup ( number_pads, analog_pins, analog_pins_rimshot, 2 );
}


void loop()
{
  // this function is blocking at the system sampling rate
  edrumulus.process();

#ifdef USE_MIDI
  // first pad
  if ( number_pads > 0 )
  {
    if ( edrumulus.get_peak_found ( 0 ) )
    {
      MIDI.sendControlChange ( 16,                                        edrumulus.get_midi_pos ( 0 ),      10 ); // positional sensing
      MIDI.sendNoteOn        ( edrumulus.get_is_rim_shot ( 0 ) ? 40 : 38, edrumulus.get_midi_velocity ( 0 ), 10 ); // (note, velocity, channel)
      MIDI.sendNoteOff       ( 38,                                        0,                                 10 ); // we need a note off
    }
  }

  // second pad
  if ( number_pads > 1 )
  {
    if ( edrumulus.get_peak_found ( 1 ) )
    {
      MIDI.sendControlChange ( 16,                                        edrumulus.get_midi_pos ( 1 ),      10 ); // positional sensing
      MIDI.sendNoteOn        ( edrumulus.get_is_rim_shot ( 1 ) ? 48 : 48, edrumulus.get_midi_velocity ( 1 ), 10 ); // (note, velocity, channel)
      MIDI.sendNoteOff       ( 38,                                        0,                                 10 ); // we need a note off
    }
  }
#endif

/*
// TEST receiving MIDI messages to change the pad settings: Virtual MIDI Piano Keyboard -> loopMIDI -> Hairless MIDI
if ( MIDI.read ( 1 ) ) // read only on channel 1
{
  if ( MIDI.getType() == midi::ControlChange )
  {
    bool           is_used    = false;
    midi::DataByte controller = MIDI.getData1();
    midi::DataByte value      = MIDI.getData2();

    // controller 1: threshold
    if ( controller == 1 )
    {
      edrumulus.set_velocity_threshold ( 0, static_cast<int> ( static_cast<float> ( value ) / 127 * 31 ) );
      is_used = true;
    }

    // controller 2: sensitivity
    if ( controller == 2 )
    {
      edrumulus.set_velocity_sensitivity ( 0, static_cast<int> ( static_cast<float> ( value ) / 127 * 31 ) );
      is_used = true;
    }

// TEST some audio feedback that the settings was correctly received
if ( is_used )
{
  MIDI.sendNoteOn  ( 48, value, 10 );
  MIDI.sendNoteOff ( 48, 0,     10 );
}

  }
}
*/

/*
// For debugging: measure the sampling rate and optionally output it to the serial interface
static int           prev_micros_cnt = 0;
static unsigned long prev_micros     = micros();
if ( prev_micros_cnt >= 10000 )
{
  Serial.println ( 1.0f / ( micros() - prev_micros ) * 10000 * 1e6f, 7 );
  prev_micros_cnt = 0;
  prev_micros     = micros();
}
prev_micros_cnt++;
*/
}
