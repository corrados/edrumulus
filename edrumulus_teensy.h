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

#ifdef CORE_TEENSY

#include "Arduino.h"
#include <ADC.h>

#define MAX_NUM_PADS         12   // a maximum of 12 pads are supported
#define MAX_NUM_PAD_INPUTS   2    // a maximum of 2 sensors per pad is supported
#define ADC_MAX_RANGE        4096 // Teensy 4.0 ADC has 12 bits -> 0..4095
#define ADC_MAX_NOISE_AMPL   8    // highest assumed ADC noise amplitude in the ADC input range unit (measured)

class Edrumulus_teensy
{
public:
  Edrumulus_teensy();

  void setup ( const int conf_Fs,
               const int number_pads,
               const int number_inputs[],
               int       analog_pin[][MAX_NUM_PAD_INPUTS] );

  void capture_samples ( const int number_pads,
                         const int number_inputs[],
                         int       analog_pin[][MAX_NUM_PAD_INPUTS],
                         int       sample_org[][MAX_NUM_PAD_INPUTS] );

  float cancel_ADC_spikes ( const float input,
                            const int,
                            const int ) { return input; } // no ADC spike cancellation needed for the Teensy

protected:
  int                        Fs;
//  volatile SemaphoreHandle_t timer_semaphore;
//  hw_timer_t*                timer = nullptr;
//  static void IRAM_ATTR      on_timer();

  int         total_number_inputs;
  int         input_pin[MAX_NUM_PADS * MAX_NUM_PAD_INPUTS];
  uint16_t    input_sample[MAX_NUM_PADS * MAX_NUM_PAD_INPUTS];
};

#endif
