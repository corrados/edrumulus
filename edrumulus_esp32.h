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

#pragma once

#include "Arduino.h"
#include "soc/sens_reg.h"
#include "driver/dac.h"

#define MAX_NUM_PADS         12   // a maximum of 12 pads are supported
#define MAX_NUM_PAD_INPUTS   2    // a maximum of 2 sensors per pad is supported
#define ADC_MAX_RANGE        4096 // ESP32 ADC has 12 bits -> 0..4095
#define ADC_MAX_NOISE_AMPL   10   // highest assumed ADC noise amplitude in the ADC input range unit (measured)

class Edrumulus_esp32
{
public:
  enum Espikestate
  {
    ST_NOISE,
    ST_SPIKE,
    ST_OTHER
  };

  Edrumulus_esp32();

  void setup ( const int conf_Fs,
               const int number_pads,
               const int number_inputs[],
               int       analog_pin[][MAX_NUM_PAD_INPUTS] );

  void capture_samples ( const int number_pads,
                         const int number_inputs[],
                         int       analog_pin[][MAX_NUM_PAD_INPUTS],
                         int       sample_org[][MAX_NUM_PAD_INPUTS] );

  float cancel_ADC_spikes ( const float input,
                            const int   pad_index,
                            const int   input_channel_index );

protected:
  int                        Fs;
  volatile SemaphoreHandle_t timer_semaphore;
  hw_timer_t*                timer = nullptr;
  static void IRAM_ATTR      on_timer();
  static void                start_timer_core0_task ( void* param );

  void     init_my_analogRead();
  uint16_t my_analogRead ( const uint8_t pin );
  void     my_analogRead_parallel ( const uint8_t pin_adc1,
                                    const uint8_t pin_adc2,
                                    uint16_t&     out_adc1,
                                    uint16_t&     out_adc2 );

  int         total_number_inputs;
  int         input_pin[MAX_NUM_PADS * MAX_NUM_PAD_INPUTS];
  uint16_t    input_sample[MAX_NUM_PADS * MAX_NUM_PAD_INPUTS];

  int         num_pin_pairs;
  int         adc1_index[MAX_NUM_PADS * MAX_NUM_PAD_INPUTS];
  int         adc2_index[MAX_NUM_PADS * MAX_NUM_PAD_INPUTS];

  int         num_pin_single;
  int         single_index[MAX_NUM_PADS * MAX_NUM_PAD_INPUTS];

  Espikestate prev1_input_state[MAX_NUM_PADS][MAX_NUM_PAD_INPUTS];
  Espikestate prev2_input_state[MAX_NUM_PADS][MAX_NUM_PAD_INPUTS];
  Espikestate prev3_input_state[MAX_NUM_PADS][MAX_NUM_PAD_INPUTS];
  float       prev_input1[MAX_NUM_PADS][MAX_NUM_PAD_INPUTS];
  float       prev_input2[MAX_NUM_PADS][MAX_NUM_PAD_INPUTS];
};
