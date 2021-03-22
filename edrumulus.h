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

#pragma once

#include "Arduino.h"

#define MAX_NUM_PADS         12 // a maximum of 12 pads are supported
#define MAX_NUM_PAD_INPUTS   2  // a maximum of 2 sensors per pad is supported

class Edrumulus
{
public:
  enum Epadtype
  {
    PD120,
    PD80R,
    PD8
  };

  Edrumulus();

  // call this function during the Setup function of the main program
  void setup ( const int  conf_num_pads,
               const int* conf_analog_pins,
               const int* conf_analog_pins_rim_shot );

  // call the process function during the main loop
  void process();

  // after calling the process function, query the results for each configured pad
  bool get_peak_found    ( const int pad_idx ) { return peak_found[pad_idx]; }
  int  get_midi_velocity ( const int pad_idx ) { return midi_velocity[pad_idx]; }
  int  get_midi_pos      ( const int pad_idx ) { return midi_pos[pad_idx]; }
  bool get_is_rim_shot   ( const int pad_idx ) { return is_rim_shot[pad_idx]; }

  // configure the pads
  void set_pad_type             ( const int pad_idx, const Epadtype new_pad_type ) { pad[pad_idx].set_pad_type ( new_pad_type ); }
  void set_velocity_threshold   ( const int pad_idx, const int new_threshold )     { pad[pad_idx].set_velocity_threshold ( new_threshold ); }
  void set_velocity_sensitivity ( const int pad_idx, const int new_velocity )      { pad[pad_idx].set_velocity_sensitivity ( new_velocity ); }
  void set_pos_threshold        ( const int pad_idx, const int new_threshold )     { pad[pad_idx].set_pos_threshold ( new_threshold ); }
  void set_pos_sensitivity      ( const int pad_idx, const int new_velocity )      { pad[pad_idx].set_pos_sensitivity ( new_velocity ); }
  void set_mask_time            ( const int pad_idx, const int new_time )          { pad[pad_idx].set_mask_time ( new_time ); }
  void set_rim_shot_treshold    ( const int pad_idx, const int new_threshold )     { pad[pad_idx].set_rim_shot_treshold ( new_threshold ); }

  // overload and error handling
  bool get_status_is_overload() { return status_is_overload; }
  bool get_status_is_error()    { return status_is_error; }


protected:
  class Pad
  {
    public:
      void setup ( const int conf_Fs,
                   const int conf_number_inputs = 1 );

      void process_sample ( const float* input,
                            bool&        peak_found,
                            int&         midi_velocity,
                            int&         midi_pos,
                            bool&        is_rim_shot,
                            float&       debug );

      void set_pad_type ( const Epadtype new_pad_type );

      void set_velocity_threshold   ( const int new_threshold ) { pad_settings.velocity_threshold   = new_threshold; initialize(); }
      void set_velocity_sensitivity ( const int new_velocity )  { pad_settings.velocity_sensitivity = new_velocity;  initialize(); }
      void set_pos_threshold        ( const int new_threshold ) { pad_settings.pos_threshold        = new_threshold; initialize(); }
      void set_pos_sensitivity      ( const int new_velocity )  { pad_settings.pos_sensitivity      = new_velocity;  initialize(); }
      void set_mask_time            ( const int new_time_ms )   { pad_settings.mask_time_ms         = new_time_ms;   initialize(); }
      void set_rim_shot_treshold    ( const int new_threshold ) { pad_settings.rim_shot_treshold    = new_threshold; initialize(); }


    protected:
      struct Epadsettings
      {
        Epadtype pad_type;
        int      velocity_threshold;   // 0..31
        int      velocity_sensitivity; // 0..31, high values give higher sensitivity
        int      mask_time_ms;         // 0..31 (ms)
        int      pos_threshold;        // 0..31
        int      pos_sensitivity;      // 0..31, high values give higher sensitivity
        int      rim_shot_treshold;    // 0..31
        bool     pos_sense_is_used;    // switches positional sensing support on or off
        float    energy_win_len_ms;
        float    scan_time_ms;
        float    main_peak_dist_ms;
        float    decay_est_delay2nd_ms;
        float    decay_est_len_ms;
        float    decay_est_fact_db;
        float    decay_fact_db;
        float    decay_len1_ms,    decay_len2_ms,    decay_len3_ms;
        float    decay_grad_fact1, decay_grad_fact2, decay_grad_fact3;
        float    pos_energy_win_len_ms;
        float    pos_iir_alpha;
        float    rim_shot_window_len_ms;
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

      // high pass filter coefficients used for rim shot detection (they are constant and must not be changed)
      const float b_rim_high[2] = { 0.969531252908746f, -0.969531252908746f };
      const float a_rim_high    = -0.939062505817492f;

      float* hil_hist                = nullptr;
      float* mov_av_hist_re          = nullptr;
      float* mov_av_hist_im          = nullptr;
      float* decay                   = nullptr;
      float* hist_main_peak_pow_left = nullptr;
      float* hil_hist_re             = nullptr;
      float* hil_hist_im             = nullptr;
      float* hil_low_hist_re         = nullptr;
      float* hil_low_hist_im         = nullptr;
      float* rim_x_high_hist         = nullptr;

      int          Fs;
      int          number_inputs;
      int          energy_window_len;
      int          scan_time;
      int          scan_time_cnt;
      int          decay_len, decay_len1, decay_len2, decay_len3;
      int          mask_time;
      int          mask_back_cnt;
      float        threshold;
      float        velocity_range_db;
      float        pos_threshold;
      float        pos_range_db;
      bool         was_above_threshold;
      float        prev_hil_filt_val;
      int          main_peak_dist;
      int          decay_est_delay2nd;
      int          decay_est_len;
      float        decay_est_fact;
      float        power_hypo_left;
      int          power_hypo_right_cnt;
      int          decay_pow_est_start_cnt;
      int          decay_pow_est_cnt;
      float        decay_pow_est_sum;
      float        decay_fact;
      int          decay_back_cnt;
      float        decay_scaling;
      float        alpha;
      float        rim_high_prev_x;
      float        rim_x_high;
      int          rim_shot_window_len;
      float        rim_shot_treshold_dB;
      int          pos_energy_window_len;
      int          pos_sense_cnt;
      int          rim_shot_cnt;
      float        hil_filt_max_pow;
      int          stored_midi_velocity;
      int          stored_midi_pos;
      bool         stored_is_rimshot;
      float        max_hil_filt_val;
      int          peak_found_offset;
      bool         was_peak_found;
      bool         was_pos_sense_ready;
      bool         was_rim_shot_ready;
      float        hil_low_re;
      float        hil_low_im;
      Epadsettings pad_settings;
  };

  // constant definitions
  const int dc_offset_est_len       = 5000; // samples
  const int samplerate_max_cnt      = 10000; // samples
  const int samplerate_max_error_Hz = 100; // tolerate a sample rate deviation of 100 Hz

  int           Fs;
  int           number_pads;
  int           number_inputs[MAX_NUM_PADS];
  int           analog_pin[MAX_NUM_PADS][MAX_NUM_PAD_INPUTS];
  float         dc_offset[MAX_NUM_PADS][MAX_NUM_PAD_INPUTS];
  int           overload_LED_cnt;
  int           overload_LED_on_time;
  bool          status_is_overload;
  bool          status_is_error;
  int           samplerate_prev_micros_cnt;
  unsigned long samplerate_prev_micros;
  Pad           pad[MAX_NUM_PADS];
  bool          peak_found[MAX_NUM_PADS];
  int           midi_velocity[MAX_NUM_PADS];
  int           midi_pos[MAX_NUM_PADS];
  bool          is_rim_shot[MAX_NUM_PADS];

  volatile SemaphoreHandle_t timer_semaphore;
  hw_timer_t*                timer = nullptr;
  static void IRAM_ATTR      on_timer();
};
