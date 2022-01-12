/******************************************************************************\
 * Copyright (c) 2020-2022
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

#define VERSION_MAJOR   0
#define VERSION_MINOR   3

//#define USE_SERIAL_DEBUG_PLOTTING

#include "Arduino.h"
#include "edrumulus_hardware.h"

class Edrumulus
{
public:
  enum Epadtype // note that the enums need assigned integers for MIDI settings transfer
  {
// TODO if new pads are added, check if get_is_control() and get_is_rim_switch() must be updated
    PD120     = 0,
    PD80R     = 1,
    PD8       = 2,
    FD8       = 3, // control pedal
    VH12      = 4,
    VH12CTRL  = 5,
    KD7       = 6,
    TP80      = 7,
    CY6       = 8,
    CY8       = 9,
    DIABOLO12 = 10,
    CY5       = 11,
    HD1TOM    = 12,
    PD6       = 13,
    KD8       = 14,
    PDX8      = 15,
    KD120     = 16,
    PD5       = 17
  };

  enum Ecurvetype // note that the enums need assigned integers for MIDI settings transfer
  {
    LINEAR = 0,
    EXP1   = 1,
    EXP2   = 2,
    LOG1   = 3,
    LOG2   = 4
  };

  Edrumulus();

  // call this function during the Setup function of the main program
  void setup ( const int  conf_num_pads,
               const int* conf_analog_pins,
               const int* conf_analog_pins_rim_shot );

  // call the process function during the main loop
  void process();

  // after calling the process function, query the results for each configured pad
  bool get_peak_found          ( const int pad_idx ) { return peak_found[pad_idx]; }
  bool get_choke_on_found      ( const int pad_idx ) { return !pad[pad_idx].get_is_control() && is_choke_on[pad_idx]; }
  bool get_choke_off_found     ( const int pad_idx ) { return !pad[pad_idx].get_is_control() && is_choke_off[pad_idx]; }
  bool get_control_found       ( const int pad_idx ) { return pad[pad_idx].get_is_control() && control_found[pad_idx]; }
  int  get_midi_velocity       ( const int pad_idx ) { return midi_velocity[pad_idx]; }
  int  get_midi_pos            ( const int pad_idx ) { return midi_pos[pad_idx]; }
  int  get_midi_note           ( const int pad_idx ) { return is_rim_shot[pad_idx] ? pad[pad_idx].get_midi_note_rim() : pad[pad_idx].get_midi_note(); }
  int  get_midi_note_norm      ( const int pad_idx ) { return pad[pad_idx].get_midi_note(); }
  int  get_midi_note_rim       ( const int pad_idx ) { return pad[pad_idx].get_midi_note_rim(); }
  int  get_midi_note_open      ( const int pad_idx ) { return is_rim_shot[pad_idx] ? pad[pad_idx].get_midi_note_open_rim() : pad[pad_idx].get_midi_note_open(); }
  int  get_midi_note_open_norm ( const int pad_idx ) { return pad[pad_idx].get_midi_note_open(); }
  int  get_midi_note_open_rim  ( const int pad_idx ) { return pad[pad_idx].get_midi_note_open_rim(); }
  int  get_midi_ctrl_ch        ( const int pad_idx ) { return pad[pad_idx].get_midi_ctrl_ch(); }
  int  get_midi_ctrl_value     ( const int pad_idx ) { return midi_ctrl_value[pad_idx]; }
  bool get_midi_ctrl_is_open   ( const int pad_idx ) { return midi_ctrl_value[pad_idx] < Pad::hi_hat_is_open_MIDI_threshold; }

  // configure the pads
  void set_pad_type             ( const int pad_idx, const Epadtype   new_pad_type )  { pad[pad_idx].set_pad_type ( new_pad_type ); }
  Epadtype get_pad_type         ( const int pad_idx )                                 { return pad[pad_idx].get_pad_type(); }
  void set_velocity_threshold   ( const int pad_idx, const int        new_threshold ) { pad[pad_idx].set_velocity_threshold ( new_threshold ); }
  int  get_velocity_threshold   ( const int pad_idx )                                 { return pad[pad_idx].get_velocity_threshold(); }
  void set_velocity_sensitivity ( const int pad_idx, const int        new_velocity )  { pad[pad_idx].set_velocity_sensitivity ( new_velocity ); }
  int  get_velocity_sensitivity ( const int pad_idx )                                 { return pad[pad_idx].get_velocity_sensitivity(); }
  void set_pos_threshold        ( const int pad_idx, const int        new_threshold ) { pad[pad_idx].set_pos_threshold ( new_threshold ); }
  int  get_pos_threshold        ( const int pad_idx )                                 { return pad[pad_idx].get_pos_threshold(); }
  void set_pos_sensitivity      ( const int pad_idx, const int        new_velocity )  { pad[pad_idx].set_pos_sensitivity ( new_velocity ); }
  int  get_pos_sensitivity      ( const int pad_idx )                                 { return pad[pad_idx].get_pos_sensitivity(); }
  void set_mask_time            ( const int pad_idx, const int        new_time )      { pad[pad_idx].set_mask_time ( new_time ); }
  int  get_mask_time            ( const int pad_idx )                                 { return pad[pad_idx].get_mask_time(); }
  void set_rim_shot_treshold    ( const int pad_idx, const int        new_threshold ) { pad[pad_idx].set_rim_shot_treshold ( new_threshold ); }
  int  get_rim_shot_treshold    ( const int pad_idx )                                 { return pad[pad_idx].get_rim_shot_treshold(); }
  void set_curve                ( const int pad_idx, const Ecurvetype new_curve )     { pad[pad_idx].set_curve ( new_curve ); }
  Ecurvetype get_curve          ( const int pad_idx )                                 { return pad[pad_idx].get_curve(); }
  void set_cancellation         ( const int pad_idx, const int        new_cancel )    { pad[pad_idx].set_cancellation ( new_cancel ); }
  int  get_cancellation         ( const int pad_idx )                                 { return pad[pad_idx].get_cancellation(); }

  void set_midi_notes           ( const int pad_idx, const int new_midi_note, const int new_midi_note_rim ) { pad[pad_idx].set_midi_notes ( new_midi_note, new_midi_note_rim ); }
  void set_midi_notes_open      ( const int pad_idx, const int new_midi_note, const int new_midi_note_rim ) { pad[pad_idx].set_midi_notes_open ( new_midi_note, new_midi_note_rim ); }
  void set_midi_ctrl_ch         ( const int pad_idx, const int new_midi_ctrl_ch )                           { pad[pad_idx].set_midi_ctrl_ch ( new_midi_ctrl_ch ); }
  void set_rim_shot_is_used     ( const int pad_idx, const bool new_is_used ) { pad[pad_idx].set_rim_shot_is_used ( new_is_used ); }
  void set_pos_sense_is_used    ( const int pad_idx, const bool new_is_used ) { pad[pad_idx].set_pos_sense_is_used ( new_is_used ); }
  void set_spike_cancel_level   ( const int new_level )                       { spike_cancel_level = new_level; }
  int  get_spike_cancel_level   ()                                            { return spike_cancel_level; }

  // overload and error handling
  bool get_status_is_overload() { return status_is_overload; }
  bool get_status_is_error()    { return status_is_error; }

  // query functions
  bool get_pos_sense_is_used ( const int pad_idx ) { return pad[pad_idx].get_pos_sense_is_used(); }

protected:
  class Pad
  {
    public:
      void setup ( const int conf_Fs,
                   const int conf_number_inputs = 1 );

      float process_sample ( const float* input,
                             const bool   overload_detected,
                             bool&        peak_found,
                             int&         midi_velocity,
                             int&         midi_pos,
                             bool&        is_rim_shot,
                             bool&        is_choke_on,
                             bool&        is_choke_off );

      void process_control_sample ( const int* input,
                                    bool&      change_found,
                                    int&       midi_ctrl_value,
                                    bool&      peak_found,
                                    int&       midi_velocity );

      void set_pad_type          ( const Epadtype new_pad_type );
      Epadtype get_pad_type()    { return pad_settings.pad_type; }
      void set_midi_notes        ( const int new_midi_note, const int new_midi_note_rim ) { midi_note = new_midi_note; midi_note_rim = new_midi_note_rim; }
      void set_midi_notes_open   ( const int new_midi_note, const int new_midi_note_rim ) { midi_note_open = new_midi_note; midi_note_open_rim = new_midi_note_rim; }
      void set_midi_ctrl_ch      ( const int new_midi_ctrl_ch )                           { midi_ctrl_ch = new_midi_ctrl_ch; }
      void set_rim_shot_is_used  ( const bool new_is_used ) { pad_settings.rim_shot_is_used = new_is_used; }
      void set_pos_sense_is_used ( const bool new_is_used ) { pad_settings.pos_sense_is_used = new_is_used; }

      void set_velocity_threshold   ( const int        new_threshold ) { pad_settings.velocity_threshold = new_threshold; initialize(); }
      int  get_velocity_threshold   ()                                 { return pad_settings.velocity_threshold; }
      void set_velocity_sensitivity ( const int        new_velocity )  { pad_settings.velocity_sensitivity = new_velocity; initialize(); }
      int  get_velocity_sensitivity ()                                 { return pad_settings.velocity_sensitivity; }
      void set_pos_threshold        ( const int        new_threshold ) { pad_settings.pos_threshold = new_threshold; initialize(); }
      int  get_pos_threshold        ()                                 { return pad_settings.pos_threshold; }
      void set_pos_sensitivity      ( const int        new_velocity )  { pad_settings.pos_sensitivity = new_velocity; initialize(); }
      int  get_pos_sensitivity      ()                                 { return pad_settings.pos_sensitivity; }
      void set_mask_time            ( const int        new_time_ms )   { pad_settings.mask_time_ms = new_time_ms; initialize(); }
      int  get_mask_time            ()                                 { return pad_settings.mask_time_ms; }
      void set_rim_shot_treshold    ( const int        new_threshold ) { pad_settings.rim_shot_treshold = new_threshold; initialize(); }
      int  get_rim_shot_treshold    ()                                 { return pad_settings.rim_shot_treshold; }
      void set_curve                ( const Ecurvetype new_curve )     { pad_settings.curve_type = new_curve; initialize(); }
      Ecurvetype get_curve          ()                                 { return pad_settings.curve_type; }
      void set_cancellation         ( const int        new_cancel )    { pad_settings.cancellation = new_cancel; initialize(); }
      int  get_cancellation         ()                                 { return pad_settings.cancellation; }

      int   get_midi_note()           { return midi_note; }
      int   get_midi_note_rim()       { return midi_note_rim; }
      int   get_midi_note_open()      { return midi_note_open; }
      int   get_midi_note_open_rim()  { return midi_note_open_rim; }
      int   get_midi_ctrl_ch()        { return midi_ctrl_ch; }
      float get_cancellation_factor() { return cancellation_factor; }
      bool  get_is_control()          { return ( pad_settings.pad_type == FD8 ) ||
                                               ( pad_settings.pad_type == VH12CTRL ); } // TODO check if new pads must be added here
      bool  get_is_rim_switch()       { return ( pad_settings.pad_type == PD6 ) ||
                                               ( pad_settings.pad_type == PD8 ) ||
                                               ( pad_settings.pad_type == VH12 ) ||
                                               ( pad_settings.pad_type == TP80 ) ||
                                               ( pad_settings.pad_type == CY5 ) ||
                                               ( pad_settings.pad_type == CY6 ) ||
                                               ( pad_settings.pad_type == CY8 ); } // TODO check if new pads must be added here
      bool  get_pos_sense_is_used()   { return pad_settings.pos_sense_is_used; }

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
        int        rim_shot_treshold;    // 0..31
        int        cancellation;         // 0..31
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
        int        rim_shot_velocity_thresh;
      };

      void apply_preset_pad_settings();
      void initialize();

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

      float* bp_filt_hist_x    = nullptr;
      float* bp_filt_hist_y    = nullptr;
      float* x_sq_hist         = nullptr;
      float* decay             = nullptr;
      float* lp_filt_b         = nullptr;
      float* x_low_hist        = nullptr;
      float* lp_filt_hist      = nullptr;
      float* rim_bp_hist_x     = nullptr;
      float* rim_bp_hist_y     = nullptr;
      float* rim_bp_filt_a     = nullptr;
      float* rim_bp_filt_b     = nullptr;
      float* x_rim_hist        = nullptr;
      float* x_rim_switch_hist = nullptr;
      float* ctrl_hist         = nullptr;
      float* overload_hist     = nullptr;

      int          Fs;
      int          number_inputs;
      int          overload_hist_len;
      int          max_num_overloads;
      bool         is_overloaded_state;
      int          scan_time;
      int          scan_time_cnt;
      int          pre_scan_time;
      int          total_scan_time;
      int          decay_len, decay_len1, decay_len2, decay_len3;
      int          mask_time;
      int          mask_back_cnt;
      int          x_sq_hist_len;
      float        threshold;
      float        velocity_factor;
      float        velocity_exponent;
      float        velocity_offset;
      float        pos_threshold;
      float        pos_range_db;
      float        control_threshold;
      float        control_range;
      float        first_peak_diff_thresh;
      float        first_peak_val;
      float        peak_val;
      bool         was_above_threshold;
      int          decay_est_delay;
      int          decay_est_len;
      float        decay_est_fact;
      int          decay_pow_est_start_cnt;
      int          decay_pow_est_cnt;
      float        decay_pow_est_sum;
      float        decay_fact;
      int          decay_back_cnt;
      float        decay_scaling;
      float        decay_mask_fact;
      int          x_rim_hist_len;
      int          x_rim_hist_idx;
      int          rim_shot_window_len;
      float        rim_shot_treshold_dB;
      float        rim_switch_treshold;
      int          rim_switch_on_cnt;
      int          rim_switch_on_cnt_thresh;
      int          lp_filt_len;
      int          x_low_hist_len;
      int          x_low_hist_idx;
      int          pos_sense_cnt;
      int          rim_shot_cnt;
      int          stored_midi_velocity;
      int          stored_midi_pos;
      bool         stored_is_rimshot;
      float        max_x_filt_val;
      float        max_mask_x_filt_val;
      bool         was_peak_found;
      bool         was_pos_sense_ready;
      bool         was_rim_shot_ready;
      Epadsettings pad_settings;
      int          midi_note;
      int          midi_note_rim;
      int          midi_note_open;
      int          midi_note_open_rim;
      int          midi_ctrl_ch;
      int          ctrl_history_len;
      int          ctrl_velocity_threshold;
      float        ctrl_velocity_range_fact;
      int          prev_ctrl_value;
      float        cancellation_factor;

      // real-time debugging support
#ifdef USE_SERIAL_DEBUG_PLOTTING
# ifndef TEENSYDUINO // MIDI+Serial possible with the Teensy
#  undef USE_MIDI // only MIDI or Serial possible with the ESP32
# endif
      static const int debug_buffer_size    = 500;
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
              serial_print += String ( 10.0f * log10 ( debug_buffer[j][i % debug_buffer_size] ) ) + "\t";
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
        // set debug count to have the peak in the middle of the range
        debug_out_cnt = debug_buffer_size - debug_buffer_size / 2;
      }
#else
      void DEBUG_ADD_VALUES ( const float, const float, const float, const float ) {}
      void DEBUG_START_PLOTTING() {}
#endif
  };

  // constant definitions
  const int dc_offset_est_len       = 10000; // samples (about a second at 8 kHz sampling rate)
  const int samplerate_max_cnt      = 10000; // samples
  const int samplerate_max_error_Hz = 100;   // tolerate a sample rate deviation of 100 Hz
  const int cancel_time_ms          = 30;    // on same stand approx. 10 ms + some margin (20 ms)

#ifdef ESP_PLATFORM
  // for ESP we have a coupling of ADC inputs so that a hi-hat control pedal movement may
  // influence the DC offset of some pad inputs, therefore we need to adapt faster to
  // compensate for this
  const int dc_offset_iir_tau_seconds = 5;  // DC offset update IIR filter tau in seconds
#else
  const int dc_offset_iir_tau_seconds = 30; // DC offset update IIR filter tau in seconds
#endif

  int                Fs;
  Edrumulus_hardware edrumulus_hardware;
  int                number_pads;
  int                number_inputs[MAX_NUM_PADS];
  int                analog_pin[MAX_NUM_PADS][MAX_NUM_PAD_INPUTS];
  double             dc_offset[MAX_NUM_PADS][MAX_NUM_PAD_INPUTS]; // must be double type for IIR filter
  int                sample_org[MAX_NUM_PADS][MAX_NUM_PAD_INPUTS];
  float              dc_offset_iir_gamma;
  float              dc_offset_iir_one_minus_gamma;
  int                spike_cancel_level;
  int                overload_LED_cnt;
  int                overload_LED_on_time;
  bool               status_is_overload;
  bool               status_is_error;
  int                samplerate_prev_micros_cnt;
  unsigned long      samplerate_prev_micros;
  Pad                pad[MAX_NUM_PADS];
  bool               peak_found[MAX_NUM_PADS];
  bool               control_found[MAX_NUM_PADS];
  int                midi_velocity[MAX_NUM_PADS];
  int                midi_pos[MAX_NUM_PADS];
  int                midi_ctrl_value[MAX_NUM_PADS];
  bool               is_rim_shot[MAX_NUM_PADS];
  bool               is_choke_on[MAX_NUM_PADS];
  bool               is_choke_off[MAX_NUM_PADS];
  int                cancel_num_samples;
  int                cancel_cnt;
  int                cancel_MIDI_velocity;
  int                cancel_pad_index;
};


// Utility functions -----------------------------------------------------------------

inline void update_fifo ( const float input,
                          const int   fifo_length,
                          float*      fifo_memory )
{
  // move all values in the history one step back and put new value on the top
  for ( int i = 0; i < fifo_length - 1; i++ )
  {
    fifo_memory[i] = fifo_memory[i + 1];
  }
  fifo_memory[fifo_length - 1] = input;
}

inline void allocate_initialize ( float**   array_memory,
                                  const int array_length )
{
  // (delete and) allocate memory
  if ( *array_memory != nullptr )
  {
    delete[] *array_memory;
  }

  *array_memory = new float[array_length];

  // initialization values
  for ( int i = 0; i < array_length; i++ )
  {
    ( *array_memory )[i] = 0.0f;
  }
}
