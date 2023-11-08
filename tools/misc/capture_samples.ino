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

#include "soc/sens_reg.h"
#include "driver/dac.h"

//#define DO_MULT_INPUT_CAPTURE_TEST
//#define DO_SAMPLE_RATE_TEST
//#define DO_INPUT_CAPTURE
#define DO_INPUT_BUFFER_CAPTURE

const int                  analog_pin                     = 35;//25;
const int                  num_all_pins                  = 12;//15;
//                                                    ADC:    1   1   1   1   1   1   2   2   2   2   2   2   2  2  2
const int                  all_analog_pins[num_all_pins] = { 36, 39, 34, 35, 32, 33, 25, 26, 27, 14, 12, 13};//, 4, 2, 15 };
const int                  Fs                             = 8000;
const int                  samplerate_max_cnt             = 1000; // samples
int                        samplerate_prev_micros_cnt     = 0;
unsigned long              samplerate_prev_micros         = micros();
volatile SemaphoreHandle_t timer_semaphore;
hw_timer_t*                timer = nullptr;
const int                  dc_offset_est_len   = 5000; // samples
float                      dc_offset           = 0.0f;
int                        prvious_sample1     = 0;
int                        prvious_sample2     = 0;
int                        prvious_sample1_out = 0;

void IRAM_ATTR on_timer()
{
  // tell the main loop that a sample can be read by setting the semaphore
  xSemaphoreGiveFromISR ( timer_semaphore, NULL );
}

// Since arduino-esp32 library version 1.0.5, the analogRead was changed to use the IDF interface
// which made the analogRead function so slow that we cannot use that anymore for Edrumulus:
// https://github.com/espressif/arduino-esp32/issues/4973, https://github.com/espressif/arduino-esp32/pull/3377
// As a workaround, we had to write our own analogRead function.
void my_init_analogRead()
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
}

uint16_t my_analogRead ( uint8_t pin )
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


void my_analogRead2 ( uint8_t pin1, uint8_t pin2, uint16_t& out1, uint16_t& out2 )
{
  const int8_t channel  = digitalPinToAnalogChannel ( pin1 );
  const int8_t channel2 = digitalPinToAnalogChannel ( pin2 );

/*
  pinMode ( pin1, ANALOG );
  pinMode ( pin2, ANALOG );
*/

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

  if ( channel2 > 9 )
  {
    const int8_t channel_modified = channel2 - 10;
    CLEAR_PERI_REG_MASK ( SENS_SAR_MEAS_START2_REG, SENS_MEAS2_START_SAR_M );
    SET_PERI_REG_BITS   ( SENS_SAR_MEAS_START2_REG, SENS_SAR2_EN_PAD, ( 1 << channel_modified ), SENS_SAR2_EN_PAD_S );
    SET_PERI_REG_MASK   ( SENS_SAR_MEAS_START2_REG, SENS_MEAS2_START_SAR_M );
  }
  else
  {
    CLEAR_PERI_REG_MASK ( SENS_SAR_MEAS_START1_REG, SENS_MEAS1_START_SAR_M );
    SET_PERI_REG_BITS   ( SENS_SAR_MEAS_START1_REG, SENS_SAR1_EN_PAD, ( 1 << channel2 ), SENS_SAR1_EN_PAD_S );
    SET_PERI_REG_MASK   ( SENS_SAR_MEAS_START1_REG, SENS_MEAS1_START_SAR_M );
  }

  if ( channel > 7 )
  {
    while ( GET_PERI_REG_MASK ( SENS_SAR_MEAS_START2_REG, SENS_MEAS2_DONE_SAR ) == 0 );
    out1 = GET_PERI_REG_BITS2 ( SENS_SAR_MEAS_START2_REG, SENS_MEAS2_DATA_SAR, SENS_MEAS2_DATA_SAR_S );
  }
  else
  {
    while ( GET_PERI_REG_MASK ( SENS_SAR_MEAS_START1_REG, SENS_MEAS1_DONE_SAR ) == 0 );
    out1 = GET_PERI_REG_BITS2 ( SENS_SAR_MEAS_START1_REG, SENS_MEAS1_DATA_SAR, SENS_MEAS1_DATA_SAR_S );
  }

  if ( channel2 > 7 )
  {
    while ( GET_PERI_REG_MASK ( SENS_SAR_MEAS_START2_REG, SENS_MEAS2_DONE_SAR ) == 0 );
    out2 = GET_PERI_REG_BITS2 ( SENS_SAR_MEAS_START2_REG, SENS_MEAS2_DATA_SAR, SENS_MEAS2_DATA_SAR_S );
  }
  else
  {
    while ( GET_PERI_REG_MASK ( SENS_SAR_MEAS_START1_REG, SENS_MEAS1_DONE_SAR ) == 0 );
    out2 = GET_PERI_REG_BITS2 ( SENS_SAR_MEAS_START1_REG, SENS_MEAS1_DATA_SAR, SENS_MEAS1_DATA_SAR_S );
  }
}


void setup()
{
#if defined ( DO_MULT_INPUT_CAPTURE_TEST ) || defined ( DO_SAMPLE_RATE_TEST ) || defined ( DO_INPUT_BUFFER_CAPTURE )
  Serial.begin ( 115200 );
#else
  Serial.begin ( 500000 );
#endif

  my_init_analogRead();

  // estimate the DC offset
  float dc_offset_sum = 0.0f;

  for ( int i = 0; i < dc_offset_est_len; i++ )
  {
    dc_offset_sum += my_analogRead ( analog_pin );
    delayMicroseconds ( 100 );
  }

// TEST
for ( int i = 0; i < num_all_pins / 2; i++ )
{
  pinMode ( all_analog_pins[i], ANALOG );
  pinMode ( all_analog_pins[i + 6], ANALOG );
}

  dc_offset = dc_offset_sum / dc_offset_est_len;

  // prepare timer at a rate of given sampling rate
  timer_semaphore = xSemaphoreCreateBinary();
  timer           = timerBegin ( 0, 80, true ); // prescaler of 80 (i.e. below we have 1 MHz instead of 80 MHz)
  timerAttachInterrupt ( timer, &on_timer, true );
  timerAlarmWrite      ( timer, 1000000 / Fs, true ); // here we define the sampling rate (1 MHz / Fs)
  timerAlarmEnable     ( timer );
}

void loop()
{
  // wait for the timer to get the correct sampling rate when reading the analog value
  if ( xSemaphoreTake ( timer_semaphore, portMAX_DELAY ) == pdTRUE )
  {

#ifdef DO_SAMPLE_RATE_TEST
/*
// TEST
int sample_org[num_all_pins];
for ( int i = 0; i < num_all_pins; i++ )
{
  sample_org[i] = my_analogRead ( all_analog_pins[i] );
}
*/

uint16_t sample_org2[num_all_pins];
for ( int i = 0; i < num_all_pins / 2; i++ )
{
  my_analogRead2 ( all_analog_pins[i], all_analog_pins[i + 6], sample_org2[i], sample_org2[i + 6] );
}


    if ( samplerate_prev_micros_cnt >= samplerate_max_cnt )
    {
      const float sample_rate = 1.0f / ( micros() - samplerate_prev_micros ) * samplerate_max_cnt * 1e6f;
      Serial.println ( sample_rate, 7 );

      samplerate_prev_micros_cnt = 0;
      samplerate_prev_micros     = micros();
    }
    samplerate_prev_micros_cnt++;
#endif


#ifdef DO_MULT_INPUT_CAPTURE_TEST
    // capture from all analog inputs

// TEST
uint16_t sample_org[num_all_pins];
for ( int i = 0; i < num_all_pins / 2; i++ )
{
  my_analogRead2 ( all_analog_pins[i], all_analog_pins[i + 6], sample_org[i], sample_org[i + 6] );
}

// TODO we have a better implementation of this in the edrumulus class
Serial.println ( String ( sample_org[0] ) + "\t" +
                 String ( sample_org[1] ) + "\t" +
                 String ( sample_org[2] ) + "\t" +
                 String ( sample_org[3] ) + "\t" +
                 String ( sample_org[4] ) + "\t" +
                 String ( sample_org[5] ) + "\t" +
                 String ( sample_org[6] ) + "\t" +
                 String ( sample_org[7] ) + "\t" +
                 String ( sample_org[8] ) + "\t" +
                 String ( sample_org[9] ) + "\t" +
                 String ( sample_org[10] ) + "\t" +
                 String ( sample_org[11] ) );

#endif

#ifdef DO_INPUT_CAPTURE
    const int sample_raw = my_analogRead ( analog_pin );

// TEST
int sample = sample_raw; // TEST

    // for debugging: send samples to Octave with binary format via serial interface
    byte send_buf[4];
    send_buf[0] = 0xFF;
    send_buf[1] = 0xFF;
    send_buf[2] = sample >> 8;   // high byte
    send_buf[3] = sample & 0xFF; // low byte
    Serial.write ( send_buf, 4 );
#endif

#ifdef DO_INPUT_BUFFER_CAPTURE

  // capture one block of samples
  const int number_samples = 200;
  uint16_t  sample_org[number_samples][num_all_pins];

  for ( int j = 0; j < number_samples; j++ )
  {
    for ( int i = 0; i < num_all_pins / 2; i++ )
    {
      // read with given sampling rate
      if ( xSemaphoreTake ( timer_semaphore, portMAX_DELAY ) == pdTRUE )
      {
        my_analogRead2 ( all_analog_pins[i], all_analog_pins[i + 6], sample_org[j][i], sample_org[j][i + 6] );
      }
    }
  }
  
  // first, transfer marker
  Serial.println ( String ( 0 ) + "\t" +
                   String ( 0 ) + "\t" +
                   String ( 0 ) + "\t" +
                   String ( 0 ) + "\t" +
                   String ( 0 ) + "\t" +
                   String ( 0 ) + "\t" +
                   String ( 0 ) + "\t" +
                   String ( 0 ) + "\t" +
                   String ( 0 ) + "\t" +
                   String ( 0 ) + "\t" +
                   String ( 0 ) + "\t" +
                   String ( 0 ) );

  // transfer complete buffer
  for ( int j = 0; j < number_samples; j++ )
  {
    Serial.println ( String ( sample_org[j][0] ) + "\t" +
                     String ( sample_org[j][1] ) + "\t" +
                     String ( sample_org[j][2] ) + "\t" +
                     String ( sample_org[j][3] ) + "\t" +
                     String ( sample_org[j][4] ) + "\t" +
                     String ( sample_org[j][5] ) + "\t" +
                     String ( sample_org[j][6] ) + "\t" +
                     String ( sample_org[j][7] ) + "\t" +
                     String ( sample_org[j][8] ) + "\t" +
                     String ( sample_org[j][9] ) + "\t" +
                     String ( sample_org[j][10] ) + "\t" +
                     String ( sample_org[j][11] ) );
  }

#endif
  }
}
