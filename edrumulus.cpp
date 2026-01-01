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

#include "edrumulus.h"

Edrumulus::Edrumulus()
{
  // initializations
  overload_LED_on_time       = round(overload_LED_on_time_s * Fs);
  error_LED_blink_time       = round(error_LED_blink_time_s * Fs);
  dc_offset_est_len          = round(dc_offset_est_len_s * Fs);
  samplerate_max_cnt         = round(samplerate_max_cnt_len_s * Fs);
  load_indicator_max_cnt     = round(load_indicator_max_cnt_len_s * Fs);
  dc_offset_min_limit        = round(ADC_MAX_RANGE / 2 - ADC_MAX_RANGE * dc_offset_max_rel_error);
  dc_offset_max_limit        = round(ADC_MAX_RANGE / 2 + ADC_MAX_RANGE * dc_offset_max_rel_error);
  overload_LED_cnt           = 0;
  status_is_overload         = false;
  samplerate_prev_micros_cnt = 0;
  samplerate_prev_micros     = 0;
  use_load_indicator         = false;
  load_indicator_cnt         = 0;
  load_indicator_prev_micros = 0;
  load_indicator_sum         = 0;
  load_indicator             = -1; // initialize with invalid result
  status_is_error            = false;
  dc_offset_error_channel    = -1;
#ifdef ESP_PLATFORM
  spike_cancel_level = 4; // use max. spike cancellation on the ESP32 per default (note that it increases the latency)
#else
  spike_cancel_level = 0; // default
#endif
  cancel_num_samples            = (cancel_time_ms * Fs) / 1000;
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
  dc_offset_iir_gamma           = exp(-1.0 / (Fs * dc_offset_iir_tau_seconds));
  dc_offset_iir_one_minus_gamma = 1.0f - static_cast<float>(dc_offset_iir_gamma);
}

void Edrumulus::setup(const int  conf_num_pads,
                      const int* conf_analog_pins,
                      const int* conf_analog_pins_rim_shot)
{
  number_pads = min(conf_num_pads, MAX_NUM_PADS);

  for (int i = 0; i < number_pads; i++)
  {
    // set the pad GIOP pin numbers
    analog_pin[i][0] = conf_analog_pins[i];
    analog_pin[i][1] = conf_analog_pins_rim_shot[i];
    number_inputs[i] = conf_analog_pins_rim_shot[i] >= 0 ? 2 : 1;

    // setup the pad
    pad[i].setup(Fs);
  }

  // setup the ESP32 specific object, this has to be done after assigning the analog
  // pin numbers and before using the analog read function (as in the DC offset estimator)
  edrumulus_hardware.setup(Fs,
                           number_pads,
                           number_inputs,
                           analog_pin);

  // estimate the DC offset for all inputs
  float dc_offset_sum[MAX_NUM_PADS][MAX_NUM_PAD_INPUTS];

  for (int k = 0; k < dc_offset_est_len; k++)
  {
    edrumulus_hardware.capture_samples(number_pads,
                                       number_inputs,
                                       analog_pin,
                                       sample_org);

    for (int i = 0; i < number_pads; i++)
    {
      for (int j = 0; j < number_inputs[i]; j++)
      {
        if (k == 0)
        {
          // initial value
          dc_offset_sum[i][j] = sample_org[i][j];
        }
        else if (k == dc_offset_est_len - 1)
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
  edrumulus_hardware.capture_samples(number_pads,
                                     number_inputs,
                                     analog_pin,
                                     sample_org);

  // for load indicator we need to store current time right after blocking function
  if (use_load_indicator)
  {
    load_indicator_prev_micros = micros();
  }

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
  for (int i = 0; i < number_pads; i++)
  {
    uint16_t* sample_org_pad = sample_org[i];
    peak_found[i]            = false;
    control_found[i]         = false;

    if (pad[i].get_is_control())
    {
      // process sample for control input
      pad[i].process_control_sample(sample_org_pad, control_found[i], midi_ctrl_value[i], peak_found[i], midi_velocity[i]);
    }
    else
    {
      // prepare samples for processing
      for (int j = 0; j < number_inputs[i]; j++)
      {
        double&    cur_dc_offset       = dc_offset[i][j];
        const bool is_rim_switch_input = (j == 1) && pad[i].get_is_rim_switch(); // rim is always on second channel

        // overload detection: check for the lowest/largest possible ADC range values with noise consideration
        if (sample_org_pad[j] >= ADC_MAX_RANGE - ADC_MAX_NOISE_AMPL)
        {
          overload_LED_cnt     = overload_LED_on_time;
          overload_detected[j] = 2;
        }
        else if (sample_org_pad[j] <= ADC_MAX_NOISE_AMPL - 1)
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
        if (!(is_rim_switch_input && pad[i].get_is_rim_switch_on()))
        {
          cur_dc_offset = dc_offset_iir_gamma * cur_dc_offset + dc_offset_iir_one_minus_gamma * sample_org_pad[j];
        }

        // compensate DC offset
        sample[j] = sample_org_pad[j] - static_cast<float>(cur_dc_offset);

        // ADC spike cancellation (do not use spike cancellation for rim switches since they have short peaks)
        if ((spike_cancel_level > 0) && !is_rim_switch_input)
        {
          cancel_ADC_spikes(sample[j], overload_detected[j], i, j, spike_cancel_level);
        }
      }

      // process sample
      if (any_coupling_used && // note: short-cut for speed optimization of normal non-coupling mode
          (((coupled_pad_idx_primary >= 0) && ((i == coupled_pad_idx_secondary) || (i == coupled_pad_idx_primary))) ||
           ((coupled_pad_idx_rim_primary >= 0) && ((i == coupled_pad_idx_rim_secondary) || (i == coupled_pad_idx_rim_primary)))))
      {
        // special case: couple pad inputs for multiple head sensor capturing (assume that both pads have dual-inputs)
        if ((i == coupled_pad_idx_primary) || (i == coupled_pad_idx_secondary))
        {
          if (((coupled_pad_idx_primary < coupled_pad_idx_secondary) && (i == coupled_pad_idx_primary)) ||
              ((coupled_pad_idx_secondary < coupled_pad_idx_primary) && (i == coupled_pad_idx_secondary)))
          {
            stored_sample_coupled_head[0]            = sample[0]; // store 1st input
            stored_sample_coupled_head[1]            = sample[1];
            stored_overload_detected_coupled_head[0] = overload_detected[0]; // store 2nd input
            stored_overload_detected_coupled_head[1] = overload_detected[1];
          }
          else
          {
            // combine samples and process pad coupled_pad_idx_primary which is the primary coupled pad,
            // new "sample" layout: sum, rim, 1st head, 2nd head, 3rd head
            if (coupled_pad_idx_primary > coupled_pad_idx_secondary)
            {
              sample[2]            = sample[0]; // 1st head (note that rim is already at correct place)
              overload_detected[2] = overload_detected[0];
              sample[3]            = stored_sample_coupled_head[0]; // 2nd head
              overload_detected[3] = stored_overload_detected_coupled_head[0];
              sample[4]            = stored_sample_coupled_head[1]; // 3rd head
              overload_detected[4] = stored_overload_detected_coupled_head[1];
            }
            else
            {
              sample[3]            = sample[0]; // 2nd head
              overload_detected[3] = overload_detected[0];
              sample[4]            = sample[1]; // 3rd head
              overload_detected[4] = overload_detected[1];
              sample[1]            = stored_sample_coupled_head[1]; // rim (no overload_detected used for rim)
              sample[2]            = stored_sample_coupled_head[0]; // 1st head
              overload_detected[2] = stored_overload_detected_coupled_head[0];
            }
            sample[0] = (sample[2] + sample[3] + sample[4]) / 3; // sum is on channel 0

            pad[coupled_pad_idx_primary].process_sample(sample,
                                                        5,
                                                        overload_detected,
                                                        peak_found[coupled_pad_idx_primary],
                                                        midi_velocity[coupled_pad_idx_primary],
                                                        midi_pos[coupled_pad_idx_primary],
                                                        rim_state[coupled_pad_idx_primary],
                                                        is_choke_on[coupled_pad_idx_primary],
                                                        is_choke_off[coupled_pad_idx_primary]);
          }
        }

        // special case: couple pad inputs for two-rim sensor capturing
        if ((i == coupled_pad_idx_rim_primary) || (i == coupled_pad_idx_rim_secondary))
        {
          if (((coupled_pad_idx_rim_primary < coupled_pad_idx_rim_secondary) && (i == coupled_pad_idx_rim_primary)) ||
              ((coupled_pad_idx_rim_secondary < coupled_pad_idx_rim_primary) && (i == coupled_pad_idx_rim_secondary)))
          {
            stored_sample_coupled_rim[0]            = sample[0]; // store 1st input
            stored_sample_coupled_rim[1]            = sample[1];
            stored_overload_detected_coupled_rim[0] = overload_detected[0]; // store 2nd input
            stored_overload_detected_coupled_rim[1] = overload_detected[1];
          }
          else
          {
            // combine samples and process pad coupled_pad_idx_rim_primary which is the primary coupled pad,
            // new "sample" layout: 1st head, 1st rim, 2nd rim
            if (coupled_pad_idx_rim_primary > coupled_pad_idx_rim_secondary)
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

            pad[coupled_pad_idx_rim_primary].process_sample(sample,
                                                            3,
                                                            overload_detected,
                                                            peak_found[coupled_pad_idx_rim_primary],
                                                            midi_velocity[coupled_pad_idx_rim_primary],
                                                            midi_pos[coupled_pad_idx_rim_primary],
                                                            rim_state[coupled_pad_idx_rim_primary],
                                                            is_choke_on[coupled_pad_idx_rim_primary],
                                                            is_choke_off[coupled_pad_idx_rim_primary]);
          }
        }
      }
      else
      {
        // normal case: process samples directly
        pad[i].process_sample(sample,
                              number_inputs[i],
                              overload_detected,
                              peak_found[i],
                              midi_velocity[i],
                              midi_pos[i],
                              rim_state[i],
                              is_choke_on[i],
                              is_choke_off[i]);
      }
    }
  }

  // Cross talk cancellation ---------------------------------------------------
  for (int i = 0; i < number_pads; i++)
  {
    if (peak_found[i])
    {
      // reset cancellation count if conditions are met
      if ((cancel_cnt == 0) || ((cancel_cnt > 0) && (midi_velocity[i] > cancel_MIDI_velocity)))
      {
        cancel_cnt           = cancel_num_samples;
        cancel_MIDI_velocity = midi_velocity[i];
        cancel_pad_index     = i;
      }
      else if ((cancel_cnt > 0) && (cancel_pad_index != i))
      {
        // check if current pad is to be cancelled
        if (cancel_MIDI_velocity * pad[i].get_cancellation_factor() > midi_velocity[i])
        {
          peak_found[i] = false;
        }
      }
    }
  }

  if (cancel_cnt > 0)
  {
    cancel_cnt--;
  }

  // Overload detection: keep LED on for a while -------------------------------
  if (overload_LED_cnt > 0)
  {
    overload_LED_cnt--;
    status_is_overload = (overload_LED_cnt > 0);
  }

  // Load indicator ------------------------------------------------------------
  load_indicator = -1; // always default to -1 first

  if (use_load_indicator)
  {
    load_indicator_sum += micros() - load_indicator_prev_micros;
    load_indicator_cnt++;

    if (load_indicator_cnt >= load_indicator_max_cnt)
    {
      // calculate load indicator value in range 0 to 127
      const float avg_micros = static_cast<float>(load_indicator_sum) / load_indicator_max_cnt;
      load_indicator         = round(avg_micros / 1e6f * Fs * 127.0f);
      load_indicator         = max(0, min(127, load_indicator));
      load_indicator_sum     = 0;
      load_indicator_cnt     = 0;
    }
  }

  // Sampling rate and DC offset check -----------------------------------------
  // (i.e. if CPU is overloaded, the sample rate will drop which is bad)
  samplerate_prev_micros_cnt++;

  if (samplerate_prev_micros_cnt >= samplerate_max_cnt)
  {
    const unsigned long samplerate_cur_micros = micros();

    // do not update status if micros() has wrapped around (at about 70 minutes) and if
    // we have the very first measurement after start (previous micros set to 0)
    if ((samplerate_prev_micros != 0) && (samplerate_cur_micros - samplerate_prev_micros > 0))
    {
      // set error flag if sample rate deviation is too large
      status_is_error = (abs(1.0f / (samplerate_cur_micros - samplerate_prev_micros) * samplerate_max_cnt * 1e6f - Fs) > samplerate_max_error_Hz);
    }

    samplerate_prev_micros_cnt = 0;
    samplerate_prev_micros     = samplerate_cur_micros;

    // DC offset check
    dc_offset_error_channel = -1; // invalidate for "no DC offset error" case
    for (int i = 0; i < number_pads; i++)
    {
      if (!pad[i].get_is_control())
      {
        for (int j = 0; j < number_inputs[i]; j++)
        {
          // Serial.println(String(i) + ", " + String(cur_dc_offset)); // TEST for plotting all DC offsets
          if ((dc_offset[i][j] < dc_offset_min_limit) || (dc_offset[i][j] > dc_offset_max_limit))
          {
            status_is_error         = true;
            dc_offset_error_channel = i + 32 * j; // 0 to 31: input 0, 32 to 63: input 1
          }
        }
      }
    }
  }
}

void Edrumulus::set_coupled_pad_idx(const int pad_idx, const int new_idx)
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
  if (new_idx < MAX_NUM_PADS)
  {
    if (pad[pad_idx].get_pad_type() == Pad::PDA120LS)
    {
      // Case 1. ---------------------------------------------------------------
      if ((coupled_pad_idx_primary < 0) || (pad_idx == coupled_pad_idx_primary))
      {
        // special case: always set coupled pad index parameter regardless if it is valid
        // or not to avoid issues in the GUI but if the index is invalid (i.e., no two inputs
        // available), do not enable the coupling
        pad[pad_idx].set_coupled_pad_idx(new_idx);

        const int cur_idx         = number_inputs[new_idx] > 1 ? new_idx : 0 /* 0 disables coupling */;
        coupled_pad_idx_primary   = cur_idx > 0 ? pad_idx : -1; // primary set to -1 switches coupling OFF
        coupled_pad_idx_secondary = cur_idx;
        pad[pad_idx].set_head_sensor_coupling(cur_idx > 0);
      }
    }
    else if ((pad[pad_idx].get_pad_type() == Pad::CY6) ||
             (pad[pad_idx].get_pad_type() == Pad::CY8) ||
             (pad[pad_idx].get_pad_type() == Pad::CY5))
    {
      // Case 2. ---------------------------------------------------------------
      if ((coupled_pad_idx_rim_primary < 0) || (pad_idx == coupled_pad_idx_rim_primary))
      {
        pad[pad_idx].set_coupled_pad_idx(new_idx);
        coupled_pad_idx_rim_primary   = new_idx > 0 ? pad_idx : -1; // primary set to -1 switches coupling OFF
        coupled_pad_idx_rim_secondary = new_idx;
        pad[pad_idx].set_use_second_rim(new_idx > 0);
      }
    }

    any_coupling_used = (coupled_pad_idx_primary >= 0) || (coupled_pad_idx_rim_primary >= 0);
  }
}

void Edrumulus::cancel_ADC_spikes(float&    signal,
                                  int&      overload_detected,
                                  const int pad_index,
                                  const int input_channel_index,
                                  const int level)
{
  // remove single/dual sample spikes by checking if right before and right after the
  // detected spike(s) we only have noise and no useful signal (since the ESP32 spikes
  // mostly are on just one or two sample(s))
  const int max_peak_threshold = 150; // maximum assumed ESP32 spike amplitude

  const float signal_org          = signal;
  signal                          = prev_input4[pad_index][input_channel_index]; // normal return value in case no spike was detected
  const int overload_detected_org = overload_detected;
  overload_detected               = prev_overload4[pad_index][input_channel_index]; // normal return value in case no spike was detected
  const float input_abs           = abs(signal_org);
  Espikestate input_state         = ST_OTHER; // initialization value, might be overwritten

  if (input_abs < ADC_MAX_NOISE_AMPL)
  {
    input_state = ST_NOISE;
  }
  else if ((signal_org < max_peak_threshold) && (signal_org > 0))
  {
    input_state = ST_SPIKE_HIGH;
  }
  else if ((signal_org > -max_peak_threshold) && (signal_org < 0))
  {
    input_state = ST_SPIKE_LOW;
  }

  // check for single high spike sample case
  if (((prev5_input_state[pad_index][input_channel_index] == ST_NOISE) || (prev5_input_state[pad_index][input_channel_index] == ST_SPIKE_LOW)) &&
      (prev4_input_state[pad_index][input_channel_index] == ST_SPIKE_HIGH) &&
      ((prev3_input_state[pad_index][input_channel_index] == ST_NOISE) || (prev3_input_state[pad_index][input_channel_index] == ST_SPIKE_LOW)))
  {
    signal = 0.0f; // remove single spike
  }

  // check for single low spike sample case
  if (((prev5_input_state[pad_index][input_channel_index] == ST_NOISE) || (prev5_input_state[pad_index][input_channel_index] == ST_SPIKE_HIGH)) &&
      (prev4_input_state[pad_index][input_channel_index] == ST_SPIKE_LOW) &&
      ((prev3_input_state[pad_index][input_channel_index] == ST_NOISE) || (prev3_input_state[pad_index][input_channel_index] == ST_SPIKE_HIGH)))
  {
    signal = 0.0f; // remove single spike
  }

  if (level >= 2)
  {
    // check for two sample spike case
    if (((prev5_input_state[pad_index][input_channel_index] == ST_NOISE) || (prev5_input_state[pad_index][input_channel_index] == ST_SPIKE_LOW)) &&
        (prev4_input_state[pad_index][input_channel_index] == ST_SPIKE_HIGH) &&
        (prev3_input_state[pad_index][input_channel_index] == ST_SPIKE_HIGH) &&
        ((prev2_input_state[pad_index][input_channel_index] == ST_NOISE) || (prev2_input_state[pad_index][input_channel_index] == ST_SPIKE_LOW)))
    {
      signal                                      = 0.0f; // remove two sample spike
      prev_input3[pad_index][input_channel_index] = 0.0f; // remove two sample spike
    }

    // check for two sample low spike case
    if (((prev5_input_state[pad_index][input_channel_index] == ST_NOISE) || (prev5_input_state[pad_index][input_channel_index] == ST_SPIKE_HIGH)) &&
        (prev4_input_state[pad_index][input_channel_index] == ST_SPIKE_LOW) &&
        (prev3_input_state[pad_index][input_channel_index] == ST_SPIKE_LOW) &&
        ((prev2_input_state[pad_index][input_channel_index] == ST_NOISE) || (prev2_input_state[pad_index][input_channel_index] == ST_SPIKE_HIGH)))
    {
      signal                                      = 0.0f; // remove two sample spike
      prev_input3[pad_index][input_channel_index] = 0.0f; // remove two sample spike
    }
  }

  if (level >= 3)
  {
    // check for three sample high spike case
    if (((prev5_input_state[pad_index][input_channel_index] == ST_NOISE) || (prev5_input_state[pad_index][input_channel_index] == ST_SPIKE_LOW)) &&
        (prev4_input_state[pad_index][input_channel_index] == ST_SPIKE_HIGH) &&
        (prev3_input_state[pad_index][input_channel_index] == ST_SPIKE_HIGH) &&
        (prev2_input_state[pad_index][input_channel_index] == ST_SPIKE_HIGH) &&
        ((prev1_input_state[pad_index][input_channel_index] == ST_NOISE) || (prev1_input_state[pad_index][input_channel_index] == ST_SPIKE_LOW)))
    {
      signal                                      = 0.0f; // remove three sample spike
      prev_input3[pad_index][input_channel_index] = 0.0f; // remove three sample spike
      prev_input2[pad_index][input_channel_index] = 0.0f; // remove three sample spike
    }

    // check for three sample low spike case
    if (((prev5_input_state[pad_index][input_channel_index] == ST_NOISE) || (prev5_input_state[pad_index][input_channel_index] == ST_SPIKE_HIGH)) &&
        (prev4_input_state[pad_index][input_channel_index] == ST_SPIKE_LOW) &&
        (prev3_input_state[pad_index][input_channel_index] == ST_SPIKE_LOW) &&
        (prev2_input_state[pad_index][input_channel_index] == ST_SPIKE_LOW) &&
        ((prev1_input_state[pad_index][input_channel_index] == ST_NOISE) || (prev1_input_state[pad_index][input_channel_index] == ST_SPIKE_HIGH)))
    {
      signal                                      = 0.0f; // remove three sample spike
      prev_input3[pad_index][input_channel_index] = 0.0f; // remove three sample spike
      prev_input2[pad_index][input_channel_index] = 0.0f; // remove three sample spike
    }
  }

  if (level >= 4)
  {
    // check for four sample high spike case
    if (((prev5_input_state[pad_index][input_channel_index] == ST_NOISE) || (prev5_input_state[pad_index][input_channel_index] == ST_SPIKE_LOW)) &&
        (prev4_input_state[pad_index][input_channel_index] == ST_SPIKE_HIGH) &&
        (prev3_input_state[pad_index][input_channel_index] == ST_SPIKE_HIGH) &&
        (prev2_input_state[pad_index][input_channel_index] == ST_SPIKE_HIGH) &&
        (prev1_input_state[pad_index][input_channel_index] == ST_SPIKE_HIGH) &&
        ((input_state == ST_NOISE) || (input_state == ST_SPIKE_LOW)))
    {
      signal                                      = 0.0f; // remove four sample spike
      prev_input3[pad_index][input_channel_index] = 0.0f; // remove four sample spike
      prev_input2[pad_index][input_channel_index] = 0.0f; // remove four sample spike
      prev_input1[pad_index][input_channel_index] = 0.0f; // remove four sample spike
    }

    // check for four sample low spike case
    if (((prev5_input_state[pad_index][input_channel_index] == ST_NOISE) || (prev5_input_state[pad_index][input_channel_index] == ST_SPIKE_HIGH)) &&
        (prev4_input_state[pad_index][input_channel_index] == ST_SPIKE_LOW) &&
        (prev3_input_state[pad_index][input_channel_index] == ST_SPIKE_LOW) &&
        (prev2_input_state[pad_index][input_channel_index] == ST_SPIKE_LOW) &&
        (prev1_input_state[pad_index][input_channel_index] == ST_SPIKE_LOW) &&
        ((input_state == ST_NOISE) || (input_state == ST_SPIKE_HIGH)))
    {
      signal                                      = 0.0f; // remove four sample spike
      prev_input3[pad_index][input_channel_index] = 0.0f; // remove four sample spike
      prev_input2[pad_index][input_channel_index] = 0.0f; // remove four sample spike
      prev_input1[pad_index][input_channel_index] = 0.0f; // remove four sample spike
    }
  }

  // update five-step input signal memory where we store the last five states of
  // the input signal and four previous untouched input samples
  prev5_input_state[pad_index][input_channel_index] = prev4_input_state[pad_index][input_channel_index];
  prev4_input_state[pad_index][input_channel_index] = prev3_input_state[pad_index][input_channel_index];
  prev3_input_state[pad_index][input_channel_index] = prev2_input_state[pad_index][input_channel_index];
  prev2_input_state[pad_index][input_channel_index] = prev1_input_state[pad_index][input_channel_index];
  prev_input4[pad_index][input_channel_index]       = prev_input3[pad_index][input_channel_index];
  prev_input3[pad_index][input_channel_index]       = prev_input2[pad_index][input_channel_index];
  prev_input2[pad_index][input_channel_index]       = prev_input1[pad_index][input_channel_index];
  prev_overload4[pad_index][input_channel_index]    = prev_overload3[pad_index][input_channel_index];
  prev_overload3[pad_index][input_channel_index]    = prev_overload2[pad_index][input_channel_index];
  prev_overload2[pad_index][input_channel_index]    = prev_overload1[pad_index][input_channel_index];

  // adjust the latency of the algorithm according to the spike cancellation
  // level, i.e., the higher the level, the higher the latency
  if (level >= 3)
  {
    prev1_input_state[pad_index][input_channel_index] = input_state;
    prev_input1[pad_index][input_channel_index]       = signal_org;
    prev_overload1[pad_index][input_channel_index]    = overload_detected_org;
  }
  else if (level >= 2)
  {
    prev2_input_state[pad_index][input_channel_index] = input_state;
    prev_input2[pad_index][input_channel_index]       = signal_org;
    prev_overload2[pad_index][input_channel_index]    = overload_detected_org;
  }
  else
  {
    prev3_input_state[pad_index][input_channel_index] = input_state;
    prev_input3[pad_index][input_channel_index]       = signal_org;
    prev_overload3[pad_index][input_channel_index]    = overload_detected_org;
  }
}
