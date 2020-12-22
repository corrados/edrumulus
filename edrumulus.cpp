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

#include "edrumulus.h"

Edrumulus* edrumulus_pointer = nullptr;


Edrumulus::Edrumulus()
{
  // initializations
  edrumulus_pointer    = this;                 // global pointer to this class needed for static callback function
  Fs                   = 8000;                 // this is the most fundamental system parameter: system sampling rate
  overload_LED_on_time = round ( 0.25f * Fs ); // minimum overload LED on time (e.g., 250 ms)

  // prepare timer at a rate of 8 kHz
  timer_semaphore = xSemaphoreCreateBinary();
  timer           = timerBegin ( 0, 80, true ); // prescaler of 80 (i.e. below we have 1 MHz instead of 80 MHz)
  timerAttachInterrupt ( timer, &on_timer, true );
  timerAlarmWrite      ( timer, 1000000 / Fs, true ); // here we define the sampling rate (1 MHz / Fs)
  timerAlarmEnable     ( timer );
}


void IRAM_ATTR Edrumulus::on_timer()
{
  // tell the main loop that a sample can be read by setting the semaphore
  xSemaphoreGiveFromISR ( edrumulus_pointer->timer_semaphore, NULL );
}


void Edrumulus::setup ( const int conf_analog_pin,
                        const int conf_overload_LED_pin )
{
  // set the GIOP pin numbers
  analog_pin       = conf_analog_pin;
  overload_LED_pin = conf_overload_LED_pin;

  // if an overload LED shall be used, initialize GPIO port
  if ( overload_LED_pin >= 0 )
  {
    pinMode ( overload_LED_pin, OUTPUT );
  }

  // estimate the DC offset
  const int dc_offset_est_len = 5000; // samples
  float     dc_offset_sum     = 0.0f;

  for ( int i = 0; i < dc_offset_est_len; i++ )
  {
    dc_offset_sum += analogRead ( analog_pin );
    delayMicroseconds ( 100 );
  }

  dc_offset = dc_offset_sum / dc_offset_est_len;

  // initialize the algorithm
  initialize();
}


void Edrumulus::initialize()
{
  // set algorithm parameters
  threshold              = pow   ( 10.0f, -64.0f / 20 ); // -64 dB threshold
  energy_window_len      = round ( 2e-3f * Fs );         // scan time (e.g. 2 ms)
  mask_time              = round ( 10e-3f * Fs );        // mask time (e.g. 10 ms)
  decay_len              = round ( 0.2f * Fs );          // decay time (e.g. 200 ms)
  decay_att              = pow   ( 10.0f, -1.0f / 20 );  // decay attenuation of 1 dB
  const float decay_grad = 200.0f / Fs;                  // decay gradient factor
  alpha                  = 200.0f / Fs;                  // IIR low pass filter coefficient

  // allocate memory for vectors
  if ( hil_hist        != nullptr ) delete[] hil_hist;
  if ( mov_av_hist_re  != nullptr ) delete[] mov_av_hist_re;
  if ( mov_av_hist_im  != nullptr ) delete[] mov_av_hist_im;
  if ( decay           != nullptr ) delete[] decay;
  if ( hil_hist_re     != nullptr ) delete[] hil_hist_re;
  if ( hil_hist_im     != nullptr ) delete[] hil_hist_im;
  if ( hil_low_hist_re != nullptr ) delete[] hil_low_hist_re;
  if ( hil_low_hist_im != nullptr ) delete[] hil_low_hist_im;

  hil_hist        = new float[hil_filt_len];      // memory for Hilbert filter history
  mov_av_hist_re  = new float[energy_window_len]; // real part memory for moving average filter history
  mov_av_hist_im  = new float[energy_window_len]; // imaginary part memory for moving average filter history
  decay           = new float[decay_len];         // memory for decay function
  hil_hist_re     = new float[energy_window_len]; // real part of memory for moving average of Hilbert filtered signal
  hil_hist_im     = new float[energy_window_len]; // imaginary part of memory for moving average of Hilbert filtered signal
  hil_low_hist_re = new float[energy_window_len]; // real part of memory for moving average of low-pass filtered Hilbert signal
  hil_low_hist_im = new float[energy_window_len]; // imaginary part of memory for moving average of low-pass filtered Hilbert signal

  // initialization values
  for ( int i = 0; i < hil_filt_len; i++ )
  {
    hil_hist[i] = 0.0f;
  }

  for ( int i = 0; i < energy_window_len; i++ )
  {
    mov_av_hist_re[i]  = 0.0f;
    mov_av_hist_im[i]  = 0.0f;
    hil_hist_re[i]     = 0.0f;
    hil_hist_im[i]     = 0.0f;
    hil_low_hist_re[i] = 0.0f;
    hil_low_hist_im[i] = 0.0f;
  }

  mask_back_cnt         = 0;
  was_above_threshold   = false;
  prev_hil_filt_val     = 0.0f;
  prev_hil_filt_new_val = 0.0f;
  decay_back_cnt        = 0;
  decay_scaling         = 1.0f;
  pos_sense_cnt         = 0;
  hil_low_re            = 0.0f;
  hil_low_im            = 0.0f;
  overload_LED_cnt      = 0;

  // calculate the decay curve
  for ( int i = 0; i < decay_len; i++ )
  {
    decay[i] = pow ( 10.0f, -i / 20.0f * decay_grad );
  }
}


bool Edrumulus::process ( int&   midi_velocity,
                          int&   midi_pos )
{
  bool  peak_found = false;
  float debug;

/*
// for debugging: take samples from Octave, process and return result to Octave
if ( Serial.available() > 0 )
{
  const float fIn = Serial.parseFloat();
  process_sample ( fIn, peak_found, midi_velocity, midi_pos, debug );
  Serial.println ( debug, 7 );
}
return false;
*/

  // wait for the timer to get the correct sampling rate when reading the analog value
  if ( xSemaphoreTake ( timer_semaphore, portMAX_DELAY ) == pdTRUE )
  {
    // get sample from ADC
    const int sample_org = analogRead ( analog_pin );

    // prepare sample for processing
    float sample = sample_org - dc_offset; // compensate DC offset
    sample      /= 30000;                  // scaling -> TODO we need a better solution for the scaling

    // process sample
    process_sample ( sample, peak_found, midi_velocity, midi_pos, debug );

    // optional overload detection
    if ( overload_LED_pin >= 0 )
    {
      if ( ( sample_org >= 4094 ) || ( sample_org <= 1 ) )
      {
        overload_LED_cnt = overload_LED_on_time;
        digitalWrite ( overload_LED_pin, HIGH );
      }
  
      if ( overload_LED_cnt > 1 )
      {
        overload_LED_cnt--;
      }
      else if ( overload_LED_cnt == 1 ) // transition to off state
      {
        digitalWrite ( overload_LED_pin, LOW );
        overload_LED_cnt = 0; // idle state
      }
    }
  }

  return peak_found;
}


void Edrumulus::process_sample ( const float fIn,
                                 bool&       peak_found,
                                 int&        midi_velocity,
                                 int&        midi_pos,
                                 float&      debug )
{
  // initialize return parameter
  peak_found    = false;
  midi_velocity = 0;
  midi_pos      = 0;

debug = 0.0f; // TEST


  // Calculate peak detection -----------------------------------------------------
  // hilbert filter
  for ( int i = 0; i < hil_filt_len - 1; i++ )
  {
    hil_hist[i] = hil_hist[i + 1];
  }
  hil_hist[hil_filt_len - 1] = fIn;

  float hil_re = 0;
  float hil_im = 0;
  for ( int i = 0; i < hil_filt_len; i++ )
  {
    hil_re += hil_hist[i] * a_re[i];
    hil_im += hil_hist[i] * a_im[i];
  }

  // moving average filter
  for ( int i = 0; i < energy_window_len - 1; i++ )
  {
    mov_av_hist_re[i] = mov_av_hist_re[i + 1];
    mov_av_hist_im[i] = mov_av_hist_im[i + 1];
  }
  mov_av_hist_re[energy_window_len - 1] = hil_re;
  mov_av_hist_im[energy_window_len - 1] = hil_im;

  float mov_av_re = 0;
  float mov_av_im = 0;
  for ( int i = 0; i < energy_window_len; i++ )
  {
    mov_av_re += mov_av_hist_re[i];
    mov_av_im += mov_av_hist_im[i];
  }
  mov_av_re /= energy_window_len;
  mov_av_im /= energy_window_len;

  const float hil_filt = sqrt ( mov_av_re * mov_av_re + mov_av_im * mov_av_im );


  // exponential decay assumption (note that we must not use hil_filt_org since a
  // previous peak might not be faded out and the peak detection works on hil_filt)
  // subtract decay (with clipping at zero)
  float hil_filt_new = hil_filt;

  if ( decay_back_cnt > 0 )
  {
    const float cur_decay = decay_scaling * decay[decay_len - decay_back_cnt];
    hil_filt_new          = hil_filt - cur_decay;
    decay_back_cnt--;

    if ( hil_filt_new < 0.0f )
    {
      hil_filt_new = 0.0f;
    }
  }


  // threshold test
  if ( ( ( hil_filt_new > threshold ) || was_above_threshold ) && ( mask_back_cnt == 0 ) )
  {
    was_above_threshold = true;

    // climb to the maximum of the current peak
    if ( prev_hil_filt_new_val < hil_filt_new )
    {
      prev_hil_filt_new_val = hil_filt_new;
      prev_hil_filt_val     = hil_filt; // needed for further processing
    }
    else
    {
      // maximum found
      prev_hil_filt_new_val = 0.0f;
      was_above_threshold   = false;
      decay_back_cnt        = decay_len;
      decay_scaling         = prev_hil_filt_val * decay_att;
      mask_back_cnt         = mask_time;
      peak_found            = true;

// TEST
// velocity sensing MIDI mapping
midi_velocity = static_cast<int> ( ( 20 * log10 ( prev_hil_filt_val ) / 33 + 1.9f ) * 127 );
midi_velocity = max ( 1, min ( 127, midi_velocity ) );

    }
  }

  if ( mask_back_cnt > 0 )
  {
    mask_back_cnt--;
  }


  // Calculate positional sensing -------------------------------------------------
  // low pass filter of the Hilbert signal
  hil_low_re = ( 1.0f - alpha ) * hil_low_re + alpha * hil_re;
  hil_low_im = ( 1.0f - alpha ) * hil_low_im + alpha * hil_im;

  for ( int i = 0; i < energy_window_len - 1; i++ )
  {
    hil_hist_re[i]     = hil_hist_re[i + 1];
    hil_hist_im[i]     = hil_hist_im[i + 1];
    hil_low_hist_re[i] = hil_low_hist_re[i + 1];
    hil_low_hist_im[i] = hil_low_hist_im[i + 1];
  }
  hil_hist_re[energy_window_len - 1]     = hil_re;
  hil_hist_im[energy_window_len - 1]     = hil_im;
  hil_low_hist_re[energy_window_len - 1] = hil_low_re;
  hil_low_hist_im[energy_window_len - 1] = hil_low_im;

  if ( peak_found || ( pos_sense_cnt > 0 ) )
  {
    if ( peak_found && ( pos_sense_cnt == 0 ) )
    {
      // a peak was found, we now have to start the delay process to fill up the
      // required buffer length for our metric
      pos_sense_cnt        = energy_window_len / 2 - 2; // the "- 2" is to match the reference model (which is block-based)
      peak_found           = false; // will be set after delay process is done
      stored_midi_velocity = midi_velocity;
    }
    else if ( pos_sense_cnt == 1 )
    {
      // the buffers are filled, now calculate the metric
      float peak_energy     = 0;
      float peak_energy_low = 0;
      for ( int i = 0; i < energy_window_len; i++ )
      {
        peak_energy     += ( hil_hist_re[i]     * hil_hist_re[i]     + hil_hist_im[i]     * hil_hist_im[i] );
        peak_energy_low += ( hil_low_hist_re[i] * hil_low_hist_re[i] + hil_low_hist_im[i] * hil_low_hist_im[i] );
      }

      const float pos_sense_metric = peak_energy / peak_energy_low;
      pos_sense_cnt                = 0;
      peak_found                   = true;
      midi_velocity                = stored_midi_velocity;

// TEST positional sensing MIDI mapping
midi_pos = static_cast<int> ( ( 10 * log10 ( pos_sense_metric ) / 4 ) * 127 - 510 );
midi_pos = max ( 1, min ( 127, midi_pos ) );

    }
    else
    {
      // we still need to wait for the buffers to fill up
      pos_sense_cnt--;
    }
  }

// TEST
debug = hil_low_re;

}
