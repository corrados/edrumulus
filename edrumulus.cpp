/******************************************************************************\
 * Copyright (c) 2020-2020
 *
 * Author(s):
 *  Volker Fischer
 *
 ******************************************************************************
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 *
\******************************************************************************/

#include "edrumulus.h"

Edrumulus::Edrumulus()
{
  // allocate memory for the vectors
  hil_hist        = new float[hil_filt_len];           // memory for Hilbert filter history
  mov_av_hist_re  = new float[energy_window_len];      // real part memory for moving average filter history
  mov_av_hist_im  = new float[energy_window_len];      // imaginary part memory for moving average filter history
  decay           = new float[decay_len];              // memory for decay function
  hil_hist_re     = new float[energy_window_len_half]; // real part of memory for moving average of Hilbert filtered signal
  hil_hist_im     = new float[energy_window_len_half]; // imaginary part of memory for moving average of Hilbert filtered signal
  hil_low_hist_re = new float[energy_window_len_half]; // real part of memory for moving average of low-pass filtered Hilbert signal
  hil_low_hist_im = new float[energy_window_len_half]; // imaginary part of memory for moving average of low-pass filtered Hilbert signal

  initialize();
}


void Edrumulus::initialize()
{
  // set algorithm parameters
  mask_time              = round ( 10e-3f * Fs );      // mask time (e.g. 10 ms)
  threshold              = pow ( 10.0f, -64.0f / 20 ); // -64 dB threshold
  decay_att              = pow ( 10.0f, -1.0f / 20 );  // decay attenuation of 1 dB
  const float decay_grad = 200.0f / Fs;                // decay gradient factor
  alpha                  = 0.025f * 8e3f / Fs;         // IIR low pass filter coefficient

  // initialization values
  for ( int i = 0; i < hil_filt_len; i++ )
  {
    hil_hist[i] = 0.0f;
  }

  for ( int i = 0; i < energy_window_len; i++ )
  {
    mov_av_hist_re[i] = 0.0f;
    mov_av_hist_im[i] = 0.0f;
  }

  for ( int i = 0; i < energy_window_len_half; i++ )
  {
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

// debug = cur_decay; // TEST

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

  for ( int i = 0; i < energy_window_len_half - 1; i++ )
  {
    hil_hist_re[i]     = hil_hist_re[i + 1];
    hil_hist_im[i]     = hil_hist_im[i + 1];
    hil_low_hist_re[i] = hil_low_hist_re[i + 1];
    hil_low_hist_im[i] = hil_low_hist_im[i + 1];
  }
  hil_hist_re[energy_window_len_half - 1]     = hil_re;
  hil_hist_im[energy_window_len_half - 1]     = hil_im;
  hil_low_hist_re[energy_window_len_half - 1] = hil_low_re;
  hil_low_hist_im[energy_window_len_half - 1] = hil_low_im;

  if ( peak_found )
  {
// note that the following code is not exactly what the reference code does: we
// do not move the window half the window size to the right
    float peak_energy     = 0;
    float peak_energy_low = 0;
    for ( int i = 0; i < energy_window_len_half; i++ )
    {
      peak_energy     += ( hil_hist_re[i] * hil_hist_re[i] + hil_hist_im[i] * hil_hist_im[i] );
      peak_energy_low += ( hil_low_hist_re[i] * hil_low_hist_re[i] + hil_low_hist_im[i] * hil_low_hist_im[i] );
    }

    const float pos_sense_metric = peak_energy / peak_energy_low;

// TEST
midi_pos = ( 10 * log10 ( pos_sense_metric ) / 8 - 2.1 ) * 127;
midi_pos = max ( 1, min ( 127, midi_pos ) );

  }

// TEST
debug = hil_low_re;
//debug = peak_found;

}
