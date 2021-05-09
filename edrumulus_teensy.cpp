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

#include "edrumulus_teensy.h"

#ifdef TEENSYDUINO

Edrumulus_teensy* edrumulus_teensy_pointer = nullptr;


Edrumulus_teensy::Edrumulus_teensy()
{
  // global pointer to this class needed for static callback function
  edrumulus_teensy_pointer = this;
}


void Edrumulus_teensy::setup ( const int conf_Fs,
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

  // set the ADC properties
  adc_obj.adc0->setResolution      ( 12 ); // we want to get the full ADC resultion of the Teensy 4.0
  adc_obj.adc0->setAveraging       ( 1 );  // no averaging
  adc_obj.adc0->setConversionSpeed ( ADC_CONVERSION_SPEED::LOW_SPEED );    // to reduce spikes
  adc_obj.adc0->setSamplingSpeed   ( ADC_SAMPLING_SPEED::VERY_LOW_SPEED ); // to reduce spikes

  adc_obj.adc1->setResolution      ( 12 ); // we want to get the full ADC resultion of the Teensy 4.0
  adc_obj.adc1->setAveraging       ( 1 );  // no averaging
  adc_obj.adc1->setConversionSpeed ( ADC_CONVERSION_SPEED::LOW_SPEED );    // to reduce spikes
  adc_obj.adc1->setSamplingSpeed   ( ADC_SAMPLING_SPEED::VERY_LOW_SPEED ); // to reduce spikes

  // disable MIMXRT1062DVL6A "keeper" on all possible Teensy ADC input pins
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

  // initialize timer flag (semaphore)
  timer_ready = false;

  // prepare timer at a rate of given sampling rate
  myTimer.begin ( on_timer, 1000000 / Fs ); // here we define the sampling rate (1 MHz / Fs)
}


void Edrumulus_teensy::on_timer()
{
  // tell the main loop that a sample can be read by setting the flag (semaphore)
  edrumulus_teensy_pointer->timer_ready = true;
}


void Edrumulus_teensy::capture_samples ( const int number_pads,
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
// TEST
if ( ( input_pin[i] == 12 ) || ( input_pin[i] == 13 ) )
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
