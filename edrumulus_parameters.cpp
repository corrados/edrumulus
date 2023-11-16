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

#include "edrumulus.h"


void Edrumulus::Pad::apply_preset_pad_settings()
{
  // apply PRESET settings (might be overwritten by pad-specific properties)
  pad_settings.velocity_threshold        = 8;  // 0..31
  pad_settings.velocity_sensitivity      = 9;  // 0..31
  pad_settings.mask_time_ms              = 6;  // 0..31 (ms)
  pad_settings.pos_threshold             = 9;  // 0..31
  pad_settings.pos_sensitivity           = 14; // 0..31
  pad_settings.rim_shot_threshold        = 12; // 0..31
  pad_settings.rim_shot_boost            = 15; // 0..31
  pad_settings.cancellation              = 0;  // 0..31
  pad_settings.coupled_pad_idx           = 0;  // 0..[number of pads - 1] (0 means disabled)
  pad_settings.curve_type                = LINEAR;
  pad_settings.is_control                = false;  // default is a normal pad
  pad_settings.is_rim_switch             = false;  // default is a piezo for the rim sensor
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
  pad_settings.clip_comp_ampmap_step     = 0.08f;  // pad specific parameter: clipping compensation amplitude mapping step, conservative value from PD80R as default

  switch ( pad_settings.pad_type )
  {
    case LEHHS12C: // dual trigger
      pad_settings.is_rim_switch        = true;
      pad_settings.scan_time_ms         = 4.0f;
      pad_settings.decay_fact_db        = 5.0f;
      pad_settings.decay_len2_ms        = 600.0f;
      pad_settings.decay_grad_fact2     = 100.0f;
      pad_settings.rim_shot_is_used     = true;
      pad_settings.rim_shot_threshold   = 25;
      pad_settings.rim_shot_boost       = 0;
      pad_settings.velocity_threshold   = 18;
      pad_settings.velocity_sensitivity = 6;
      break;

    case LEHHS12C_CTRL: // 0-35k poti
      pad_settings.is_control           = true;
      pad_settings.pos_threshold        = 6;
      pad_settings.pos_sensitivity      = 23;
      pad_settings.velocity_threshold   = 0;
      pad_settings.velocity_sensitivity = 22;
      break;

    case MPS750X_TOM: // dual trigger
      pad_settings.scan_time_ms         = 6.0f;
      pad_settings.pre_scan_time_ms     = 3.5f;
      pad_settings.decay_grad_fact2     = 150.0f;
      pad_settings.decay_len2_ms        = 450.0f;
      pad_settings.rim_shot_is_used     = false;
      pad_settings.velocity_threshold   = 2;
      pad_settings.velocity_sensitivity = 3;
      break;

    case MPS750X_SNARE: // dual trigger
      pad_settings.scan_time_ms         = 6.0f;
      pad_settings.pre_scan_time_ms     = 3.5f;
      pad_settings.decay_grad_fact2     = 150.0f;
      pad_settings.decay_len2_ms        = 450.0f;
      pad_settings.rim_use_low_freq_bp  = false;
      pad_settings.rim_shot_is_used     = true;
      pad_settings.rim_shot_threshold   = 12;
      pad_settings.velocity_threshold   = 2;
      pad_settings.velocity_sensitivity = 6;
      pad_settings.pos_sense_is_used    = true;
      pad_settings.pos_sensitivity      = 3;
      break;

    case MPS750X_KICK: // single trigger
      pad_settings.first_peak_diff_thresh_db = 3.0f;
      pad_settings.scan_time_ms              = 6.0f;
      pad_settings.pre_scan_time_ms          = 3.5f;
      pad_settings.decay_grad_fact2          = 150.0f;
      pad_settings.decay_len2_ms             = 450.0f;
      pad_settings.velocity_threshold        = 10;
      pad_settings.velocity_sensitivity      = 10;
      pad_settings.curve_type                = LOG2;
      break;

    case MPS750X_RIDE: // 3-zone
      pad_settings.is_rim_switch        = true;
      pad_settings.scan_time_ms         = 3.0f;
      pad_settings.decay_est_delay_ms   = 1.0f;
      pad_settings.decay_grad_fact2     = 150.0f;
      pad_settings.decay_len2_ms        = 450.0f;
      pad_settings.rim_use_low_freq_bp  = false;
      pad_settings.rim_shot_is_used     = true;
      pad_settings.rim_shot_threshold   = 18;
      pad_settings.rim_shot_boost       = 0;
      pad_settings.velocity_threshold   = 12;
      pad_settings.velocity_sensitivity = 6;
      break;

    case MPS750X_CRASH: // 2-zone
      pad_settings.is_rim_switch        = true;
      pad_settings.scan_time_ms         = 3.0f;
      pad_settings.decay_est_delay_ms   = 1.0f;
      pad_settings.decay_grad_fact2     = 150.0f;
      pad_settings.decay_len2_ms        = 450.0f;
      pad_settings.rim_use_low_freq_bp  = false;
      pad_settings.rim_shot_is_used     = true;
      pad_settings.rim_shot_threshold   = 20;
      pad_settings.rim_shot_boost       = 0;
      pad_settings.velocity_threshold   = 12;
      pad_settings.velocity_sensitivity = 6;
      break;

    case PD120: // dual trigger
      pad_settings.velocity_threshold   = 6;
      pad_settings.velocity_sensitivity = 6;
      pad_settings.pos_threshold        = 7;
      pad_settings.pos_sensitivity      = 14;
      pad_settings.rim_shot_threshold   = 24;
      pad_settings.pos_sense_is_used    = true;
      pad_settings.rim_shot_is_used     = true;
      break;

    case PDA120LS: // dual trigger
      pad_settings.velocity_threshold        = 11;
      pad_settings.velocity_sensitivity      = 7;
      pad_settings.pos_threshold             = 31;
      pad_settings.pos_sensitivity           = 16;
      pad_settings.rim_shot_threshold        = 30;
      pad_settings.decay_grad_fact2          = 250.0f;
      pad_settings.decay_fact_db             = 5.0f;
      pad_settings.pre_scan_time_ms          = 3.5f;
      pad_settings.first_peak_diff_thresh_db = 7.0f;
      pad_settings.pos_invert                = true;
      pad_settings.pos_sense_is_used         = true;
      pad_settings.rim_shot_is_used          = true;
      break;

    case PD80R: // dual trigger
      pad_settings.velocity_sensitivity = 1;
      pad_settings.rim_shot_threshold   = 10;
      pad_settings.pos_threshold        = 9;
      pad_settings.pos_sensitivity      = 15;
      pad_settings.scan_time_ms         = 3.0f;
      pad_settings.decay_len2_ms        = 75.0f;
      pad_settings.decay_grad_fact2     = 300.0f;
      pad_settings.decay_len3_ms        = 300.0f;
      pad_settings.decay_grad_fact3     = 100.0f;
      pad_settings.rim_use_low_freq_bp  = false;
      pad_settings.pos_sense_is_used    = true;
      pad_settings.rim_shot_is_used     = true;
      break;

    case PDX100:
      pad_settings.velocity_threshold   = 6;
      pad_settings.velocity_sensitivity = 5;
      pad_settings.pos_threshold        = 9;
      pad_settings.pos_sensitivity      = 16;
      pad_settings.rim_shot_threshold   = 24;
      pad_settings.decay_fact_db        = 2.0f;
      pad_settings.decay_len2_ms        = 65.0f;
      pad_settings.decay_grad_fact2     = 300.0f;
      pad_settings.decay_len3_ms        = 350.0f;
      pad_settings.decay_grad_fact3     = 150.0f;
      pad_settings.pos_low_pass_cutoff  = 120.0f;
      pad_settings.pos_sense_is_used    = true;
      pad_settings.rim_shot_is_used     = true;
      break;

    case PDX8:
      pad_settings.velocity_threshold   = 6;
      pad_settings.velocity_sensitivity = 4;
      pad_settings.rim_shot_threshold   = 14;
      pad_settings.pos_threshold        = 21;
      pad_settings.pos_sensitivity      = 27;
      pad_settings.pos_sense_is_used    = true;
      pad_settings.rim_shot_is_used     = true;
      break;

    case PD5: // single trigger
      pad_settings.scan_time_ms     = 4.0f;
      pad_settings.mask_time_ms     = 9.0f;
      pad_settings.decay_grad_fact2 = 400.0f;
      break;

    case PD6: // single trigger
      pad_settings.velocity_sensitivity = 4;
      pad_settings.scan_time_ms         = 1.5f;
      pad_settings.decay_grad_fact2     = 400.0f;
      break;

    case PD8: // dual trigger
      pad_settings.is_rim_switch         = true;
      pad_settings.velocity_sensitivity  = 3;
      pad_settings.pos_threshold         = 26;
      pad_settings.pos_sensitivity       = 11;
      pad_settings.rim_shot_threshold    = 22;
      pad_settings.mask_time_ms          = 7;
      pad_settings.scan_time_ms          = 1.3f;
      pad_settings.decay_est_delay_ms    = 6.0f;
      pad_settings.decay_fact_db         = 5.0f;
      pad_settings.decay_len2_ms         = 30.0f;
      pad_settings.decay_grad_fact2      = 600.0f;
      pad_settings.decay_len3_ms         = 150.0f;
      pad_settings.decay_grad_fact3      = 120.0f;
      pad_settings.pos_sense_is_used     = true;
      pad_settings.rim_shot_is_used      = true;
      pad_settings.clip_comp_ampmap_step = 0.4f;
      break;

    case TP80: // single trigger
      pad_settings.velocity_sensitivity = 8;
      pad_settings.pos_threshold        = 27;
      pad_settings.pos_sensitivity      = 19;
      pad_settings.scan_time_ms         = 2.75f;
      pad_settings.decay_est_delay_ms   = 11.0f;
      pad_settings.decay_len2_ms        = 60.0f;
      pad_settings.decay_grad_fact2     = 400.0f;
      pad_settings.decay_len3_ms        = 700.0f;
      pad_settings.decay_grad_fact3     = 60.0f;
      pad_settings.pos_invert           = true;
      pad_settings.pos_sense_is_used    = true;
      break;

    case FD8:
      pad_settings.is_control           = true;
      pad_settings.pos_threshold        = 5;
      pad_settings.pos_sensitivity      = 0;
      pad_settings.velocity_threshold   = 9;  // pedal stomp configuration
      pad_settings.velocity_sensitivity = 11; // pedal stomp configuration
      break;

    case VH12: // dual trigger
// TODO if the Hi-Hat is open just a little bit, we get double triggers
      pad_settings.is_rim_switch        = true;
      pad_settings.velocity_sensitivity = 5;
      pad_settings.rim_shot_threshold   = 23;
      pad_settings.rim_shot_boost       = 0;
      pad_settings.scan_time_ms         = 4.0f;
      pad_settings.decay_est_delay_ms   = 9.0f;
      pad_settings.decay_fact_db        = 5.0f;
      pad_settings.decay_len2_ms        = 27.0f;
      pad_settings.decay_grad_fact2     = 700.0f;
      pad_settings.decay_len3_ms        = 600.0f; // must be long because of open Hi-Hat ringing
      pad_settings.decay_grad_fact3     = 75.0f;
      break;

    case VH12CTRL:
      pad_settings.is_control      = true;
      pad_settings.pos_threshold   = 19;
      pad_settings.pos_sensitivity = 28;
      break;

    case KD7: // single trigger
      pad_settings.velocity_threshold   = 11;
      pad_settings.velocity_sensitivity = 6;
      pad_settings.curve_type           = LOG2; // less dynamic on kick (similar to other drum modules)
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

    case KD8: // single trigger
      pad_settings.velocity_sensitivity    = 2;
      pad_settings.curve_type              = LOG2; // less dynamic on kick (similar to other drum modules)
      pad_settings.scan_time_ms            = 3.0f;
      pad_settings.mask_time_decay_fact_db = 10.0f;
      pad_settings.decay_grad_fact2        = 450.0f;
      pad_settings.decay_len3_ms           = 500.0f;
      pad_settings.decay_grad_fact3        = 45.0f;
      break;

    case KT10: // single trigger
      pad_settings.velocity_sensitivity = 6;
      pad_settings.scan_time_ms         = 2.0f;
      pad_settings.decay_est_delay_ms   = 8.0f;
      pad_settings.decay_fact_db        = 5.0f;
      pad_settings.decay_len1_ms        = 4.0f;
      pad_settings.decay_grad_fact1     = 30.0f;
      pad_settings.decay_len2_ms        = 30.0f;
      pad_settings.decay_grad_fact2     = 350.0f;
      pad_settings.decay_len3_ms        = 500.0f;
      pad_settings.decay_grad_fact3     = 100.0f;
      break;

    case KD120: // single trigger
      pad_settings.scan_time_ms      = 6.0f;
      pad_settings.decay_fact_db     = 12.0f;
      pad_settings.decay_est_fact_db = 20.0f;
      break;

    case CY5: // dual trigger
      pad_settings.is_rim_switch        = true;
      pad_settings.velocity_threshold   = 6;
      pad_settings.velocity_sensitivity = 4;
      pad_settings.rim_shot_threshold   = 12;
      pad_settings.rim_shot_boost       = 0;
      pad_settings.scan_time_ms         = 3.0f;
      pad_settings.mask_time_ms         = 8.0f;
      pad_settings.decay_fact_db        = 3.0f;
      pad_settings.rim_shot_is_used     = true;
      break;

    case CY6: // dual trigger
      pad_settings.is_rim_switch        = true;
      pad_settings.velocity_sensitivity = 6;
      pad_settings.rim_shot_threshold   = 12;
      pad_settings.rim_shot_boost       = 0;
      pad_settings.scan_time_ms         = 6.0f;
      pad_settings.decay_len2_ms        = 150.0f;
      pad_settings.decay_grad_fact2     = 120.0f;
      pad_settings.decay_len3_ms        = 450.0f;
      pad_settings.decay_grad_fact3     = 30.0f;
      pad_settings.rim_shot_is_used     = true;
      break;

    case CY8: // dual trigger
      pad_settings.is_rim_switch        = true;
      pad_settings.velocity_threshold   = 10;
      pad_settings.velocity_sensitivity = 5;
      pad_settings.rim_shot_threshold   = 10;
      pad_settings.rim_shot_boost       = 0;
      pad_settings.curve_type           = LOG2;
      pad_settings.scan_time_ms         = 6.0f;
      pad_settings.decay_len1_ms        = 10.0f;
      pad_settings.decay_grad_fact1     = 10.0f;
      pad_settings.decay_len2_ms        = 100.0f;
      pad_settings.decay_grad_fact2     = 200.0f;
      pad_settings.decay_len3_ms        = 450.0f;
      pad_settings.decay_grad_fact3     = 30.0f;
      pad_settings.rim_shot_is_used     = true;
      break;

    case DIABOLO12:
      pad_settings.scan_time_ms              = 2.0f;
      pad_settings.mask_time_ms              = 8.0f;
      pad_settings.first_peak_diff_thresh_db = 13.0f;
      pad_settings.mask_time_decay_fact_db   = 20.0f;
      pad_settings.decay_grad_fact2          = 270.0f;
      pad_settings.decay_fact_db             = 6.0f;
      pad_settings.decay_est_delay_ms        = 20.0f;
      pad_settings.pos_low_pass_cutoff       = 50.0f; // positional sensing seems not to work correctly
      pad_settings.pos_sense_is_used         = true;
      pad_settings.rim_shot_is_used          = true;
      break;

    case HD1TOM: // single trigger
      pad_settings.velocity_threshold   = 8;
      pad_settings.velocity_sensitivity = 5;
      pad_settings.curve_type           = LOG1;
      pad_settings.scan_time_ms         = 1.5f;
      pad_settings.decay_grad_fact2     = 300.0f;
      pad_settings.pos_low_pass_cutoff  = 300.0f;
      break;
  }
}
