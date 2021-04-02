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

const int                  analog_pin = 25;
const int                  Fs         = 8000;
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

void setup()
{
  Serial.begin ( 500000 );

  // estimate the DC offset
  float dc_offset_sum = 0.0f;

  for ( int i = 0; i < dc_offset_est_len; i++ )
  {
    dc_offset_sum += analogRead ( analog_pin );
    delayMicroseconds ( 100 );
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
    const int sample_raw = analogRead ( analog_pin );

// TEST
int sample = sample_raw; // TEST
/*
// remove single spikes
const int noise_threshold    = 6;
const int max_peak_threshold = 100;

const int sample_abs_no_dc = abs ( sample_raw - static_cast<int> ( dc_offset ) );

const bool remove_spike = ( prvious_sample2 < noise_threshold ) &&
                          ( ( prvious_sample1 > noise_threshold ) && ( prvious_sample1 < max_peak_threshold ) ) &&
                          ( sample_abs_no_dc < noise_threshold );

prvious_sample2     = prvious_sample1;
prvious_sample1     = sample_abs_no_dc;
prvious_sample1_out = sample_raw;

int sample = 2000;//dc_offset;
if ( !remove_spike )
{
  sample = prvious_sample1_out;
}
*/


    // for debugging: send samples to Octave with binary format via serial interface
    byte send_buf[4];
    send_buf[0] = 0xFF;
    send_buf[1] = 0xFF;
    send_buf[2] = sample >> 8;   // high byte
    send_buf[3] = sample & 0xFF; // low byte
    Serial.write ( send_buf, 4 );
  }
}
