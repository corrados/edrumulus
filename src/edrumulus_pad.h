/******************************************************************************\
 * Copyright (c) 2020-2024
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

#pragma once

#include "edrumulus_hardware.h"
#include "edrumulus_util.h"


class Pad
{
  public:
    enum Epadtype // note that the enums need assigned integers for MIDI settings transfer
    {
      PD120         = 0,
      PD80R         = 1,
      PD8           = 2,
      FD8           = 3,
      VH12          = 4,
      VH12CTRL      = 5,
      KD7           = 6,
      TP80          = 7,
      CY6           = 8,
      CY8           = 9,
      DIABOLO12     = 10,
      CY5           = 11,
      HD1TOM        = 12,
      PD6           = 13,
      KD8           = 14,
      PDX8          = 15,
      KD120         = 16,
      PD5           = 17,
      PDA120LS      = 18,
      PDX100        = 19,
      KT10          = 20,
      MPS750X_TOM   = 21,
      MPS750X_SNARE = 22,
      MPS750X_KICK  = 23,
      MPS750X_RIDE  = 24,
      MPS750X_CRASH = 25,
      LEHHS12C      = 26,
      LEHHS12C_CTRL = 27
    };

    enum Ecurvetype // note that the enums need assigned integers for MIDI settings transfer
    {
      LINEAR = 0,
      EXP1   = 1,
      EXP2   = 2,
      LOG1   = 3,
      LOG2   = 4
    };

    enum Erimstate
    {
      NO_RIM,
      RIM_SHOT,
      RIM_ONLY,
      RIM_SIDE_STICK
    };

    void setup ( const int conf_Fs );

    float process_sample ( const float* input,
                            const int    input_len,
                            const int*   overload_detected,
                            bool&        peak_found,
                            int&         midi_velocity,
                            int&         midi_pos,
                            Erimstate&   rim_state,
                            bool&        is_choke_on,
                            bool&        is_choke_off );

    void process_control_sample ( const int* input,
                                  bool&      change_found,
                                  int&       midi_ctrl_value,
                                  bool&      peak_found,
                                  int&       midi_velocity );

    void set_pad_type ( const Epadtype new_pad_type );
    Epadtype get_pad_type() { return pad_settings.pad_type; }
    void set_midi_notes         ( const int new_midi_note, const int new_midi_note_rim ) { midi_note = new_midi_note; midi_note_rim = new_midi_note_rim; }
    void set_midi_notes_open    ( const int new_midi_note, const int new_midi_note_rim ) { midi_note_open = new_midi_note; midi_note_open_rim = new_midi_note_rim; }
    void set_midi_note          ( const int new_midi_note )                              { midi_note = new_midi_note; }
    int  get_midi_note          ()                                                       { return midi_note; }
    void set_midi_note_rim      ( const int new_midi_note_rim )                          { midi_note_rim = new_midi_note_rim; }
    int  get_midi_note_rim      ()                                                       { return midi_note_rim; }
    void set_midi_note_open     ( const int new_midi_note )                              { midi_note_open = new_midi_note; }
    int  get_midi_note_open     ()                                                       { return midi_note_open; }
    void set_midi_note_open_rim ( const int new_midi_note_rim )                          { midi_note_open_rim = new_midi_note_rim; }
    int  get_midi_note_open_rim ()                                                       { return midi_note_open_rim; }
    void set_midi_ctrl_ch       ( const int new_midi_ctrl_ch )                           { midi_ctrl_ch = new_midi_ctrl_ch; }
    int  get_midi_ctrl_ch       ()                                                       { return midi_ctrl_ch; }
    void set_rim_shot_is_used   ( const bool new_is_used ) { pad_settings.rim_shot_is_used = new_is_used; }
    bool get_rim_shot_is_used   ()                         { return pad_settings.rim_shot_is_used; }
    void set_pos_sense_is_used  ( const bool new_is_used ) { pad_settings.pos_sense_is_used = new_is_used; }
    bool get_pos_sense_is_used  ()                         { return pad_settings.pos_sense_is_used; }

    void set_velocity_threshold   ( const int        new_threshold ) { pad_settings.velocity_threshold = new_threshold; sched_init(); }
    int  get_velocity_threshold   ()                                 { return pad_settings.velocity_threshold; }
    void set_velocity_sensitivity ( const int        new_velocity )  { pad_settings.velocity_sensitivity = new_velocity; sched_init(); }
    int  get_velocity_sensitivity ()                                 { return pad_settings.velocity_sensitivity; }
    void set_pos_threshold        ( const int        new_threshold ) { pad_settings.pos_threshold = new_threshold; sched_init(); }
    int  get_pos_threshold        ()                                 { return pad_settings.pos_threshold; }
    void set_pos_sensitivity      ( const int        new_velocity )  { pad_settings.pos_sensitivity = new_velocity; sched_init(); }
    int  get_pos_sensitivity      ()                                 { return pad_settings.pos_sensitivity; }
    void set_rim_pos_threshold    ( const int        new_threshold ) { pad_settings.rim_pos_threshold = new_threshold; sched_init(); }
    int  get_rim_pos_threshold    ()                                 { return pad_settings.rim_pos_threshold; }
    void set_rim_pos_sensitivity  ( const int        new_velocity )  { pad_settings.rim_pos_sensitivity = new_velocity; sched_init(); }
    int  get_rim_pos_sensitivity  ()                                 { return pad_settings.rim_pos_sensitivity; }
    void set_mask_time            ( const int        new_time_ms )   { pad_settings.mask_time_ms = new_time_ms; sched_init(); }
    int  get_mask_time            ()                                 { return pad_settings.mask_time_ms; }
    void set_rim_shot_threshold   ( const int        new_threshold ) { pad_settings.rim_shot_threshold = new_threshold; sched_init(); }
    int  get_rim_shot_threshold   ()                                 { return pad_settings.rim_shot_threshold; }
    void set_rim_shot_boost       ( const int        new_boost )     { pad_settings.rim_shot_boost = new_boost; sched_init(); }
    int  get_rim_shot_boost       ()                                 { return pad_settings.rim_shot_boost; }
    void set_curve                ( const Ecurvetype new_curve )     { pad_settings.curve_type = new_curve; sched_init(); }
    Ecurvetype get_curve          ()                                 { return pad_settings.curve_type; }
    void set_cancellation         ( const int        new_cancel )    { pad_settings.cancellation = new_cancel; sched_init(); }
    int  get_cancellation         ()                                 { return pad_settings.cancellation; }
    void set_coupled_pad_idx      ( const int        new_idx )       { pad_settings.coupled_pad_idx = new_idx; sched_init(); }
    int  get_coupled_pad_idx      ()                                 { return pad_settings.coupled_pad_idx; }
    void set_head_sensor_coupling ( const bool       new_coupling )  { use_head_sensor_coupling = new_coupling; sched_init(); }
    void set_use_second_rim       ( const bool       new_sec_rim )   { use_second_rim = new_sec_rim; sched_init(); }

    float get_cancellation_factor() { return cancellation_factor; }
    bool  get_is_control()          { return pad_settings.is_control; }
    bool  get_is_rim_switch()       { return pad_settings.is_rim_switch; }
    bool  get_is_rim_switch_on()    { return sSensor[0].rim_switch_on_cnt > 0; }

    // definitions which can be used outside the pad class, too
    static const int control_midi_hysteresis       = ADC_MAX_NOISE_AMPL / 2; // MIDI hysteresis for the controller to suppress noise
    static const int hi_hat_is_open_MIDI_threshold = 100; // MIDI values smaller than the limit value are "open hi-hat"

  protected:
    struct Epadsettings
    {
      Epadtype   pad_type;
      int        velocity_threshold;   // 0..31
      int        velocity_sensitivity; // 0..31, high values give higher sensitivity
      int        mask_time_ms;         // 0..31 (ms)
      int        pos_threshold;        // 0..31
      int        pos_sensitivity;      // 0..31, high values give higher sensitivity
      int        rim_pos_threshold;    // 0..31
      int        rim_pos_sensitivity;  // 0..31, high values give higher sensitivity
      int        rim_shot_threshold;   // 0..31
      int        rim_shot_boost;       // 0..31
      int        cancellation;         // 0..31
      int        coupled_pad_idx;      // 0..[number of pads - 1]
      bool       is_control;           // whether it is a normal pad or a hi-hat control pedal
      bool       is_rim_switch;        // whether the pad supports rim/egde sensor based on a physical switch (i.e. no piezo)
      bool       pos_sense_is_used;    // switches positional sensing support on or off
      bool       rim_shot_is_used;     // switches rim shot detection on or off
      Ecurvetype curve_type;
      float      first_peak_diff_thresh_db;
      float      scan_time_ms;
      float      pre_scan_time_ms;
      float      mask_time_decay_fact_db;
      float      decay_est_delay_ms;
      float      decay_est_len_ms;
      float      decay_est_fact_db;
      float      decay_fact_db;
      float      decay_len1_ms,    decay_len2_ms,    decay_len3_ms;
      float      decay_grad_fact1, decay_grad_fact2, decay_grad_fact3;
      float      pos_low_pass_cutoff;
      bool       pos_invert;
      bool       rim_use_low_freq_bp;
      float      rim_shot_window_len_ms;
      float      clip_comp_ampmap_step;
    };

    void apply_preset_pad_settings();
    void manage_delayed_initialization();
    void initialize(); // this function should not be called directly but use sched_init() instead
    void sched_init() { init_delay_cnt = init_delay_value; }; // schedule initialization function (for delayed initialization)

    void overload_correction ( FastWriteFIFO& x_sq_hist,
                                FastWriteFIFO& overload_hist,
                                const int      first_peak_idx,
                                const int      peak_velocity_idx,
                                bool&          is_overloaded_state,
                                float&         peak_val );

    const float      init_delay_value_s = 0.2; // init delay value in seconds
    static const int max_length_ampmap  = 20;  // maxmimum number of amplification mappings for clipping compensation
    static const int ctrl_history_len   = 10;  // (MUST BE AN EVEN VALUE) control history length; the longer, the more noise reduction but more delay

    // band-pass filter coefficients (they are constant and must not be changed)
    const int   bp_filt_len           = 5;
    const float bp_filt_a[4]          = { 0.6704579059531744f, -2.930427216820138f, 4.846289804288025f, -3.586239808116909f };
    const float bp_filt_b[5]          = { 0.01658193166930305f, 0.0f, -0.0331638633386061f, 0.0f, 0.01658193166930305f };
    const float rim_bp_low_freq_a[4]  = { 0.8008026466657076f, -3.348944421626415f, 5.292099516163272f, -3.743650976941178f };
    const float rim_bp_low_freq_b[5]  = { 0.005542717210280682f, 0.0f, -0.01108543442056136f, 0.0f, 0.005542717210280682f };
    const float rim_bp_high_freq_a[4] = { 0.8008026466657077f, -3.021126408169798f, 4.637919662489649f, -3.377196335768073f };
    const float rim_bp_high_freq_b[5] = { 0.00554271721028068f, 0.0f, -0.01108543442056136f, 0.0f, 0.00554271721028068f };
    const int   x_filt_delay          = 5;

    // ADC noise scaling after band-pass filtering (e.g., for the Teensy ADC the noise has high
    // energy at high frequencies which are cut by the band-pass filter) -> hardware dependend parameter
#ifdef TEENSYDUINO
    const float ADC_noise_peak_velocity_scaling = 1.0f / 6.0f;
#else
// TODO set the correct value for the ESP32 hardware -> as a first approximation, use the Teensy value...
const float ADC_noise_peak_velocity_scaling = 1.0f / 6.0f;
#endif

    float* decay         = nullptr;
    float* lp_filt_b     = nullptr;
    float* rim_bp_filt_a = nullptr;
    float* rim_bp_filt_b = nullptr;
    float* ctrl_hist     = nullptr;

    struct SResults
    {
      int       midi_velocity;
      int       midi_pos;
      int       first_peak_delay;
      float     first_peak_sub_sample;
      Erimstate rim_state;
    };

    struct SSensor
    {
      FastWriteFIFO x_sq_hist;
      FastWriteFIFO overload_hist;
      FastWriteFIFO x_low_hist;
      FastWriteFIFO x_rim_switch_hist;
      FastWriteFIFO x_sec_rim_switch_hist;
      float* bp_filt_hist_x = nullptr;
      float* bp_filt_hist_y = nullptr;
      float* lp_filt_hist   = nullptr;
      float* rim_bp_hist_x  = nullptr;
      float* rim_bp_hist_y  = nullptr;
      float* x_rim_hist     = nullptr;

      int       mask_back_cnt;
      int       decay_back_cnt;
      int       scan_time_cnt;
      float     max_x_filt_val;
      float     max_mask_x_filt_val;
      float     first_peak_val;
      float     peak_val;
      bool      was_above_threshold;
      bool      was_peak_found;
      bool      was_pos_sense_ready;
      bool      was_rim_shot_ready;
      bool      is_overloaded_state;
      float     decay_scaling;
      int       decay_pow_est_start_cnt;
      int       decay_pow_est_cnt;
      float     decay_pow_est_sum;
      int       pos_sense_cnt;
      int       x_low_hist_idx;
      int       x_rim_hist_idx;
      int       rim_switch_on_cnt;
      int       rim_shot_cnt;
      Erimstate rim_state;
      float     pos_sense_metric;
      float     rim_pos_sense_metric;
      SResults  sResults;
    };

    class MultiHeadSensor
    {
    public:
      void initialize();

      void calculate_subsample_peak_value ( FastWriteFIFO& x_sq_hist,
                                            const int      x_sq_hist_len,
                                            const int      total_scan_time,
                                            const int      first_peak_idx,
                                            float&         first_peak_sub_sample );

      void calculate ( SSensor*   sSensor,
                        const bool sensor0_has_results,
                        const int  number_head_sensors,
                        const int  pos_sensitivity,
                        const int  pos_threshold,
                        bool&      peak_found,
                        int&       midi_velocity,
                        int&       midi_pos,
                        Erimstate& rim_state );

    protected:
      int   multiple_sensor_cnt;
      float get_pos_x0;
      float get_pos_y0;
      float get_pos_x1;
      float get_pos_y1;
      float get_pos_x2;
      float get_pos_y2;
      float get_pos_x0_sq_plus_y0_sq;
      float get_pos_a1;
      float get_pos_b1;
      float get_pos_a2;
      float get_pos_b2;
      float get_pos_div1_fact;
      float get_pos_div2_fact;
      float get_pos_rim_radius;
    };

    SSensor         sSensor[MAX_NUM_PAD_INPUTS];
    bool            use_head_sensor_coupling;
    bool            use_second_rim;
    int             Fs;
    int             number_inputs;
    int             number_head_sensors;
    int             init_delay_cnt;
    int             init_delay_value;
    int             overload_hist_len;
    int             max_num_overloads;
    int             length_ampmap;
    float           amplification_mapping[max_length_ampmap];
    int             scan_time;
    int             pre_scan_time;
    int             total_scan_time;
    int             decay_len, decay_len1, decay_len2, decay_len3;
    int             mask_time;
    int             x_sq_hist_len;
    float           threshold;
    float           velocity_factor;
    float           velocity_exponent;
    float           velocity_offset;
    float           pos_threshold;
    float           pos_range_db;
    float           rim_pos_threshold;
    float           rim_pos_range_db;
    float           control_threshold;
    float           control_range;
    float           first_peak_diff_thresh;
    int             decay_est_delay;
    int             decay_est_len;
    float           decay_est_fact;
    float           decay_fact;
    float           decay_mask_fact;
    int             x_rim_hist_len;
    int             rim_shot_window_len;
    float           rim_shot_threshold;
    float           rim_shot_boost;
    float           rim_switch_threshold;
    int             rim_switch_on_cnt_thresh;
    int             lp_filt_len;
    int             x_low_hist_len;
    Epadsettings    pad_settings;
    int             midi_note;
    int             midi_note_rim;
    int             midi_note_open;
    int             midi_note_open_rim;
    int             midi_ctrl_ch;
    int             ctrl_history_len_half;
    float           ctrl_velocity_threshold;
    float           ctrl_velocity_range_fact;
    int             prev_ctrl_value;
    float           cancellation_factor;
    float           rim_max_power_low_limit;
    MultiHeadSensor multi_head_sensor;

    // real-time debugging support
#ifdef USE_SERIAL_DEBUG_PLOTTING
# ifdef TEENSYDUINO // MIDI+Serial possible with the Teensy
    static const int debug_buffer_size    = 500;
# else
#  undef USE_MIDI // only MIDI or Serial possible with the ESP32
    static const int debug_buffer_size    = 400; // smaller size needed for ESP32
# endif
    static const int number_debug_buffers = 4;
    int              debug_buffer_idx     = 0;
    int              debug_out_cnt        = 0;
    float            debug_buffer[number_debug_buffers][debug_buffer_size];

    void DEBUG_ADD_VALUES ( const float value0,
                            const float value1,
                            const float value2,
                            const float value3 )
    {
      debug_buffer[0][debug_buffer_idx] = value0;
      debug_buffer[1][debug_buffer_idx] = value1;
      debug_buffer[2][debug_buffer_idx] = value2;
      debug_buffer[3][debug_buffer_idx] = value3;
      debug_buffer_idx++;

      if ( debug_buffer_idx == debug_buffer_size )
      {
        debug_buffer_idx = 0;
      }

      if ( debug_out_cnt == 1 )
      {
        String serial_print;
        for ( int i = debug_buffer_idx; i < debug_buffer_idx + debug_buffer_size; i++ )
        {
          for ( int j = 0; j < number_debug_buffers; j++ )
          {
            serial_print += String ( 10.0f * log10 ( max ( 1e-3f, debug_buffer[j][i % debug_buffer_size] ) ) ) + "\t";
          }
          serial_print += "\n";
        }
        Serial.println ( serial_print );
      }

      if ( debug_out_cnt > 0 )
      {
        debug_out_cnt--;
      }
    }

    void DEBUG_START_PLOTTING()
    {
      // set debug count to have the peak shortly after the start of the range
      debug_out_cnt = debug_buffer_size - debug_buffer_size / 4;
    }
#else
    void DEBUG_ADD_VALUES ( const float, const float, const float, const float ) {}
    void DEBUG_START_PLOTTING() {}
#endif
};
