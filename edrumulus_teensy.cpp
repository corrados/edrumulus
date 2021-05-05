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

  // create timer semaphore
  //timer_semaphore = xSemaphoreCreateBinary();

  // prepare timer at a rate of given sampling rate
  //my_obj->timer = timerBegin ( 0, 80, true ); // prescaler of 80 (i.e. below we have 1 MHz instead of 80 MHz)
  //timerAttachInterrupt ( timer, &on_timer, true );
  //timerAlarmWrite      ( timer, 1000000 / Fs, true ); // here we define the sampling rate (1 MHz / Fs)
  //timerAlarmEnable     ( timer );
}


//void IRAM_ATTR Edrumulus_teensy::on_timer()
//{
  // tell the main loop that a sample can be read by setting the semaphore
  //static BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  //xSemaphoreGiveFromISR ( edrumulus_teensy_pointer->timer_semaphore, &xHigherPriorityTaskWoken );

  //if ( xHigherPriorityTaskWoken == pdTRUE )
  //{
  //  portYIELD_FROM_ISR();
  //}
//}


void Edrumulus_teensy::capture_samples ( const int number_pads,
                                         const int number_inputs[],
                                         int       analog_pin[][MAX_NUM_PAD_INPUTS],
                                         int       sample_org[][MAX_NUM_PAD_INPUTS] )
{
  // wait for the timer to get the correct sampling rate when reading the analog value
  if ( true)//xSemaphoreTake ( timer_semaphore, portMAX_DELAY ) == pdTRUE )
  {
    // read the ADC samples
    for ( int i = 0; i < total_number_inputs; i++ )
    {
      input_sample[i] = analogRead ( input_pin[i] );
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
}

#endif
