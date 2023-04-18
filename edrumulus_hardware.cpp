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
  // analog pins setup:                 snare | kick | hi-hat | hi-hat-ctrl | crash | tom1 | ride | tom2 | tom3  
  static int analog_pins_s3[]         = {  4,     6,      7,        9,         10,     12,    13,    15,    16 };
  static int analog_pins_rimshot_s3[] = {  5,    -1,      8,       -1,         11,     -1,    14,    -1,    -1 };
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
  // read the ADC samples
  for ( int i = 0; i < edrumulus_hardware_pointer->total_number_inputs; i++ )
  {
    edrumulus_hardware_pointer->input_sample[i] =
      edrumulus_hardware_pointer->my_analogRead ( edrumulus_hardware_pointer->input_pin[i] );
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
  int cur_sample;
  for ( int channel = 0; channel < 10; channel++ ) // 10 channels per ADC
  {
    // configure the attenuation and let the get_raw() do all the ADC initialization for us...
    adc1_config_channel_atten ( static_cast<adc1_channel_t> ( channel ), ADC_ATTEN_DB_11 );
    adc2_config_channel_atten ( static_cast<adc2_channel_t> ( channel ), ADC_ATTEN_DB_11 );
    adc1_get_raw              ( static_cast<adc1_channel_t> ( channel ) );
    adc2_get_raw              ( static_cast<adc2_channel_t> ( channel ), ADC_WIDTH_BIT_12, &cur_sample );
  }
  adc_power_on();

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
    SENS.sar_meas2_ctrl2.meas2_start_sar = 0;
    SENS.sar_meas2_ctrl2.sar2_en_pad     = ( 1 << channel_modified );
    SENS.sar_meas2_ctrl2.meas2_start_sar = 1;
    while ( !SENS.sar_meas2_ctrl2.meas2_done_sar );
    return HAL_FORCE_READ_U32_REG_FIELD ( SENS.sar_meas2_ctrl2, meas2_data_sar );
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
    SENS.sar_meas1_ctrl2.meas1_start_sar = 0;
    SENS.sar_meas1_ctrl2.sar1_en_pad     = ( 1 << channel );
    SENS.sar_meas1_ctrl2.meas1_start_sar = 1;
    while ( !SENS.sar_meas1_ctrl2.meas1_done_sar );
    return HAL_FORCE_READ_U32_REG_FIELD ( SENS.sar_meas1_ctrl2, meas1_data_sar );
#endif
  }
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
