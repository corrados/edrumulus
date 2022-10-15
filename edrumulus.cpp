/******************************************************************************\
 * Copyright (c) 2020-2022
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


Edrumulus::Edrumulus() :
  Fs ( 8000 ) // this is the most fundamental system parameter: system sampling rate
{
  // initializations
  overload_LED_on_time       = round ( 0.25f * Fs ); // minimum overload LED on time (e.g., 250 ms)
  overload_LED_cnt           = 0;
  status_is_overload         = false;
  samplerate_prev_micros_cnt = 0;
  samplerate_prev_micros     = micros();
  status_is_error            = false;
#ifdef ESP_PLATFORM
  spike_cancel_level         = 4; // use max. spike cancellation on the ESP32 per default (note that it increases the latency)
#else
  spike_cancel_level         = 0; // default
#endif
  cancel_num_samples         = ( cancel_time_ms * Fs ) / 1000;
  cancel_cnt                 = 0;
  cancel_MIDI_velocity       = 1;
  cancel_pad_index           = 0;

  // calculate DC offset IIR1 low pass filter parameters, see
  // http://www.tsdconseil.fr/tutos/tuto-iir1-en.pdf: gamma = exp(-Ts/tau)
  dc_offset_iir_gamma           = exp ( - 1.0f / ( Fs * dc_offset_iir_tau_seconds ) );
  dc_offset_iir_one_minus_gamma = 1.0f - dc_offset_iir_gamma;
}


void Edrumulus::setup ( const int  conf_num_pads,
                        const int* conf_analog_pins,
                        const int* conf_analog_pins_rim_shot,
                        const int* conf_analog_pins_second,
                        const int* conf_analog_pins_third )
{
  number_pads = min ( conf_num_pads, MAX_NUM_PADS );

  for ( int i = 0; i < number_pads; i++ )
  {
    // set the pad GIOP pin numbers
    analog_pin[i][0] = conf_analog_pins[i];
    analog_pin[i][1] = conf_analog_pins_rim_shot[i];
    analog_pin[i][2] = conf_analog_pins_second[i];
    analog_pin[i][3] = conf_analog_pins_third[i];
    number_inputs[i] = conf_analog_pins_rim_shot[i] >= 0 ? ( conf_analog_pins_second[i] >= 0 ? 4 : 2 ) : 1;

    // setup the pad
    pad[i].setup ( Fs, number_inputs[i] );
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
  float sample[MAX_NUM_PAD_INPUTS];
  bool  overload_detected[MAX_NUM_PAD_INPUTS];

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
        // update DC offset by using an IIR1 low pass filter
        dc_offset[i][j] = dc_offset_iir_gamma * dc_offset[i][j] + dc_offset_iir_one_minus_gamma * sample_org_pad[j];

        // compensate DC offset
        sample[j] = sample_org_pad[j] - dc_offset[i][j];

        // ADC spike cancellation (do not use spike cancellation for rim switches since they have short peaks)
        if ( ( spike_cancel_level > 0 ) && !( pad[i].get_is_rim_switch() && ( j == 1 ) ) ) // rim is always on second channel
        {
          sample[j] = edrumulus_hardware.cancel_ADC_spikes ( sample[j], i, j, spike_cancel_level );
        }
      }

      // overload detection
      for ( int j = 0; j < number_inputs[i]; j++ )
      {
        // check for the lowest/largest possible ADC range values with noise consideration
        if ( ( sample_org_pad[j] >= ( ADC_MAX_RANGE - ADC_MAX_NOISE_AMPL ) ) || ( sample_org_pad[j] <= ADC_MAX_NOISE_AMPL - 1 ) )
        {
          overload_LED_cnt     = overload_LED_on_time;
          overload_detected[j] = true;
        }
        else
        {
          overload_detected[j] = false;
        }
      }

      // process sample
      pad[i].process_sample ( sample,         overload_detected,
                              peak_found[i],  midi_velocity[i], midi_pos[i],
                              is_rim_shot[i], is_choke_on[i],   is_choke_off[i] );
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


  // Sampling rate check -------------------------------------------------------
  // (i.e. if CPU is overloaded, the sample rate will drop which is bad)
  if ( samplerate_prev_micros_cnt >= samplerate_max_cnt )
  {
    const unsigned long samplerate_cur_micros = micros();

// TEST check the measured sampling rate
//Serial.println ( 1.0f / ( samplerate_cur_micros - samplerate_prev_micros ) * samplerate_max_cnt * 1e6f, 7 );

    // do not update status if micros() has wrapped around (at about 70 minutes)
    if ( samplerate_cur_micros - samplerate_prev_micros > 0 )
    {
      // set error flag if sample rate deviation is too large
      status_is_error = ( abs ( 1.0f / ( samplerate_cur_micros - samplerate_prev_micros ) * samplerate_max_cnt * 1e6f - Fs ) > samplerate_max_error_Hz );
    }

    samplerate_prev_micros_cnt = 0;
    samplerate_prev_micros     = samplerate_cur_micros;

/*
// TEST check DC offset values
String serial_print;
String serial_print2;
for ( int i = 0; i < number_pads; i++ )
{
  if ( !pad[i].get_is_control() )
  {
    for ( int j = 0; j < number_inputs[i]; j++ )
    {
      serial_print += String ( sample_org[i][j] ) + "\t" + String ( dc_offset[i][j] ) + "\t";
      serial_print2 += String ( sample_org[i][j] - dc_offset[i][j] ) + "\t";
    }
  }
}
//Serial.println ( serial_print );
Serial.println ( serial_print2 );
*/

  }
  samplerate_prev_micros_cnt++;
}


// -----------------------------------------------------------------------------
// Pad -------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void Edrumulus::Pad::setup ( const int conf_Fs,
                             const int conf_number_inputs )
{
  // set essential parameters
  Fs                  = conf_Fs;
  number_inputs       = conf_number_inputs;
  number_head_sensors = max ( 1, number_inputs - 1 ); // exclude rim input: 1 or 3 head sensor inputs

  // initialize with default pad type and other defaults
  set_pad_type ( PD120 );
  midi_note          = 38;
  midi_note_rim      = 40;
  midi_note_open     = 46;
  midi_note_open_rim = 26;
  midi_ctrl_ch       = 4; // CC4, usually used for hi-hat
}


void Edrumulus::Pad::set_pad_type ( const Epadtype new_pad_type )
{
  // apply new pad type and set all parameters to the default values for that pad type
  pad_settings.pad_type = new_pad_type;

  apply_preset_pad_settings();
  initialize();
}


void Edrumulus::Pad::initialize()
{
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
  decay_est_delay          = round ( pad_settings.decay_est_delay_ms * 1e-3f * Fs );
  decay_est_len            = round ( pad_settings.decay_est_len_ms   * 1e-3f * Fs );
  decay_est_fact           = pow ( 10.0f, pad_settings.decay_est_fact_db / 10 );
  rim_shot_window_len      = round ( pad_settings.rim_shot_window_len_ms * 1e-3f * Fs );        // window length (e.g. 5 ms)
  rim_shot_treshold_dB     = static_cast<float> ( pad_settings.rim_shot_treshold ) - 44;    // rim shot threshold
  rim_switch_treshold      = -ADC_MAX_NOISE_AMPL + 9 * ( pad_settings.rim_shot_treshold - 31 ); // rim switch linear threshold
  rim_switch_on_cnt_thresh = round ( 10.0f * 1e-3f * Fs );                                      // number of on samples until we detect a choke
  rim_max_power_low_limit  = ADC_MAX_NOISE_AMPL * ADC_MAX_NOISE_AMPL / 31.0f; // lower limit on detected rim power, 15 dB below max noise amplitude
  x_rim_hist_len           = x_sq_hist_len + rim_shot_window_len;
  cancellation_factor      = static_cast<float> ( pad_settings.cancellation ) / 31.0f;          // cancellation factor: range of 0.0..1.0
  ctrl_history_len         = 10;   // (MUST BE AN EVEN VALUE) control history length, use a fixed value
  ctrl_velocity_range_fact = 4.0f; // use a fixed value (TODO make it adjustable)
  ctrl_velocity_threshold  = 5.0f; // use a fixed value (TODO make it adjustable)
  overload_hist_len        = scan_time + x_filt_delay;
  max_num_overloads        = 3; // maximum allowed number of overloaded samples until the overload special case is activated
  overload_num_thresh_2db  = 5;
  overload_num_thresh_3db  = 7;

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
  float curve_param;

  switch ( pad_settings.curve_type )
  {
    case EXP1:            curve_param = 1.035f; break;
    case EXP2:            curve_param = 1.04f;  break;
    case LOG1:            curve_param = 1.018f; break;
    case LOG2:            curve_param = 1.01f;  break;
    default: /* LINEAR */ curve_param = 1.023f; break; // this curve parameter comes close to what Roland is doing for "linear"
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
  control_threshold = pad_settings.velocity_threshold / 31.0f * ( 0.6f * ADC_MAX_RANGE ) + ( 0.1f * ADC_MAX_RANGE );
  control_range     = ( ADC_MAX_RANGE - control_threshold ) * ( 32 - pad_settings.velocity_sensitivity ) / 32;

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
    allocate_initialize ( &s.x_sq_hist,         x_sq_hist_len );       // memory for sqr(x) history
    allocate_initialize ( &s.bp_filt_hist_x,    bp_filt_len );         // band-pass filter x-signal history
    allocate_initialize ( &s.bp_filt_hist_y,    bp_filt_len - 1 );     // band-pass filter y-signal history
    allocate_initialize ( &s.x_low_hist,        x_low_hist_len );      // memory for low-pass filter result
    allocate_initialize ( &s.lp_filt_hist,      lp_filt_len );         // memory for low-pass filter input
    allocate_initialize ( &s.rim_bp_hist_x,     bp_filt_len );         // rim band-pass filter x-signal history
    allocate_initialize ( &s.rim_bp_hist_y,     bp_filt_len - 1 );     // rim band-pass filter y-signal history
    allocate_initialize ( &s.x_rim_hist,        x_rim_hist_len );      // memory for rim shot detection
    allocate_initialize ( &s.x_rim_switch_hist, rim_shot_window_len ); // memory for rim switch detection
    allocate_initialize ( &s.overload_hist,     overload_hist_len );   // memory for overload detection status

    s.was_above_threshold            = false;
    s.is_overloaded_state            = false;
    s.mask_back_cnt                  = 0;
    s.first_peak_val                 = 0.0f;
    s.peak_val                       = 0.0f;
    s.decay_back_cnt                 = 0;
    s.decay_scaling                  = 1.0f;
    s.scan_time_cnt                  = 0;
    s.decay_pow_est_start_cnt        = 0;
    s.decay_pow_est_cnt              = 0;
    s.decay_pow_est_sum              = 0.0f;
    s.pos_sense_cnt                  = 0;
    s.x_low_hist_idx                 = 0;
    s.rim_shot_cnt                   = 0;
    s.rim_switch_on_cnt              = 0;
    s.max_x_filt_val                 = 0.0f;
    s.max_mask_x_filt_val            = 0.0f;
    s.was_peak_found                 = false;
    s.was_pos_sense_ready            = false;
    s.was_rim_shot_ready             = false;
    s.stored_is_rimshot              = false;
    sSensorResults[in].Clear();
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
                                       const bool*  overload_detected,
                                       bool&        peak_found,
                                       int&         midi_velocity,
                                       int&         midi_pos,
                                       bool&        is_rim_shot,
                                       bool&        is_choke_on,
                                       bool&        is_choke_off )
{
  // initialize return parameters and configuration parameters
  peak_found                        = false;
  midi_velocity                     = 0;
  midi_pos                          = 0;
  is_rim_shot                       = false;
  is_choke_on                       = false;
  is_choke_off                      = false;
  const bool pos_sense_is_used      = pad_settings.pos_sense_is_used;                         // can be applied directly without calling initialize()
  const bool rim_shot_is_used       = pad_settings.rim_shot_is_used && ( number_inputs > 1 ); // can be applied directly without calling initialize()
  const bool pos_sense_inverted     = pad_settings.pos_invert;                                // can be applied directly without calling initialize()
  float      x_filt                 = 0.0f; // needed for debugging
  float      cur_decay              = 1;    // needed for debugging, initialization value (0 dB) only used for debugging
  bool       any_sensor_has_results = false;

  for ( int head_sensor_cnt = 0; head_sensor_cnt < number_head_sensors; head_sensor_cnt++ )
  {
    const int in               = head_sensor_cnt == 0 ? 0 : head_sensor_cnt + 1; // exclude rim input
    SSensor&  s                = sSensor[head_sensor_cnt];
    bool      first_peak_found = false; // only used internally
    int       peak_delay       = 0;     // only used internally
    int       first_peak_delay = 0;     // only used internally
    s.sResults.Clear();

    // square input signal and store in FIFO buffer
    const float x_sq = input[in] * input[in];
    update_fifo ( x_sq,                                x_sq_hist_len,     s.x_sq_hist );
    update_fifo ( overload_detected[in] ? 1.0f : 0.0f, overload_hist_len, s.overload_hist );


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
        s.first_peak_val   = s.x_sq_hist[x_sq_hist_len - total_scan_time];
        int first_peak_idx = 0;

        for ( int idx = 1; idx < total_scan_time; idx++ )
        {
          const float cur_x_sq_hist_val  = s.x_sq_hist[x_sq_hist_len - total_scan_time + idx];
          const float prev_x_sq_hist_val = s.x_sq_hist[x_sq_hist_len - total_scan_time + idx - 1];

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

        // get the maximum velocity in the scan time using the unfiltered signal
        s.peak_val            = 0.0f;
        int peak_velocity_idx = 0;
        for ( int i = 0; i < scan_time; i++ )
        {
          if ( s.x_sq_hist[x_sq_hist_len - scan_time + i] > s.peak_val )
          {
            s.peak_val        = s.x_sq_hist[x_sq_hist_len - scan_time + i];
            peak_velocity_idx = i;
          }
        }

        // peak detection results
        peak_delay       = scan_time - ( peak_velocity_idx + 1 );
        first_peak_delay = total_scan_time - ( first_peak_idx + 1 );
        first_peak_found = true; // for special case signal only increments, the peak found would be false -> correct this
        s.was_peak_found = true;

        // check overload status
        int number_overloaded_samples = 0;
        for ( int i = 0; i < overload_hist_len; i++ )
        {
          if ( s.overload_hist[i] > 0.0f )
          {
            number_overloaded_samples++;
          }
        }
        if ( number_overloaded_samples > max_num_overloads )
        {
          s.is_overloaded_state = true;

          // overload correctdion: correct the peak value according to the number of clipped samples
          if ( number_overloaded_samples <= max_num_overloads )
          {
            s.peak_val *= 1.2589; // 1 dB
          }
          else if ( number_overloaded_samples <= overload_num_thresh_2db )
          {
            s.peak_val *= 1.5849; // 2 dB
          }
          else if ( number_overloaded_samples <= overload_num_thresh_3db )
          {
            s.peak_val *= 2; // 3 dB
          }
          else
          {
            s.peak_val *= 2.5119; // 4 dB
          }
        }

        // calculate the MIDI velocity value with clipping to allowed MIDI value range
        s.stored_midi_velocity = velocity_factor * pow ( s.peak_val * ADC_noise_peak_velocity_scaling, velocity_exponent ) + velocity_offset;
        s.stored_midi_velocity = max ( 1, min ( 127, s.stored_midi_velocity ) );
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

      update_fifo ( x_low * x_low, x_low_hist_len, s.x_low_hist );

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

          float pos_sense_metric;

          if ( pos_sense_inverted )
          {
            // add offset (dB) to get to similar range as non-inverted metric
            pos_sense_metric = peak_energy_low / s.first_peak_val * 10000.0f;
          }
          else
          {
            pos_sense_metric = s.first_peak_val / peak_energy_low;
          }

          s.was_pos_sense_ready = true;

          // positional sensing MIDI mapping with clipping to allowed MIDI value range
          s.stored_midi_pos = static_cast<int> ( ( 10 * log10 ( pos_sense_metric / pos_threshold ) / pos_range_db ) * 127 );
          s.stored_midi_pos = max ( 1, min ( 127, s.stored_midi_pos ) );
        }
      }
    }


    // Calculate rim shot/choke detection -----------------------------------------
    if ( rim_shot_is_used )
    {
      if ( get_is_rim_switch() )
      {
        const bool rim_switch_on = ( input[1] < rim_switch_treshold );

        // as a quick hack we re-use the length parameter for the switch on detection
        update_fifo ( rim_switch_on, rim_shot_window_len, s.x_rim_switch_hist );

        // at the end of the scan time search the history buffer for any switch on
        if ( s.was_peak_found )
        {
          s.stored_is_rimshot = false;

          for ( int i = 0; i < rim_shot_window_len; i++ )
          {
            if ( s.x_rim_switch_hist[i] > 0 )
            {
              s.stored_is_rimshot = true;
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

            const float rim_metric_db = 10 * log10 ( rim_max_pow / s.peak_val );
            s.stored_is_rimshot       = ( rim_metric_db > rim_shot_treshold_dB ) && ( rim_max_pow > rim_max_power_low_limit );
            s.rim_shot_cnt            = 0;
            s.was_rim_shot_ready      = true;
          }
        }
      }
    }

    // check for all estimations are ready and we can set the peak found flag and
    // return all results
    if ( s.was_peak_found && ( !pos_sense_is_used || s.was_pos_sense_ready ) && ( !rim_shot_is_used || s.was_rim_shot_ready ) )
    {

// TODO in case of signal clipping, we cannot use the positional sensing and rim shot detection results
if ( s.is_overloaded_state )
{
  s.stored_is_rimshot = false; // as a quick hack, assume we do not have a rim shot
  s.stored_midi_pos   = 0;     // overloads will only happen if the strike is located near the middle of the pad
}

// TODO:
// - positional sensing must be adjusted if a rim shot is detected (note that this must be done BEFORE the MIDI clipping!)
// - only use one counter instead of rim_shot_cnt and pos_sense_cnt
// - as long as counter is not finished, do check "hil_filt_new > threshold" again to see if we have a higher peak in that
//   time window -> if yes, restart everything using the new detected peak
if ( s.stored_is_rimshot )
{
  s.stored_midi_pos = 0; // as a quick hack, disable positional sensing if a rim shot is detected
}

      any_sensor_has_results   = true;
      s.sResults.midi_velocity = s.stored_midi_velocity;
      s.sResults.midi_pos      = s.stored_midi_pos;
      s.sResults.peak_found    = true;
      s.sResults.is_rim_shot   = s.stored_is_rimshot;
      s.was_peak_found         = false;
      s.was_pos_sense_ready    = false;
      s.was_rim_shot_ready     = false;
      DEBUG_START_PLOTTING();
    }
  }

  if ( number_head_sensors == 1 )
  {
    // normal case: only one head sensor -> use detection results directly
    midi_velocity = sSensor[0].sResults.midi_velocity;
    midi_pos      = sSensor[0].sResults.midi_pos;
    peak_found    = sSensor[0].sResults.peak_found;
    is_rim_shot   = sSensor[0].sResults.is_rim_shot;
  }
  else
  {
    // start condition of delay process to query all head sensor results
    if ( any_sensor_has_results && ( multiple_sensor_cnt == 0 ) )
    {
// TODO put number somewhere else
const int max_sensor_sample_diff = 20; // 2.5 ms at 8 kHz sampling rate

      multiple_sensor_cnt = max_sensor_sample_diff;
    }

    // special case with multiple head sensors
    if ( multiple_sensor_cnt > 0 )
    {
      multiple_sensor_cnt--;

      // store current head sensor results
      for ( int head_sensor_cnt = 0; head_sensor_cnt < number_head_sensors; head_sensor_cnt++ )
      {
        if ( sSensor[head_sensor_cnt].sResults.peak_found )
        {
          sSensorResults[head_sensor_cnt] = sSensor[head_sensor_cnt].sResults;
        }
      }

      // end condition
      if ( multiple_sensor_cnt == 0 )
      {

// TODO quick hack test -> take results of any sensor
for ( int head_sensor_cnt = 0; head_sensor_cnt < number_head_sensors; head_sensor_cnt++ )
{
  if ( sSensorResults[head_sensor_cnt].peak_found )
  {
    midi_velocity = sSensorResults[head_sensor_cnt].midi_velocity;
    midi_pos      = sSensorResults[head_sensor_cnt].midi_pos;
    peak_found    = sSensorResults[head_sensor_cnt].peak_found;
    is_rim_shot   = sSensorResults[head_sensor_cnt].is_rim_shot;
  }
}

        // clear all sensor results
        for ( int head_sensor_cnt = 0; head_sensor_cnt < number_head_sensors; head_sensor_cnt++ )
        {
          sSensorResults[head_sensor_cnt].Clear();
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
  // map the input value to the MIDI range
  int cur_midi_ctrl_value = ( ( ADC_MAX_RANGE - input[0] - control_threshold ) / control_range * 127 );
  cur_midi_ctrl_value     = max ( 0, min ( 127, cur_midi_ctrl_value ) );

  // detect pedal hit
  update_fifo ( cur_midi_ctrl_value, ctrl_history_len, ctrl_hist );

  float prev_ctrl_average = 0;
  float cur_ctrl_average  = 0;
  for ( int i = 0; i < ctrl_history_len / 2; i++ )
  {
    prev_ctrl_average += ctrl_hist[i];                        // use first half for previous value
    cur_ctrl_average  += ctrl_hist[i + ctrl_history_len / 2]; // use second half for current value
  }
  prev_ctrl_average /= ctrl_history_len / 2;
  cur_ctrl_average  /= ctrl_history_len / 2;

  if ( ( prev_ctrl_average < hi_hat_is_open_MIDI_threshold ) &&
       ( cur_ctrl_average >= hi_hat_is_open_MIDI_threshold ) &&
       ( cur_ctrl_average - prev_ctrl_average > ctrl_velocity_threshold ) )
  {
    // map curve difference (gradient) to velocity
    midi_velocity = min ( 127, static_cast<int> ( ( cur_ctrl_average - prev_ctrl_average - ctrl_velocity_threshold ) * ctrl_velocity_range_fact ) );
    peak_found    = true;

    // reset the history after a detection to suppress multiple detections
    for ( int i = 0; i < ctrl_history_len; i++ )
    {
      ctrl_hist[i] = hi_hat_is_open_MIDI_threshold;
    }
  }

  // introduce hysteresis to avoid sending too many MIDI control messages
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
