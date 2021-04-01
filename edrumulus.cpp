/******************************************************************************\
 * Copyright (c) 2020-2021
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


Edrumulus::Edrumulus() :
  Fs ( 8000 ) // this is the most fundamental system parameter: system sampling rate
{
  // initializations
  edrumulus_pointer          = this;                 // global pointer to this class needed for static callback function
  overload_LED_on_time       = round ( 0.25f * Fs ); // minimum overload LED on time (e.g., 250 ms)
  overload_LED_cnt           = 0;
  status_is_overload         = false;
  samplerate_prev_micros_cnt = 0;
  samplerate_prev_micros     = micros();
  status_is_error            = false;

  // prepare timer at a rate of given sampling rate
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

    // setup the pad and init pad parameters
    pad[i].setup ( Fs, number_inputs[i] );
    peak_found[i]    = false;
    control_found[i] = false;
  }

  // estimate the DC offset for all inputs
  float dc_offset_sum[MAX_NUM_PADS][MAX_NUM_PAD_INPUTS];

  for ( int k = 0; k < dc_offset_est_len; k++ )
  {
    for ( int i = 0; i < number_pads; i++ )
    {
      for ( int j = 0; j < number_inputs[i]; j++ )
      {
        if ( k == 0 )
        {
          // initial value
          dc_offset_sum[i][j] = analogRead ( analog_pin[i][j] );
        }
        else
        {
          // intermediate value, add to the existing value
          dc_offset_sum[i][j] += analogRead ( analog_pin[i][j] );
        }

        if ( k == dc_offset_est_len - 1 )
        {
          // we are done, calculate the DC offset now
          dc_offset[i][j] = dc_offset_sum[i][j] / dc_offset_est_len;
        }
      }
    }
    delayMicroseconds ( 100 );
  }
}


void Edrumulus::process()
{
  bool  is_overload = false;
  float debug;

/*
// for debugging: take samples from Octave, process and return result to Octave
if ( Serial.available() > 0 )
{
  const float fIn = Serial.parseFloat();
  process_sample ( fIn, peak_found, midi_velocity, midi_pos, is_rim_shot, debug );
  Serial.println ( debug, 7 );
}
return;
*/

  // wait for the timer to get the correct sampling rate when reading the analog value
  if ( xSemaphoreTake ( timer_semaphore, portMAX_DELAY ) == pdTRUE )
  {
    for ( int i = 0; i < number_pads; i++ )
    {    
      int   sample_org[MAX_NUM_PAD_INPUTS];
      float sample[MAX_NUM_PAD_INPUTS];

      // get sample(s) from ADC and prepare sample(s) for processing
      for ( int j = 0; j < number_inputs[i]; j++ )
      {
        sample_org[j] = analogRead ( analog_pin[i][j] );
        sample[j]     = sample_org[j] - dc_offset[i][j]; // compensate DC offset
      }

      // process sample
      if ( pad[i].get_is_control() )
      {
        pad[i].process_control_sample ( sample_org, control_found[i], midi_ctrl_value[i] );
      }
      else
      {
        pad[i].process_sample ( sample, peak_found[i], midi_velocity[i], midi_pos[i], is_rim_shot[i], debug );

        // overload detection
        for ( int j = 0; j < number_inputs[i]; j++ )
        {
          is_overload |= ( sample_org[j] >= 4094 ) || ( sample_org[j] <= 1 );
        }
      }
    }

    // overload detection
    if ( is_overload )
    {
      overload_LED_cnt   = overload_LED_on_time;
      status_is_overload = true;
    }

    if ( overload_LED_cnt > 1 )
    {
      overload_LED_cnt--;
    }
    else if ( overload_LED_cnt == 1 ) // transition to off state
    {
      overload_LED_cnt   = 0; // idle state
      status_is_overload = false;
    }

    // sampling rate check (i.e. if CPU is overloaded, the sample rate will drop which is bad)
    if ( samplerate_prev_micros_cnt >= samplerate_max_cnt )
    {
      // set error flag if sample rate deviation is too large
      status_is_error            = ( abs ( 1.0f / ( micros() - samplerate_prev_micros ) * samplerate_max_cnt * 1e6f - Fs ) > samplerate_max_error_Hz );
      samplerate_prev_micros_cnt = 0;
      samplerate_prev_micros     = micros();
    }
    samplerate_prev_micros_cnt++;
  }
}


// -----------------------------------------------------------------------------
// Pad -------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void Edrumulus::Pad::setup ( const int conf_Fs,
                             const int conf_number_inputs )
{
  // set essential parameters
  Fs            = conf_Fs;
  number_inputs = conf_number_inputs;

  // initialize with default pad type and other defaults
  set_pad_type ( PD120 );
  midi_note     = 38;
  midi_note_rim = 40;
  midi_ctrl_ch  = 4; // CC4, usually used for hi-hat
}


void Edrumulus::Pad::set_pad_type ( const Epadtype new_pad_type )
{
  // apply new pad type and set all parameters to the default values for that pad type
  pad_settings.pad_type = new_pad_type;

  // apply PRESET settings (might be overwritten by pad-specific properties)
  pad_settings.velocity_threshold     = 15; // 0..31
  pad_settings.velocity_sensitivity   = 1;  // 0..31
  pad_settings.mask_time_ms           = 6;  // 0..31 (ms)
  pad_settings.pos_threshold          = 9;  // 0..31
  pad_settings.pos_sensitivity        = 14; // 0..31
  pad_settings.rim_shot_treshold      = 11; // 0..31
  pad_settings.midi_curve_type        = LINEAR;
  pad_settings.pos_sense_is_used      = false; // must be explicitely enabled if it shall be used
  pad_settings.rim_shot_is_used       = false; // must be explicitely enabled if it shall be used
  pad_settings.energy_win_len_ms      = 2.0f;   // pad specific parameter: hit energy estimation time window length
  pad_settings.scan_time_ms           = 2.5f;   // pad specific parameter: scan time after first detected peak
  pad_settings.main_peak_dist_ms      = 2.25f;  // pad specific parameter: distance between the two main peaks
  pad_settings.decay_est_delay2nd_ms  = 2.5f;   // pad specific parameter: delay after second main peak until decay power estimation starts
  pad_settings.decay_est_len_ms       = 3.0f;   // pad specific parameter: decay power estimation window length
  pad_settings.decay_est_fact_db      = 15.0f;  // pad specific parameter: decay power estimation factor (to get over decay ripple)
  pad_settings.decay_fact_db          = 1.0f;   // pad specific parameter: vertical shift of the decay function in dB
  pad_settings.decay_len1_ms          = 0.0f;   // pad specific parameter: length of the decay 1
  pad_settings.decay_grad_fact1       = 200.0f; // pad specific parameter: decay function gradient factor 1
  pad_settings.decay_len2_ms          = 250.0f; // pad specific parameter: length of the decay 2
  pad_settings.decay_grad_fact2       = 200.0f; // pad specific parameter: decay function gradient factor 2
  pad_settings.decay_len3_ms          = 0.0f;   // pad specific parameter: length of the decay 3
  pad_settings.decay_grad_fact3       = 200.0f; // pad specific parameter: decay function gradient factor 3
  pad_settings.pos_energy_win_len_ms  = 2.0f;   // pad specific parameter: pos sense energy estimation time window length
  pad_settings.pos_iir_alpha          = 200.0f; // pad specific parameter: IIR low-pass alpha value for positional sensing
  pad_settings.rim_shot_window_len_ms = 5.0f;   // pad specific parameter: window length for rim shot detection

  switch ( new_pad_type )
  {
    case PD120:
      // note: the PRESET settings are from the PD-120 pad
      break;

    case PD80R:
// TODO add support for PD-80R pad
pad_settings.velocity_sensitivity = 4;
      break;

    case PD8:
      pad_settings.mask_time_ms          = 7;
      pad_settings.pos_threshold         = 29;
      pad_settings.pos_sensitivity       = 29;
      pad_settings.scan_time_ms          = 1.3f;
      pad_settings.main_peak_dist_ms     = 0.75f;
      pad_settings.decay_est_delay2nd_ms = 6.0f;
      pad_settings.decay_fact_db         = 5.0f;
      pad_settings.decay_len1_ms         = 10.0f;
      pad_settings.decay_grad_fact1      = 30.0f;
      pad_settings.decay_len2_ms         = 30.0f;
      pad_settings.decay_grad_fact2      = 600.0f;
      pad_settings.decay_len3_ms         = 150.0f;
      pad_settings.decay_grad_fact3      = 120.0f;
      break;
  }

  initialize();
}


void Edrumulus::Pad::initialize()
{
  // set algorithm parameters
  const float threshold_db = 15.0f + pad_settings.velocity_threshold;           // gives us a threshold range of 15..46 dB
  threshold                = pow   ( 10.0f, threshold_db / 10 );                // linear power threshold
  energy_window_len        = round ( pad_settings.energy_win_len_ms * 1e-3f * Fs ); // hit energy estimation time window length (e.g. 2 ms)
  scan_time                = round ( pad_settings.scan_time_ms  * 1e-3f * Fs ); // scan time from first detected peak
  mask_time                = round ( pad_settings.mask_time_ms  * 1e-3f * Fs ); // mask time (e.g. 10 ms)
  decay_len1               = round ( pad_settings.decay_len1_ms * 1e-3f * Fs ); // decay time 1 (e.g. 250 ms)
  decay_len2               = round ( pad_settings.decay_len2_ms * 1e-3f * Fs ); // decay time 2 (e.g. 250 ms)
  decay_len3               = round ( pad_settings.decay_len3_ms * 1e-3f * Fs ); // decay time 3 (e.g. 250 ms)
  decay_len                = decay_len1 + decay_len2 + decay_len3;
  decay_fact               = pow   ( 10.0f, pad_settings.decay_fact_db / 10 );  // decay factor of 1 dB
  const float decay_grad1  = pad_settings.decay_grad_fact1 / Fs;                // decay gradient factor 1
  const float decay_grad2  = pad_settings.decay_grad_fact2 / Fs;                // decay gradient factor 2
  const float decay_grad3  = pad_settings.decay_grad_fact3 / Fs;                // decay gradient factor 3
  main_peak_dist           = round ( pad_settings.main_peak_dist_ms     * 1e-3f * Fs );
  decay_est_delay2nd       = round ( pad_settings.decay_est_delay2nd_ms * 1e-3f * Fs );
  decay_est_len            = round ( pad_settings.decay_est_len_ms      * 1e-3f * Fs );
  decay_est_fact           = pow ( 10.0f, pad_settings.decay_est_fact_db / 10 );
  pos_energy_window_len    = round ( pad_settings.pos_energy_win_len_ms * 1e-3f * Fs );      // positional sensing energy estimation time window length (e.g. 2 ms)
  alpha                    = pad_settings.pos_iir_alpha / Fs;                                // IIR low pass filter coefficient
  rim_shot_window_len      = round ( pad_settings.rim_shot_window_len_ms * 1e-3f * Fs );     // window length (e.g. 5 ms)
  rim_shot_treshold_dB     = static_cast<float> ( pad_settings.rim_shot_treshold ) / 2 - 13; // gives us a rim shot threshold range of -13..2.5 dB

  // The ESP32 ADC has 12 bits resulting in a range of 20*log10(2048)=66.2 dB minus the threshold value.
  // The sensitivity parameter shall be in the range of 0..31. This range should then be mapped to the
  // maximum possible dynamic where sensitivity of 31 means that we have no dynamic at all and 0 means
  // that we use the full possible ADC range.
  const float max_velocity_range_db = 20 * log10 ( 2048 ) - threshold_db;
  velocity_range_db                 = max_velocity_range_db * ( 32 - pad_settings.velocity_sensitivity ) / 32;

  // The positional sensing MIDI assignment parameters are dependent on, e.g., the filter design
  // parameters and cannot easily be derived from the ADC properties as is done for the velocity.
  // Based on the measurement results with the PD120 pad, we tryed to derive some meaningful parameter ranges.
  const float pos_threshold_db = pad_settings.pos_threshold;           // gives us a threshold range of 0..31 dB
  pos_threshold                = pow ( 10.0f, pos_threshold_db / 10 ); // linear power threshold
  const float max_pos_range_db = 11; // dB (found by analyzing pd120_pos_sense2.wav test signal)
  pos_range_db                 = max_pos_range_db * ( 32 - pad_settings.pos_sensitivity ) / 32;

  // allocate and initialize memory for vectors
  allocate_initialize ( &hil_hist,                hil_filt_len );          // memory for Hilbert filter history
  allocate_initialize ( &mov_av_hist_re,          energy_window_len );     // real part memory for moving average filter history
  allocate_initialize ( &mov_av_hist_im,          energy_window_len );     // imaginary part memory for moving average filter history
  allocate_initialize ( &decay,                   decay_len );             // memory for decay function
  allocate_initialize ( &hist_main_peak_pow_left, main_peak_dist );        // memory for left main peak power
  allocate_initialize ( &hil_hist_re,             pos_energy_window_len ); // real part of memory for moving average of Hilbert filtered signal
  allocate_initialize ( &hil_hist_im,             pos_energy_window_len ); // imaginary part of memory for moving average of Hilbert filtered signal
  allocate_initialize ( &hil_low_hist_re,         pos_energy_window_len ); // real part of memory for moving average of low-pass filtered Hilbert signal
  allocate_initialize ( &hil_low_hist_im,         pos_energy_window_len ); // imaginary part of memory for moving average of low-pass filtered Hilbert signal
  allocate_initialize ( &rim_x_high_hist,         rim_shot_window_len );   // memory for rim shot detection
  allocate_initialize ( &midi_curve,              128 );                   // memory for MIDI curve

  mask_back_cnt           = 0;
  was_above_threshold     = false;
  prev_hil_filt_val       = 0.0f;
  decay_back_cnt          = 0;
  decay_scaling           = 1.0f;
  scan_time_cnt           = 0;
  power_hypo_left         = 0.0f;
  power_hypo_right_cnt    = 0;
  decay_pow_est_start_cnt = 0;
  decay_pow_est_cnt       = 0;
  decay_pow_est_sum       = 0.0f;
  pos_sense_cnt           = 0;
  hil_low_re              = 0.0f;
  hil_low_im              = 0.0f;
  rim_shot_cnt            = 0;
  rim_high_prev_x         = 0.0f;
  rim_x_high              = 0.0f;
  hil_filt_max_pow        = 0.0f;
  max_hil_filt_val        = 0.0f;
  peak_found_offset       = 0;
  was_peak_found          = false;
  was_pos_sense_ready     = false;
  was_rim_shot_ready      = false;
  stored_is_rimshot       = false;
  prev_ctrl_value         = 0;

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

  // calculate MIDI curve (taken from RyoKosaka HelloDrum-arduino-Library: int HelloDrum::curve() function)
  float midi_curve_param;

  switch ( pad_settings.midi_curve_type )
  {
    case EXP1: midi_curve_param = 1.02; break;
    case EXP2: midi_curve_param = 1.04; break;
    case LOG1: midi_curve_param = 0.98; break;
    case LOG2: midi_curve_param = 0.96; break;
  }

  for ( int i = 0; i < 128; i++ )
  {
    if ( pad_settings.midi_curve_type == LINEAR )
    {
      midi_curve[i] = i;
    }
    else
    {
      midi_curve[i] = round ( ( 126 / ( pow ( midi_curve_param, 126 ) - 1 ) ) * ( pow ( midi_curve_param, i - 1 ) - 1 ) + 1 );
    }
  }
}


void Edrumulus::Pad::process_sample ( const float* input,
                                      bool&        peak_found,
                                      int&         midi_velocity,
                                      int&         midi_pos,
                                      bool&        is_rim_shot,
                                      float&       debug )
{
  // initialize return parameter
  peak_found                   = false;
  midi_velocity                = 0;
  midi_pos                     = 0;
  is_rim_shot                  = false;
  bool       first_peak_found  = false; // only used internally
  const bool pos_sense_is_used = pad_settings.pos_sense_is_used;                         // can be applied directly without calling initialize()
  const bool rim_shot_is_used  = pad_settings.rim_shot_is_used && ( number_inputs > 1 ); // can be applied directly without calling initialize()

debug = 0.0f; // TEST


  // Calculate peak detection -----------------------------------------------------
  // Hilbert filter
  update_fifo ( input[0], hil_filt_len, hil_hist );

  float hil_re = 0;
  float hil_im = 0;
  for ( int i = 0; i < hil_filt_len; i++ )
  {
    hil_re += hil_hist[i] * a_re[i];
    hil_im += hil_hist[i] * a_im[i];
  }

  // moving average filter
  update_fifo ( hil_re, energy_window_len, mov_av_hist_re );
  update_fifo ( hil_im, energy_window_len, mov_av_hist_im );

  float mov_av_re = 0;
  float mov_av_im = 0;
  for ( int i = 0; i < energy_window_len; i++ )
  {
    mov_av_re += mov_av_hist_re[i];
    mov_av_im += mov_av_hist_im[i];
  }
  mov_av_re /= energy_window_len;
  mov_av_im /= energy_window_len;

  const float hil_filt = mov_av_re * mov_av_re + mov_av_im * mov_av_im;

  // exponential decay assumption
  float hil_filt_decay = hil_filt;

  if ( decay_back_cnt > 0 )
  {
    // subtract decay (with clipping at zero)
    const float cur_decay = decay_scaling * decay[decay_len - decay_back_cnt];
    hil_filt_decay        = hil_filt - cur_decay;
    decay_back_cnt--;

    if ( hil_filt_decay < 0.0f )
    {
      hil_filt_decay = 0.0f;
    }
  }

  // threshold test
  if ( ( ( hil_filt_decay > threshold ) || was_above_threshold ) && ( mask_back_cnt == 0 ) )
  {
    was_above_threshold = true;

    // climb to the maximum of the first peak
    if ( ( prev_hil_filt_val < hil_filt ) && ( scan_time_cnt == 0 ) )
    {
      prev_hil_filt_val = hil_filt;
    }
    else
    {
      // start condition of scan time
      if ( scan_time_cnt == 0 )
      {
        // search in a pre-defined scan time for the highest peak
        scan_time_cnt     = scan_time;                  // initialize scan time counter
        max_hil_filt_val  = prev_hil_filt_val;          // initialize maximum value with first peak
        peak_found_offset = scan_time;                  // position of first peak after scan time expired
        power_hypo_left   = hist_main_peak_pow_left[0]; // for left/right main peak detection
        first_peak_found  = true;
      }

      // search for a maximum in the scan time interval
      if ( hil_filt > max_hil_filt_val )
      {
        max_hil_filt_val  = hil_filt;                   // we need to store the origianl Hilbert filtered signal for the decay
        peak_found_offset = scan_time_cnt - 1;          // update position of detected peak
        power_hypo_left   = hist_main_peak_pow_left[0]; // for left/right main peak detection
      }

      scan_time_cnt--;

      // end condition of scan time
      if ( scan_time_cnt <= 0 )
      {
        // calculate the MIDI velocity value with clipping to allowed MIDI value range
        stored_midi_velocity = static_cast<int> ( ( 10 * log10 ( prev_hil_filt_val / threshold ) / velocity_range_db ) * 127 );
        stored_midi_velocity = midi_curve[max ( 1, min ( 127, stored_midi_velocity ) )];

        // scan time expired
        prev_hil_filt_val   = 0.0f;
        was_above_threshold = false;
        decay_scaling       = max_hil_filt_val * decay_fact;
        decay_back_cnt      = decay_len - peak_found_offset;
        mask_back_cnt       = mask_time - peak_found_offset;
        was_peak_found      = true;

        // for left/right main peak detection (note that we have to add one because
        // we first decrement and then we check for end condition)
        power_hypo_right_cnt = max ( 1, main_peak_dist - peak_found_offset + 1 );
      }
    }
  }

  if ( mask_back_cnt > 0 )
  {
    mask_back_cnt--;
  }

  // manage left/right main peak detection by power comparision
  update_fifo ( hil_filt, main_peak_dist, hist_main_peak_pow_left );

  if ( power_hypo_right_cnt > 0 )
  {
    power_hypo_right_cnt--;

    // end condition
    if ( power_hypo_right_cnt <= 0 )
    {
      // now we can detect if the main peak was the left/right main peak and we can
      // now start the counter for the decay power estimation interval start (note
      // that we have to add one because we first decrement and then we check for
      // end condition)
      if ( power_hypo_left > hil_filt )
      {
        decay_pow_est_start_cnt = decay_est_delay2nd - main_peak_dist + 1; // detected peak is right main peak
      }
      else
      {
        decay_pow_est_start_cnt = decay_est_delay2nd + 1; // detected peak is left main peak
      }
    }
  }

  // decay power estimation
  if ( decay_pow_est_start_cnt > 0 )
  {
    decay_pow_est_start_cnt--;

    // end condition
    if ( decay_pow_est_start_cnt <= 0 )
    {
      decay_pow_est_cnt = decay_est_len; // now the power estimation can start
    }
  }

  if ( decay_pow_est_cnt > 0 )
  {
    decay_pow_est_sum += hil_filt; // sum up the powers in pre-defined interval
    decay_pow_est_cnt--;

    // end condition
    if ( decay_pow_est_cnt <= 0 )
    {
      const float decay_power = decay_pow_est_sum / decay_est_len;                   // calculate average power
      decay_pow_est_sum       = 0.0f;                                                // we have to reset the sum for the next calculation
      decay_scaling           = min ( decay_scaling, decay_est_fact * decay_power ); // adjust the decay curve
    }
  }


  // Calculate positional sensing -------------------------------------------------
  if ( pos_sense_is_used )
  {
    // low pass filter of the Hilbert signal
    hil_low_re = ( 1.0f - alpha ) * hil_low_re + alpha * hil_re;
    hil_low_im = ( 1.0f - alpha ) * hil_low_im + alpha * hil_im;
  
    update_fifo ( hil_re,     pos_energy_window_len, hil_hist_re );
    update_fifo ( hil_im,     pos_energy_window_len, hil_hist_im );
    update_fifo ( hil_low_re, pos_energy_window_len, hil_low_hist_re );
    update_fifo ( hil_low_im, pos_energy_window_len, hil_low_hist_im );
  
    float peak_energy     = 0;
    float peak_energy_low = 0;
    for ( int i = 0; i < pos_energy_window_len; i++ )
    {
      peak_energy     += ( hil_hist_re[i]     * hil_hist_re[i]     + hil_hist_im[i]     * hil_hist_im[i] );
      peak_energy_low += ( hil_low_hist_re[i] * hil_low_hist_re[i] + hil_low_hist_im[i] * hil_low_hist_im[i] );
    }
  
    // start condition of delay process to fill up the required buffers
    if ( first_peak_found && ( !was_pos_sense_ready ) && ( pos_sense_cnt == 0 ) )
    {
      // a peak was found, we now have to start the delay process to fill up the
      // required buffer length for our metric
      pos_sense_cnt = energy_window_len / 2 - 1;
    }
  
    if ( pos_sense_cnt > 0 )
    {
      pos_sense_cnt--;
  
      // end condition
      if ( pos_sense_cnt <= 0 )
      {
        // the buffers are filled, now calculate the metric
        const float pos_sense_metric = peak_energy / peak_energy_low;
        was_pos_sense_ready          = true;

        // positional sensing MIDI mapping with clipping to allowed MIDI value range
        stored_midi_pos = static_cast<int> ( ( 10 * log10 ( pos_sense_metric / pos_threshold ) / pos_range_db ) * 127 );
        stored_midi_pos = max ( 1, min ( 127, stored_midi_pos ) );
      }
      else
      {
        // we need a further delay for the positional sensing estimation, consider
        // this additional delay for the overall peak found offset
        if ( was_peak_found )
        {
          peak_found_offset++;
        }
      }
    }
  }


  // Calculate rim shot detection -------------------------------------------------
  if ( rim_shot_is_used )
  {
    // one pole IIR high pass filter (y1 = (b0 * x1 + b1 * x0 - a1 * y0) / a0)
    // note that rim input signal is in second dimension of the input vector
    rim_x_high      = ( b_rim_high[0] * input[1] + b_rim_high[1] * rim_high_prev_x - a_rim_high * rim_x_high );
    rim_high_prev_x = input[1]; // store previous x

    update_fifo ( rim_x_high, rim_shot_window_len, rim_x_high_hist );

    // start condition of delay process to fill up the required buffers
    // note that rim_shot_window_len must be larger than energy_window_len,
    // pos_energy_window_len and scan_time for this to work
    if ( first_peak_found && ( !was_rim_shot_ready ) && ( rim_shot_cnt == 0 ) )
    {
      // a peak was found, we now have to start the delay process to fill up the
      // required buffer length for our metric
      rim_shot_cnt     = rim_shot_window_len / 2 - 1;
      hil_filt_max_pow = hil_filt;
    }

    if ( rim_shot_cnt > 0 )
    {
      rim_shot_cnt--;

      // end condition
      if ( rim_shot_cnt <= 0 )
      {
        // the buffers are filled, now calculate the metric
        float rim_max_pow = 0;
        for ( int i = 0; i < rim_shot_window_len; i++ )
        {
          rim_max_pow = max ( rim_max_pow, rim_x_high_hist[i] * rim_x_high_hist[i] );
        }

        const float rim_metric_db = 10 * log10 ( rim_max_pow / hil_filt_max_pow );
        stored_is_rimshot         = rim_metric_db > rim_shot_treshold_dB;
        rim_shot_cnt              = 0;
        was_rim_shot_ready        = true;

// TODO:
// - positional sensing must be adjusted if a rim shot is detected (note that this must be done BEFORE the MIDI clipping!)
// - only use one counter instead of rim_shot_cnt and pos_sense_cnt
// - as long as counter is not finished, do check "hil_filt_new > threshold" again to see if we have a higher peak in that
//   time window -> if yes, restart everything using the new detected peak
if ( stored_is_rimshot )
{
  stored_midi_pos = 0; // as a quick hack, disable positional sensing if a rim shot is detected
}

      }
      else
      {
        // we need a further delay for the positional sensing estimation, consider
        // this additional delay for the overall peak found offset
        if ( was_peak_found && ( !pos_sense_is_used || was_pos_sense_ready ) )
        {
          peak_found_offset++;
        }
      }
    }
  }

  // check for all estimations are ready and we can set the peak found flag and
  // return all results
  if ( was_peak_found && ( !pos_sense_is_used || was_pos_sense_ready ) && ( !rim_shot_is_used || was_rim_shot_ready ) )
  {
    midi_velocity = stored_midi_velocity;
    midi_pos      = stored_midi_pos;
    peak_found    = true;
    is_rim_shot   = stored_is_rimshot;

    was_peak_found      = false;
    was_pos_sense_ready = false;
    was_rim_shot_ready  = false;
  }

// TEST
debug = hil_low_re;

}

void Edrumulus::Pad::process_control_sample ( const int* input,
                                              bool&      change_found,
                                              int&       midi_ctrl_value )
{
// TEST just a quick hack implementation for now -> TODO improve the implementation

  change_found = false;

  if ( input[0] < 500 )
  {
    midi_ctrl_value = 127;
  }
  else if ( input[0] < 1000 )
  {
    midi_ctrl_value = 100;
  }
  else if ( input[0] < 1500 )
  {
    midi_ctrl_value = 80;
  }
  else if ( input[0] < 2000 )
  {
    midi_ctrl_value = 60;
  }
  else if ( input[0] < 2500 )
  {
    midi_ctrl_value = 40;
  }
  else if ( input[0] < 3000 )
  {
    midi_ctrl_value = 20;
  }
  else
  {
    midi_ctrl_value = 0;
  }

  if ( midi_ctrl_value != prev_ctrl_value )
  {
    change_found    = true;
    prev_ctrl_value = midi_ctrl_value;
  }
}
