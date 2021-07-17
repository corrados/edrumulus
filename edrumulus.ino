/******************************************************************************\
 * Copyright (c) 2020-2021
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

#define USE_MIDI

#include "edrumulus.h"

#ifdef USE_MIDI
#ifdef ESP_PLATFORM
# include <MIDI.h>
MIDI_CREATE_DEFAULT_INSTANCE();
# define MYMIDI                     MIDI
# define MIDI_CONTROL_CHANGE_TYPE   midi::ControlChange
#endif
#ifdef TEENSYDUINO
# define MYMIDI                     usbMIDI
# define MIDI_CONTROL_CHANGE_TYPE   usbMIDI.ControlChange
#endif
#endif

Edrumulus edrumulus;
const int number_pads       = 8;
const int status_LED_pin    = BOARD_LED_PIN; // internal LED used for overload indicator
const int midi_channel      = 10; // default for edrums is 10
const int hihat_pad_idx     = 2;
const int hihatctrl_pad_idx = 3;
bool      is_status_LED_on  = false;
int       selected_pad      = 0;


void setup()
{
#ifdef USE_MIDI
  MYMIDI.begin();
  Serial.begin ( 38400 );
#else
  Serial.begin ( 115200 );
#endif

#ifdef ESP_PLATFORM
  // NOTE: avoid ESP32 GPIO 25/26 for piezo inputs since they are DAC pins which cause an incorrect DC offset
  //       estimation and DC offset drift which makes the spike cancellation algorithm not working correctly
  // analog pins setup:             snare | kick | hi-hat | hi-hat-ctrl | crash | tom1 | ride | tom2 | tom3
  const int analog_pins[]         = { 36,    33,     32,       25,         34,     39,    27,    12,    15 };
  const int analog_pins_rimshot[] = { 35,    -1,     26,       -1,         14,     -1,    13,    -1,    -1 };
#endif
#ifdef TEENSYDUINO
  // analog pins setup:             snare | kick | hi-hat | hi-hat-ctrl | crash | tom1 | ride | tom2
  const int analog_pins[]         = { 10,    11,    12,        13,          1,      6,     4,     5 };
  const int analog_pins_rimshot[] = {  9,    -1,     0,        -1,          3,      8,     2,     7 };
#endif

  edrumulus.setup ( number_pads, analog_pins, analog_pins_rimshot );

  // some fundamental settings which do not change during operation
  edrumulus.set_midi_notes      ( 0, 38, 40 ); // snare
  edrumulus.set_midi_notes      ( 1, 36, 36 ); // kick
  edrumulus.set_midi_notes      ( hihat_pad_idx, 22 /*42*/, 22 );
  edrumulus.set_midi_notes_open ( hihat_pad_idx, 26 /*46*/, 26 );
  edrumulus.set_midi_ctrl_ch    ( hihatctrl_pad_idx, 4 ); // Hi-Hat control
  edrumulus.set_midi_notes      ( 4, 49, 55 ); // crash
  edrumulus.set_midi_notes      ( 5, 48, 50 ); // tom 1
  edrumulus.set_midi_notes      ( 6, 51, 66 ); // ride
  edrumulus.set_midi_notes      ( 7, 45, 47 ); // tom 2
  edrumulus.set_midi_notes      ( 8, 43, 58 ); // tom 3

// my prototype setup configuration...
  edrumulus.set_pad_type          ( 0, Edrumulus::PD8 ); // snare
  edrumulus.set_rim_shot_is_used  ( 0, true );
  edrumulus.set_pos_sense_is_used ( 0, true );
  edrumulus.set_pad_type          ( 1, Edrumulus::KD7 ); // kick
  edrumulus.set_pad_type          ( 2, Edrumulus::PD8 ); // Hi-Hat, using rim switch
  edrumulus.set_rim_shot_is_used  ( 2, true );
  edrumulus.set_pad_type          ( 3, Edrumulus::FD8 ); // Hi-Hat-ctrl
  edrumulus.set_pad_type          ( 4, Edrumulus::CY8 ); // crash, using rim switch
  edrumulus.set_rim_shot_is_used  ( 4, true );
  edrumulus.set_pad_type          ( 5, Edrumulus::PD8 ); // tom 1
  edrumulus.set_pad_type          ( 6, Edrumulus::PD8 ); // ride, using rim switch
  edrumulus.set_rim_shot_is_used  ( 6, true );
  edrumulus.set_pad_type          ( 7, Edrumulus::PD8 ); // tom 2

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
      // get current MIDI note and velocity (maybe note will be overwritten later on)
      const int midi_velocity = edrumulus.get_midi_velocity ( pad_idx );
      int       midi_note     = edrumulus.get_midi_note     ( pad_idx );

      // send midi positional control message if positional sensing is enabled for the current pad
      if ( edrumulus.get_pos_sense_is_used ( pad_idx ) )
      {
        const int midi_pos = edrumulus.get_midi_pos ( pad_idx );
        MYMIDI.sendControlChange ( 16, midi_pos, midi_channel ); // positional sensing
      }

      // send Hi-Hat control message right before each Hi-Hat pad hit
      if ( pad_idx == hihat_pad_idx )
      {
        const int midi_ctrl_ch    = edrumulus.get_midi_ctrl_ch    ( hihatctrl_pad_idx );
        const int midi_ctrl_value = edrumulus.get_midi_ctrl_value ( hihatctrl_pad_idx );
        MYMIDI.sendControlChange ( midi_ctrl_ch, midi_ctrl_value, midi_channel );

        // if Hi-Hat is open, overwrite MIDI note
// TODO define the threshold somewhere else, maybe inside of the Edrumulus class
        if ( midi_ctrl_value < 100 )
        {
          midi_note = edrumulus.get_midi_note_open ( pad_idx );
        }
      }

      MYMIDI.sendNoteOn  ( midi_note, midi_velocity, midi_channel ); // (note, velocity, channel)
      MYMIDI.sendNoteOff ( midi_note, 0,             midi_channel ); // we need a note off
    }

    if ( edrumulus.get_control_found ( pad_idx ) )
    {
      const int midi_ctrl_ch    = edrumulus.get_midi_ctrl_ch    ( pad_idx );
      const int midi_ctrl_value = edrumulus.get_midi_ctrl_value ( pad_idx );
      MYMIDI.sendControlChange ( midi_ctrl_ch, midi_ctrl_value, midi_channel );
    }

    if ( edrumulus.get_choke_on_found ( pad_idx ) )
    {
      // if grabbed edge found, polyphonic aftertouch at 127 is transmitted for all notes of the pad
      MYMIDI.sendAfterTouch ( edrumulus.get_midi_note_norm ( pad_idx ), 127, midi_channel );
      MYMIDI.sendAfterTouch ( edrumulus.get_midi_note_rim  ( pad_idx ), 127, midi_channel );

      if ( pad_idx == hihat_pad_idx )
      {
        MYMIDI.sendAfterTouch ( edrumulus.get_midi_note_open_norm ( pad_idx ), 127, midi_channel );
        MYMIDI.sendAfterTouch ( edrumulus.get_midi_note_open_rim  ( pad_idx ), 127, midi_channel );
      }
    }
    else if ( edrumulus.get_choke_off_found ( pad_idx ) )
    {
      // if released edge found, polyphonic aftertouch at 0 is transmitted for all notes of the pad
      MYMIDI.sendAfterTouch ( edrumulus.get_midi_note_norm ( pad_idx ), 0, midi_channel );
      MYMIDI.sendAfterTouch ( edrumulus.get_midi_note_rim  ( pad_idx ), 0, midi_channel );

      if ( pad_idx == hihat_pad_idx )
      {
        MYMIDI.sendAfterTouch ( edrumulus.get_midi_note_open_norm ( pad_idx ), 0, midi_channel );
        MYMIDI.sendAfterTouch ( edrumulus.get_midi_note_open_rim  ( pad_idx ), 0, midi_channel );
      }
    }
  }

  // receiving MIDI messages to change the pad settings: edrumuluscontrol.m -> loopMIDI -> Hairless MIDI
  if ( MYMIDI.read ( midi_channel ) )
  {
    if ( MYMIDI.getType() == MIDI_CONTROL_CHANGE_TYPE )
    {
      bool      is_used    = false;
      const int controller = MYMIDI.getData1();
      const int value      = MYMIDI.getData2();

      // controller 102: pad type
      if ( controller == 102 )
      {
        edrumulus.set_pad_type ( selected_pad, static_cast<Edrumulus::Epadtype> ( value ) );
        is_used = true;
      }

      // controller 103: threshold
      if ( controller == 103 )
      {
        edrumulus.set_velocity_threshold ( selected_pad, value );
        is_used = true;
      }
  
      // controller 104: sensitivity
      if ( controller == 104 )
      {
        edrumulus.set_velocity_sensitivity ( selected_pad, value );
        is_used = true;
      }
  
      // controller 105: positional sensing threshold
      if ( controller == 105 )
      {
        edrumulus.set_pos_threshold ( selected_pad, value );
        is_used = true;
      }
  
      // controller 106: positional sensing sensitivity
      if ( controller == 106 )
      {
        edrumulus.set_pos_sensitivity ( selected_pad, value );
        is_used = true;
      }

      // controller 107: rim shot threshold
      if ( controller == 107 )
      {
        edrumulus.set_rim_shot_treshold ( selected_pad, value );
        is_used = true;
      }

      // controller 108: select pad
      if ( ( controller == 108 ) && ( value < MAX_NUM_PADS ) )
      {
        selected_pad = value;
        is_used      = true;
      }

      // controller 109: MIDI curve type
      if ( controller == 109 )
      {
        edrumulus.set_curve ( selected_pad, static_cast<Edrumulus::Ecurvetype> ( value ) );
        is_used = true;
      }

      // controller 110: enable spike cancellation algorithm
      if ( controller == 110 )
      {
        edrumulus.set_spike_cancel_is_used ( value > 0 );
        is_used = true;
      }

      // controller 111: enable/disable rim shot and positional sensing support
      if ( controller == 111 )
      {
        switch ( value )
        {
          case 0: edrumulus.set_rim_shot_is_used ( selected_pad, false ); edrumulus.set_pos_sense_is_used ( selected_pad, false ); break;
          case 1: edrumulus.set_rim_shot_is_used ( selected_pad, true );  edrumulus.set_pos_sense_is_used ( selected_pad, false ); break;
          case 2: edrumulus.set_rim_shot_is_used ( selected_pad, false ); edrumulus.set_pos_sense_is_used ( selected_pad, true );  break;
          case 3: edrumulus.set_rim_shot_is_used ( selected_pad, true );  edrumulus.set_pos_sense_is_used ( selected_pad, true );  break;
        }
        is_used = true;
      }

      // controller 112: normal MIDI note
      if ( controller == 112 )
      {
        edrumulus.set_midi_notes ( selected_pad, value, edrumulus.get_midi_note_rim ( selected_pad ) );
        is_used = true;
      }

      // controller 113: MIDI note for rim
      if ( controller == 113 )
      {
        edrumulus.set_midi_notes ( selected_pad, edrumulus.get_midi_note_norm ( selected_pad ), value );
        is_used = true;
      }

      // controller 114: cross talk cancellation
      if ( controller == 114 )
      {
        edrumulus.set_cancellation ( selected_pad, value );
        is_used = true;
      }

      // give some feedback that the setting was correctly received
      if ( is_used )
      {
        MYMIDI.sendNoteOff ( controller, value, 1 ); // can be checked, e.g., in the log file
      }
    }
  }
#endif
}
