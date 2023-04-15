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

#include "edrumulus_hardware.h"

Edrumulus_hardware* edrumulus_hardware_pointer = nullptr;

Edrumulus_hardware::Edrumulus_hardware()
{
  // global pointer to this class needed for static callback function
  edrumulus_hardware_pointer = this;
}


// -----------------------------------------------------------------------------
// Teensy 4.0/4.1/3.6 ----------------------------------------------------------
// -----------------------------------------------------------------------------
#ifdef TEENSYDUINO

int Edrumulus_hardware::get_prototype_pins ( int** analog_pins,
                                             int** analog_pins_rimshot,
                                             int*  number_pins,
                                             int*  status_LED_pin )
{

  // analog pins setup:               snare | kick | hi-hat | hi-hat-ctrl | crash | tom1 | ride | tom2 | tom3
  static int analog_pins1[]         = { 10,    11,    12,        13,          1,      6,     4,     5 };
  static int analog_pins_rimshot1[] = {  9,    -1,     0,        -1,          3,      8,     2,     7 };
  *analog_pins         = analog_pins1;
  *analog_pins_rimshot = analog_pins_rimshot1;
  *number_pins         = sizeof ( analog_pins1 ) / sizeof ( int );
  *status_LED_pin      = BOARD_LED_PIN;
  return 0;
}


void Edrumulus_hardware::setup ( const int conf_Fs,
                                 const int number_pads,
                                 const int number_inputs[],
                                 int       analog_pin[][MAX_NUM_PAD_INPUTS] )
{
  // set essential parameters
  Fs = conf_Fs;

  // create linear vectors containing the pin information for each pad and pad-input
  total_number_inputs = 0; // we use it as a counter, too

  for ( int i = 0; i < number_pads; i++ )
  {
    for ( int j = 0; j < number_inputs[i]; j++ )
    {
      // store pin number in vector
      input_pin[total_number_inputs] = analog_pin[i][j];
      total_number_inputs++;
    }
  }

#ifdef ARDUINO_TEENSY36 // Teensy 3.6 specific code
  // A word about conversion time:
  //  conv_time = SFCAdder + Averages * (BCT + LSTAdder + HSCAdder)
  //            = 5 ADCK + 5 bus + 1 *(25 ADCK + 0 ADCK + 2 ADCK) = 32 ADCK + 5 bus
  //            = 13/7.5 MHz + 5/60 MHz = 4.35 us per conversion
  // For 22 conversions, 22 * 4.35 us = 95.7 us which is still under 125 us.
  // F_BUS is 60 MHz and F_ADC is max allowed 24 MHz in 12-bit mode and 12 MHz in
  // 16-bit mode to stay within specs. F_ADC is derived from F_BUS with dividers
  // 1, 2, 4, 8, or 16. F_BUS of 60 MHz offers 30, 15, and 7.5 MHz options.
  // For 12-bit mode, 15 and 7.5 can work; for 16-bit mode, only 7.5 can work.
  // The definition for F_BUS can be found in kinetics.h and it's a function of
  // the F_CPU. For F_CPU = 180 MHz, F_BUS = 60 MHz. For F_CPU = 192 MHz, F_BUS = 48 MHz.
  // In other words, overclocking the teensy will affect the conversion rate.
  adc_obj.adc0->setResolution      ( 16 ); // we want to get the full ADC resolution of the Teensy 3.6
  adc_obj.adc0->setAveraging       ( 1 );
  adc_obj.adc0->setConversionSpeed ( ADC_CONVERSION_SPEED::HIGH_SPEED_16BITS );
  adc_obj.adc0->setSamplingSpeed   ( ADC_SAMPLING_SPEED::VERY_HIGH_SPEED );
  adc_obj.adc1->setResolution      ( 16 ); // we want to get the full ADC resolution of the Teensy 3.6
  adc_obj.adc1->setAveraging       ( 1 );
  adc_obj.adc1->setConversionSpeed ( ADC_CONVERSION_SPEED::HIGH_SPEED_16BITS );
  adc_obj.adc1->setSamplingSpeed   ( ADC_SAMPLING_SPEED::VERY_HIGH_SPEED );
#else
  // set the ADC properties: averaging 8 samples with high speed sampling gives
  // us the best compromise between ADC speed and spike protection
  adc_obj.adc0->setResolution      ( 12 ); // we want to get the full ADC resolution of the Teensy 4.0
  adc_obj.adc0->setAveraging       ( 8 );
  adc_obj.adc0->setConversionSpeed ( ADC_CONVERSION_SPEED::HIGH_SPEED );
  adc_obj.adc0->setSamplingSpeed   ( ADC_SAMPLING_SPEED::HIGH_SPEED );
  adc_obj.adc1->setResolution      ( 12 ); // we want to get the full ADC resolution of the Teensy 4.0
  adc_obj.adc1->setAveraging       ( 8 );
  adc_obj.adc1->setConversionSpeed ( ADC_CONVERSION_SPEED::HIGH_SPEED );
  adc_obj.adc1->setSamplingSpeed   ( ADC_SAMPLING_SPEED::HIGH_SPEED );
#endif

#if defined(ARDUINO_TEENSY40) || defined(ARDUINO_TEENSY41) // Teensy 4.0/4.1 specific code
  // disable MIMXRT1062DVL6A "keeper" on all possible Teensy 4.0/4.1 ADC input pins
  IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_B1_02 &= ~( 1 << 12 ); // A0
  IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_B1_03 &= ~( 1 << 12 ); // A1
  IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_B1_07 &= ~( 1 << 12 ); // A2
  IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_B1_06 &= ~( 1 << 12 ); // A3
  IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_B1_01 &= ~( 1 << 12 ); // A4
  IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_B1_00 &= ~( 1 << 12 ); // A5
  IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_B1_10 &= ~( 1 << 12 ); // A6
  IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_B1_11 &= ~( 1 << 12 ); // A7
  IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_B1_08 &= ~( 1 << 12 ); // A8
  IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_B1_09 &= ~( 1 << 12 ); // A9
  IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_B0_12 &= ~( 1 << 12 ); // A10
  IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_B0_13 &= ~( 1 << 12 ); // A11
  IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_B1_14 &= ~( 1 << 12 ); // A12
  IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_B1_15 &= ~( 1 << 12 ); // A13
#endif

  // initialize timer flag (semaphore)
  timer_ready = false;

  // prepare timer at a rate of given sampling rate
  myTimer.begin ( on_timer, 1000000 / Fs ); // here we define the sampling rate (1 MHz / Fs)
}


void Edrumulus_hardware::write_setting ( const int  pad_index,
                                         const int  address,
                                         const byte value )
{
  EEPROM.update ( pad_index * MAX_NUM_SET_PER_PAD + address, value );
}


byte Edrumulus_hardware::read_setting ( const int pad_index,
                                        const int address )
{
  return EEPROM.read ( pad_index * MAX_NUM_SET_PER_PAD + address );
}


void Edrumulus_hardware::on_timer()
{
  // tell the main loop that a sample can be read by setting the flag (semaphore)
  edrumulus_hardware_pointer->timer_ready = true;
}


void Edrumulus_hardware::capture_samples ( const int number_pads,
                                           const int number_inputs[],
                                           int       analog_pin[][MAX_NUM_PAD_INPUTS],
                                           int       sample_org[][MAX_NUM_PAD_INPUTS] )
{
  // wait for the timer to get the correct sampling rate when reading the analog value
  while ( !timer_ready ) delayMicroseconds ( 5 );
  timer_ready = false; // it is important to reset the flag here

  // read the ADC samples
  for ( int i = 0; i < total_number_inputs; i++ )
  {
    // pins 12 and 13 (and 22 for Teensy 3.6) are ADC1 only, pins 10 and 11 are ADC0 only
    // note that pin 8 gave large spikes on ADC0 but seems to work ok with ADC1
    if ( ( input_pin[i] == 8 ) || ( input_pin[i] == 12 ) || ( input_pin[i] == 13 ) || ( input_pin[i] == 22 ) )
    {
      input_sample[i] = adc_obj.adc1->analogRead ( input_pin[i] );
    }
    else
    {
      input_sample[i] = adc_obj.adc0->analogRead ( input_pin[i] );
    }
  }

  // copy captured samples in pad buffer
  int input_cnt = 0;

  for ( int i = 0; i < number_pads; i++ )
  {
    for ( int j = 0; j < number_inputs[i]; j++ )
    {
      sample_org[i][j] = input_sample[input_cnt++];
    }
  }
}

#endif


// -----------------------------------------------------------------------------
// ESP32 Dual Core -------------------------------------------------------------
// -----------------------------------------------------------------------------
#ifdef ESP_PLATFORM

int Edrumulus_hardware::get_prototype_pins ( int** analog_pins,
                                             int** analog_pins_rimshot,
                                             int*  number_pins,
                                             int*  status_LED_pin )
{
#ifdef CONFIG_IDF_TARGET_ESP32
  // Definition:
  // - Pin 5 is "input enabled, pull-up resistor" -> if read value is 1, we know that we have a
  //   legacy or custom board. Boards which support the identification set this pin to low.
  // - Pin 18, 19, 22, 23 define a 4 bit sequence which identifies the prototype hardware.
  // NOTE: avoid ESP32 GPIO 25/26 for piezo inputs since they are DAC pins which cause an incorrect DC offset
  //       estimation and DC offset drift which makes the spike cancellation algorithm not working correctly
  pinMode ( 5, INPUT );

  // check support of protoype board identification
  if ( digitalRead ( 5 ) == 0 )
  {
    // read the identification bit field and check the states
    pinMode ( 18, INPUT ); const int bit1 = digitalRead ( 18 );
    pinMode ( 19, INPUT ); const int bit2 = digitalRead ( 19 );
    pinMode ( 22, INPUT ); const int bit3 = digitalRead ( 22 );
    pinMode ( 23, INPUT ); const int bit4 = digitalRead ( 23 );

    if ( ( bit1 == 0 ) && ( bit2 == 0 ) && ( bit3 == 0 ) && ( bit4 == 0 ) )
    {
      // Prototype 5: 0, 0, 0, 0 -----------------------------------------------
      // analog pins setup:               snare | kick | hi-hat | hi-hat-ctrl | crash | tom1 | ride | tom2 | tom3      
      static int analog_pins5[]         = { 12,     2,     33,        4,         34,     15,    35,    27,    32 };
      static int analog_pins_rimshot5[] = { 14,    -1,     26,       -1,         36,     13,    25,    -1,    -1 };
      *analog_pins         = analog_pins5;
      *analog_pins_rimshot = analog_pins_rimshot5;
      *number_pins         = sizeof ( analog_pins5 ) / sizeof ( int );
      *status_LED_pin      = 21; // LED is connected to IO21 on prototype 5
      return 5;
    }
    else if ( ( bit1 > 0 ) && ( bit2 == 0 ) && ( bit3 == 0 ) && ( bit4 == 0 ) )
    {
      // Prototype 6: 1, 0, 0, 0 -----------------------------------------------
      // analog pins setup:               snare | kick | hi-hat | hi-hat-ctrl | crash | tom1 | ride | tom2 | tom3      
      static int analog_pins6[]         = { 36,    33,     32,       25,         34,     39,    27,    12,    15 };
      static int analog_pins_rimshot6[] = { 35,    -1,     26,       -1,         14,     -1,    13,    -1,    -1 };
      *analog_pins         = analog_pins6;
      *analog_pins_rimshot = analog_pins_rimshot6;
      *number_pins         = sizeof ( analog_pins6 ) / sizeof ( int );
      *status_LED_pin      = BOARD_LED_PIN;
      return 6;
    }
  }

  // if no GPIO prototype identification is available, we assume it is Prototype 4
  // analog pins setup:               snare | kick | hi-hat | hi-hat-ctrl | crash | tom1 | ride | tom2 | tom3  
  static int analog_pins4[]         = { 36,    33,     32,       25,         34,     39,    27,    12,    15 };
  static int analog_pins_rimshot4[] = { 35,    -1,     26,       -1,         14,     -1,    13,    -1,    -1 };
  *analog_pins         = analog_pins4;
  *analog_pins_rimshot = analog_pins_rimshot4;
  *number_pins         = sizeof ( analog_pins4 ) / sizeof ( int );
  *status_LED_pin      = BOARD_LED_PIN;
  return 4;
#else // CONFIG_IDF_TARGET_ESP32S3
  // ESP32-S3 testing...
  // analog pins setup:                 snare | kick | hi-hat | hi-hat-ctrl | crash | tom1 | ride | tom2 | tom3  
  static int analog_pins_s3[]         = {  4,     6,      7};//,        9,         10,     12,    13,    15,    16 };
  static int analog_pins_rimshot_s3[] = {  5,    -1,      8};//,       -1,         11,     -1,    14,    -1,    -1 };
  *analog_pins         = analog_pins_s3;
  *analog_pins_rimshot = analog_pins_rimshot_s3;
  *number_pins         = sizeof ( analog_pins_s3 ) / sizeof ( int );
  *status_LED_pin      = BOARD_LED_PIN;
  return 4;
#endif
}


void Edrumulus_hardware::setup ( const int conf_Fs,
                                 const int number_pads,
                                 const int number_inputs[],
                                 int       analog_pin[][MAX_NUM_PAD_INPUTS] )
{
  // set essential parameters
  Fs = conf_Fs;
  eeprom_settings.begin ( ( number_pads + 1 ) * MAX_NUM_SET_PER_PAD ); // "+ 1" for pad-independent global settings

  // create linear vectors containing the pin/ADC information for each pad and pad-input
  bool input_is_used[MAX_NUM_PADS * MAX_NUM_PAD_INPUTS];
  int  input_adc[MAX_NUM_PADS * MAX_NUM_PAD_INPUTS];
  total_number_inputs = 0; // we use it as a counter, too

  for ( int i = 0; i < number_pads; i++ )
  {
    for ( int j = 0; j < number_inputs[i]; j++ )
    {
      // store pin number in vector and identify ADC number for each pin
      input_pin[total_number_inputs]     = analog_pin[i][j];
      input_adc[total_number_inputs]     = ( digitalPinToAnalogChannel ( analog_pin[i][j] ) >= 10 ); // channel < 10 -> ADC1, channel >= 10 -> ADC2
      input_is_used[total_number_inputs] = false; // initialization needed for ADC pairs identification
      total_number_inputs++;
    }
  }

  // find ADC pairs, i.e., one pin uses ADC1 and the other uses ADC2
  num_pin_pairs = 0; // we use it as a counter, too

#ifdef CONFIG_IDF_TARGET_ESP32
  for ( int i = 0; i < total_number_inputs - 1; i++ )
  {
    if ( !input_is_used[i] )
    {
      for ( int j = total_number_inputs - 1; j > i; j-- )
      {
        // check for different ADCs: 0+0=0, 1+0=1 (pair), 0+1=1 (pair), 1+1=2
        if ( !input_is_used[j] && ( input_adc[i] + input_adc[j] == 1 ) )
        {
          if ( input_adc[i] == 0 )
          {
            adc1_index[num_pin_pairs] = i;
            adc2_index[num_pin_pairs] = j;
          }
          else
          {
            adc1_index[num_pin_pairs] = j;
            adc2_index[num_pin_pairs] = i;
          }

          // create the mask bit needed for SENS_SAR1_EN_PAD and SENS_SAR2_EN_PAD
          const int8_t channel_adc1          = digitalPinToAnalogChannel ( input_pin[adc1_index[num_pin_pairs]] );
          const int8_t channel_adc2          = digitalPinToAnalogChannel ( input_pin[adc2_index[num_pin_pairs]] ) - 10;
          channel_adc1_bitval[num_pin_pairs] = ( 1 << channel_adc1 );
          channel_adc2_bitval[num_pin_pairs] = ( 1 << channel_adc2 );

          num_pin_pairs++;
          input_is_used[i] = true;
          input_is_used[j] = true;
          break;
        }
      }
    }
  }
#endif

  // find remaining single pins which we cannot create an ADC pair with
  num_pin_single = 0; // we use it as a counter, too

  for ( int i = 0; i < total_number_inputs; i++ )
  {
    if ( !input_is_used[i] )
    {
      single_index[num_pin_single] = i;
      num_pin_single++;
    }
  }

  // prepare the ADC and analog GPIO inputs
  init_my_analogRead();

  // create timer semaphore
  timer_semaphore = xSemaphoreCreateBinary();

  // create task pinned to core 0 for creating the timer interrupt so that the
  // timer function is not running in our working core 1
#ifdef CONFIG_IDF_TARGET_ESP32
  xTaskCreatePinnedToCore ( start_timer_core0_task, "start_timer_core0_task", 800, this, 1, NULL, 0 );
#else // CONFIG_IDF_TARGET_ESP32S3
  xTaskCreatePinnedToCore ( start_timer_core0_task, "start_timer_core0_task", 1000, this, 1, NULL, 0 );
#endif
}


void Edrumulus_hardware::setup_timer()
{
  // prepare timer at a rate of given sampling rate
  timer = timerBegin ( 0, 80, true ); // prescaler of 80 (i.e. below we have 1 MHz instead of 80 MHz)
  timerAttachInterrupt ( timer, &on_timer, true );
  timerAlarmWrite      ( timer, 1000000 / Fs, true ); // here we define the sampling rate (1 MHz / Fs)
  timerAlarmEnable     ( timer );
}


void Edrumulus_hardware::start_timer_core0_task ( void* param )
{
  reinterpret_cast<Edrumulus_hardware*> ( param )->setup_timer();

  // tasks must not return: forever loop with delay to keep watchdog happy
  for ( ; ; )
  {
    delay ( 1000 );
  }
}


void IRAM_ATTR Edrumulus_hardware::on_timer()
{
  // first read the ADC pairs samples
  for ( int i = 0; i < edrumulus_hardware_pointer->num_pin_pairs; i++ )
  {
    edrumulus_hardware_pointer->my_analogRead_parallel (
      edrumulus_hardware_pointer->channel_adc1_bitval[i],
      edrumulus_hardware_pointer->channel_adc2_bitval[i],
      edrumulus_hardware_pointer->input_sample[edrumulus_hardware_pointer->adc1_index[i]],
      edrumulus_hardware_pointer->input_sample[edrumulus_hardware_pointer->adc2_index[i]] );
  }

  // second read the single ADC samples
  for ( int i = 0; i < edrumulus_hardware_pointer->num_pin_single; i++ )
  {
    edrumulus_hardware_pointer->input_sample[edrumulus_hardware_pointer->single_index[i]] =
      edrumulus_hardware_pointer->my_analogRead ( edrumulus_hardware_pointer->input_pin[edrumulus_hardware_pointer->single_index[i]] );
  }

  // tell the main loop that a sample can be processed by setting the semaphore
  static BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  xSemaphoreGiveFromISR ( edrumulus_hardware_pointer->timer_semaphore, &xHigherPriorityTaskWoken );

  if ( xHigherPriorityTaskWoken == pdTRUE )
  {
    portYIELD_FROM_ISR();
  }
}


void Edrumulus_hardware::capture_samples ( const int number_pads,
                                           const int number_inputs[],
                                           int       analog_pin[][MAX_NUM_PAD_INPUTS],
                                           int       sample_org[][MAX_NUM_PAD_INPUTS] )
{
  // wait for the timer to get the correct sampling rate when reading the analog value
  if ( xSemaphoreTake ( timer_semaphore, portMAX_DELAY ) == pdTRUE )
  {
    // copy captured samples in pad buffer
    int input_cnt = 0;

    for ( int i = 0; i < number_pads; i++ )
    {
      for ( int j = 0; j < number_inputs[i]; j++ )
      {
        sample_org[i][j] = input_sample[input_cnt++];
      }
    }
  }
}


// Since arduino-esp32 library version 1.0.5, the analogRead was changed to use the IDF interface
// which made the analogRead function so slow that we cannot use that anymore for Edrumulus:
// https://github.com/espressif/arduino-esp32/issues/4973, https://github.com/espressif/arduino-esp32/pull/3377
// As a workaround, we had to write our own analogRead function.
void Edrumulus_hardware::init_my_analogRead()
{
#ifdef CONFIG_IDF_TARGET_ESP32
  // if the GIOP 25/26 are used, we have to set the DAC to 0 to get correct DC offset
  // estimates and reduce the number of large spikes
  dac_i2s_enable();
  dac_output_enable  ( DAC_CHANNEL_1 );
  dac_output_voltage ( DAC_CHANNEL_1, 0 );
  dac_output_disable ( DAC_CHANNEL_1 );
  dac_output_enable  ( DAC_CHANNEL_2 );
  dac_output_voltage ( DAC_CHANNEL_2, 0 );
  dac_output_disable ( DAC_CHANNEL_2 );
  dac_i2s_disable();
#endif

  // set attenuation of 11 dB
  WRITE_PERI_REG ( SENS_SAR_ATTEN1_REG, 0xFFFFFFFFF );
  WRITE_PERI_REG ( SENS_SAR_ATTEN2_REG, 0xFFFFFFFFF );

  // set both ADCs to 12 bit resolution using 8 cycles and 1 sample
#ifdef CONFIG_IDF_TARGET_ESP32
  SET_PERI_REG_BITS ( SENS_SAR_READ_CTRL_REG,   SENS_SAR1_SAMPLE_CYCLE, 8, SENS_SAR1_SAMPLE_CYCLE_S ); // cycles
  SET_PERI_REG_BITS ( SENS_SAR_READ_CTRL2_REG,  SENS_SAR2_SAMPLE_CYCLE, 8, SENS_SAR2_SAMPLE_CYCLE_S );
  SET_PERI_REG_BITS ( SENS_SAR_READ_CTRL_REG,   SENS_SAR1_SAMPLE_NUM,   0, SENS_SAR1_SAMPLE_NUM_S ); // # samples
  SET_PERI_REG_BITS ( SENS_SAR_READ_CTRL2_REG,  SENS_SAR2_SAMPLE_NUM,   0, SENS_SAR2_SAMPLE_NUM_S );
  SET_PERI_REG_BITS ( SENS_SAR_READ_CTRL_REG,   SENS_SAR1_CLK_DIV,      1, SENS_SAR1_CLK_DIV_S ); // clock div
  SET_PERI_REG_BITS ( SENS_SAR_READ_CTRL2_REG,  SENS_SAR2_CLK_DIV,      1, SENS_SAR2_CLK_DIV_S );
  SET_PERI_REG_BITS ( SENS_SAR_START_FORCE_REG, SENS_SAR1_BIT_WIDTH,    3, SENS_SAR1_BIT_WIDTH_S ); // width
  SET_PERI_REG_BITS ( SENS_SAR_READ_CTRL_REG,   SENS_SAR1_SAMPLE_BIT,   3, SENS_SAR1_SAMPLE_BIT_S );
  SET_PERI_REG_BITS ( SENS_SAR_START_FORCE_REG, SENS_SAR2_BIT_WIDTH,    3, SENS_SAR2_BIT_WIDTH_S );
  SET_PERI_REG_BITS ( SENS_SAR_READ_CTRL2_REG,  SENS_SAR2_SAMPLE_BIT,   3, SENS_SAR2_SAMPLE_BIT_S );
#else // CONFIG_IDF_TARGET_ESP32S3
  adc1_config_width ( ADC_WIDTH_BIT_12 ); // ADC2 bit width is configured when started
  adc_ll_set_controller ( ADC_NUM_1, ADC_LL_CTRL_RTC );
  adc_ll_set_controller ( ADC_NUM_2, ADC_LL_CTRL_ARB );
#endif

  // some other initializations
#ifdef CONFIG_IDF_TARGET_ESP32
  SET_PERI_REG_MASK   ( SENS_SAR_READ_CTRL_REG,   SENS_SAR1_DATA_INV );
  SET_PERI_REG_MASK   ( SENS_SAR_READ_CTRL2_REG,  SENS_SAR2_DATA_INV );
  SET_PERI_REG_MASK   ( SENS_SAR_MEAS_START1_REG, SENS_MEAS1_START_FORCE_M ); // SAR ADC1 controller (in RTC) is started by SW
  SET_PERI_REG_MASK   ( SENS_SAR_MEAS_START1_REG, SENS_SAR1_EN_PAD_FORCE_M ); // SAR ADC1 pad enable bitmap is controlled by SW
  SET_PERI_REG_MASK   ( SENS_SAR_MEAS_START2_REG, SENS_MEAS2_START_FORCE_M ); // SAR ADC2 controller (in RTC) is started by SW
  SET_PERI_REG_MASK   ( SENS_SAR_MEAS_START2_REG, SENS_SAR2_EN_PAD_FORCE_M ); // SAR ADC2 pad enable bitmap is controlled by SW
  CLEAR_PERI_REG_MASK ( SENS_SAR_MEAS_WAIT2_REG,  SENS_FORCE_XPD_SAR_M ); // force XPD_SAR=0, use XPD_FSM
  SET_PERI_REG_BITS   ( SENS_SAR_MEAS_WAIT2_REG,  SENS_FORCE_XPD_AMP, 0x2, SENS_FORCE_XPD_AMP_S ); // force XPD_AMP=0
  CLEAR_PERI_REG_MASK ( SENS_SAR_MEAS_CTRL_REG,   0xfff << SENS_AMP_RST_FB_FSM_S ); // clear FSM
  SET_PERI_REG_BITS   ( SENS_SAR_MEAS_WAIT1_REG,  SENS_SAR_AMP_WAIT1, 0x1, SENS_SAR_AMP_WAIT1_S );
  SET_PERI_REG_BITS   ( SENS_SAR_MEAS_WAIT1_REG,  SENS_SAR_AMP_WAIT2, 0x1, SENS_SAR_AMP_WAIT2_S );
  SET_PERI_REG_BITS   ( SENS_SAR_MEAS_WAIT2_REG,  SENS_SAR_AMP_WAIT3, 0x1, SENS_SAR_AMP_WAIT3_S );
  while ( GET_PERI_REG_BITS2 ( SENS_SAR_SLAVE_ADDR1_REG, 0x7, SENS_MEAS_STATUS_S ) != 0 );
#endif

  // configure all pins to analog read
  for ( int i = 0; i < total_number_inputs; i++ )
  {
    pinMode ( input_pin[i], ANALOG );
  }
}


uint16_t Edrumulus_hardware::my_analogRead ( const uint8_t pin )
{
  const int8_t channel = digitalPinToAnalogChannel ( pin );

  if ( channel > 9 )
  {
    const int8_t channel_modified = channel - 10;
#ifdef CONFIG_IDF_TARGET_ESP32
    CLEAR_PERI_REG_MASK ( SENS_SAR_MEAS_START2_REG, SENS_MEAS2_START_SAR_M );
    SET_PERI_REG_BITS   ( SENS_SAR_MEAS_START2_REG, SENS_SAR2_EN_PAD, ( 1 << channel_modified ), SENS_SAR2_EN_PAD_S );
    SET_PERI_REG_MASK   ( SENS_SAR_MEAS_START2_REG, SENS_MEAS2_START_SAR_M );
    while ( GET_PERI_REG_MASK ( SENS_SAR_MEAS_START2_REG, SENS_MEAS2_DONE_SAR ) == 0 );
    return GET_PERI_REG_BITS2 ( SENS_SAR_MEAS_START2_REG, SENS_MEAS2_DATA_SAR, SENS_MEAS2_DATA_SAR_S );
#else // CONFIG_IDF_TARGET_ESP32S3
    int cur_sample;
    adc2_get_raw ( static_cast<adc2_channel_t> ( channel_modified ), ADC_WIDTH_BIT_12, &cur_sample );
    return cur_sample;
#endif
  }
  else
  {
#ifdef CONFIG_IDF_TARGET_ESP32
    CLEAR_PERI_REG_MASK ( SENS_SAR_MEAS_START1_REG, SENS_MEAS1_START_SAR_M );
    SET_PERI_REG_BITS   ( SENS_SAR_MEAS_START1_REG, SENS_SAR1_EN_PAD, ( 1 << channel ), SENS_SAR1_EN_PAD_S );
    SET_PERI_REG_MASK   ( SENS_SAR_MEAS_START1_REG, SENS_MEAS1_START_SAR_M );
    while ( GET_PERI_REG_MASK ( SENS_SAR_MEAS_START1_REG, SENS_MEAS1_DONE_SAR ) == 0 );
    return GET_PERI_REG_BITS2 ( SENS_SAR_MEAS_START1_REG, SENS_MEAS1_DATA_SAR, SENS_MEAS1_DATA_SAR_S );
#else // CONFIG_IDF_TARGET_ESP32S3
/*
    // set channel
    SENS.sar_meas1_ctrl2.sar1_en_pad = ( 1 << channel );

    // ADC one shot start
    while ( HAL_FORCE_READ_U32_REG_FIELD ( SENS.sar_slave_addr1, meas_status ) != 0 );
    SENS.sar_meas1_ctrl2.meas1_start_sar = 0;
    SENS.sar_meas1_ctrl2.meas1_start_sar = 1;

    // wait
// TODO this does not work...
//while ( SENS.sar_meas1_ctrl2.meas1_done_sar != true );

    return HAL_FORCE_READ_U32_REG_FIELD ( SENS.sar_meas1_ctrl2, meas1_data_sar );

    //adc_hal_convert ( ADC_NUM_1, channel, clk_src_freq_hz, &adc_value );
*/
/*
adc1_channel_t channel = static_cast<adc1_channel_t> ( channel );

    int adc_value;

//static _lock_t adc1_dma_lock;
    static int s_sar_power_on_cnt = 0;
    //static uint32_t clk_src_freq_hz;
//extern portMUX_TYPE rtc_spinlock;
    //typedef enum {
    //SAR_CTRL_LL_POWER_FSM,     //SAR power controlled by FSM
    //SAR_CTRL_LL_POWER_ON,      //SAR power on
    //SAR_CTRL_LL_POWER_OFF,     //SAR power off
    //} sar_ctrl_ll_power_t;
    
    //adc1_rtc_mode_acquire()
//_lock_acquire( &adc1_dma_lock ); // SARADC1_ACQUIRE()

    //s_sar_power_acquire(); // sar_periph_ctrl_adc_oneshot_power_acquire();
//portENTER_CRITICAL_SAFE(&rtc_spinlock);
    s_sar_power_on_cnt++;
    if (s_sar_power_on_cnt == 1) {
        //sar_ctrl_ll_set_power_mode(SAR_CTRL_LL_POWER_ON);
        SENS.sar_peri_clk_gate_conf.saradc_clk_en = 1;
        SENS.sar_power_xpd_sar.force_xpd_sar = 0x3;
    }
//portEXIT_CRITICAL_SAFE(&rtc_spinlock);


#if SOC_ADC_CALIBRATION_V1_SUPPORTED
    //adc_atten_t atten = adc_ll_get_atten(ADC_NUM_1, channel)
    //if (adc_n == ADC_UNIT_1) {
        adc_atten_t atten = (adc_atten_t)((SENS.sar_atten1 >> (channel * 2)) & 0x3);
    //} else {
    //    return (adc_atten_t)((SENS.sar_atten2 >> (channel * 2)) & 0x3);
    //}
    
    //adc_set_hw_calibration_code(ADC_UNIT_1, atten)
    //adc_hal_set_calibration_param(ADC_NUM_1, s_adc_cali_param[adc_n][atten]);

// test
//uint32_t param = 1000;
//uint8_t msb = param >> 8;
//uint8_t lsb = param & 0xFF;
////if (adc_n == ADC_UNIT_1) {
//    REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SAR1_INITIAL_CODE_HIGH_ADDR, msb);
//    REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SAR1_INITIAL_CODE_LOW_ADDR, lsb);
    
#endif  //SOC_ADC_CALIBRATION_V1_SUPPORTED


    
//portENTER_CRITICAL(&rtc_spinlock); // RTC_ENTER_CRITICAL(); // SARADC1_ENTER();
    
    // switch SARADC into RTC channel.
    //adc_ll_set_controller(ADC_NUM_1, ADC_LL_CTRL_RTC)
    SENS.sar_meas1_mux.sar1_dig_force       = 0;    // 1: Select digital control;       0: Select RTC control.
    SENS.sar_meas1_ctrl2.meas1_start_force  = 1;    // 1: SW control RTC ADC start;     0: ULP control RTC ADC start.
    SENS.sar_meas1_ctrl2.sar1_en_pad_force  = 1;    // 1: SW control RTC ADC bit map;   0: ULP control RTC ADC bit map;    
    
//portEXIT_CRITICAL(&rtc_spinlock); // RTC_EXIT_CRITICAL(); // SARADC1_EXIT();

//portENTER_CRITICAL(&rtc_spinlock); // RTC_ENTER_CRITICAL(); // SARADC1_ENTER();
    //adc_ll_set_controller(ADC_NUM_1, ADC_LL_CTRL_RTC);    //Set controller
    
    //adc_oneshot_ll_set_channel(ADC_UNIT_1, channel)
    //if (adc_n == ADC_NUM_1) {
        SENS.sar_meas1_ctrl2.sar1_en_pad = (1 << channel); //only one channel is selected.
    //} else { // adc_n == ADC_UNIT_2
    //    SENS.sar_meas2_ctrl2.sar2_en_pad = (1 << channel); //only one channel is selected.
    //}

    
    //adc_hal_convert(ADC_NUM_1, channel, clk_src_freq_hz, &adc_value);   //Start conversion, For ADC1, the data always valid.
    //uint32_t event = ADC_LL_EVENT_ADC1_ONESHOT_DONE;//(adc_n == ADC_UNIT_1) ? ADC_LL_EVENT_ADC1_ONESHOT_DONE : ADC_LL_EVENT_ADC2_ONESHOT_DONE;
    //adc_oneshot_ll_clear_event(event); //For compatibility
    //adc_oneshot_ll_disable_all_unit(); //For compatibility
    //adc_oneshot_ll_enable(adc_n); //For compatibility
    
    //adc_oneshot_ll_set_channel(adc_n, channel)
    //if (adc_n == ADC_UNIT_1) {
        SENS.sar_meas1_ctrl2.sar1_en_pad = (1 << channel); //only one channel is selected.
    //} else { // adc_n == ADC_UNIT_2
    //    SENS.sar_meas2_ctrl2.sar2_en_pad = (1 << channel); //only one channel is selected.
    //}

    //adc_hal_onetime_start(adc_n, clk_src_freq_hz);
    //static inline void adc_oneshot_ll_start(adc_unit_t adc_n)
    //if (adc_n == ADC_UNIT_1) {
        while (HAL_FORCE_READ_U32_REG_FIELD(SENS.sar_slave_addr1, meas_status) != 0) {}
        SENS.sar_meas1_ctrl2.meas1_start_sar = 0;
        SENS.sar_meas1_ctrl2.meas1_start_sar = 1;
    //} else { // adc_n == ADC_UNIT_2
    //    SENS.sar_meas2_ctrl2.meas2_start_sar = 0; //start force 0
    //    SENS.sar_meas2_ctrl2.meas2_start_sar = 1; //start force 1
    //}

    //while (adc_oneshot_ll_get_event(event) != true) {
    while ((bool)SENS.sar_meas1_ctrl2.meas1_done_sar != true) {
        ;
    }

    //*out_raw = adc_oneshot_ll_get_raw_result(adc_n);
    //if (adc_n == ADC_UNIT_1) {
        adc_value = HAL_FORCE_READ_U32_REG_FIELD(SENS.sar_meas1_ctrl2, meas1_data_sar);
    //} else { // adc_n == ADC_UNIT_2
    //    ret_val = HAL_FORCE_READ_U32_REG_FIELD(SENS.sar_meas2_ctrl2, meas2_data_sar);
    //}
    
    //if (adc_oneshot_ll_raw_check_valid(adc_n, *out_raw) == false) {
    //    return ESP_ERR_INVALID_STATE;
    //}

    //HW workaround: when enabling periph clock, this should be false
    //adc_oneshot_ll_disable_all_unit(); //For compatibility
    
    
    //adc_ll_rtc_reset()    //Reset FSM of rtc controller
    SENS.sar_peri_reset_conf.saradc_reset = 1;
    SENS.sar_peri_reset_conf.saradc_reset = 0;
    
//portEXIT_CRITICAL(&rtc_spinlock); // RTC_EXIT_CRITICAL(); // SARADC1_EXIT();

    //adc1_lock_release()
    
    //s_sar_power_release(); // sar_periph_ctrl_adc_oneshot_power_release();
//portENTER_CRITICAL_SAFE(&rtc_spinlock);
    s_sar_power_on_cnt--;
    //if (s_sar_power_on_cnt < 0) {
    //    portEXIT_CRITICAL(&rtc_spinlock);
    //    ESP_LOGE(TAG, "%s called, but s_sar_power_on_cnt == 0", __func__);
    //    abort();
    //} else
    if (s_sar_power_on_cnt == 0) {
        //sar_ctrl_ll_set_power_mode(SAR_CTRL_LL_POWER_FSM)
        //if (mode == SAR_CTRL_LL_POWER_FSM) {
            SENS.sar_peri_clk_gate_conf.saradc_clk_en = 1;
            SENS.sar_power_xpd_sar.force_xpd_sar = 0x0;
        //} else if (mode == SAR_CTRL_LL_POWER_ON) {
        //    SENS.sar_peri_clk_gate_conf.saradc_clk_en = 1;
        //    SENS.sar_power_xpd_sar.force_xpd_sar = 0x3;
        //} else {
        //    SENS.sar_peri_clk_gate_conf.saradc_clk_en = 0;
        //    SENS.sar_power_xpd_sar.force_xpd_sar = 0x2;
    }
    //}
//portEXIT_CRITICAL_SAFE(&rtc_spinlock);
    
//_lock_release( &adc1_dma_lock ); // SARADC1_RELEASE();
        
    return adc_value;
*/

    return adc1_get_raw ( static_cast<adc1_channel_t> ( channel ) );
#endif
  }
}


void Edrumulus_hardware::my_analogRead_parallel ( const uint32_t channel_adc1_bitval,
                                                  const uint32_t channel_adc2_bitval,
                                                  uint16_t&      out_adc1,
                                                  uint16_t&      out_adc2 )
{
#ifdef CONFIG_IDF_TARGET_ESP32
  // start ADC1
  CLEAR_PERI_REG_MASK ( SENS_SAR_MEAS_START1_REG, SENS_MEAS1_START_SAR_M );
  SET_PERI_REG_BITS   ( SENS_SAR_MEAS_START1_REG, SENS_SAR1_EN_PAD, channel_adc1_bitval, SENS_SAR1_EN_PAD_S );
  SET_PERI_REG_MASK   ( SENS_SAR_MEAS_START1_REG, SENS_MEAS1_START_SAR_M );

  // start ADC2
  CLEAR_PERI_REG_MASK ( SENS_SAR_MEAS_START2_REG, SENS_MEAS2_START_SAR_M );
  SET_PERI_REG_BITS   ( SENS_SAR_MEAS_START2_REG, SENS_SAR2_EN_PAD, channel_adc2_bitval, SENS_SAR2_EN_PAD_S );
  SET_PERI_REG_MASK   ( SENS_SAR_MEAS_START2_REG, SENS_MEAS2_START_SAR_M );

  // wait for ADC1 and read value
  while ( GET_PERI_REG_MASK ( SENS_SAR_MEAS_START1_REG, SENS_MEAS1_DONE_SAR ) == 0 );
  out_adc1 = GET_PERI_REG_BITS2 ( SENS_SAR_MEAS_START1_REG, SENS_MEAS1_DATA_SAR, SENS_MEAS1_DATA_SAR_S );

  // wait for ADC2 and read value
  while ( GET_PERI_REG_MASK ( SENS_SAR_MEAS_START2_REG, SENS_MEAS2_DONE_SAR ) == 0 );
  out_adc2 = GET_PERI_REG_BITS2 ( SENS_SAR_MEAS_START2_REG, SENS_MEAS2_DATA_SAR, SENS_MEAS2_DATA_SAR_S );
#endif
}

#endif


// -----------------------------------------------------------------------------
// Common hardware functions ---------------------------------------------------
// -----------------------------------------------------------------------------
void Edrumulus_hardware::cancel_ADC_spikes ( float&    signal,
                                             int&      overload_detected,
                                             const int pad_index,
                                             const int input_channel_index,
                                             const int level )
{
  // remove single/dual sample spikes by checking if right before and right after the
  // detected spike(s) we only have noise and no useful signal (since the ESP32 spikes
  // mostly are on just one or two sample(s))
  const int max_peak_threshold = 150; // maximum assumed ESP32 spike amplitude

  const float signal_org            = signal;
  signal                            = prev_input4[pad_index][input_channel_index];    // normal return value in case no spike was detected
  const int   overload_detected_org = overload_detected;
  overload_detected                 = prev_overload4[pad_index][input_channel_index]; // normal return value in case no spike was detected
  const float input_abs             = abs ( signal_org );
  Espikestate input_state           = ST_OTHER; // initialization value, might be overwritten

  if ( input_abs < ADC_MAX_NOISE_AMPL )
  {
    input_state = ST_NOISE;
  }
  else if ( ( signal_org < max_peak_threshold ) && ( signal_org > 0 ) )
  {
    input_state = ST_SPIKE_HIGH;
  }
  else if ( ( signal_org > -max_peak_threshold ) && ( signal_org < 0 ) )
  {
    input_state = ST_SPIKE_LOW;
  }

  // check for single high spike sample case
  if ( ( ( prev5_input_state[pad_index][input_channel_index] == ST_NOISE ) || ( prev5_input_state[pad_index][input_channel_index] == ST_SPIKE_LOW ) ) &&
       ( prev4_input_state[pad_index][input_channel_index] == ST_SPIKE_HIGH ) &&
       ( ( prev3_input_state[pad_index][input_channel_index] == ST_NOISE ) || ( prev3_input_state[pad_index][input_channel_index] == ST_SPIKE_LOW ) ) )
  {
    signal = 0.0f; // remove single spike
  }

  // check for single low spike sample case
  if ( ( ( prev5_input_state[pad_index][input_channel_index] == ST_NOISE ) || ( prev5_input_state[pad_index][input_channel_index] == ST_SPIKE_HIGH ) ) &&
       ( prev4_input_state[pad_index][input_channel_index] == ST_SPIKE_LOW ) &&
       ( ( prev3_input_state[pad_index][input_channel_index] == ST_NOISE ) || ( prev3_input_state[pad_index][input_channel_index] == ST_SPIKE_HIGH ) ) )
  {
    signal = 0.0f; // remove single spike
  }

  if ( level >= 2 )
  {
    // check for two sample spike case
    if ( ( ( prev5_input_state[pad_index][input_channel_index] == ST_NOISE ) || ( prev5_input_state[pad_index][input_channel_index] == ST_SPIKE_LOW ) ) &&
         ( prev4_input_state[pad_index][input_channel_index] == ST_SPIKE_HIGH ) &&
         ( prev3_input_state[pad_index][input_channel_index] == ST_SPIKE_HIGH ) &&
         ( ( prev2_input_state[pad_index][input_channel_index] == ST_NOISE ) || ( prev2_input_state[pad_index][input_channel_index] == ST_SPIKE_LOW ) ) )
    {
      signal                                      = 0.0f; // remove two sample spike
      prev_input3[pad_index][input_channel_index] = 0.0f; // remove two sample spike
    }
  
    // check for two sample low spike case
    if ( ( ( prev5_input_state[pad_index][input_channel_index] == ST_NOISE ) || ( prev5_input_state[pad_index][input_channel_index] == ST_SPIKE_HIGH ) ) &&
         ( prev4_input_state[pad_index][input_channel_index] == ST_SPIKE_LOW ) &&
         ( prev3_input_state[pad_index][input_channel_index] == ST_SPIKE_LOW ) &&
         ( ( prev2_input_state[pad_index][input_channel_index] == ST_NOISE ) || ( prev2_input_state[pad_index][input_channel_index] == ST_SPIKE_HIGH ) ) )
    {
      signal                                      = 0.0f; // remove two sample spike
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
      signal                                      = 0.0f; // remove three sample spike
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
      signal                                      = 0.0f; // remove three sample spike
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
      signal                                      = 0.0f; // remove four sample spike
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
  if ( level >= 3 )
  {
    prev1_input_state[pad_index][input_channel_index] = input_state;
    prev_input1[pad_index][input_channel_index]       = signal_org;
    prev_overload1[pad_index][input_channel_index]    = overload_detected_org;
  }
  else if ( level >= 2 )
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
