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

#include "edrumulus.h"


void Edrumulus::Pad::apply_preset_pad_settings()
{
  // apply PRESET settings (might be overwritten by pad-specific properties)
  pad_settings.velocity_threshold        = 8;  // 0..31
  pad_settings.velocity_sensitivity      = 9;  // 0..31
  pad_settings.mask_time_ms              = 6;  // 0..31 (ms)
  pad_settings.pos_threshold             = 9;  // 0..31
  pad_settings.pos_sensitivity           = 14; // 0..31
  pad_settings.rim_shot_treshold         = 12; // 0..31
  pad_settings.cancellation              = 0;  // 0..31
  pad_settings.curve_type                = LINEAR;
  pad_settings.pos_sense_is_used         = false;  // must be explicitely enabled if it shall be used
  pad_settings.rim_shot_is_used          = false;  // must be explicitely enabled if it shall be used
  pad_settings.first_peak_diff_thresh_db = 8.0f;   // pad specific parameter: allowed difference between first peak and later peak in scan time
  pad_settings.mask_time_decay_fact_db   = 15.0f;  // pad specific parameter: during mask time decay factor for maximum peak in scan time
  pad_settings.scan_time_ms              = 2.5f;   // pad specific parameter: scan time after first detected peak
  pad_settings.pre_scan_time_ms          = 2.5f;   // pad specific parameter: pre-scan time to search for first peak
  pad_settings.decay_est_delay_ms        = 7.0f;   // pad specific parameter: delay after second main peak until decay power estimation starts
  pad_settings.decay_est_len_ms          = 4.0f;   // pad specific parameter: decay power estimation window length
  pad_settings.decay_est_fact_db         = 16.0f;  // pad specific parameter: decay power estimation factor (to get over decay ripple)
  pad_settings.decay_fact_db             = 1.0f;   // pad specific parameter: vertical shift of the decay function in dB
  pad_settings.decay_len1_ms             = 0.0f;   // pad specific parameter: length of the decay 1
  pad_settings.decay_grad_fact1          = 200.0f; // pad specific parameter: decay function gradient factor 1
  pad_settings.decay_len2_ms             = 350.0f; // pad specific parameter: length of the decay 2
  pad_settings.decay_grad_fact2          = 200.0f; // pad specific parameter: decay function gradient factor 2
  pad_settings.decay_len3_ms             = 0.0f;   // pad specific parameter: length of the decay 3
  pad_settings.decay_grad_fact3          = 200.0f; // pad specific parameter: decay function gradient factor 3
  pad_settings.pos_low_pass_cutoff       = 150.0f; // pad specific parameter: low-pass filter cut-off in Hz for positional sensing
  pad_settings.pos_invert                = false;  // pad specific parameter: invert the positional sensing metric
  pad_settings.rim_use_low_freq_bp       = true;   // pad specific parameter: use low frequency band-pass filter for rim shot detection
  pad_settings.rim_shot_window_len_ms    = 3.5f;   // pad specific parameter: window length for rim shot detection
  pad_settings.rim_shot_velocity_thresh  = 0;      // pad specific parameter: velocity threshold for rim shots -> disabled per default

  switch ( pad_settings.pad_type )
  {
    case PD120:
      // note: the PRESET settings are from the PD-120 pad
      break;

    case PD80R:
      pad_settings.velocity_sensitivity     = 5;
      pad_settings.rim_shot_treshold        = 11;
      pad_settings.pos_threshold            = 11;
      pad_settings.pos_sensitivity          = 10;
      pad_settings.scan_time_ms             = 3.0f;
      pad_settings.decay_len2_ms            = 75.0f;
      pad_settings.decay_grad_fact2         = 300.0f;
      pad_settings.decay_len3_ms            = 300.0f;
      pad_settings.decay_grad_fact3         = 100.0f;
      pad_settings.rim_use_low_freq_bp      = false;
      pad_settings.rim_shot_velocity_thresh = 10; // suppress incorrect rim shot detections on low velocity hits
      break;

    case PD8:
      pad_settings.velocity_sensitivity = 5;
      pad_settings.pos_threshold        = 26;
      pad_settings.pos_sensitivity      = 11;
      pad_settings.rim_shot_treshold    = 16;
      pad_settings.mask_time_ms         = 7;
      pad_settings.scan_time_ms         = 1.3f;
      pad_settings.decay_est_delay_ms   = 6.0f;
      pad_settings.decay_fact_db        = 5.0f;
      pad_settings.decay_len2_ms        = 30.0f;
      pad_settings.decay_grad_fact2     = 600.0f;
      pad_settings.decay_len3_ms        = 150.0f;
      pad_settings.decay_grad_fact3     = 120.0f;
      break;

    case TP80:
      pad_settings.velocity_sensitivity = 10;
      pad_settings.pos_threshold        = 22;
      pad_settings.pos_sensitivity      = 23;
      pad_settings.scan_time_ms         = 2.75f;
      pad_settings.decay_est_delay_ms   = 11.0f;
      pad_settings.decay_len2_ms        = 60.0f;
      pad_settings.decay_grad_fact2     = 400.0f;
      pad_settings.decay_len3_ms        = 700.0f;
      pad_settings.decay_grad_fact3     = 60.0f;
      pad_settings.pos_invert           = true;
      break;

    case FD8:
      pad_settings.velocity_threshold   = 5;
      pad_settings.velocity_sensitivity = 0;
      break;

    case VH12:
// TODO if the Hi-Hat is open just a little bit, we get double triggers
      pad_settings.scan_time_ms       = 4.0f;
      pad_settings.decay_est_delay_ms = 9.0f;
      pad_settings.decay_fact_db      = 5.0f;
      pad_settings.decay_len2_ms      = 27.0f;
      pad_settings.decay_grad_fact2   = 700.0f;
      pad_settings.decay_len3_ms      = 600.0f; // must be long because of open Hi-Hat ringing
      pad_settings.decay_grad_fact3   = 75.0f;
      break;

    case VH12CTRL:
      pad_settings.velocity_threshold   = 19;
      pad_settings.velocity_sensitivity = 28;
      break;

    case KD7:
      pad_settings.velocity_threshold   = 11;
      pad_settings.velocity_sensitivity = 6;
      pad_settings.scan_time_ms         = 2.0f;
      pad_settings.decay_est_delay_ms   = 8.0f;
      pad_settings.decay_fact_db        = 5.0f;
      pad_settings.decay_len1_ms        = 4.0f;
      pad_settings.decay_grad_fact1     = 30.0f;
      pad_settings.decay_len2_ms        = 30.0f;
      pad_settings.decay_grad_fact2     = 450.0f;
      pad_settings.decay_len3_ms        = 500.0f;
      pad_settings.decay_grad_fact3     = 45.0f;
      break;

    case CY6:
      pad_settings.scan_time_ms     = 6.0f;
      pad_settings.decay_len2_ms    = 150.0f;
      pad_settings.decay_grad_fact2 = 120.0f;
      pad_settings.decay_len3_ms    = 450.0f;
      pad_settings.decay_grad_fact3 = 30.0f;
      break;

    case CY8:
      pad_settings.velocity_threshold   = 13;
      pad_settings.velocity_sensitivity = 8;
      pad_settings.rim_shot_treshold    = 30;
      pad_settings.curve_type           = LOG2;
      pad_settings.scan_time_ms         = 6.0f;
      pad_settings.decay_len1_ms        = 10.0f;
      pad_settings.decay_grad_fact1     = 10.0f;
      pad_settings.decay_len2_ms        = 100.0f;
      pad_settings.decay_grad_fact2     = 200.0f;
      pad_settings.decay_len3_ms        = 450.0f;
      pad_settings.decay_grad_fact3     = 30.0f;
      break;
  }
}
