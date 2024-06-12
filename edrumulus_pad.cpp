/******************************************************************************\
 * Copyright (c) 2020-2024
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

#include "edrumulus_pad.h"


void Pad::setup ( const int conf_Fs )
{
  // set essential parameters
  Fs               = conf_Fs;
  init_delay_value = static_cast<int> ( init_delay_value_s * conf_Fs );

  // initialize with default pad type and other defaults
  set_pad_type ( PD6 );
  midi_note                = 127;
  midi_note_rim            = 127;
  midi_note_open           = 127;
  midi_note_open_rim       = 127;
  midi_ctrl_ch             = 4; // CC4, usually used for hi-hat
  use_head_sensor_coupling = false;
  use_second_rim           = false;
  init_delay_cnt           = 0; // note that it resets value of set_pad_type above
  initialize(); // do very first initialization without delay
}


void Pad::set_pad_type ( const Epadtype new_pad_type )
{
  // apply new pad type and set all parameters to the default values for that pad type
  pad_settings.pad_type = new_pad_type;

  apply_preset_pad_settings();
  sched_init();
}


void Pad::manage_delayed_initialization()
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


void Pad::initialize()
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

  // positional sensing for rim shots MIDI assignment parameters
  const float rim_pos_threshold_db = pad_settings.rim_pos_threshold - 40;      // gives us a threshold range of -40..-9 dB
  rim_pos_threshold                = pow ( 10.0f, rim_pos_threshold_db / 10 ); // linear power threshold
  const float max_rim_pos_range_db = 11; // db (found by testing with PD-80R)
  rim_pos_range_db                 = max_rim_pos_range_db * ( 32 - pad_settings.rim_pos_sensitivity ) / 32;

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
  length_ampmap = 0;
  for ( int i = 0; i < max_length_ampmap; i++ )
  {
    const float amp_map_val = pow ( 10.0f, ( i * pad_settings.clip_comp_ampmap_step ) *
                                           ( i * pad_settings.clip_comp_ampmap_step ) );

    // never to higher than 5 but at least two values
    if ( ( length_ampmap < 2 ) || ( amp_map_val <= 5.0f ) )
    {
      amplification_mapping[i] = amp_map_val;
      length_ampmap++;
    }
  }

  multi_head_sensor.initialize();

  // allocate and initialize memory for vectors and initialize scalars
  allocate_initialize ( &rim_bp_filt_b, bp_filt_len );      // rim band-pass filter coefficients b
  allocate_initialize ( &rim_bp_filt_a, bp_filt_len - 1 );  // rim band-pass filter coefficients a
  allocate_initialize ( &decay,         decay_len );        // memory for decay function
  allocate_initialize ( &lp_filt_b,     lp_filt_len );      // memory for low-pass filter coefficients
  allocate_initialize ( &ctrl_hist,     ctrl_history_len ); // memory for Hi-Hat control pad hit detection
  prev_ctrl_value = 0;

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


float Pad::process_sample ( const float* input,
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

        // calculate sub-sample first peak value
        if ( number_head_sensors > 1 )
        {
          multi_head_sensor.calculate_subsample_peak_value ( s_x_sq_hist,
                                                             x_sq_hist_len,
                                                             total_scan_time,
                                                             first_peak_idx,
                                                             s.sResults.first_peak_sub_sample );
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

        // overload correction
        overload_correction ( s_x_sq_hist,
                              s.overload_hist,
                              first_peak_idx,
                              peak_velocity_idx,
                              s.is_overloaded_state,
                              s.peak_val );
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

            // rim power is assumed to be constant for each rim shot but distance to center mounted piezo
            // will change power and therefore the rim metric can be used for positional sensing for rim shots
            s.rim_pos_sense_metric = rim_metric;
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

      // positional sensing must be adjusted if a rim shot is detected (note that this must be done BEFORE the MIDI clipping!)
      if ( s.rim_state != NO_RIM )
      {
        // positional sensing for rim shots (no rim only and side stick) is only supported for rim piezos
        if ( ( s.rim_state == RIM_SHOT ) && !get_is_rim_switch() )
        {
          // rim shot positional sensing MIDI mapping with clipping to allowed MIDI value range
          current_midi_pos = static_cast<int> ( ( 10 * log10 ( s.rim_pos_sense_metric / rim_pos_threshold ) / rim_pos_range_db ) * 127 );
          current_midi_pos = max ( 0, min ( 127, current_midi_pos ) );
        }
        else
        {
          current_midi_pos = 0; // rim shot positional sensing not supported
        }
      }

      // in case of signal clipping, we cannot use the positional sensing results (overloads will
      // only happen if the strike is located near the middle of the pad)
      if ( s.is_overloaded_state )
      {
        current_midi_pos = 0; // set to middle position
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

  // signal processing for multiple head sensor pads
  if ( number_head_sensors > 1 )
  {
    multi_head_sensor.calculate ( sSensor,
                                  sensor0_has_results,
                                  number_head_sensors,
                                  pad_settings.pos_sensitivity,
                                  pad_settings.pos_threshold,
                                  peak_found,
                                  midi_velocity,
                                  midi_pos,
                                  rim_state );
  }

  DEBUG_ADD_VALUES ( input[0] * input[0], x_filt, sSensor[0].scan_time_cnt > 0 ? 0.5 : sSensor[0].mask_back_cnt > 0 ? 0.2 : cur_decay, threshold );
  return x_filt; // here, you can return debugging values for verification with Ocatve
}

void Pad::process_control_sample ( const int* input,
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
