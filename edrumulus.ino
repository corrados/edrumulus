/******************************************************************************\
 * Copyright (c) 2020-2021
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
const int number_pads      = 1;//5; // <- tested: with current code the ESP32 can only handle up to 5 pads
const int status_LED_pin   = 2; // internal LED used for overload indicator
bool      is_status_LED_on = false;


void setup()
{
#ifdef USE_MIDI
  MIDI.begin();
  Serial.begin ( 38400 );
#else
  Serial.begin ( 115200 );
#endif

  // analog pins are 34 and 35
  const int analog_pins[]         = { 34, 35 };
  const int analog_pins_rimshot[] = { 35, -1 }; // optional rim shot
  edrumulus.setup ( number_pads, analog_pins, analog_pins_rimshot );

  // initialize GPIO port for status LED
  pinMode ( status_LED_pin, OUTPUT );
}


void loop()
{
  // this function is blocking at the system sampling rate
  edrumulus.process();

  // status LED handling
  if ( edrumulus.get_status_is_overload() || edrumulus.get_status_is_error() )
  {
    if ( !is_status_LED_on )
    {
      digitalWrite ( status_LED_pin, HIGH );
      is_status_LED_on = true;
    }
  }
  else
  {
    if ( is_status_LED_on )
    {
      digitalWrite ( status_LED_pin, LOW );
      is_status_LED_on = false;
    }
  }

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

  // receiving MIDI messages to change the pad settings: edrumuluscontrol.m -> loopMIDI -> Hairless MIDI
  if ( MIDI.read ( 1 ) ) // read only on channel 1
  {
    if ( MIDI.getType() == midi::ControlChange )
    {
      bool           is_used    = false;
      midi::DataByte controller = MIDI.getData1();
      midi::DataByte value      = MIDI.getData2();

      // controller 1: pad type
      if ( controller == 1 )
      {
        switch ( value )
        {
          case 0: edrumulus.set_pad_type ( 0, Edrumulus::PD120 ); break;
          case 1: edrumulus.set_pad_type ( 0, Edrumulus::PD80R ); break;
          case 2: edrumulus.set_pad_type ( 0, Edrumulus::PD8 );   break;
        }
        is_used = true;
      }

      // controller 2: threshold
      if ( controller == 2 )
      {
        edrumulus.set_velocity_threshold ( 0, value );
        is_used = true;
      }
  
      // controller 3: sensitivity
      if ( controller == 3 )
      {
        edrumulus.set_velocity_sensitivity ( 0, value );
        is_used = true;
      }
  
      // controller 4: positional sensing threshold
      if ( controller == 4 )
      {
        edrumulus.set_pos_threshold ( 0, value );
        is_used = true;
      }
  
      // controller 5: positional sensing sensitivity
      if ( controller == 5 )
      {
        edrumulus.set_pos_sensitivity ( 0, value );
        is_used = true;
      }

      // controller 6: rim shot threshold
      if ( controller == 6 )
      {
        edrumulus.set_rim_shot_treshold ( 0, value );
        is_used = true;
      }

      // give some audio feedback that the setting was correctly received
      if ( is_used )
      {
        MIDI.sendNoteOn  ( 30 + controller, value, 10 );
        MIDI.sendNoteOff ( 30 + controller, 0,     10 );
      }
    }
  }
#endif
}
