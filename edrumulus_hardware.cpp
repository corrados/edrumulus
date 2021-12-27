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

#include "edrumulus_hardware.h"

Edrumulus_hardware* edrumulus_hardware_pointer = nullptr;

Edrumulus_hardware::Edrumulus_hardware()
{
  // global pointer to this class needed for static callback function
  edrumulus_hardware_pointer = this;
}


float Edrumulus_hardware::cancel_ADC_spikes ( const float input,
                                              const int   pad_index,
                                              const int   input_channel_index,
                                              const int   level )
{
  // remove single/dual sample spikes by checking if right before and right after the
  // detected spike(s) we only have noise and no useful signal (since the ESP32 spikes
  // mostly are on just one or two sample(s))
  const int max_peak_threshold = 150; // maximum assumed ESP32 spike amplitude

  float       return_value = prev_input4[pad_index][input_channel_index]; // normal return value in case no spike was detected
  const float input_abs    = abs ( input );
  Espikestate input_state  = ST_OTHER; // initialization value, might be overwritten

  if ( input_abs < ADC_MAX_NOISE_AMPL )
  {
    input_state = ST_NOISE;
  }
  else if ( ( input < max_peak_threshold ) && ( input > 0 ) )
  {
    input_state = ST_SPIKE_HIGH;
  }
  else if ( ( input > -max_peak_threshold ) && ( input < 0 ) )
  {
    input_state = ST_SPIKE_LOW;
  }

  // check for single high spike sample case
  if ( ( ( prev5_input_state[pad_index][input_channel_index] == ST_NOISE ) || ( prev5_input_state[pad_index][input_channel_index] == ST_SPIKE_LOW ) ) &&
       ( prev4_input_state[pad_index][input_channel_index] == ST_SPIKE_HIGH ) &&
       ( ( prev3_input_state[pad_index][input_channel_index] == ST_NOISE ) || ( prev3_input_state[pad_index][input_channel_index] == ST_SPIKE_LOW ) ) )
  {
    return_value = 0.0f; // remove single spike
  }

  // check for single low spike sample case
  if ( ( ( prev5_input_state[pad_index][input_channel_index] == ST_NOISE ) || ( prev5_input_state[pad_index][input_channel_index] == ST_SPIKE_HIGH ) ) &&
       ( prev4_input_state[pad_index][input_channel_index] == ST_SPIKE_LOW ) &&
       ( ( prev3_input_state[pad_index][input_channel_index] == ST_NOISE ) || ( prev3_input_state[pad_index][input_channel_index] == ST_SPIKE_HIGH ) ) )
  {
    return_value = 0.0f; // remove single spike
  }

  if ( level >= 2 )
  {
    // check for two sample spike case
    if ( ( ( prev5_input_state[pad_index][input_channel_index] == ST_NOISE ) || ( prev5_input_state[pad_index][input_channel_index] == ST_SPIKE_LOW ) ) &&
         ( prev4_input_state[pad_index][input_channel_index] == ST_SPIKE_HIGH ) &&
         ( prev3_input_state[pad_index][input_channel_index] == ST_SPIKE_HIGH ) &&
         ( ( prev2_input_state[pad_index][input_channel_index] == ST_NOISE ) || ( prev2_input_state[pad_index][input_channel_index] == ST_SPIKE_LOW ) ) )
    {
      return_value                                = 0.0f; // remove two sample spike
      prev_input3[pad_index][input_channel_index] = 0.0f; // remove two sample spike
    }
  
    // check for two sample low spike case
    if ( ( ( prev5_input_state[pad_index][input_channel_index] == ST_NOISE ) || ( prev5_input_state[pad_index][input_channel_index] == ST_SPIKE_HIGH ) ) &&
         ( prev4_input_state[pad_index][input_channel_index] == ST_SPIKE_LOW ) &&
         ( prev3_input_state[pad_index][input_channel_index] == ST_SPIKE_LOW ) &&
         ( ( prev2_input_state[pad_index][input_channel_index] == ST_NOISE ) || ( prev2_input_state[pad_index][input_channel_index] == ST_SPIKE_HIGH ) ) )
    {
      return_value                                = 0.0f; // remove two sample spike
      prev_input3[pad_index][input_channel_index] = 0.0f; // remove two sample spike
    }
  }

  if ( level >= 3 )
  {
    // check for three sample high spike case
    if ( ( ( prev5_input_state[pad_index][input_channel_index] == ST_NOISE ) || ( prev5_input_state[pad_index][input_channel_index] == ST_SPIKE_LOW ) ) &&
         ( prev4_input_state[pad_index][input_channel_index] == ST_SPIKE_HIGH ) &&
         ( prev3_input_state[pad_index][input_channel_index] == ST_SPIKE_HIGH ) &&
         ( prev2_input_state[pad_index][input_channel_index] == ST_SPIKE_HIGH ) &&
         ( ( prev1_input_state[pad_index][input_channel_index] == ST_NOISE ) || ( prev1_input_state[pad_index][input_channel_index] == ST_SPIKE_LOW ) ) )
    {
      return_value                                = 0.0f; // remove three sample spike
      prev_input3[pad_index][input_channel_index] = 0.0f; // remove three sample spike
      prev_input2[pad_index][input_channel_index] = 0.0f; // remove three sample spike
    }
  
    // check for three sample low spike case
    if ( ( ( prev5_input_state[pad_index][input_channel_index] == ST_NOISE ) || ( prev5_input_state[pad_index][input_channel_index] == ST_SPIKE_HIGH ) ) &&
         ( prev4_input_state[pad_index][input_channel_index] == ST_SPIKE_LOW ) &&
         ( prev3_input_state[pad_index][input_channel_index] == ST_SPIKE_LOW ) &&
         ( prev2_input_state[pad_index][input_channel_index] == ST_SPIKE_LOW ) &&
         ( ( prev1_input_state[pad_index][input_channel_index] == ST_NOISE ) || ( prev1_input_state[pad_index][input_channel_index] == ST_SPIKE_HIGH ) ) )
    {
      return_value                                = 0.0f; // remove three sample spike
      prev_input3[pad_index][input_channel_index] = 0.0f; // remove three sample spike
      prev_input2[pad_index][input_channel_index] = 0.0f; // remove three sample spike
    }
  }

  if ( level >= 4 )
  {
    // check for four sample high spike case
    if ( ( ( prev5_input_state[pad_index][input_channel_index] == ST_NOISE ) || ( prev5_input_state[pad_index][input_channel_index] == ST_SPIKE_LOW ) ) &&
         ( prev4_input_state[pad_index][input_channel_index] == ST_SPIKE_HIGH ) &&
         ( prev3_input_state[pad_index][input_channel_index] == ST_SPIKE_HIGH ) &&
         ( prev2_input_state[pad_index][input_channel_index] == ST_SPIKE_HIGH ) &&
         ( prev1_input_state[pad_index][input_channel_index] == ST_SPIKE_HIGH ) &&
         ( ( input_state == ST_NOISE ) || ( input_state == ST_SPIKE_LOW ) ) )
    {
      return_value                                = 0.0f; // remove four sample spike
      prev_input3[pad_index][input_channel_index] = 0.0f; // remove four sample spike
      prev_input2[pad_index][input_channel_index] = 0.0f; // remove four sample spike
      prev_input1[pad_index][input_channel_index] = 0.0f; // remove four sample spike
    }
  
    // check for four sample low spike case
    if ( ( ( prev5_input_state[pad_index][input_channel_index] == ST_NOISE ) || ( prev5_input_state[pad_index][input_channel_index] == ST_SPIKE_HIGH ) ) &&
         ( prev4_input_state[pad_index][input_channel_index] == ST_SPIKE_LOW ) &&
         ( prev3_input_state[pad_index][input_channel_index] == ST_SPIKE_LOW ) &&
         ( prev2_input_state[pad_index][input_channel_index] == ST_SPIKE_LOW ) &&
         ( prev1_input_state[pad_index][input_channel_index] == ST_SPIKE_LOW ) &&
         ( ( input_state == ST_NOISE ) || ( input_state == ST_SPIKE_HIGH ) ) )
    {
      return_value                                = 0.0f; // remove four sample spike
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

  // adjust the latency of the algorithm according to the spike cancellation
  // level, i.e., the higher the level, the higher the latency
  if ( level >= 3 )
  {
    prev1_input_state[pad_index][input_channel_index] = input_state;
    prev_input1[pad_index][input_channel_index]       = input;
  }
  else if ( level >= 2 )
  {
    prev2_input_state[pad_index][input_channel_index] = input_state;
    prev_input2[pad_index][input_channel_index]       = input;
  }
  else
  {
    prev3_input_state[pad_index][input_channel_index] = input_state;
    prev_input3[pad_index][input_channel_index]       = input;
  }

  return return_value;
}
