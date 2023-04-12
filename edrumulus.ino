/******************************************************************************\
 * Copyright (c) 2020-2023
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

//#define USE_MIDI

#include "edrumulus.h"

#ifdef USE_MIDI
# ifdef ESP_PLATFORM
#  include <MIDI.h>
MIDI_CREATE_DEFAULT_INSTANCE();
#  define MYMIDI                     MIDI
#  define MIDI_CONTROL_CHANGE_TYPE   midi::ControlChange
#  define MIDI_SEND_AFTER_TOUCH      sendAfterTouch
#  define MIDI_SERIAL                38400
# endif
# ifdef TEENSYDUINO
#  define MYMIDI                     usbMIDI
#  define MIDI_CONTROL_CHANGE_TYPE   usbMIDI.ControlChange
#  define MIDI_SEND_AFTER_TOUCH      sendAfterTouchPoly
# endif
#endif

Edrumulus edrumulus;
const int midi_channel      = 10;    // default for edrums is 10
const int hihat_pad_idx     = 2;
const int hihatctrl_pad_idx = 3;
int       number_pads       = 0;     // initialization value, will be set in setup()
int       status_LED_pin    = 0;     // initialization value, will be set in setup()
bool      is_status_LED_on  = false; // initialization value
int       selected_pad      = 0;     // initialization value


void setup()
{
  // get the pin-to-pad assignments
  int* analog_pins         = nullptr;
  int* analog_pins_rimshot = nullptr;
  const int prototype = Edrumulus_hardware::get_prototype_pins ( &analog_pins,
                                                                 &analog_pins_rimshot,
                                                                 &number_pads,
                                                                 &status_LED_pin );

#ifdef USE_MIDI
  MYMIDI.begin();
#endif
#ifdef MIDI_SERIAL
  if ( prototype == 5 )
  {
    Serial.begin ( 115200 ); // faster communication on prototype 5
  }
  else
  {
    Serial.begin ( MIDI_SERIAL );
  }
#else
  Serial.begin ( 115200 );
#endif

  edrumulus.setup ( number_pads, analog_pins, analog_pins_rimshot );
#ifdef ESP_PLATFORM
  preset_settings(); // for ESP32, the load/save of settings is not supported, preset instead
#else
  read_settings();
#endif

/*
// TEST use the ride input for 2nd/3rd head sensor input for the snare
edrumulus.set_coupled_pad_idx      ( 6 );
edrumulus.set_pad_type             ( 0, Edrumulus::PDA120LS );
edrumulus.set_velocity_threshold   ( 0, 4 );
edrumulus.set_velocity_sensitivity ( 0, 7 );
edrumulus.set_pos_threshold        ( 0, 3 );
edrumulus.set_pos_sensitivity      ( 0, 12 );
edrumulus.set_rim_shot_treshold    ( 0, 24 );
*/

  // initialize GPIO port for status LED
  pinMode ( status_LED_pin, OUTPUT );
}


void preset_settings()
{
  if ( number_pads >= 9 )
  {
    // default MIDI note assignments
    edrumulus.set_midi_notes      ( 0, 38, 40 ); // snare
    edrumulus.set_midi_notes      ( 1, 36, 36 ); // kick
    edrumulus.set_midi_notes      ( hihat_pad_idx, 22 /*42*/, 22 );
    edrumulus.set_midi_notes_open ( hihat_pad_idx, 26 /*46*/, 26 );
    edrumulus.set_midi_notes      ( hihatctrl_pad_idx, 44, 44 ); // Hi-Hat pedal hit
    edrumulus.set_midi_notes      ( 4, 49, 55 ); // crash
    edrumulus.set_midi_notes      ( 5, 48, 50 ); // tom 1
    edrumulus.set_midi_notes      ( 6, 51, 53 /*59*/ ); // ride (edge: 59, bell: 53)
    edrumulus.set_midi_notes      ( 7, 45, 47 ); // tom 2
    edrumulus.set_midi_notes      ( 8, 43, 58 ); // tom 3

    // default drum kit setup
    edrumulus.set_pad_type ( 0, Edrumulus::PD8 );  // snare
    edrumulus.set_pad_type ( 1, Edrumulus::KD7 );  // kick
    edrumulus.set_pad_type ( 2, Edrumulus::PD6 );  // Hi-Hat
    edrumulus.set_pad_type ( 3, Edrumulus::FD8 );  // Hi-Hat-ctrl
    edrumulus.set_pad_type ( 4, Edrumulus::CY6 );  // crash
    edrumulus.set_pad_type ( 5, Edrumulus::TP80 ); // tom 1
    edrumulus.set_pad_type ( 6, Edrumulus::CY8 );  // ride
    edrumulus.set_pad_type ( 7, Edrumulus::TP80 ); // tom 2
    edrumulus.set_pad_type ( 8, Edrumulus::TP80 ); // tom 3
  }
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
        const int  midi_ctrl_ch    = edrumulus.get_midi_ctrl_ch      ( hihatctrl_pad_idx );
        const int  midi_ctrl_value = edrumulus.get_midi_ctrl_value   ( hihatctrl_pad_idx );
        const bool hi_hat_is_open  = edrumulus.get_midi_ctrl_is_open ( hihatctrl_pad_idx );
        MYMIDI.sendControlChange ( midi_ctrl_ch, midi_ctrl_value, midi_channel );

        // if Hi-Hat is open, overwrite MIDI note
        if ( hi_hat_is_open )
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
      MYMIDI.MIDI_SEND_AFTER_TOUCH ( edrumulus.get_midi_note_norm ( pad_idx ), 127, midi_channel );
      MYMIDI.MIDI_SEND_AFTER_TOUCH ( edrumulus.get_midi_note_rim  ( pad_idx ), 127, midi_channel );

      if ( pad_idx == hihat_pad_idx )
      {
        MYMIDI.MIDI_SEND_AFTER_TOUCH ( edrumulus.get_midi_note_open_norm ( pad_idx ), 127, midi_channel );
        MYMIDI.MIDI_SEND_AFTER_TOUCH ( edrumulus.get_midi_note_open_rim  ( pad_idx ), 127, midi_channel );
      }
    }
    else if ( edrumulus.get_choke_off_found ( pad_idx ) )
    {
      // if released edge found, polyphonic aftertouch at 0 is transmitted for all notes of the pad
      MYMIDI.MIDI_SEND_AFTER_TOUCH ( edrumulus.get_midi_note_norm ( pad_idx ), 0, midi_channel );
      MYMIDI.MIDI_SEND_AFTER_TOUCH ( edrumulus.get_midi_note_rim  ( pad_idx ), 0, midi_channel );

      if ( pad_idx == hihat_pad_idx )
      {
        MYMIDI.MIDI_SEND_AFTER_TOUCH ( edrumulus.get_midi_note_open_norm ( pad_idx ), 0, midi_channel );
        MYMIDI.MIDI_SEND_AFTER_TOUCH ( edrumulus.get_midi_note_open_rim  ( pad_idx ), 0, midi_channel );
      }
    }
  }

  // receiving MIDI messages to change the pad settings: edrumuluscontrol.m -> loopMIDI -> Hairless MIDI
  if ( MYMIDI.read ( midi_channel ) )
  {
    if ( MYMIDI.getType() == MIDI_CONTROL_CHANGE_TYPE )
    {
      const int controller = MYMIDI.getData1();
      const int value      = MYMIDI.getData2();

      // controller 102: pad type
      if ( controller == 102 )
      {
        edrumulus.set_pad_type  ( selected_pad,    static_cast<Edrumulus::Epadtype> ( value ) );
        edrumulus.write_setting ( selected_pad, 0, value );

        // on a pad type change, return all parameters of the selected pad
        confirm_setting ( controller, value, true );
      }

      // controller 103: threshold
      if ( controller == 103 )
      {
        edrumulus.set_velocity_threshold ( selected_pad,    value );
        edrumulus.write_setting          ( selected_pad, 1, value );
        confirm_setting                  ( controller,      value, false );
      }

      // controller 104: sensitivity
      if ( controller == 104 )
      {
        edrumulus.set_velocity_sensitivity ( selected_pad,    value );
        edrumulus.write_setting            ( selected_pad, 2, value );
        confirm_setting                    ( controller,      value, false );
      }

      // controller 105: positional sensing threshold
      if ( controller == 105 )
      {
        edrumulus.set_pos_threshold ( selected_pad,    value );
        edrumulus.write_setting     ( selected_pad, 3, value );
        confirm_setting             ( controller,      value, false );
      }

      // controller 106: positional sensing sensitivity
      if ( controller == 106 )
      {
        edrumulus.set_pos_sensitivity ( selected_pad,    value );
        edrumulus.write_setting       ( selected_pad, 4, value );
        confirm_setting               ( controller,      value, false );
      }

      // controller 107: rim shot threshold
      if ( controller == 107 )
      {
        edrumulus.set_rim_shot_treshold ( selected_pad,    value );
        edrumulus.write_setting         ( selected_pad, 5, value );
        confirm_setting                 ( controller,      value, false );
      }

      // controller 108: select pad
      if ( ( controller == 108 ) && ( value < MAX_NUM_PADS ) )
      {
        selected_pad = value;

        // on a pad selection, return all parameters of the selected pad
        confirm_setting ( controller, value, true );
      }

      // controller 109: MIDI curve type
      if ( controller == 109 )
      {
        edrumulus.set_curve     ( selected_pad,    static_cast<Edrumulus::Ecurvetype> ( value ) );
        edrumulus.write_setting ( selected_pad, 6, value );
        confirm_setting         ( controller,      value, false );
      }

      // controller 110: spike cancellation level
      if ( controller == 110 )
      {
        edrumulus.set_spike_cancel_level (                 value );
        edrumulus.write_setting          ( number_pads, 0, value );
        confirm_setting                  ( controller,     value, false );
      }

      // controller 111: enable/disable rim shot and positional sensing support
      if ( controller == 111 )
      {
        switch ( value )
        {
          case 0:
            edrumulus.set_rim_shot_is_used  ( selected_pad,    false );
            edrumulus.write_setting         ( selected_pad, 7, false );
            edrumulus.set_pos_sense_is_used ( selected_pad,    false );
            edrumulus.write_setting         ( selected_pad, 8, false );
            break;
          case 1:
            edrumulus.set_rim_shot_is_used  ( selected_pad,    true );
            edrumulus.write_setting         ( selected_pad, 7, true );
            edrumulus.set_pos_sense_is_used ( selected_pad,    false );
            edrumulus.write_setting         ( selected_pad, 8, false );
            break;
          case 2:
            edrumulus.set_rim_shot_is_used  ( selected_pad,    false );
            edrumulus.write_setting         ( selected_pad, 7, false );
            edrumulus.set_pos_sense_is_used ( selected_pad,    true );
            edrumulus.write_setting         ( selected_pad, 8, true );
            break;
          case 3:
            edrumulus.set_rim_shot_is_used  ( selected_pad,    true );
            edrumulus.write_setting         ( selected_pad, 7, true );
            edrumulus.set_pos_sense_is_used ( selected_pad,    true );
            edrumulus.write_setting         ( selected_pad, 8, true );
            break;
        }
        confirm_setting ( controller, value, false );
      }

      // controller 112: normal MIDI note
      if ( controller == 112 )
      {
        edrumulus.set_midi_note_norm ( selected_pad,    value );
        edrumulus.write_setting      ( selected_pad, 9, value );
        confirm_setting              ( controller,      value, false );
      }

      // controller 113: MIDI note for rim
      if ( controller == 113 )
      {
        edrumulus.set_midi_note_rim ( selected_pad,     value );
        edrumulus.write_setting     ( selected_pad, 10, value );
        confirm_setting             ( controller,       value, false );
      }

      // controller 114: cross talk cancellation
      if ( controller == 114 )
      {
        edrumulus.set_cancellation ( selected_pad,     value );
        edrumulus.write_setting    ( selected_pad, 11, value );
        confirm_setting            ( controller,       value, false );
      }

      // controller 115: apply preset settings and store these to the EEPROM
      if ( controller == 115 )
      {
        preset_settings();
        write_all_settings();
        confirm_setting ( controller, value, false );
      }

      // controller 116: normal MIDI note open (Hi-Hat)
      if ( controller == 116 )
      {
        edrumulus.set_midi_note_open_norm ( selected_pad,     value );
        edrumulus.write_setting           ( selected_pad, 12, value );
        confirm_setting                   ( controller,       value, false );
      }

      // controller 117: MIDI note open (Hi-Hat) for rim
      if ( controller == 117 )
      {
        edrumulus.set_midi_note_open_rim ( selected_pad,     value );
        edrumulus.write_setting          ( selected_pad, 13, value );
        confirm_setting                  ( controller,       value, false );
      }
    }
  }
#endif
}


#ifdef USE_MIDI
// give feedback to the controller GUI via MIDI Note Off
void confirm_setting ( const int  controller,
                       const int  value,
                       const bool send_all )
{
  if ( send_all )
  {
    // return all parameters of the selected pad
    MYMIDI.sendNoteOff ( 102, static_cast<int> ( edrumulus.get_pad_type ( selected_pad ) ), 1 );
    MYMIDI.sendNoteOff ( 103, edrumulus.get_velocity_threshold ( selected_pad ), 1 );
    MYMIDI.sendNoteOff ( 104, edrumulus.get_velocity_sensitivity ( selected_pad ), 1 );
    MYMIDI.sendNoteOff ( 105, edrumulus.get_pos_threshold ( selected_pad ), 1 );
    MYMIDI.sendNoteOff ( 106, edrumulus.get_pos_sensitivity ( selected_pad ), 1 );
    MYMIDI.sendNoteOff ( 107, edrumulus.get_rim_shot_treshold ( selected_pad ), 1 );
    MYMIDI.sendNoteOff ( 108, selected_pad, 1 );
    MYMIDI.sendNoteOff ( 109, static_cast<int> ( edrumulus.get_curve ( selected_pad ) ), 1 );
    MYMIDI.sendNoteOff ( 110, edrumulus.get_spike_cancel_level(), 1 );
    MYMIDI.sendNoteOff ( 111, edrumulus.get_rim_shot_is_used ( selected_pad ) + 2 * edrumulus.get_pos_sense_is_used ( selected_pad ), 1 );
    MYMIDI.sendNoteOff ( 112, edrumulus.get_midi_note_norm ( selected_pad ), 1 );
    MYMIDI.sendNoteOff ( 113, edrumulus.get_midi_note_rim ( selected_pad ), 1 );
    MYMIDI.sendNoteOff ( 114, edrumulus.get_cancellation ( selected_pad ), 1 );
    MYMIDI.sendNoteOff ( 126, VERSION_MINOR, 1 );
    MYMIDI.sendNoteOff ( 127, VERSION_MAJOR, 1 );
  }
  else
  {
    // return only the given parameter
    MYMIDI.sendNoteOff ( controller, value, 1 ); // can be checked, e.g., in the log file
  }
}
#endif


void read_settings()
{
  for ( int i = 0; i < number_pads; i++ )
  {
    // NOTE that it is important that set_pad_type() is called first because it resets all other parameters
    edrumulus.set_pad_type             ( i, static_cast<Edrumulus::Epadtype> (   edrumulus.read_setting ( i, 0 ) ) );
    edrumulus.set_velocity_threshold   ( i,                                      edrumulus.read_setting ( i, 1 ) );
    edrumulus.set_velocity_sensitivity ( i,                                      edrumulus.read_setting ( i, 2 ) );
    edrumulus.set_pos_threshold        ( i,                                      edrumulus.read_setting ( i, 3 ) );
    edrumulus.set_pos_sensitivity      ( i,                                      edrumulus.read_setting ( i, 4 ) );
    edrumulus.set_rim_shot_treshold    ( i,                                      edrumulus.read_setting ( i, 5 ) );
    edrumulus.set_curve                ( i, static_cast<Edrumulus::Ecurvetype> ( edrumulus.read_setting ( i, 6 ) ) );
    edrumulus.set_rim_shot_is_used     ( i,                                      edrumulus.read_setting ( i, 7 ) );
    edrumulus.set_pos_sense_is_used    ( i,                                      edrumulus.read_setting ( i, 8 ) );
    edrumulus.set_midi_note_norm       ( i,                                      edrumulus.read_setting ( i, 9 ) );
    edrumulus.set_midi_note_rim        ( i,                                      edrumulus.read_setting ( i, 10 ) );
    edrumulus.set_cancellation         ( i,                                      edrumulus.read_setting ( i, 11 ) );
    edrumulus.set_midi_note_open_norm  ( i,                                      edrumulus.read_setting ( i, 12 ) );
    edrumulus.set_midi_note_open_rim   ( i,                                      edrumulus.read_setting ( i, 13 ) );
  }
  edrumulus.set_spike_cancel_level ( edrumulus.read_setting ( number_pads, 0 ) );
}


void write_all_settings()
{
  for ( int i = 0; i < number_pads; i++ )
  {
    edrumulus.write_setting ( i, 0,  edrumulus.get_pad_type             ( i ) );
    edrumulus.write_setting ( i, 1,  edrumulus.get_velocity_threshold   ( i ) );
    edrumulus.write_setting ( i, 2,  edrumulus.get_velocity_sensitivity ( i ) );
    edrumulus.write_setting ( i, 3,  edrumulus.get_pos_threshold        ( i ) );
    edrumulus.write_setting ( i, 4,  edrumulus.get_pos_sensitivity      ( i ) );
    edrumulus.write_setting ( i, 5,  edrumulus.get_rim_shot_treshold    ( i ) );
    edrumulus.write_setting ( i, 6,  edrumulus.get_curve                ( i ) );
    edrumulus.write_setting ( i, 7,  edrumulus.get_rim_shot_is_used     ( i ) );
    edrumulus.write_setting ( i, 8,  edrumulus.get_pos_sense_is_used    ( i ) );
    edrumulus.write_setting ( i, 9,  edrumulus.get_midi_note_norm       ( i ) );
    edrumulus.write_setting ( i, 10, edrumulus.get_midi_note_rim        ( i ) );
    edrumulus.write_setting ( i, 11, edrumulus.get_cancellation         ( i ) );
    edrumulus.write_setting ( i, 12, edrumulus.get_midi_note_open_norm  ( i ) );
    edrumulus.write_setting ( i, 13, edrumulus.get_midi_note_open_rim   ( i ) );
  }
  edrumulus.write_setting ( number_pads, 0, edrumulus.get_spike_cancel_level() );
}
