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

//#define USE_MIDI

#include "edrumulus.h"

#ifdef USE_MIDI
#include <MIDI.h>                   // Hairless USB MIDI
MIDI_CREATE_DEFAULT_INSTANCE();     // Hairless USB MIDI
//#include <BLEMIDI_Transport.h>                 // BLE MIDI
//#include <hardware/BLEMIDI_ESP32.h>            // BLE MIDI
//BLEMIDI_CREATE_INSTANCE ( "Edrumulus", MIDI ); // BLE MIDI
#endif

Edrumulus edrumulus;
const int number_pads      = 6;//5; // <- tested: with current code the ESP32 can only handle up to 5 pads
const int status_LED_pin   = 2; // internal LED used for overload indicator
const int midi_channel     = 10; // default for edrums is 10
bool      is_status_LED_on = false;


void setup()
{
#ifdef USE_MIDI
  MIDI.begin();
  Serial.begin ( 38400 );
#else
  Serial.begin ( 115200 );
#endif

  // analog pins setup:             snare | kick | hi-hat | hi-hat-ctrl | tom1 | tom2 | crash | ride | tom3
  const int analog_pins[]         = { 34,    32,     33,       26,         27,    14,    12,      4,    15 };
//  const int analog_pins_rimshot[] = { 35,    -1,     25,       -1,         -1,    -1,    13,      2,    -1 };
  const int analog_pins_rimshot[] = { 35,    -1,     -1,       -1,         -1,    -1,    -1,      -1,    -1 };

  edrumulus.setup ( number_pads, analog_pins, analog_pins_rimshot );

  // some fundamental settings which do not change during operation
  edrumulus.set_pad_type          ( 0, Edrumulus::PD120 );
  edrumulus.set_midi_notes        ( 0, 38, 40 ); // snare
  edrumulus.set_rim_shot_is_used  ( 0, true );
  edrumulus.set_pos_sense_is_used ( 0, true );

  edrumulus.set_midi_notes ( 1, 36, 36 ); // kick
  edrumulus.set_midi_notes ( 2, 42, 42 ); // TODO hi-hat
  edrumulus.set_midi_notes ( 3, 22, 22 ); // TODO hi-hat-ctrl
  edrumulus.set_midi_notes ( 4, 48, 48 ); // tom 1
  edrumulus.set_midi_notes ( 5, 45, 45 ); // tom 2
  edrumulus.set_midi_notes ( 6, 55, 49 ); // crash
  edrumulus.set_midi_notes ( 7, 51, 66 ); // ride
  edrumulus.set_midi_notes ( 8, 41, 41 ); // tom 3

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
  // send MIDI note to drum synthesizer
  for ( int pad_idx = 0; pad_idx < number_pads; pad_idx++ )
  {
    if ( edrumulus.get_peak_found ( pad_idx ) )
    {
      const int midi_pos      = edrumulus.get_midi_pos ( pad_idx );
      const int midi_velocity = edrumulus.get_midi_velocity ( pad_idx );
      const int midi_note     = edrumulus.get_midi_note ( pad_idx );

      MIDI.sendControlChange ( 16,        midi_pos,      midi_channel ); // positional sensing
      MIDI.sendNoteOn        ( midi_note, midi_velocity, midi_channel ); // (note, velocity, channel)
      MIDI.sendNoteOff       ( midi_note, 0,             midi_channel ); // we need a note off
    }
  }

  // receiving MIDI messages to change the pad settings: edrumuluscontrol.m -> loopMIDI -> Hairless MIDI
  if ( MIDI.read ( midi_channel ) )
  {
    if ( MIDI.getType() == midi::ControlChange )
    {
      bool           is_used    = false;
      midi::DataByte controller = MIDI.getData1();
      midi::DataByte value      = MIDI.getData2();

      // controller 102: pad type
      if ( controller == 102 )
      {
        switch ( value )
        {
          case 0: edrumulus.set_pad_type ( 0, Edrumulus::PD120 ); break;
          case 1: edrumulus.set_pad_type ( 0, Edrumulus::PD80R ); break;
          case 2: edrumulus.set_pad_type ( 0, Edrumulus::PD8 );   break;
        }
        is_used = true;
      }

      // controller 103: threshold
      if ( controller == 103 )
      {
        edrumulus.set_velocity_threshold ( 0, value );
        is_used = true;
      }
  
      // controller 104: sensitivity
      if ( controller == 104 )
      {
        edrumulus.set_velocity_sensitivity ( 0, value );
        is_used = true;
      }
  
      // controller 105: positional sensing threshold
      if ( controller == 105 )
      {
        edrumulus.set_pos_threshold ( 0, value );
        is_used = true;
      }
  
      // controller 106: positional sensing sensitivity
      if ( controller == 106 )
      {
        edrumulus.set_pos_sensitivity ( 0, value );
        is_used = true;
      }

      // controller 107: rim shot threshold
      if ( controller == 107 )
      {
        edrumulus.set_rim_shot_treshold ( 0, value );
        is_used = true;
      }

      // give some feedback that the setting was correctly received
      if ( is_used )
      {
        MIDI.sendNoteOff ( controller, value, 1 ); // can be checked, e.g., in the log file
      }
    }
  }
#endif
}
