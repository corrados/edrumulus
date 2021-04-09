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

#include "edrumulus_esp32.h"

Edrumulus_esp32* edrumulus_esp32_pointer = nullptr;


Edrumulus_esp32::Edrumulus_esp32()
{
  // global pointer to this class needed for static callback function
  edrumulus_esp32_pointer = this;
}


void Edrumulus_esp32::setup ( const int conf_Fs )
{
  // set essential parameters
  Fs = conf_Fs;

  // prepare the ADC and analog GPIO inputs
  init_my_analogRead();

  // prepare timer at a rate of given sampling rate
  timer_semaphore = xSemaphoreCreateBinary();
  timer           = timerBegin ( 0, 80, true ); // prescaler of 80 (i.e. below we have 1 MHz instead of 80 MHz)
  timerAttachInterrupt ( timer, &on_timer, true );
  timerAlarmWrite      ( timer, 1000000 / Fs, true ); // here we define the sampling rate (1 MHz / Fs)
  timerAlarmEnable     ( timer );
}


void IRAM_ATTR Edrumulus_esp32::on_timer()
{
  // tell the main loop that a sample can be read by setting the semaphore
  xSemaphoreGiveFromISR ( edrumulus_esp32_pointer->timer_semaphore, NULL );
}


void Edrumulus_esp32::capture_samples ( const int number_pads,
                                        const int number_inputs[],
                                        int       analog_pin[][MAX_NUM_PAD_INPUTS],
                                        int       sample_org[][MAX_NUM_PAD_INPUTS] )
{
  // wait for the timer to get the correct sampling rate when reading the analog value
  if ( xSemaphoreTake ( timer_semaphore, portMAX_DELAY ) == pdTRUE )
  {
    // get samples from the ADCs
    for ( int i = 0; i < number_pads; i++ )
    {
      for ( int j = 0; j < number_inputs[i]; j++ )
      {
        sample_org[i][j] = my_analogRead ( analog_pin[i][j] );
      }
    }
  }
}


float Edrumulus_esp32::cancel_ADC_spikes ( const float input,
                                           const int   pad_index,
                                           const int   input_channel_index )
{
  const int max_peak_threshold = 100; // maximum assumed ESP32 spike amplitude

  float       return_value = prev_input2[pad_index][input_channel_index]; // normal return value in case no spike was detected
  const float input_abs    = abs ( input );
  Espikestate input_state  = ST_OTHER; // initialization value, might be overwritten

  if ( input_abs < ADC_MAX_NOISE_AMPL )
  {
    input_state = ST_NOISE;
  }
  else if ( input_abs < max_peak_threshold )
  {
    input_state = ST_SPIKE;
  }

  // remove single spikes by checking if right before and right after the detected
  // spike(s) we only have noise and no useful signal (since the ESP32 spikes mostly
  // are on just one or two sample(s))
  //
  // check for single spike sample case
  if ( ( prev3_input_state[pad_index][input_channel_index] == ST_NOISE ) &&
       ( prev2_input_state[pad_index][input_channel_index] == ST_SPIKE ) &&
       ( prev1_input_state[pad_index][input_channel_index] == ST_NOISE ) )
  {
    return_value = 0.0f; // remove single spike
  }

  // check for two sample spike case
  if ( ( prev3_input_state[pad_index][input_channel_index] == ST_NOISE ) &&
       ( prev2_input_state[pad_index][input_channel_index] == ST_SPIKE ) &&
       ( prev1_input_state[pad_index][input_channel_index] == ST_SPIKE ) &&
       ( input_state                                       == ST_NOISE ) )
  {
    prev_input1[pad_index][input_channel_index] = 0.0f; // remove two sample spike
    return_value                                = 0.0f; // remove two sample spike
  }

  // update three-step input signal memory where we store the last three states of
  // the input signal and two previous untouched input samples
  prev3_input_state[pad_index][input_channel_index] = prev2_input_state[pad_index][input_channel_index];
  prev2_input_state[pad_index][input_channel_index] = prev1_input_state[pad_index][input_channel_index];
  prev1_input_state[pad_index][input_channel_index] = input_state;
  prev_input2[pad_index][input_channel_index]       = prev_input1[pad_index][input_channel_index];
  prev_input1[pad_index][input_channel_index]       = input;

  return return_value;
}


// Since arduino-esp32 library version 1.0.5, the analogRead was changed to use the IDF interface
// which made the analogRead function so slow that we cannot use that anymore for Edrumulus:
// https://github.com/espressif/arduino-esp32/issues/4973, https://github.com/espressif/arduino-esp32/pull/3377
// As a workaround, we had to write our own analogRead function.
void Edrumulus_esp32::init_my_analogRead()
{
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
}


uint16_t Edrumulus_esp32::my_analogRead ( uint8_t pin )
{
  const int8_t channel = digitalPinToAnalogChannel ( pin );

  pinMode ( pin, ANALOG );

  if ( channel > 9 )
  {
    const int8_t channel_modified = channel - 10;
    CLEAR_PERI_REG_MASK ( SENS_SAR_MEAS_START2_REG, SENS_MEAS2_START_SAR_M );
    SET_PERI_REG_BITS   ( SENS_SAR_MEAS_START2_REG, SENS_SAR2_EN_PAD, ( 1 << channel_modified ), SENS_SAR2_EN_PAD_S );
    SET_PERI_REG_MASK   ( SENS_SAR_MEAS_START2_REG, SENS_MEAS2_START_SAR_M );
  }
  else
  {
    CLEAR_PERI_REG_MASK ( SENS_SAR_MEAS_START1_REG, SENS_MEAS1_START_SAR_M );
    SET_PERI_REG_BITS   ( SENS_SAR_MEAS_START1_REG, SENS_SAR1_EN_PAD, ( 1 << channel ), SENS_SAR1_EN_PAD_S );
    SET_PERI_REG_MASK   ( SENS_SAR_MEAS_START1_REG, SENS_MEAS1_START_SAR_M );
  }

  if ( channel > 7 )
  {
    while ( GET_PERI_REG_MASK ( SENS_SAR_MEAS_START2_REG, SENS_MEAS2_DONE_SAR ) == 0 );
    return GET_PERI_REG_BITS2 ( SENS_SAR_MEAS_START2_REG, SENS_MEAS2_DATA_SAR, SENS_MEAS2_DATA_SAR_S );
  }
  else
  {
    while ( GET_PERI_REG_MASK ( SENS_SAR_MEAS_START1_REG, SENS_MEAS1_DONE_SAR ) == 0 );
    return GET_PERI_REG_BITS2 ( SENS_SAR_MEAS_START1_REG, SENS_MEAS1_DATA_SAR, SENS_MEAS1_DATA_SAR_S );
  }
}
