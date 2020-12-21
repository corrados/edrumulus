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

void Edrumulus::initialize()
{
  // set algorithm parameters
  Fs                     = 8000;                       // sampling rate of 8 kHz
  energy_window_len      = round ( 2e-3f * Fs );       // scan time (e.g. 2 ms)
  decay_len              = round ( 0.2f * Fs );        // decay time (e.g. 200 ms)
  mask_time              = round ( 10e-3f * Fs );      // mask time (e.g. 10 ms)
  threshold              = pow ( 10.0f, -64.0f / 20 ); // -64 dB threshold
  decay_att              = pow ( 10.0f, -1.0f / 20 );  // decay attenuation of 1 dB
  const float decay_grad = 200.0f / Fs;                // decay gradient factor
  alpha                  = 0.025f * 8e3f / Fs;         // IIR low pass filter coefficient

  // allocate memory for vectors
  if ( hil_hist        == nullptr ) delete[] hil_hist;
  if ( mov_av_hist_re  == nullptr ) delete[] mov_av_hist_re;
  if ( mov_av_hist_im  == nullptr ) delete[] mov_av_hist_im;
  if ( decay           == nullptr ) delete[] decay;
  if ( hil_hist_re     == nullptr ) delete[] hil_hist_re;
  if ( hil_hist_im     == nullptr ) delete[] hil_hist_im;
  if ( hil_low_hist_re == nullptr ) delete[] hil_low_hist_re;
  if ( hil_low_hist_im == nullptr ) delete[] hil_low_hist_im;

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
    mov_av_hist_re[i] = 0.0f;
    mov_av_hist_im[i] = 0.0f;
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

  // calculate the decay curve
  for ( int i = 0; i < decay_len; i++ )
  {
    decay[i] = pow ( 10.0f, -i / 20.0f * decay_grad );
  }
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
// velocity/positional sensing mapping and play MIDI notes
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
      pos_sense_cnt        = energy_window_len / 2 - 2;
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
        peak_energy     += ( hil_hist_re[i] * hil_hist_re[i] + hil_hist_im[i] * hil_hist_im[i] );
        peak_energy_low += ( hil_low_hist_re[i] * hil_low_hist_re[i] + hil_low_hist_im[i] * hil_low_hist_im[i] );
      }

      const float pos_sense_metric = peak_energy / peak_energy_low;
      pos_sense_cnt                = 0;
      peak_found                   = true;
      midi_velocity                = stored_midi_velocity;

// TEST
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
//debug = peak_found;

}
