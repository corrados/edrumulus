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


// -----------------------------------------------------------------------------
// Teensy 4.0 ------------------------------------------------------------------
// -----------------------------------------------------------------------------
#ifdef TEENSYDUINO

extern Edrumulus_hardware* edrumulus_hardware_pointer;

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

#if defined(ARDUINO_TEENSY40) || defined(ARDUINO_TEENSY41) // Teensy 4.0/4.1 specific code
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

#elif defined(ARDUINO_TEENSY36)  // Teensy 3.6 specific code
  adc_obj.adc0->setResolution      ( 16 ); // we want to get the full ADC resolution of the Teensy 4.0
  adc_obj.adc0->setAveraging       ( 1 );
  adc_obj.adc0->setConversionSpeed ( ADC_CONVERSION_SPEED::HIGH_SPEED_16BITS );
  adc_obj.adc0->setSamplingSpeed   ( ADC_SAMPLING_SPEED::VERY_HIGH_SPEED );

  adc_obj.adc1->setResolution      ( 16 ); // we want to get the full ADC resolution of the Teensy 4.0
  adc_obj.adc1->setAveraging       ( 1 );
  adc_obj.adc1->setConversionSpeed ( ADC_CONVERSION_SPEED::HIGH_SPEED_16BITS );
  adc_obj.adc1->setSamplingSpeed   ( ADC_SAMPLING_SPEED::VERY_HIGH_SPEED );

/* A word about conversion time.
    conv_time = SFCAdder + Averages*(BCT + LSTAdder + HSCAdder)
              = 5 ADCK + 5 bus + 1 *(25 ADCK + 0 ADCK + 2 ADCK) = 32 ADCK + 5 bus
              = 13/7.5MHz + 5/60MHz = 4.35uS per conversion
    For 22 conversions, 22*4.35us = 95.7uS which is still under 125uS.

    F_BUS is 60MHz and F_ADC is max allowed 24MHz in 12-bit mode and 12MHz in
    16-bit mode to stay within specs.  F_ADC is derived from F_BUS with dividers
    1, 2, 4, 8, or 16.  F_BUS of 60MHz offers 30, 15, and 7.5 MHz options.

    For 12-bit mode, 15 and 7.5 can work
    For 16-bit mode, only 7.5 can work.

    The definition for F_BUS can be found in kinetics.h and it's a function of
    the F_CPU.  For F_CPU=180MHz, F_BUS=60MHz.  For F_CPU=192MHz, F_BUS=48MHz.
    In other words, overclocking the teensy will affect the conversion rate.
*/
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
#if defined(ARDUINO_TEENSY40) || defined(ARDUINO_TEENSY41) // Teensy 4.0/4.1 specific code
    // pins 12 and 13 are ADC1 only, pins 10 and 11 are ADC0 only
    // note that pin 8 gave large spikes on ADC0 but seems to work ok with ADC1
    if ( ( input_pin[i] == 8 ) || ( input_pin[i] == 12 ) || ( input_pin[i] == 13 ) )
    {
      input_sample[i] = adc_obj.adc1->analogRead ( input_pin[i] );
    }
#elif defined(ARDUINO_TEENSY36)  // Teensy 3.6 specific code
    // pins 12 and 13 and 22 are ADC1 only
    if ( ( input_pin[i] == 12 ) || ( input_pin[i] == 13 ) || ( input_pin[i] == 22 ) )
    {
      input_sample[i] = adc_obj.adc1->analogRead ( input_pin[i] );
    }
#endif
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
#if defined(ARDUINO_TEENSY36)  // Teensy 3.6 specific code
      sample_org[i][i] >>= 4;  // 16-bit to 12-bit hack
#endif
    }
  }
}

#endif
