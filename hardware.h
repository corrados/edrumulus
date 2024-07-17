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

#pragma once

#include "Arduino.h"
#include "EEPROM.h"
#include "common.h"

#define MAX_EEPROM_SIZE 512    // bytes (Teensy 4.0: max 1024 bytes)
#define MAX_NUM_SET_PER_PAD 30 // maximum number of settings which can be stored per pad

// -----------------------------------------------------------------------------
// Teensy 4.0/4.1 --------------------------------------------------------------
// -----------------------------------------------------------------------------
#ifdef TEENSYDUINO

#  include <ADC.h>

#  define BOARD_LED_PIN 13     // pin number of the LED on the Teensy 4.0 board
#  define ADC_MAX_RANGE 4096   // Teensy 4.0/4.1 ADC has 12 bits -> 0..4095
#  define ADC_MAX_NOISE_AMPL 8 // highest assumed ADC noise amplitude in the ADC input range unit (measured)

class Edrumulus_hardware
{
 public:
  Edrumulus_hardware();

  static int get_prototype_pins(int** analog_pins,
                                int** analog_pins_rimshot,
                                int*  number_pins,
                                int*  status_LED_pin);

  void setup(const int conf_Fs,
             const int number_pads,
             const int number_inputs[],
             int       analog_pin[][MAX_NUM_PAD_INPUTS]);

  void capture_samples(const int number_pads,
                       const int number_inputs[],
                       int       analog_pin[][MAX_NUM_PAD_INPUTS],
                       int       sample_org[][MAX_NUM_PAD_INPUTS]);

  void write_setting(const int pad_index, const int address, const byte value);
  byte read_setting(const int pad_index, const int address);

 protected:
  int           Fs;
  IntervalTimer myTimer;
  static void   on_timer();
  volatile bool timer_ready;
  ADC           adc_obj;

  int      total_number_inputs;
  int      input_pin[MAX_NUM_PADS * MAX_NUM_PAD_INPUTS];
  uint16_t input_sample[MAX_NUM_PADS * MAX_NUM_PAD_INPUTS];
};

#endif

// -----------------------------------------------------------------------------
// ESP32 Dual Core -------------------------------------------------------------
// -----------------------------------------------------------------------------
#ifdef ESP_PLATFORM

#  include "driver/adc.h"
#  include "soc/sens_reg.h"
#  ifdef CONFIG_IDF_TARGET_ESP32
#    include "driver/dac.h"
#  else // CONFIG_IDF_TARGET_ESP32S3
#    include "hal/adc_hal.h"
#  endif

#  define BOARD_LED_PIN 2      // pin number of the LED on the ESP32 board
#  define ADC_MAX_RANGE 4096   // ESP32 ADC has 12 bits -> 0..4095
#  define ADC_MAX_NOISE_AMPL 8 // highest assumed ADC noise amplitude in the ADC input range unit (measured)

class Edrumulus_hardware
{
 public:
  Edrumulus_hardware();

  static int get_prototype_pins(int** analog_pins,
                                int** analog_pins_rimshot,
                                int*  number_pins,
                                int*  status_LED_pin);

  void setup(const int conf_Fs,
             const int number_pads,
             const int number_inputs[],
             int       analog_pin[][MAX_NUM_PAD_INPUTS]);

  void capture_samples(const int number_pads,
                       const int number_inputs[],
                       int       analog_pin[][MAX_NUM_PAD_INPUTS],
                       int       sample_org[][MAX_NUM_PAD_INPUTS]);

  void write_setting(const int, const int, const byte){}; // not supported
  byte read_setting(const int, const int) { return 0; };  // not supported

 protected:
  int                        Fs;
  EEPROMClass                eeprom_settings;
  volatile SemaphoreHandle_t timer_semaphore;
  hw_timer_t*                timer = nullptr;
  static void IRAM_ATTR      on_timer();
  static void                start_timer_core0_task(void* param);

  void     setup_timer();
  void     init_my_analogRead();
  uint16_t my_analogRead(const uint8_t pin);
  void     my_analogRead_parallel(const uint32_t channel_adc1_bitval,
                                  const uint32_t channel_adc2_bitval,
                                  uint16_t&      out_adc1,
                                  uint16_t&      out_adc2);

  int      total_number_inputs;
  int      input_pin[MAX_NUM_PADS * MAX_NUM_PAD_INPUTS];
  uint16_t input_sample[MAX_NUM_PADS * MAX_NUM_PAD_INPUTS];

  int      num_pin_pairs;
  int      adc1_index[MAX_NUM_PADS * MAX_NUM_PAD_INPUTS];
  int      adc2_index[MAX_NUM_PADS * MAX_NUM_PAD_INPUTS];
  uint32_t channel_adc1_bitval[MAX_NUM_PADS * MAX_NUM_PAD_INPUTS];
  uint32_t channel_adc2_bitval[MAX_NUM_PADS * MAX_NUM_PAD_INPUTS];

  int num_pin_single;
  int single_index[MAX_NUM_PADS * MAX_NUM_PAD_INPUTS];
};

#endif
