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

const int                  analog_pin = 34;
const int                  Fs         = 8000;
volatile SemaphoreHandle_t timer_semaphore;
hw_timer_t*                timer = nullptr;

void IRAM_ATTR on_timer()
{
  // tell the main loop that a sample can be read by setting the semaphore
  xSemaphoreGiveFromISR ( timer_semaphore, NULL );
}

void setup()
{
  Serial.begin ( 115200 );

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
    const int sample = analogRead ( analog_pin );

    // for debugging: send samples to Octave with binary format via serial interface
    byte send_buf[4];
    send_buf[0] = 0xFF;
    send_buf[1] = 0xFF;
    send_buf[2] = sample >> 8;   // high byte
    send_buf[3] = sample & 0xFF; // low byte
    Serial.write ( send_buf, 4 );
  }
}
