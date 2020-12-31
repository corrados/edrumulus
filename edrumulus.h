/******************************************************************************\
 * Copyright (c) 2020-2020
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

#pragma once

#include "Arduino.h"

#define MAX_NUM_PAD_INPUTS   2 // a maximum of 2 sensors per pad supported

class Edrumulus
{
public:
  Edrumulus();

  // call this function during the Setup function of the main program
  void setup ( const int conf_analog_pin,
               const int conf_analog_pin_rim_shot = -1,
               const int conf_overload_LED_pin = -1 ); // per default no overload LED is used

  // call the process function during the main loop
  // if a MIDI note is ready, the function returns true
  bool process ( int&  midi_velocity,
                 int&  midi_pos,
                 bool& is_rim_shot );


protected:
  class Pad
  {
    public:
      enum Epadtype
      {
        PD120
      };

      void setup ( const int conf_Fs,
                   const int conf_number_inputs = 1 );

      void process_sample ( const float* input,
                            bool&        peak_found,
                            int&         midi_velocity,
                            int&         midi_pos,
                            bool&        is_rim_shot,
                            float&       debug );

      void set_velocity_sensitivity ( const byte new_velocity ) { pad_settings.velocity_sensitivity = new_velocity; initialize(); }


    protected:
      struct Epadsettings
      {
        Epadtype pad_type;
        byte     velocity_threshold;   // 0-31
        byte     velocity_sensitivity; // 0-31, high value gives higher sensitivity
      };

      void initialize();
      void update_fifo ( const float input,
                         const int   fifo_length,
                         float*      fifo_memory );

      // Hilbert filter coefficients (they are constant and must not be changed)
      const int   hil_filt_len = 7;
      const float a_re[7]      = { -0.037749783581601f, -0.069256807147465f, -1.443799477299919f,  2.473967088799056f,
                                    0.551482327389238f, -0.224119735833791f, -0.011665324660691f };
      const float a_im[7]      = {  0.0f,                0.213150535195075f, -1.048981722170302f, -1.797442302898130f,
                                    1.697288080048948f,  0.0f,                0.035902177664014f };
    
      float* hil_hist             = nullptr;
      float* rim_hil_hist         = nullptr;
      float* mov_av_hist_re       = nullptr;
      float* mov_av_hist_im       = nullptr;
      float* decay                = nullptr;
      float* hil_hist_re          = nullptr;
      float* hil_hist_im          = nullptr;
      float* hil_low_hist_re      = nullptr;
      float* hil_low_hist_im      = nullptr;
      float* peak_energy_hist     = nullptr;
      float* peak_energy_low_hist = nullptr;
      float* rim_hil_hist_re      = nullptr;
      float* rim_hil_hist_im      = nullptr;

      int          Fs;
      int          number_inputs;
      int          energy_window_len;
      int          scan_time;
      int          scan_time_cnt;
      int          decay_len;
      int          mask_time;
      int          mask_back_cnt;
      float        threshold;
      float        velocity_range_db;
      bool         was_above_threshold;
      float        prev_hil_filt_val;
      float        prev_hil_filt_decay_val;
      float        decay_fact;
      int          decay_back_cnt;
      float        decay_scaling;
      float        alpha;
      int          peak_energy_hist_len;
      int          rim_shot_window_len;
      float        rim_shot_threshold;
      int          pos_sense_cnt;
      int          rim_shot_cnt;
      int          stored_midi_velocity;
      int          stored_midi_pos;
      float        max_hil_filt_val;
      float        max_hil_filt_decay_val;
      int          peak_found_offset;
      float        hil_low_re;
      float        hil_low_im;
      Epadsettings pad_settings;
  };

  int   Fs;
  int   number_inputs;
  int   analog_pin[MAX_NUM_PAD_INPUTS];
  float dc_offset[MAX_NUM_PAD_INPUTS];
  int   overload_LED_pin;
  int   overload_LED_cnt;
  int   overload_LED_on_time;
  Pad   pad; // note: must be located after Fs variable

  volatile SemaphoreHandle_t timer_semaphore;
  hw_timer_t*                timer = nullptr;
  static void IRAM_ATTR      on_timer();
};
