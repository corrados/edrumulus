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


Edrumulus::Edrumulus()
{
  // initializations
  overload_LED_on_time       = round ( overload_LED_on_time_s * Fs );
  error_LED_blink_time       = round ( error_LED_blink_time_s * Fs );
  dc_offset_est_len          = round ( dc_offset_est_len_s * Fs );
  samplerate_max_cnt         = round ( samplerate_max_cnt_len_s * Fs );
  dc_offset_min_limit        = round ( ADC_MAX_RANGE / 2 - ADC_MAX_RANGE * dc_offset_max_rel_error );
  dc_offset_max_limit        = round ( ADC_MAX_RANGE / 2 + ADC_MAX_RANGE * dc_offset_max_rel_error );
  overload_LED_cnt           = 0;
  error_LED_cnt              = 0;
  status_is_overload         = false;
  samplerate_prev_micros_cnt = 0;
  samplerate_prev_micros     = 0;
  status_is_error            = false;
  dc_offset_error_channel    = -1;
#ifdef ESP_PLATFORM
  spike_cancel_level = 4; // use max. spike cancellation on the ESP32 per default (note that it increases the latency)
#else
  spike_cancel_level = 0; // default
#endif
  cancel_num_samples            = ( cancel_time_ms * Fs ) / 1000;
  cancel_cnt                    = 0;
  cancel_MIDI_velocity          = 1;
  cancel_pad_index              = 0;
  any_coupling_used             = false;
  coupled_pad_idx_primary       = -1; // disable coupling
  coupled_pad_idx_rim_primary   = -1; // disable coupling
  coupled_pad_idx_secondary     = 0;  // disable coupling
  coupled_pad_idx_rim_secondary = 0;  // disable coupling

  // calculate DC offset IIR1 low pass filter parameters, see
  // http://www.tsdconseil.fr/tutos/tuto-iir1-en.pdf: gamma = exp(-Ts/tau)
  dc_offset_iir_gamma           = exp ( - 1.0f / ( Fs * dc_offset_iir_tau_seconds ) );
  dc_offset_iir_one_minus_gamma = 1.0f - dc_offset_iir_gamma;
}


void Edrumulus::setup ( const int  conf_num_pads,
                        const int* conf_analog_pins,
                        const int* conf_analog_pins_rim_shot )
{
  number_pads = min ( conf_num_pads, MAX_NUM_PADS );

  for ( int i = 0; i < number_pads; i++ )
  {
    // set the pad GIOP pin numbers
    analog_pin[i][0] = conf_analog_pins[i];
    analog_pin[i][1] = conf_analog_pins_rim_shot[i];
    number_inputs[i] = conf_analog_pins_rim_shot[i] >= 0 ? 2 : 1;

    // setup the pad
    pad[i].setup ( Fs );
  }

  // setup the ESP32 specific object, this has to be done after assigning the analog
  // pin numbers and before using the analog read function (as in the DC offset estimator)
  edrumulus_hardware.setup ( Fs,
                             number_pads,
                             number_inputs,
                             analog_pin );

  // estimate the DC offset for all inputs
  float dc_offset_sum[MAX_NUM_PADS][MAX_NUM_PAD_INPUTS];

  for ( int k = 0; k < dc_offset_est_len; k++ )
  {
    edrumulus_hardware.capture_samples ( number_pads,
                                         number_inputs,
                                         analog_pin,
                                         sample_org );

    for ( int i = 0; i < number_pads; i++ )
    {
      for ( int j = 0; j < number_inputs[i]; j++ )
      {
        if ( k == 0 )
        {
          // initial value
          dc_offset_sum[i][j] = sample_org[i][j];
        }
        else if ( k == dc_offset_est_len - 1 )
        {
          // we are done, calculate the DC offset now
          dc_offset[i][j] = dc_offset_sum[i][j] / dc_offset_est_len;
        }
        else
        {
          // intermediate value, add to the existing value
          dc_offset_sum[i][j] += sample_org[i][j];
        }
      }
    }
  }
}


void Edrumulus::process()
{
/*
// TEST for debugging: take samples from Octave, process and return result to Octave
if ( Serial.available() > 0 )
{
  static int m = micros(); if ( micros() - m > 500000 ) pad[0].set_velocity_threshold ( 14.938 ); m = micros(); // 17 dB threshold
  float fIn[2]; fIn[0] = Serial.parseFloat(); fIn[1] = 0.0f;//Serial.parseFloat();
  bool peak_found_debug, is_rim_shot_debug, is_choke_on_debug, is_choke_off_debug;
  int  midi_velocity_debug, midi_pos_debug;
  float y = pad[0].process_sample ( fIn, false, peak_found_debug, midi_velocity_debug, midi_pos_debug, is_rim_shot_debug, is_choke_on_debug, is_choke_off_debug );
  Serial.println ( y, 7 );
}
return;
*/


  // Query samples -------------------------------------------------------------
  // note that this is a blocking function
  edrumulus_hardware.capture_samples ( number_pads,
                                       number_inputs,
                                       analog_pin,
                                       sample_org );

/*
// TEST for plotting all captures samples in the serial plotter (but with low sampling rate)
String serial_print;
for ( int i = 0; i < number_pads; i++ )
{
  //if ( !pad[i].get_is_control() )
  {
    for ( int j = 0; j < number_inputs[i]; j++ )
    {
      serial_print += String ( sample_org[i][j] ) + "\t";
    }
  }
}
Serial.println ( serial_print );
*/


  // Process samples -----------------------------------------------------------
  for ( int i = 0; i < number_pads; i++ )
  {
    int* sample_org_pad = sample_org[i];
    peak_found[i]       = false;
    control_found[i]    = false;

    if ( pad[i].get_is_control() )
    {
      // process sample for control input
      pad[i].process_control_sample ( sample_org_pad, control_found[i], midi_ctrl_value[i],
                                      peak_found[i], midi_velocity[i] );
    }
    else
    {
      // prepare samples for processing
      for ( int j = 0; j < number_inputs[i]; j++ )
      {
        const bool is_rim_switch_input = ( j == 1 ) && pad[i].get_is_rim_switch(); // rim is always on second channel

        // overload detection: check for the lowest/largest possible ADC range values with noise consideration
        if ( sample_org_pad[j] >= ( ADC_MAX_RANGE - ADC_MAX_NOISE_AMPL ) )
        {
          overload_LED_cnt     = overload_LED_on_time;
          overload_detected[j] = 2;
        }
        else if ( sample_org_pad[j] <= ADC_MAX_NOISE_AMPL - 1 )
        {
          overload_LED_cnt     = overload_LED_on_time;
          overload_detected[j] = 1;
        }
        else
        {
          overload_detected[j] = 0;
        }

        // update DC offset by using an IIR1 low pass filter (but disable update if
        // rim switch input is on to avoid the DC offset is incorrect in case the switch is
        // held for a while by the user)
        if ( !( is_rim_switch_input && pad[i].get_is_rim_switch_on() ) )
        {
          dc_offset[i][j] = dc_offset_iir_gamma * dc_offset[i][j] + dc_offset_iir_one_minus_gamma * sample_org_pad[j];
        }

        // compensate DC offset
        sample[j] = sample_org_pad[j] - dc_offset[i][j];

        // ADC spike cancellation (do not use spike cancellation for rim switches since they have short peaks)
        if ( ( spike_cancel_level > 0 ) && !is_rim_switch_input )
        {
          edrumulus_hardware.cancel_ADC_spikes ( sample[j], overload_detected[j], i, j, spike_cancel_level );
        }
      }

      // process sample
      if ( any_coupling_used && // note: short-cut for speed optimization of normal non-coupling mode
           ( ( ( coupled_pad_idx_primary >= 0 ) &&     ( ( i == coupled_pad_idx_secondary )     || ( i == coupled_pad_idx_primary ) ) ) ||
             ( ( coupled_pad_idx_rim_primary >= 0 ) && ( ( i == coupled_pad_idx_rim_secondary ) || ( i == coupled_pad_idx_rim_primary ) ) ) ) )
      {
        // special case: couple pad inputs for multiple head sensor capturing (assume that both pads have dual-inputs)
        if ( ( i == coupled_pad_idx_primary ) || ( i == coupled_pad_idx_secondary ) )
        {
          if ( ( ( coupled_pad_idx_primary   < coupled_pad_idx_secondary ) && ( i == coupled_pad_idx_primary ) ) ||
               ( ( coupled_pad_idx_secondary < coupled_pad_idx_primary )   && ( i == coupled_pad_idx_secondary ) ) )
          {
            stored_sample_coupled_head[0]            = sample[0];            // store 1st input
            stored_sample_coupled_head[1]            = sample[1];
            stored_overload_detected_coupled_head[0] = overload_detected[0]; // store 2nd input
            stored_overload_detected_coupled_head[1] = overload_detected[1];
          }
          else
          {
            // combine samples and process pad coupled_pad_idx_primary which is the primary coupled pad,
            // new "sample" layout: sum, rim, 1st head, 2nd head, 3rd head
            if ( coupled_pad_idx_primary > coupled_pad_idx_secondary )
            {
              sample[2]            = sample[0];                     // 1st head (note that rim is already at correct place)
              overload_detected[2] = overload_detected[0];
              sample[3]            = stored_sample_coupled_head[0]; // 2nd head
              overload_detected[3] = stored_overload_detected_coupled_head[0];
              sample[4]            = stored_sample_coupled_head[1]; // 3rd head
              overload_detected[4] = stored_overload_detected_coupled_head[1];
            }
            else
            {
              sample[3]            = sample[0];                     // 2nd head
              overload_detected[3] = overload_detected[0];
              sample[4]            = sample[1];                     // 3rd head
              overload_detected[4] = overload_detected[1];
              sample[1]            = stored_sample_coupled_head[1]; // rim (no overload_detected used for rim)
              sample[2]            = stored_sample_coupled_head[0]; // 1st head
              overload_detected[2] = stored_overload_detected_coupled_head[0];
            }
            sample[0] = ( sample[2] + sample[3] + sample[4] ) / 3; // sum is on channel 0

            pad[coupled_pad_idx_primary].process_sample ( sample, 5,                            overload_detected,
                                                          peak_found[coupled_pad_idx_primary],  midi_velocity[coupled_pad_idx_primary],
                                                          midi_pos[coupled_pad_idx_primary],    rim_state[coupled_pad_idx_primary],
                                                          is_choke_on[coupled_pad_idx_primary], is_choke_off[coupled_pad_idx_primary] );
          }
        }

        // special case: couple pad inputs for two-rim sensor capturing
        if ( ( i == coupled_pad_idx_rim_primary ) || ( i == coupled_pad_idx_rim_secondary ) )
        {
          if ( ( ( coupled_pad_idx_rim_primary   < coupled_pad_idx_rim_secondary ) && ( i == coupled_pad_idx_rim_primary ) ) ||
               ( ( coupled_pad_idx_rim_secondary < coupled_pad_idx_rim_primary )   && ( i == coupled_pad_idx_rim_secondary ) ) )
          {
            stored_sample_coupled_rim[0]            = sample[0];            // store 1st input
            stored_sample_coupled_rim[1]            = sample[1];
            stored_overload_detected_coupled_rim[0] = overload_detected[0]; // store 2nd input
            stored_overload_detected_coupled_rim[1] = overload_detected[1];
          }
          else
          {
            // combine samples and process pad coupled_pad_idx_rim_primary which is the primary coupled pad,
            // new "sample" layout: 1st head, 1st rim, 2nd rim
            if ( coupled_pad_idx_rim_primary > coupled_pad_idx_rim_secondary )
            {
              sample[2] = stored_sample_coupled_rim[0]; // 1st head/rim are at correct place, copy 2nd rim
            }
            else
            {
              sample[2]            = sample[0];                    // 2nd rim
              sample[0]            = stored_sample_coupled_rim[0]; // 1st head
              sample[1]            = stored_sample_coupled_rim[1]; // 1st rim
              overload_detected[0] = stored_overload_detected_coupled_rim[0];
            }

            pad[coupled_pad_idx_rim_primary].process_sample ( sample, 3,                                overload_detected,
                                                              peak_found[coupled_pad_idx_rim_primary],  midi_velocity[coupled_pad_idx_rim_primary],
                                                              midi_pos[coupled_pad_idx_rim_primary],    rim_state[coupled_pad_idx_rim_primary],
                                                              is_choke_on[coupled_pad_idx_rim_primary], is_choke_off[coupled_pad_idx_rim_primary] );
          }
        }
      }
      else
      {
        // normal case: process samples directly
        pad[i].process_sample ( sample,        number_inputs[i], overload_detected,
                                peak_found[i], midi_velocity[i], midi_pos[i],
                                rim_state[i],  is_choke_on[i],   is_choke_off[i] );
      }
    }
  }


  // Cross talk cancellation ---------------------------------------------------
  for ( int i = 0; i < number_pads; i++ )
  {
    if ( peak_found[i] )
    {
      // reset cancellation count if conditions are met
      if ( ( cancel_cnt == 0 ) || ( ( cancel_cnt > 0 ) && ( midi_velocity[i] > cancel_MIDI_velocity ) ) )
      {
        cancel_cnt           = cancel_num_samples;
        cancel_MIDI_velocity = midi_velocity[i];
        cancel_pad_index     = i;
      }
      else if ( ( cancel_cnt > 0 ) && ( cancel_pad_index != i ) )
      {
        // check if current pad is to be cancelled
        if ( cancel_MIDI_velocity * pad[i].get_cancellation_factor() > midi_velocity[i] )
        {
          peak_found[i] = false;
        }
      }
    }
  }

  if ( cancel_cnt > 0 )
  {
    cancel_cnt--;
  }


  // Overload detection: keep LED on for a while -------------------------------
  if ( overload_LED_cnt > 0 )
  {
    overload_LED_cnt--;
    status_is_overload = ( overload_LED_cnt > 0 );
  }


  // Sampling rate and DC offset check -----------------------------------------
  // (i.e. if CPU is overloaded, the sample rate will drop which is bad)
  if ( samplerate_prev_micros_cnt >= samplerate_max_cnt )
  {
    const unsigned long samplerate_cur_micros = micros();

// TEST check the measured sampling rate
//Serial.println ( 1.0f / ( samplerate_cur_micros - samplerate_prev_micros ) * samplerate_max_cnt * 1e6f, 7 );

    // do not update status if micros() has wrapped around (at about 70 minutes) and if
    // we have the very first measurement after start (previous micros set to 0)
    if ( ( samplerate_prev_micros != 0 ) && ( samplerate_cur_micros - samplerate_prev_micros > 0 ) )
    {
      // set error flag if sample rate deviation is too large
      status_is_error = ( abs ( 1.0f / ( samplerate_cur_micros - samplerate_prev_micros ) * samplerate_max_cnt * 1e6f - Fs ) > samplerate_max_error_Hz );
    }

    samplerate_prev_micros_cnt = 0;
    samplerate_prev_micros     = samplerate_cur_micros;

    // DC offset check
    dc_offset_error_channel = -1; // invalidate for "no DC offset error" case
    for ( int i = 0; i < number_pads; i++ )
    {
      if ( !pad[i].get_is_control() )
      {
        for ( int j = 0; j < number_inputs[i]; j++ )
        {
          const float& cur_dc_offset = dc_offset[i][j];
//Serial.println ( String ( i ) + ", " + String ( cur_dc_offset ) ); // TEST for plotting all DC offsets
          if ( ( cur_dc_offset < dc_offset_min_limit ) || ( cur_dc_offset > dc_offset_max_limit ) )
          {
            status_is_error         = true;
            dc_offset_error_channel = i + 32 * j; // 0 to 31: input 0, 32 to 63: input 1
          }
        }
      }
    }
  }
  samplerate_prev_micros_cnt++;
  error_LED_cnt++;
}


void Edrumulus::set_coupled_pad_idx ( const int pad_idx, const int new_idx )
{
  // There are two modes supported:
  // 1. Coupled head sensor mode, i.e., we have three head piezo sensors and one rim sensor.
  // 2. Coupled rim sensor mode, i.e., we have a ride pad with bell/edge support so wie have on head sensor and two rim switch sensors.
  // Only special pad types support coupling:
  // Case 1.: PDA120LS
  // Case 2.: CY6, CY8, CY5 (note that we should introduce a CY12R type but in the meantime we re-use the existing cymbal pad types)
  // Case 1. requires two dual-pad inputs and Case 2. requires one dual-pad and one single pad input.
  // NOTE that coupling is only enabled if the pad type match and coupling is either OFF or this
  //      is the pad which is currently using coupling.
  if ( new_idx < MAX_NUM_PADS )
  {
    if ( pad[pad_idx].get_pad_type() == PDA120LS )
    {
      // Case 1. ---------------------------------------------------------------
      if ( ( coupled_pad_idx_primary < 0 ) || ( pad_idx == coupled_pad_idx_primary ) )
      {
        // special case: always set coupled pad index parameter regardless if it is valid
        // or not to avoid issues in the GUI but if the index is invalid (i.e., no two inputs
        // available), do not enable the coupling
        pad[pad_idx].set_coupled_pad_idx ( new_idx );

        const int cur_idx         = number_inputs[new_idx] > 1 ? new_idx : 0 /* 0 disables coupling */;
        coupled_pad_idx_primary   = cur_idx > 0 ? pad_idx : -1; // primary set to -1 switches coupling OFF
        coupled_pad_idx_secondary = cur_idx;
        pad[pad_idx].set_head_sensor_coupling ( cur_idx > 0 );
      }
    }
    else if ( ( pad[pad_idx].get_pad_type() == CY6 ) ||
              ( pad[pad_idx].get_pad_type() == CY8 ) ||
              ( pad[pad_idx].get_pad_type() == CY5 ) )
    {
      // Case 2. ---------------------------------------------------------------
      if ( ( coupled_pad_idx_rim_primary < 0 ) || ( pad_idx == coupled_pad_idx_rim_primary ) )
      {
        pad[pad_idx].set_coupled_pad_idx ( new_idx );
        coupled_pad_idx_rim_primary   = new_idx > 0 ? pad_idx : -1; // primary set to -1 switches coupling OFF
        coupled_pad_idx_rim_secondary = new_idx;
        pad[pad_idx].set_use_second_rim ( new_idx > 0 );
      }
    }

    any_coupling_used = ( coupled_pad_idx_primary >= 0 ) || ( coupled_pad_idx_rim_primary >= 0 );
  }
}


// -----------------------------------------------------------------------------
// Pad -------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void Edrumulus::Pad::setup ( const int conf_Fs )
{
  // set essential parameters
  Fs               = conf_Fs;
  init_delay_value = static_cast<int> ( init_delay_value_s * conf_Fs );

  // initialize with default pad type and other defaults
  set_pad_type ( PD6 );
  midi_note                = 38;
  midi_note_rim            = 40;
  midi_note_open           = 46;
  midi_note_open_rim       = 26;
  midi_ctrl_ch             = 4; // CC4, usually used for hi-hat
  use_head_sensor_coupling = false;
  use_second_rim           = false;
  init_delay_cnt           = 0; // note that it resets value of set_pad_type above
  initialize(); // do very first initialization without delay
}


void Edrumulus::Pad::set_pad_type ( const Epadtype new_pad_type )
{
  // apply new pad type and set all parameters to the default values for that pad type
  pad_settings.pad_type = new_pad_type;

  apply_preset_pad_settings();
  sched_init();
}


void Edrumulus::Pad::manage_delayed_initialization()
{
  // manage delayed initialization (make sure only one initialization for multiple quick settings changes)
  if ( init_delay_cnt > 0 )
  {
    init_delay_cnt--;
    if ( init_delay_cnt == 0 )
    {
      initialize();
    }
  }
}


void Edrumulus::Pad::initialize()
{
  // in case we have a coupled sensor pad, the number of head sensors is 4, where 3 sensor signals and one sum
  number_head_sensors = use_head_sensor_coupling ? 4 : 1; // 1 or 4 head sensor inputs

  // set algorithm parameters
  const float threshold_db = 20 * log10 ( ADC_MAX_NOISE_AMPL ) - 16.0f + pad_settings.velocity_threshold; // threshold range considering the maximum ADC noise level
  threshold                = pow   ( 10.0f, threshold_db / 10 );                   // linear power threshold
  first_peak_diff_thresh   = pow   ( 10.0f, pad_settings.first_peak_diff_thresh_db / 10 ); // difference allowed between first peak and later peak in scan time
  scan_time                = round ( pad_settings.scan_time_ms     * 1e-3f * Fs ); // scan time from first detected peak
  pre_scan_time            = round ( pad_settings.pre_scan_time_ms * 1e-3f * Fs );
  total_scan_time          = scan_time + pre_scan_time;                         // includes pre-scan time
  mask_time                = round ( pad_settings.mask_time_ms  * 1e-3f * Fs ); // mask time (e.g. 10 ms)
  decay_len1               = round ( pad_settings.decay_len1_ms * 1e-3f * Fs ); // decay time 1 (e.g. 250 ms)
  decay_len2               = round ( pad_settings.decay_len2_ms * 1e-3f * Fs ); // decay time 2 (e.g. 250 ms)
  decay_len3               = round ( pad_settings.decay_len3_ms * 1e-3f * Fs ); // decay time 3 (e.g. 250 ms)
  decay_len                = decay_len1 + decay_len2 + decay_len3;
  decay_fact               = pow   ( 10.0f, pad_settings.decay_fact_db / 10 );
  decay_mask_fact          = pow   ( 10.0f, pad_settings.mask_time_decay_fact_db / 10 );
  const float decay_grad1  = pad_settings.decay_grad_fact1 / Fs; // decay gradient factor 1
  const float decay_grad2  = pad_settings.decay_grad_fact2 / Fs; // decay gradient factor 2
  const float decay_grad3  = pad_settings.decay_grad_fact3 / Fs; // decay gradient factor 3
  x_sq_hist_len            = total_scan_time;
  overload_hist_len        = x_sq_hist_len;
  decay_est_delay          = round ( pad_settings.decay_est_delay_ms * 1e-3f * Fs );
  decay_est_len            = round ( pad_settings.decay_est_len_ms   * 1e-3f * Fs );
  decay_est_fact           = pow ( 10.0f, pad_settings.decay_est_fact_db / 10 );
  rim_shot_threshold       = pow ( 10.0f, ( static_cast<float> ( pad_settings.rim_shot_threshold ) - 44 ) / 10 ); // linear rim shot threshold
  rim_shot_window_len      = round ( pad_settings.rim_shot_window_len_ms * 1e-3f * Fs );             // window length (e.g. 5 ms)
  rim_shot_boost           = pow ( 10.0f, static_cast<float> ( pad_settings.rim_shot_boost ) / 40 ); // boost / 4 -> dB value
  rim_switch_threshold     = -pow ( 10.0f, pad_settings.rim_shot_threshold / 10.0f ); // rim switch linear threshold, where 10^(31/10)=1259 which is approx 4096/3 (10 bit ADC)
  rim_switch_on_cnt_thresh = round ( 10.0f * 1e-3f * Fs );                                           // number of on samples until we detect a choke
  rim_max_power_low_limit  = ADC_MAX_NOISE_AMPL * ADC_MAX_NOISE_AMPL / 31.0f; // lower limit on detected rim power, 15 dB below max noise amplitude
  x_rim_hist_len           = x_sq_hist_len + rim_shot_window_len;
  cancellation_factor      = static_cast<float> ( pad_settings.cancellation ) / 31.0f; // cancellation factor: range of 0.0..1.0
  ctrl_history_len_half    = ctrl_history_len / 2;
  max_num_overloads        = 3; // maximum allowed number of overloaded samples until the overload special case is activated

  // The ESP32 ADC has 12 bits resulting in a range of 20*log10(2048)=66.2 dB.
  // The sensitivity parameter shall be in the range of 0..31. This range should then be mapped to the
  // maximum possible dynamic where sensitivity of 31 means that we have no dynamic at all and 0 means
  // that we use the full possible ADC range.
  const float max_velocity_range_db = 20 * log10 ( ADC_MAX_RANGE / 2 ) - threshold_db;
  const float velocity_range_db     = max_velocity_range_db * ( 32 - pad_settings.velocity_sensitivity ) / 32;

  // Consider MIDI curve (taken from RyoKosaka HelloDrum-arduino-Library: int HelloDrum::curve() function)
  // by calculating three parameters: velocity_factor * x ^ velocity_exponent + velocity_offset.
  // The approach is to use the original power-to-MIDI conversion function:
  // ( 10 * log10 ( prev_hil_filt_val / threshold ) / velocity_range_db ) * 127
  // and apply the MIDI curve:
  // ( 126 / ( pow ( curve_param, 126 ) - 1 ) ) * ( pow ( curve_param, i - 1 ) - 1 ) + 1.
  // After applying some calculations (see calc_midi_curve_parameters.pdf), we get the following parameters:
  float curve_param = 1.018f; // this curve parameter comes close to what Roland is doing for "linear"
  switch ( pad_settings.curve_type )
  {
    case EXP1: curve_param *= 1.012f;    break;
    case EXP2: curve_param *= 1.017f;    break;
    case LOG1: curve_param *= 0.995f;    break;
    case LOG2: curve_param *= 0.987f;    break;
    default: /* LINEAR, nothing to do */ break;
  }

  velocity_factor = 126.0f / ( ( pow ( curve_param, 126.0f ) - 1 ) * curve_param *
    pow ( threshold, 1270.0f / velocity_range_db * log10 ( curve_param ) ) );

  velocity_exponent = 1270.0f / velocity_range_db * log10 ( curve_param );
  velocity_offset   = 1.0f - 126.0f / ( pow ( curve_param, 126.0f ) - 1 );

  // The positional sensing MIDI assignment parameters are dependent on, e.g., the filter design
  // parameters and cannot easily be derived from the ADC properties as is done for the velocity.
  // Based on the measurement results with the PD120 pad, we tryed to derive some meaningful parameter ranges.
  const float pos_threshold_db = pad_settings.pos_threshold;           // gives us a threshold range of 0..31 dB
  pos_threshold                = pow ( 10.0f, pos_threshold_db / 10 ); // linear power threshold
  const float max_pos_range_db = 11; // dB (found by analyzing pd120_pos_sense2.wav test signal)
  pos_range_db                 = max_pos_range_db * ( 32 - pad_settings.pos_sensitivity ) / 32;

  // control MIDI assignment gives us a range of 410-2867 (FD-8: 3300-0, VH-12: 2200-1900 (press: 1770))
  control_threshold = pad_settings.pos_threshold / 31.0f * ( 0.6f * ADC_MAX_RANGE ) + ( 0.1f * ADC_MAX_RANGE );
  control_range     = ( ADC_MAX_RANGE - control_threshold ) * ( 32 - pad_settings.pos_sensitivity ) / 32;

  // hi-hat pedal stomp action parameters
  ctrl_velocity_range_fact = pow ( 10.0f, pad_settings.velocity_sensitivity / 10.0f );   // linear range of 1..1259
  ctrl_velocity_threshold  = pow ( 10.0f, pad_settings.velocity_threshold / 3.0f / 10.0f ) - 1; // linear range of 0..10

  // positional sensing low-pass filter properties
  // moving average cut off frequency approximation according to:
  // https://dsp.stackexchange.com/questions/9966/what-is-the-cut-off-frequency-of-a-moving-average-filter
  const float lp_cutoff_norm = pad_settings.pos_low_pass_cutoff / Fs;
  lp_filt_len                = round ( sqrt ( 0.196202f + lp_cutoff_norm * lp_cutoff_norm ) / lp_cutoff_norm );
  if ( ( lp_filt_len % 2 ) == 0 )
  {
    lp_filt_len++; // make sure we have an odd length
  }
  const int lp_half_len = ( lp_filt_len - 1 ) / 2;
  x_low_hist_len        = x_sq_hist_len + lp_filt_len;

  // clipping compensation initialization
  for ( int i = 0; i < length_ampmap; i++ )
  {
    // never to higher than 5
    amplification_mapping[i] = min ( 5.0f, pow ( 10.0f, ( i * pad_settings.clip_comp_ampmap_step ) *
                                                        ( i * pad_settings.clip_comp_ampmap_step ) ) );
  }

  // pre-calculate equations needed for 3 sensor get position function
  get_pos_x0 =  0.433f; get_pos_y0 =  0.25f; // sensor 0 position
  get_pos_x1 =  0.0;    get_pos_y1 = -0.5f;  // sensor 1 position
  get_pos_x2 = -0.433f; get_pos_y2 =  0.25f; // sensor 2 position
  get_pos_rim_radius       = 0.75f;          // rim radius
  get_pos_x0_sq_plus_y0_sq = get_pos_x0 * get_pos_x0 + get_pos_y0 * get_pos_y0;
  get_pos_a1               = 2 * ( get_pos_x0 - get_pos_x1 );
  get_pos_b1               = 2 * ( get_pos_y0 - get_pos_y1 );
  get_pos_a2               = 2 * ( get_pos_x0 - get_pos_x2 );
  get_pos_b2               = 2 * ( get_pos_y0 - get_pos_y2 );
  get_pos_div1_fact        = 1.0f / ( get_pos_a1 * get_pos_b2 - get_pos_a2 * get_pos_b1 );
  get_pos_div2_fact        = 1.0f / ( get_pos_a2 * get_pos_b1 - get_pos_a1 * get_pos_b2 );

  // allocate and initialize memory for vectors and initialize scalars
  allocate_initialize ( &rim_bp_filt_b, bp_filt_len );      // rim band-pass filter coefficients b
  allocate_initialize ( &rim_bp_filt_a, bp_filt_len - 1 );  // rim band-pass filter coefficients a
  allocate_initialize ( &decay,         decay_len );        // memory for decay function
  allocate_initialize ( &lp_filt_b,     lp_filt_len );      // memory for low-pass filter coefficients
  allocate_initialize ( &ctrl_hist,     ctrl_history_len ); // memory for Hi-Hat control pad hit detection
  prev_ctrl_value     = 0;
  multiple_sensor_cnt = 0;

  for ( int in = 0; in < number_head_sensors; in++ )
  {
    SSensor& s = sSensor[in];
    s.x_sq_hist.initialize             ( x_sq_hist_len );       // memory for sqr(x) history
    s.overload_hist.initialize         ( overload_hist_len );   // memory for overload detection status
    s.x_low_hist.initialize            ( x_low_hist_len );      // memory for low-pass filter result
    s.x_rim_switch_hist.initialize     ( rim_shot_window_len ); // memory for rim switch detection
    s.x_sec_rim_switch_hist.initialize ( rim_shot_window_len ); // memory for second rim switch detection
    allocate_initialize ( &s.bp_filt_hist_x, bp_filt_len );     // band-pass filter x-signal history
    allocate_initialize ( &s.bp_filt_hist_y, bp_filt_len - 1 ); // band-pass filter y-signal history
    allocate_initialize ( &s.lp_filt_hist,   lp_filt_len );     // memory for low-pass filter input
    allocate_initialize ( &s.rim_bp_hist_x,  bp_filt_len );     // rim band-pass filter x-signal history
    allocate_initialize ( &s.rim_bp_hist_y,  bp_filt_len - 1 ); // rim band-pass filter y-signal history
    allocate_initialize ( &s.x_rim_hist,     x_rim_hist_len );  // memory for rim shot detection

    s.was_above_threshold     = false;
    s.is_overloaded_state     = false;
    s.mask_back_cnt           = 0;
    s.first_peak_val          = 0.0f;
    s.peak_val                = 0.0f;
    s.decay_back_cnt          = 0;
    s.decay_scaling           = 1.0f;
    s.scan_time_cnt           = 0;
    s.decay_pow_est_start_cnt = 0;
    s.decay_pow_est_cnt       = 0;
    s.decay_pow_est_sum       = 0.0f;
    s.pos_sense_cnt           = 0;
    s.x_low_hist_idx          = 0;
    s.rim_shot_cnt            = 0;
    s.rim_switch_on_cnt       = 0;
    s.max_x_filt_val          = 0.0f;
    s.max_mask_x_filt_val     = 0.0f;
    s.was_peak_found          = false;
    s.was_pos_sense_ready     = false;
    s.was_rim_shot_ready      = false;
    s.rim_state               = NO_RIM;
  }

  // calculate positional sensing low-pass filter coefficients
  for ( int i = 0; i < lp_filt_len; i++ )
  {
    if ( i < lp_half_len )
    {
      lp_filt_b[i] = ( 0.5f + i * 0.5f / lp_half_len ) / lp_filt_len;
    }
    else if ( i == lp_half_len )
    {
      lp_filt_b[i] = 1.0f / lp_filt_len;
    }
    else
    {
      lp_filt_b[i] = lp_filt_b[lp_filt_len - i - 1];
    }
  }

  // calculate the decay curve
  for ( int i = 0; i < decay_len1; i++ )
  {
    decay[i] = pow ( 10.0f, -i / 10.0f * decay_grad1 );
  }
  const float decay_fact1 = pow ( 10.0f, -decay_len1 / 10.0f * decay_grad1 );
  for ( int i = 0; i < decay_len2; i++ )
  {
    decay[decay_len1 + i] = decay_fact1 * pow ( 10.0f, -i / 10.0f * decay_grad2 );
  }
  const float decay_fact2 = decay_fact1 * pow ( 10.0f, -decay_len2 / 10.0f * decay_grad2 );
  for ( int i = 0; i < decay_len3; i++ )
  {
    decay[decay_len1 + decay_len2 + i] = decay_fact2 * pow ( 10.0f, -i / 10.0f * decay_grad3 );
  }

  // select rim shot signal band-pass filter coefficients
  if ( pad_settings.rim_use_low_freq_bp )
  {
    for ( int i = 0; i < bp_filt_len - 1; i++ )
    {
      rim_bp_filt_a[i] = rim_bp_low_freq_a[i];
    }
    for ( int i = 0; i < bp_filt_len; i++ )
    {
      rim_bp_filt_b[i] = rim_bp_low_freq_b[i];
    }
  }
  else
  {
    for ( int i = 0; i < bp_filt_len - 1; i++ )
    {
      rim_bp_filt_a[i] = rim_bp_high_freq_a[i];
    }
    for ( int i = 0; i < bp_filt_len; i++ )
    {
      rim_bp_filt_b[i] = rim_bp_high_freq_b[i];
    }
  }
}


float Edrumulus::Pad::process_sample ( const float* input,
                                       const int    input_len,
                                       const int*   overload_detected,
                                       bool&        peak_found,
                                       int&         midi_velocity,
                                       int&         midi_pos,
                                       Erimstate&   rim_state,
                                       bool&        is_choke_on,
                                       bool&        is_choke_off )
{
  // initialize return parameters and configuration parameters
  peak_found                     = false;
  midi_velocity                  = 0;
  midi_pos                       = 0;
  rim_state                      = NO_RIM;
  is_choke_on                    = false;
  is_choke_off                   = false;
  const bool pos_sense_is_used   = pad_settings.pos_sense_is_used && ( number_head_sensors == 1 ); // can be applied directly without calling initialize()
  const bool rim_shot_is_used    = pad_settings.rim_shot_is_used && ( input_len > 1 );             // can be applied directly without calling initialize()
  const bool pos_sense_inverted  = pad_settings.pos_invert;                                        // can be applied directly without calling initialize()
  float      x_filt              = 0.0f; // needed for debugging
  float      cur_decay           = 1;    // needed for debugging, initialization value (0 dB) only used for debugging
  bool       sensor0_has_results = false;

  manage_delayed_initialization();

  for ( int head_sensor_cnt = 0; head_sensor_cnt < number_head_sensors; head_sensor_cnt++ )
  {
    const int in               = head_sensor_cnt == 0 ? 0 : head_sensor_cnt + 1; // exclude rim input
    SSensor&  s                = sSensor[head_sensor_cnt];
    FastWriteFIFO& s_x_sq_hist = s.x_sq_hist; // shortcut for speed optimization
    int&      first_peak_delay = s.sResults.first_peak_delay; // use value in result struct
    bool      first_peak_found = false;
    int       peak_delay       = 0;
    first_peak_delay++; // increment first peak delay for each new sample (wraps only after some hours which is uncritical)

    // square input signal and store in FIFO buffer
    s_x_sq_hist.add     ( input[in] * input[in] );
    s.overload_hist.add ( overload_detected[in] );


    // Calculate peak detection ---------------------------------------------------
    // IIR band-pass filter
    update_fifo ( input[in], bp_filt_len, s.bp_filt_hist_x );

    float sum_b = 0.0f;
    float sum_a = 0.0f;
    for ( int i = 0; i < bp_filt_len; i++ )
    {
      sum_b += s.bp_filt_hist_x[i] * bp_filt_b[i];
    }
    for ( int i = 0; i < bp_filt_len - 1; i++ )
    {
      sum_a += s.bp_filt_hist_y[i] * bp_filt_a[i];
    }
    x_filt = sum_b - sum_a;

    update_fifo ( x_filt, bp_filt_len - 1, s.bp_filt_hist_y );
    x_filt = x_filt * x_filt; // calculate power of filter result


    // exponential decay assumption
    float x_filt_decay = x_filt;

    if ( s.decay_back_cnt > 0 )
    {
      // subtract decay (with clipping at zero)
      cur_decay    = s.decay_scaling * decay[decay_len - s.decay_back_cnt];
      x_filt_decay = x_filt - cur_decay;
      s.decay_back_cnt--;

      if ( x_filt_decay < 0.0f )
      {
        x_filt_decay = 0.0f;
      }
    }


    // during the mask time we apply a constant value to the decay way above the
    // detected peak to avoid missing a loud hit which is preceeded with a very
    // low volume hit which mask period would delete the loud hit
    if ( ( s.mask_back_cnt > 0 ) && ( s.mask_back_cnt <= mask_time ) )
    {
      if ( x_filt > s.max_mask_x_filt_val * decay_mask_fact )
      {
        s.was_above_threshold = false;  // reset the peak detection (note that x_filt_decay is always > threshold now)
        x_filt_decay          = x_filt; // remove decay subtraction
        s.pos_sense_cnt       = 0;      // needed since we reset the peak detection
        s.was_pos_sense_ready = false;  // needed since we reset the peak detection
        s.rim_shot_cnt        = 0;      // needed since we reset the peak detection
        s.was_rim_shot_ready  = false;  // needed since we reset the peak detection
      }
    }


    // threshold test
    if ( ( ( x_filt_decay > threshold ) || s.was_above_threshold ) )
    {
      // initializations at the time when the signal was above threshold for the
      // first time for the current peak
      if ( !s.was_above_threshold )
      {
        s.decay_pow_est_start_cnt = max ( 1, decay_est_delay - x_filt_delay + 1 );
        s.scan_time_cnt           = max ( 1, scan_time - x_filt_delay );
        s.mask_back_cnt           = scan_time + mask_time;
        s.decay_back_cnt          = 0;      // reset in case it was active from previous peak
        s.max_x_filt_val          = x_filt; // initialize maximum value with first value
        s.max_mask_x_filt_val     = x_filt; // initialize maximum value with first value
        s.is_overloaded_state     = false;

        // this flag ensures that we always enter the if condition after the very first
        // time the signal was above the threshold (this flag is then reset when the
        // scan time is expired)
        s.was_above_threshold = true;
      }

      // search from above threshold to corrected scan+mask time for highest peak in
      // filtered signal (needed for decay power estimation)
      if ( x_filt > s.max_x_filt_val )
      {
        s.max_x_filt_val = x_filt;
      }

      // search from above threshold in scan time region needed for decay mask factor
      if ( ( s.mask_back_cnt > mask_time ) && ( x_filt > s.max_mask_x_filt_val ) )
      {
        s.max_mask_x_filt_val = x_filt;
      }

      s.scan_time_cnt--;
      s.mask_back_cnt--;

      // end condition of scan time
      if ( s.scan_time_cnt == 0 )
      {
        // climb to the maximum of the first peak (using the unfiltered signal)
        first_peak_found   = false;
        s.first_peak_val   = s_x_sq_hist[x_sq_hist_len - total_scan_time];
        int first_peak_idx = 0;

        for ( int idx = 1; idx < total_scan_time; idx++ )
        {
          const float cur_x_sq_hist_val  = s_x_sq_hist[x_sq_hist_len - total_scan_time + idx];
          const float prev_x_sq_hist_val = s_x_sq_hist[x_sq_hist_len - total_scan_time + idx - 1];

          if ( ( s.first_peak_val < cur_x_sq_hist_val ) && !first_peak_found )
          {
            s.first_peak_val = cur_x_sq_hist_val;
            first_peak_idx   = idx;
          }
          else
          {
            first_peak_found = true;

            // check if there is a much larger first peak
            if ( ( prev_x_sq_hist_val > cur_x_sq_hist_val ) && ( s.first_peak_val * first_peak_diff_thresh < prev_x_sq_hist_val ) )
            {
              s.first_peak_val = prev_x_sq_hist_val;
              first_peak_idx   = idx - 1;
            }
          }
        }

        // calculate sub-sample first peak value using simplified metric:
        // m = (x_sq[2] - x_sq[0]) / (x_sq[1] - x_sq[0]) -> sub_sample = m * m / 2
        if ( number_head_sensors > 1 )
        {
          s.sResults.first_peak_sub_sample = 0.0; // in case no sub-sample value can be calculated
          const int cur_index              = x_sq_hist_len - total_scan_time + first_peak_idx;

          if ( ( cur_index > 0 ) && ( cur_index < x_sq_hist_len - 1 ) )
          {
            if ( s_x_sq_hist[cur_index - 1] > s_x_sq_hist[cur_index + 1] )
            {
              // sample left of main peak is bigger than right sample
              const float sub_sample_metric = ( s_x_sq_hist[cur_index - 1] - s_x_sq_hist[cur_index + 1] ) /
                                              ( s_x_sq_hist[cur_index]     - s_x_sq_hist[cur_index + 1] );

              s.sResults.first_peak_sub_sample = sub_sample_metric * sub_sample_metric / 2;
            }
            else
            {
              // sample right of main peak is bigger than left sample
              const float sub_sample_metric = ( s_x_sq_hist[cur_index + 1] - s_x_sq_hist[cur_index - 1] ) /
                                              ( s_x_sq_hist[cur_index]     - s_x_sq_hist[cur_index - 1] );

              s.sResults.first_peak_sub_sample = -sub_sample_metric * sub_sample_metric / 2;
            }
          }
        }

        // get the maximum velocity in the scan time using the unfiltered signal
        s.peak_val            = 0.0f;
        int peak_velocity_idx = 0;
        for ( int i = 0; i < scan_time; i++ )
        {
          if ( s_x_sq_hist[x_sq_hist_len - scan_time + i] > s.peak_val )
          {
            s.peak_val        = s_x_sq_hist[x_sq_hist_len - scan_time + i];
            peak_velocity_idx = i;
          }
        }

        // peak detection results
        peak_delay       = scan_time - ( peak_velocity_idx + 1 );
        first_peak_delay = total_scan_time - ( first_peak_idx + 1 );
        first_peak_found = true; // for special case signal only increments, the peak found would be false -> correct this
        s.was_peak_found = true;


        // Overload correction ----------------------------------------------------
        // if the first peak is overloaded, use this position as the maximum peak
        int       peak_velocity_idx_ovhist                    = peak_velocity_idx;
        const int first_peak_velocity_idx_in_overload_history = overload_hist_len - total_scan_time + first_peak_idx;

        if ( s.overload_hist[first_peak_velocity_idx_in_overload_history] > 0.0f )
        {
          // overwrite peak value and index in history
          s.peak_val               = s_x_sq_hist[x_sq_hist_len - total_scan_time + first_peak_idx];
          peak_velocity_idx_ovhist = scan_time - x_sq_hist_len + first_peak_idx;
        }

        float     right_neighbor, left_neighbor;
        const int peak_velocity_idx_in_overload_history = overload_hist_len - scan_time + peak_velocity_idx_ovhist;
        const int peak_velocity_idx_in_x_sq_hist        = x_sq_hist_len - scan_time + peak_velocity_idx_ovhist;
        int       number_overloaded_samples             = 1; // we check for overload history at peak position is > 0 below -> start with one
        bool      left_neighbor_ok                      = true; // initialize with ok
        bool      right_neighbor_ok                     = true; // initialize with ok

        // check overload status and correct the peak if necessary
        if ( s.overload_hist[peak_velocity_idx_in_overload_history] > 0.0f )
        {
          // NOTE: the static_cast<int> is a workaround for the ESP32 compiler issue: "unknown opcode or format name 'lsiu'"
          // run to the right to find same overloads
          int cur_idx      = peak_velocity_idx_in_overload_history;
          int cur_idx_x_sq = peak_velocity_idx_in_x_sq_hist;
          while ( ( cur_idx < overload_hist_len - 1 ) && ( static_cast<int> ( s.overload_hist[cur_idx] ) == static_cast<int> ( s.overload_hist[cur_idx + 1] ) ) )
          {
            cur_idx++;
            cur_idx_x_sq++;
            number_overloaded_samples++;
          }
          if ( cur_idx_x_sq + 1 < x_sq_hist_len )
          {
            right_neighbor = s_x_sq_hist[cur_idx_x_sq + 1];
          }
          else
          {
            right_neighbor_ok = false;
          }

          // run to the left to find same overloads
          cur_idx      = peak_velocity_idx_in_overload_history;
          cur_idx_x_sq = peak_velocity_idx_in_x_sq_hist;
          while ( ( cur_idx > 1 ) && ( static_cast<int> ( s.overload_hist[cur_idx] ) == static_cast<int> ( s.overload_hist[cur_idx - 1] ) ) )
          {
            cur_idx--;
            cur_idx_x_sq--;
            number_overloaded_samples++;
          }
          if ( cur_idx_x_sq - 1 >= 0 )
          {
            left_neighbor = s_x_sq_hist[cur_idx_x_sq - 1];
          }
          else
          {
            left_neighbor_ok = false;
          }

          s.is_overloaded_state = ( number_overloaded_samples > max_num_overloads );

          // clipping compensation (see tools/misc/clipping_compensation.m)
          const float peak_val_sqrt = sqrt ( s.peak_val );
          float       mean_neighbor = peak_val_sqrt; // if no neighbor can be calculated, use safest value, i.e., lowest resulting correction            

          if ( left_neighbor_ok && right_neighbor_ok )
          {
            mean_neighbor = ( sqrt ( left_neighbor ) + sqrt ( right_neighbor ) ) / 2.0f;
          }
          else if ( left_neighbor_ok )
          {
            mean_neighbor = sqrt ( left_neighbor ); // only left neighbor available
          }
          else if ( right_neighbor_ok )
          {
            mean_neighbor = sqrt ( right_neighbor ); // only right neighbor available
          }

          const float a_low                      = amplification_mapping[min ( length_ampmap - 1, number_overloaded_samples )];
          const float a_high                     = amplification_mapping[min ( length_ampmap - 1, number_overloaded_samples + 1 )];
          const float a_diff                     = a_high - a_low;
          const float a_diff_abs                 = a_diff * peak_val_sqrt / a_low;
          float       neighbor_to_limit_abs      = mean_neighbor - ( peak_val_sqrt - a_diff_abs );
          neighbor_to_limit_abs                  = max ( 0.0f, min ( a_diff_abs, neighbor_to_limit_abs ) );
          const float amplification_compensation = a_low + neighbor_to_limit_abs / a_diff_abs * a_diff;
          s.peak_val                            *= amplification_compensation * amplification_compensation;
/*
String overload_string = "";
for ( int ov_cnt = 0; ov_cnt < overload_hist_len; ov_cnt++ )
{
  overload_string += String ( s.overload_hist[ov_cnt] ) + " ";
}
Serial.println ( overload_string );
Serial.println ( String ( peak_velocity_idx_in_x_sq_hist ) + " " +
                 String ( x_sq_hist_len ) + " " + String ( peak_velocity_idx_in_overload_history ) + " " +
                 String ( overload_hist_len ) + " " + String ( first_peak_idx ) );
Serial.println ( String ( sqrt ( left_neighbor ) ) + " " + String ( sqrt ( right_neighbor ) ) + " " +
                 String ( number_overloaded_samples ) + " " + String ( mean_neighbor ) + " " +
                 String ( mean_neighbor - ( peak_val_sqrt - a_diff_abs ) ) + " " + String ( neighbor_to_limit_abs ) + " " +
                 String ( amplification_compensation ) + " " + String ( sqrt ( s.peak_val ) ) );
*/
        }
      }

      // end condition of mask time
      if ( s.mask_back_cnt == 0 )
      {
        s.decay_back_cnt      = decay_len; // per definition decay starts right after mask time
        s.decay_scaling       = decay_fact * s.max_x_filt_val; // take maximum of filtered signal in scan+mask time
        s.was_above_threshold = false;
      }
    }


    // decay power estimation
    if ( s.decay_pow_est_start_cnt > 0 )
    {
      s.decay_pow_est_start_cnt--;

      // end condition
      if ( s.decay_pow_est_start_cnt == 0 )
      {
        s.decay_pow_est_cnt = decay_est_len; // now the power estimation can start
      }
    }

    if ( s.decay_pow_est_cnt > 0 )
    {
      s.decay_pow_est_sum += x_filt; // sum up the powers in pre-defined interval
      s.decay_pow_est_cnt--;

      // end condition
      if ( s.decay_pow_est_cnt == 0 )
      {
        const float decay_power = s.decay_pow_est_sum / decay_est_len;                   // calculate average power
        s.decay_pow_est_sum     = 0.0f;                                                  // we have to reset the sum for the next calculation
        s.decay_scaling         = min ( s.decay_scaling, decay_est_fact * decay_power ); // adjust the decay curve
      }
    }


    // Calculate positional sensing -----------------------------------------------
    if ( pos_sense_is_used )
    {
      // low pass filter of the input signal and store results in a FIFO
      update_fifo ( input[in], lp_filt_len, s.lp_filt_hist );

      float x_low = 0.0f;
      for ( int i = 0; i < lp_filt_len; i++ )
      {
        x_low += ( s.lp_filt_hist[i] * lp_filt_b[i] );
      }

      s.x_low_hist.add ( x_low * x_low );

      // start condition of delay process to fill up the required buffers
      if ( first_peak_found && ( !s.was_pos_sense_ready ) && ( s.pos_sense_cnt == 0 ) )
      {
        // a peak was found, we now have to start the delay process to fill up the
        // required buffer length for our metric
        s.pos_sense_cnt  = max ( 1, lp_filt_len - first_peak_delay );
        s.x_low_hist_idx = x_low_hist_len - lp_filt_len - max ( 0, first_peak_delay - lp_filt_len + 1 );
      }

      if ( s.pos_sense_cnt > 0 )
      {
        s.pos_sense_cnt--;

        // end condition
        if ( s.pos_sense_cnt == 0 )
        {
          // the buffers are filled, now calculate the metric
          float peak_energy_low = 0.0f;
          for ( int i = 0; i < lp_filt_len; i++ )
          {
            peak_energy_low = max ( peak_energy_low, s.x_low_hist[s.x_low_hist_idx + i] );
          }

          if ( pos_sense_inverted )
          {
            // add offset (dB) to get to similar range as non-inverted metric
            s.pos_sense_metric = peak_energy_low / s.first_peak_val * 10000.0f;
          }
          else
          {
            s.pos_sense_metric = s.first_peak_val / peak_energy_low;
          }

          s.was_pos_sense_ready = true;
        }
      }
    }


    // Calculate rim shot/choke detection -----------------------------------------
    if ( rim_shot_is_used )
    {
      if ( get_is_rim_switch() )
      {
        // as a quick hack we re-use the length parameter for the switch on detection
        const bool rim_switch_on = ( input[1] < rim_switch_threshold );
        s.x_rim_switch_hist.add ( rim_switch_on );

        if ( use_second_rim && ( input_len > 2 ) )
        {
          // the second rim signal is on third input signal
          s.x_sec_rim_switch_hist.add ( input[2] < rim_switch_threshold );
        }

        // at the end of the scan time search the history buffer for any switch on
        if ( s.was_peak_found )
        {
          s.rim_state                = NO_RIM;
          int num_neighbor_switch_on = 0;

          for ( int i = 0; i < rim_shot_window_len; i++ )
          {
            if ( s.x_rim_switch_hist[i] > 0 )
            {
              num_neighbor_switch_on++;

              // On the ESP32, we had seen crosstalk between head/rim inputs. To avoid that the interference
              // signal from the head triggers the rim, we check that we have at least two neighbor samples
              // above the rim threshold (the switch keeps on longer than the piezo signal)
              if ( num_neighbor_switch_on >= 2 )
              {
                s.rim_state = RIM_SHOT;
              }
            }
            else
            {
              num_neighbor_switch_on = 0;
            }
          }

          // support second rim switch (usually the bell on a ride cymbal)
          if ( use_second_rim )
          {
            int num_neighbor_second_switch_on = 0;

            for ( int i = 0; i < rim_shot_window_len; i++ )
            {
              if ( s.x_sec_rim_switch_hist[i] > 0 )
              {
                num_neighbor_second_switch_on++;

                // (see comment above for normal rim switch regarding this condition)
                if ( num_neighbor_second_switch_on >= 2 )
                {
                  // re-use rim-only enum for second rim switch, overwrites RIM_SHOT state
                  s.rim_state = RIM_ONLY;
                }
              }
              else
              {
                num_neighbor_second_switch_on = 0;
              }
            }
          }

          s.was_rim_shot_ready = true;
        }

        // choke detection
        if ( rim_switch_on )
        {
          s.rim_switch_on_cnt++;
        }
        else
        {
          // if choke switch on was detected, send choke off message now
          if ( s.rim_switch_on_cnt > rim_switch_on_cnt_thresh )
          {
            is_choke_off = true;
          }

          s.rim_switch_on_cnt = 0;
        }

        // only send choke on message once we detected a choke (i.e. do not test for ">" threshold but for "==")
        if ( s.rim_switch_on_cnt == rim_switch_on_cnt_thresh )
        {
          is_choke_on = true;
        }
      }
      else
      {
        // band-pass filter the rim signal (two types are supported)
        update_fifo ( input[1], bp_filt_len, s.rim_bp_hist_x );

        float sum_b = 0.0f;
        float sum_a = 0.0f;
        for ( int i = 0; i < bp_filt_len; i++ )
        {
          sum_b += s.rim_bp_hist_x[i] * rim_bp_filt_b[i];
        }
        for ( int i = 0; i < bp_filt_len - 1; i++ )
        {
          sum_a += s.rim_bp_hist_y[i] * rim_bp_filt_a[i];
        }
        float x_rim_bp = sum_b - sum_a;

        update_fifo ( x_rim_bp, bp_filt_len - 1, s.rim_bp_hist_y );
        x_rim_bp = x_rim_bp * x_rim_bp; // calculate power of filter result
        update_fifo ( x_rim_bp, x_rim_hist_len, s.x_rim_hist );

        // start condition of delay process to fill up the required buffers
        if ( s.was_peak_found && ( !s.was_rim_shot_ready ) && ( s.rim_shot_cnt == 0 ) )
        {
          // a peak was found, we now have to start the delay process to fill up the
          // required buffer length for our metric
          s.rim_shot_cnt   = max ( 1, rim_shot_window_len - peak_delay );
          s.x_rim_hist_idx = x_rim_hist_len - rim_shot_window_len - max ( 0, peak_delay - rim_shot_window_len + 1 );
        }

        if ( s.rim_shot_cnt > 0 )
        {
          s.rim_shot_cnt--;

          // end condition
          if ( s.rim_shot_cnt == 0 )
          {
            // the buffers are filled, now calculate the metric
            float rim_max_pow = 0;
            for ( int i = 0; i < rim_shot_window_len; i++ )
            {
              rim_max_pow = max ( rim_max_pow, s.x_rim_hist[s.x_rim_hist_idx + i] );
            }

            const float rim_metric  = rim_max_pow / s.peak_val;
            const bool  is_rim_shot = ( rim_metric > rim_shot_threshold ) && ( rim_max_pow > rim_max_power_low_limit );
            s.rim_state             = is_rim_shot ? RIM_SHOT : NO_RIM;
            s.rim_shot_cnt          = 0;
            s.was_rim_shot_ready    = true;
          }
        }
      }
    }

    // check for all estimations are ready and we can set the peak found flag and
    // return all results
    if ( s.was_peak_found && ( !pos_sense_is_used || s.was_pos_sense_ready ) && ( !rim_shot_is_used || s.was_rim_shot_ready ) )
    {
      // apply rim shot velocity boost
// TODO rim shot boost is only supported for single head sensors pads -> support multiple head sensor pads, too
      if ( ( s.rim_state == RIM_SHOT ) && ( number_head_sensors == 1 ) )
      {
        s.peak_val *= rim_shot_boost;
      }

      // calculate the MIDI velocity value with clipping to allowed MIDI value range
      int current_midi_velocity = static_cast<int> ( velocity_factor * pow ( s.peak_val * ADC_noise_peak_velocity_scaling, velocity_exponent ) + velocity_offset );
      current_midi_velocity     = max ( 1, min ( 127, current_midi_velocity ) );

      // positional sensing MIDI mapping with clipping to allowed MIDI value range
      int current_midi_pos = static_cast<int> ( ( 10 * log10 ( s.pos_sense_metric / pos_threshold ) / pos_range_db ) * 127 );
      current_midi_pos     = max ( 0, min ( 127, current_midi_pos ) );

// TODO:
// - in case of signal clipping, we cannot use the positional sensing results (overloads will
//   only happen if the strike is located near the middle of the pad)
// - positional sensing must be adjusted if a rim shot is detected (note that this must be done BEFORE the MIDI clipping!)
// - only use one counter instead of rim_shot_cnt and pos_sense_cnt
// - as long as counter is not finished, do check "hil_filt_new > threshold" again to see if we have a higher peak in that
//   time window -> if yes, restart everything using the new detected peak
if ( s.is_overloaded_state || ( s.rim_state != NO_RIM ) )
{
  current_midi_pos = 0; // as a quick hack, disable positional sensing if a rim shot is detected
}

      if ( number_head_sensors == 1 )
      {
        // normal case: only one head sensor -> use detection results directly
        midi_velocity = current_midi_velocity;
        midi_pos      = current_midi_pos;
        peak_found    = true;
        rim_state     = s.rim_state;
      }
      else
      {
        s.sResults.midi_velocity = current_midi_velocity;
        s.sResults.midi_pos      = current_midi_pos;
        s.sResults.rim_state     = s.rim_state;

        if ( head_sensor_cnt == 0 )
        {
          sensor0_has_results = true;
        }
      }

      s.was_peak_found      = false;
      s.was_pos_sense_ready = false;
      s.was_rim_shot_ready  = false;
      DEBUG_START_PLOTTING();
    }
  }


  // Multiple head sensor management ----------------------------------------------

// TODO do not use hard coded "17" at the three places here but define a pad specific value and use that instead
//      -> use that value also for definition of max_sensor_sample_diff
const int sensor_distance_factor = 17;
//
// TODO put number somewhere else
const int max_sensor_sample_diff = 20; // 2.5 ms at 8 kHz sampling rate
//
// TODO calculate phase and return it with a special MIDI command
//
// TODO implement positional sensing if only two head sensor peaks are available

  if ( number_head_sensors > 1 )
  {
    // start condition of delay process to query all head sensor results
    if ( sensor0_has_results && ( multiple_sensor_cnt == 0 ) )
    {
      multiple_sensor_cnt = max_sensor_sample_diff;
    }

    // special case with multiple head sensors
    if ( multiple_sensor_cnt > 0 )
    {
      multiple_sensor_cnt--;

      // end condition
      if ( multiple_sensor_cnt == 0 )
      {

// TODO quick hack tests
int number_sensors_with_results      = 0;
int head_sensor_idx_highest_velocity = 0;
int max_velocity                     = 0;
int velocity_sum                     = 0;
int sensor0_first_peak_delay         = sSensor[0].sResults.first_peak_delay;

for ( int head_sensor_cnt = 1; head_sensor_cnt < number_head_sensors; head_sensor_cnt++ ) // do not use sensor 0
{
  if ( abs ( sSensor[head_sensor_cnt].sResults.first_peak_delay - sensor0_first_peak_delay ) < max_sensor_sample_diff )
  {
    number_sensors_with_results++;
    velocity_sum += sSensor[head_sensor_cnt].sResults.midi_velocity;

    if ( sSensor[head_sensor_cnt].sResults.midi_velocity > max_velocity )
    {
      max_velocity                     = sSensor[head_sensor_cnt].sResults.midi_velocity;
      head_sensor_idx_highest_velocity = head_sensor_cnt;
    }
  }
}

        if ( number_sensors_with_results == 3 )
        {
          // calculate time delay differences
          const float diff_1_0 = -( ( sSensor[2].sResults.first_peak_delay + sSensor[2].sResults.first_peak_sub_sample ) -
                                    ( sSensor[1].sResults.first_peak_delay + sSensor[1].sResults.first_peak_sub_sample ) );
          const float diff_2_0 = -( ( sSensor[3].sResults.first_peak_delay + sSensor[3].sResults.first_peak_sub_sample ) -
                                    ( sSensor[1].sResults.first_peak_delay + sSensor[1].sResults.first_peak_sub_sample ) );

          // get_position function from pos_det.py
          // see: https://math.stackexchange.com/questions/3373011/how-to-solve-this-system-of-hyperbola-equations
          // and discussion post of jstma: https://github.com/corrados/edrumulus/discussions/70#discussioncomment-4014893
          const float r1      = diff_1_0 / sensor_distance_factor;
          const float r2      = diff_2_0 / sensor_distance_factor;
          const float c1      = r1 * r1 + get_pos_x0_sq_plus_y0_sq - get_pos_x1 * get_pos_x1 - get_pos_y1 * get_pos_y1;
          const float c2      = r2 * r2 + get_pos_x0_sq_plus_y0_sq - get_pos_x2 * get_pos_x2 - get_pos_y2 * get_pos_y2;
          const float d1      = ( 2 * r1 * get_pos_b2 - 2 * r2 * get_pos_b1 ) * get_pos_div1_fact;
          const float e1      = (     c1 * get_pos_b2 -     c2 * get_pos_b1 ) * get_pos_div1_fact;
          const float d2      = ( 2 * r1 * get_pos_a2 - 2 * r2 * get_pos_a1 ) * get_pos_div2_fact;
          const float e2      = (     c1 * get_pos_a2 -     c2 * get_pos_a1 ) * get_pos_div2_fact;
          const float d_e1_x0 = e1 - get_pos_x0;
          const float d_e2_y0 = e2 - get_pos_y0;
          const float a       = d1 * d1 + d2 * d2 - 1;
          const float b       = 2 * d_e1_x0 * d1 + 2 * d_e2_y0 * d2;
          const float c       = d_e1_x0 * d_e1_x0 + d_e2_y0 * d_e2_y0;

          // two solutions to the quadratic equation, only one solution seems to always be correct
          const float r_2 = ( -b - sqrt ( b * b - 4 * a * c ) ) / ( 2 * a );
          const float x   = d1 * r_2 + e1;
          const float y   = d2 * r_2 + e2;
          float       r   = sqrt ( x * x + y * y );

// TEST
//Serial.println ( String ( x ) + "," + String ( y ) + ",1000.0," );

          // clip calculated radius to rim radius
          if ( ( r > get_pos_rim_radius ) || ( isnan ( r ) ) )
          {
            r = get_pos_rim_radius;
          }
          const int max_abs_diff = r * sensor_distance_factor;

// TEST use maximum offset for middle from each sensor pair
//const int diff_2_1 = -( ( sSensor[3].sResults.first_peak_delay + sSensor[3].sResults.first_peak_sub_sample ) -
//                        ( sSensor[2].sResults.first_peak_delay + sSensor[2].sResults.first_peak_sub_sample ) );
//Serial.println ( String ( diff_1_0 ) + "," + String ( diff_2_0 ) + "," + String ( diff_2_1 ) + "," );
//const int max_abs_diff = ( max ( max ( abs ( diff_1_0 ), abs ( diff_2_0 ) ), abs ( diff_2_1 ) ) );

          midi_pos = min ( 127, max ( 0, pad_settings.pos_sensitivity * ( max_abs_diff - pad_settings.pos_threshold ) ) );

          // use average MIDI velocity
          midi_velocity = velocity_sum / number_sensors_with_results;

//rim_state = sSensor[head_sensor_idx_highest_velocity].sResults.rim_state;
// TEST use second highest velocity sensor for rim shot detection
if ( head_sensor_idx_highest_velocity == 1 )
{
  if ( sSensor[2].sResults.midi_velocity > sSensor[3].sResults.midi_velocity )
  {
    rim_state = sSensor[2].sResults.rim_state;
  }
  else
  {
    rim_state = sSensor[3].sResults.rim_state;
  }
}
else if ( head_sensor_idx_highest_velocity == 2 )
{
  if ( sSensor[1].sResults.midi_velocity > sSensor[3].sResults.midi_velocity )
  {
    rim_state = sSensor[1].sResults.rim_state;
  }
  else
  {
    rim_state = sSensor[3].sResults.rim_state;
  }
}
else
{
  if ( sSensor[1].sResults.midi_velocity > sSensor[2].sResults.midi_velocity )
  {
    rim_state = sSensor[1].sResults.rim_state;
  }
  else
  {
    rim_state = sSensor[2].sResults.rim_state;
  }
}

}
else if ( ( number_sensors_with_results == 2 ) || ( number_sensors_with_results == 1 ) )
{

// TODO
midi_pos = 0;

// TEST use average MIDI velocity
midi_velocity = velocity_sum / number_sensors_with_results;
rim_state     = sSensor[head_sensor_idx_highest_velocity].sResults.rim_state;

}
else
{

// TODO
midi_pos = 0;

// TEST
midi_velocity = sSensor[0].sResults.midi_velocity;
rim_state     = sSensor[0].sResults.rim_state;

}
peak_found = true;


        // reset the first_peak_delay since this is our marker if a peak was in the interval
        for ( int head_sensor_cnt = 1; head_sensor_cnt < number_head_sensors; head_sensor_cnt++ ) // do not use sensor 0
        {
          sSensor[head_sensor_cnt].sResults.first_peak_delay = max_sensor_sample_diff;
        }
      }
    }
  }

  DEBUG_ADD_VALUES ( input[0] * input[0], x_filt, sSensor[0].scan_time_cnt > 0 ? 0.5 : sSensor[0].mask_back_cnt > 0 ? 0.2 : cur_decay, threshold );
  return x_filt; // here, you can return debugging values for verification with Ocatve
}

void Edrumulus::Pad::process_control_sample ( const int* input,
                                              bool&      change_found,
                                              int&       midi_ctrl_value,
                                              bool&      peak_found,
                                              int&       midi_velocity )
{
  manage_delayed_initialization();

  // map the input value to the MIDI range
  int cur_midi_ctrl_value = ( ( ADC_MAX_RANGE - input[0] - control_threshold ) / control_range * 127 );
  cur_midi_ctrl_value     = max ( 0, min ( 127, cur_midi_ctrl_value ) );


  // Detect pedal stomp --------------------------------------------------------
  update_fifo ( cur_midi_ctrl_value, ctrl_history_len, ctrl_hist );

  // to cope with ADC noise, we use a moving average filter for noise reduction
  float prev_ctrl_average = 0.0f;
  float cur_ctrl_average  = 0.0f;
  for ( int i = 0; i < ctrl_history_len_half; i++ )
  {
    prev_ctrl_average += ctrl_hist[i];                         // use first half for previous value
    cur_ctrl_average  += ctrl_hist[i + ctrl_history_len_half]; // use second half for current value
  }
  prev_ctrl_average /= ctrl_history_len_half;
  cur_ctrl_average  /= ctrl_history_len_half;

  // check if we just crossed the transition from open to close
  if ( ( prev_ctrl_average < hi_hat_is_open_MIDI_threshold ) &&
       ( cur_ctrl_average >= hi_hat_is_open_MIDI_threshold ) )
  {
    // calculate the gradient which is the measure for the pedal stomp velocity
    const float ctrl_gradient = ( cur_ctrl_average - prev_ctrl_average ) / ctrl_history_len_half;

    // only send MIDI note for pedal stomp if we are above the given threshold
    if ( ctrl_gradient > ctrl_velocity_threshold )
    {
      // map curve difference (gradient) to velocity
      midi_velocity = min ( 127, max ( 1, static_cast<int> ( ( ctrl_gradient - ctrl_velocity_threshold ) * ctrl_velocity_range_fact ) ) );
      peak_found    = true;

      // reset the history after a detection to suppress multiple detections
      for ( int i = 0; i < ctrl_history_len; i++ )
      {
        ctrl_hist[i] = hi_hat_is_open_MIDI_threshold;
      }
    }
  }


  // Introduce hysteresis to avoid sending too many MIDI control messages ------
  change_found = false;

  if ( ( cur_midi_ctrl_value > ( prev_ctrl_value + control_midi_hysteresis ) ) ||
       ( cur_midi_ctrl_value < ( prev_ctrl_value - control_midi_hysteresis ) ) )
  {
    // clip border values to max/min
    if ( cur_midi_ctrl_value < control_midi_hysteresis )
    {
      midi_ctrl_value = 0;
    }
    else if ( cur_midi_ctrl_value > 127 - control_midi_hysteresis )
    {
      midi_ctrl_value = 127;
    }
    else
    {
      midi_ctrl_value = cur_midi_ctrl_value;
    }

    change_found    = true;
    prev_ctrl_value = midi_ctrl_value;
  }
}
