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
#include <MIDI.h>
MIDI_CREATE_DEFAULT_INSTANCE();
#endif

const int        analog_pin        = 34; // TODO this must be configurable
const int        LED_builtin_pin   = 2;  // TODO this must be configurable
int              LED_counter       = 0;
const int        LED_on_time       = 2000; // samples
const int        dc_offset_est_len = 5000; // samples
float            dc_offset         = 0.0f;
Edrumulus        edrumulus;

// for debugging and testing
volatile int     iCnt = 0;
//volatile int   outCnt = 0;
const int        iNumSamples = 1500;
volatile float   values[iNumSamples];
//volatile bool  sending = false;
//hw_timer_t*    timer = NULL;
//portMUX_TYPE   timerMux = portMUX_INITIALIZER_UNLOCKED;

/*
void onTimer()
{
  if ( !sending )
  {
    portENTER_CRITICAL_ISR ( &timerMux );
    if ( iCnt < iNumSamples )
    {
      const int sample = analogRead ( analog_pin );

      if ( ( iCnt >= 0 ) && ( iCnt < iNumSamples ) )
      {

//process_sample ( 0.5f );
        
        //values[iCnt] = static_cast<float> ( sample );
      }

      //values[iCnt] = process_sample ( sample );
      //Serial.println ( iCnt );
      iCnt++;
    }
    portEXIT_CRITICAL_ISR ( &timerMux );
  }
}
*/
 
void setup()
{
#ifdef USE_MIDI
  MIDI.begin();
  Serial.begin ( 38400 );
#else
  Serial.begin ( 115200 );
#endif

  // estimate the DC offset
  float dc_offset_sum = 0.0f;

  for ( int i = 0; i < dc_offset_est_len; i++ )
  {
    dc_offset_sum += analogRead ( analog_pin );
    delayMicroseconds ( 100 );
  }

  dc_offset = dc_offset_sum / dc_offset_est_len;

  // configure built-in LED
  pinMode ( LED_builtin_pin, OUTPUT );

/*
  timer = timerBegin   ( 0, 80, true );
  timerAttachInterrupt ( timer, &onTimer, true );
  timerAlarmWrite      ( timer, 125, true );
  timerAlarmEnable     ( timer );
*/
}

int iHitCnt = 0;

void loop()
{
  bool  peak_found;
  int   midi_velocity, midi_pos;
  float debug;

  // get sample from ADC
  int sample_org = analogRead ( analog_pin );

  // prepare sample for processing
  float sample = sample_org - dc_offset; // compensate DC offset
  sample      /= 30000;                  // scaling

  // process sample and create MIDI message if a hit was detected
  edrumulus.process_sample ( sample, peak_found, midi_velocity, midi_pos, debug );

  if ( peak_found )
  {
#ifdef USE_MIDI
    MIDI.sendControlChange ( 16, midi_pos,      10 ); // positional sensing
    MIDI.sendNoteOn        ( 38, midi_velocity, 10 ); // (note, velocity, channel)
    MIDI.sendNoteOff       ( 38, 0,             10 );
#else
    Serial.println ( debug, 7 );
#endif
  }

  // overload detection
  if ( ( sample_org >= 4095 ) || ( sample_org <= 1 ) )
  {
    LED_counter = LED_on_time;
    digitalWrite ( LED_builtin_pin, HIGH );
  }

  if ( LED_counter > 1 )
  {
    LED_counter--;
  }
  else if ( LED_counter == 1 ) // transition to off state
  {
    digitalWrite ( LED_builtin_pin, LOW );  
    LED_counter = 0; // idle state
  }


// measurement: Hilbert+moving average: about 54 kHz sampling rate possible
delayMicroseconds ( 103 ); // to get from 56 kHz to 8 kHz sampling rate

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

/*
if ( peak_found )
{
  Serial.print ( "peak_found " );
  Serial.print ( midi_velocity ); Serial.print ( "    " );
//Serial.print ( debug, 7 ); Serial.print ( "    " );
//Serial.println ( decay[iHitCnt++], 7 );
//if ( iHitCnt == decay_len ) iHitCnt = 0;
  Serial.println ( iHitCnt++ );
}
*/
/*
  if ( Serial.available() > 0 )
  {
    // for debugging: take samples from Octave, process and return result to Octave
    const float fIn = Serial.parseFloat();
    float debug;
    const bool peak_found = process_sample ( fIn, debug );
    Serial.println ( debug, 7 );
  }
*/
/*
if ( iCnt >= iNumSamples )
{
  // for usage with debugcontroller.m
  for ( int i = 0; i < iNumSamples; i++ )
  {
    Serial.println ( values[i] );
  }
  iCnt = 0;
}
*/
//Serial.println ( iHitCnt++ ); //processed_sample );
/*
  if ( iCnt >= iNumSamples )
  {
    sending = true;
    
    portENTER_CRITICAL(&timerMux);
    
    Serial.println ( values[outCnt] );
    outCnt++;

    if ( outCnt >= iNumSamples )
    {
      outCnt  = 0;
      iCnt    = 0;
      sending = false;
    }
    
    portEXIT_CRITICAL(&timerMux);
  }
*/
}
