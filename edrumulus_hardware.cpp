/******************************************************************************\
 * Copyright (c) 2020-2022
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

//#include "EEPROM.h"
//EEPROMClass MYEEPROM ( "Edrumulus", MAX_EEPROM_SIZE );

void Edrumulus_hardware::setup ( const int conf_Fs,
                                 const int number_pads,
                                 const int number_inputs[],
                                 int       analog_pin[][MAX_NUM_PAD_INPUTS] )
{
  // set essential parameters
  Fs = conf_Fs;
  eeprom_settings.begin ( ( number_pads + 1 ) * MAX_NUM_SET_PER_PAD ); // "+ 1" for pad-independent global settings


//Serial.println ( eeprom_settings.length() );

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
  xTaskCreatePinnedToCore ( start_timer_core0_task, "start_timer_core0_task", 800, this, 1, NULL, 0 );
}


void Edrumulus_hardware::setup_timer ( const bool clear_timer )
{
  if ( !clear_timer )
  {
    // prepare timer at a rate of given sampling rate
    timer = timerBegin ( 0, 80, true ); // prescaler of 80 (i.e. below we have 1 MHz instead of 80 MHz)
    timerAttachInterrupt ( timer, &on_timer, true );
    timerAlarmWrite      ( timer, 1000000 / Fs, true ); // here we define the sampling rate (1 MHz / Fs)
    timerAlarmEnable     ( timer );
  }
  else
  {
    // clear timer to release SPI resources for writing EEPROM data
    timerAlarmDisable    ( timer );
    timerDetachInterrupt ( timer );
    timerEnd             ( timer );
  }
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

  // set attenuation of 11 dB
  WRITE_PERI_REG ( SENS_SAR_ATTEN1_REG, 0x0FFFFFFFF );
  WRITE_PERI_REG ( SENS_SAR_ATTEN2_REG, 0x0FFFFFFFF );

  // set both ADCs to 12 bit resolution using 8 cycles and 1 sample
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

  // some other initializations
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
    CLEAR_PERI_REG_MASK ( SENS_SAR_MEAS_START2_REG, SENS_MEAS2_START_SAR_M );
    SET_PERI_REG_BITS   ( SENS_SAR_MEAS_START2_REG, SENS_SAR2_EN_PAD, ( 1 << channel_modified ), SENS_SAR2_EN_PAD_S );
    SET_PERI_REG_MASK   ( SENS_SAR_MEAS_START2_REG, SENS_MEAS2_START_SAR_M );
    while ( GET_PERI_REG_MASK ( SENS_SAR_MEAS_START2_REG, SENS_MEAS2_DONE_SAR ) == 0 );
    return GET_PERI_REG_BITS2 ( SENS_SAR_MEAS_START2_REG, SENS_MEAS2_DATA_SAR, SENS_MEAS2_DATA_SAR_S );
  }
  else
  {
    CLEAR_PERI_REG_MASK ( SENS_SAR_MEAS_START1_REG, SENS_MEAS1_START_SAR_M );
    SET_PERI_REG_BITS   ( SENS_SAR_MEAS_START1_REG, SENS_SAR1_EN_PAD, ( 1 << channel ), SENS_SAR1_EN_PAD_S );
    SET_PERI_REG_MASK   ( SENS_SAR_MEAS_START1_REG, SENS_MEAS1_START_SAR_M );
    while ( GET_PERI_REG_MASK ( SENS_SAR_MEAS_START1_REG, SENS_MEAS1_DONE_SAR ) == 0 );
    return GET_PERI_REG_BITS2 ( SENS_SAR_MEAS_START1_REG, SENS_MEAS1_DATA_SAR, SENS_MEAS1_DATA_SAR_S );
  }
}


void Edrumulus_hardware::my_analogRead_parallel ( const uint32_t channel_adc1_bitval,
                                                  const uint32_t channel_adc2_bitval,
                                                  uint16_t&      out_adc1,
                                                  uint16_t&      out_adc2 )
{
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
}

#endif


// -----------------------------------------------------------------------------
// Common hardware functions ---------------------------------------------------
// -----------------------------------------------------------------------------
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
