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

// -----------------------------------------------------------------------------
// ESP32 Specific Functions ----------------------------------------------------
// -----------------------------------------------------------------------------

#pragma once

#include "Arduino.h"
#include "soc/sens_reg.h"
#include "driver/dac.h"

#define MAX_NUM_PADS         12   // a maximum of 12 pads are supported
#define MAX_NUM_PAD_INPUTS   2    // a maximum of 2 sensors per pad is supported
#define ADC_MAX_RANGE        4096 // ESP32 ADC has 12 bits -> 0..4095
#define ADC_MAX_NOISE_AMPL   8    // highest assumed ADC noise amplitude in the ADC input range unit

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

  uint16_t my_analogRead ( uint8_t pin );

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
  void                       init_my_analogRead();

  int         input_pin[MAX_NUM_PADS * MAX_NUM_PAD_INPUTS];
  int         input_adc[MAX_NUM_PADS * MAX_NUM_PAD_INPUTS];

  int         num_pin_pairs;
  int         adc1_pin[MAX_NUM_PADS * MAX_NUM_PAD_INPUTS];
  int         adc2_pin[MAX_NUM_PADS * MAX_NUM_PAD_INPUTS];

  int         num_pin_single;
  int         single_pin[MAX_NUM_PADS * MAX_NUM_PAD_INPUTS];

  Espikestate prev1_input_state[MAX_NUM_PADS][MAX_NUM_PAD_INPUTS];
  Espikestate prev2_input_state[MAX_NUM_PADS][MAX_NUM_PAD_INPUTS];
  Espikestate prev3_input_state[MAX_NUM_PADS][MAX_NUM_PAD_INPUTS];
  float       prev_input1[MAX_NUM_PADS][MAX_NUM_PAD_INPUTS];
  float       prev_input2[MAX_NUM_PADS][MAX_NUM_PAD_INPUTS];
};
