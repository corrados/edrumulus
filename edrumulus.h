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

#include "Arduino.h"
#include "common.h"
#include "hardware.h"
#include "pad.h"

class Edrumulus
{
 public:
  Edrumulus();

  // call this function during the Setup function of the main program
  void setup(const int  conf_num_pads,
             const int* conf_analog_pins,
             const int* conf_analog_pins_rim_shot);

  // call the process function during the main loop
  void process();

  // after calling the process function, query the results for each configured pad
  bool get_peak_found(const int pad_idx) { return peak_found[pad_idx]; }
  bool get_choke_on_found(const int pad_idx) { return !pad[pad_idx].get_is_control() && is_choke_on[pad_idx]; }
  bool get_choke_off_found(const int pad_idx) { return !pad[pad_idx].get_is_control() && is_choke_off[pad_idx]; }
  bool get_control_found(const int pad_idx) { return pad[pad_idx].get_is_control() && control_found[pad_idx]; }
  int  get_midi_velocity(const int pad_idx) { return midi_velocity[pad_idx]; }
  int  get_midi_pos(const int pad_idx) { return midi_pos[pad_idx]; }
  int  get_midi_note(const int pad_idx) { return rim_state[pad_idx] == Pad::RIM_SHOT ? pad[pad_idx].get_midi_note_rim() : rim_state[pad_idx] == Pad::RIM_ONLY     ? pad[pad_idx].get_midi_note_open_rim()
                                                                                                                      : rim_state[pad_idx] == Pad::RIM_SIDE_STICK ? pad[pad_idx].get_midi_note_open()
                                                                                                                                                                  : pad[pad_idx].get_midi_note(); }
  int  get_midi_note_norm(const int pad_idx) { return pad[pad_idx].get_midi_note(); }
  int  get_midi_note_rim(const int pad_idx) { return pad[pad_idx].get_midi_note_rim(); }
  int  get_midi_note_open(const int pad_idx) { return rim_state[pad_idx] == Pad::RIM_SHOT ? pad[pad_idx].get_midi_note_open_rim() : pad[pad_idx].get_midi_note_open(); }
  int  get_midi_note_open_norm(const int pad_idx) { return pad[pad_idx].get_midi_note_open(); }
  int  get_midi_note_open_rim(const int pad_idx) { return pad[pad_idx].get_midi_note_open_rim(); }
  int  get_midi_ctrl_ch(const int pad_idx) { return pad[pad_idx].get_midi_ctrl_ch(); }
  int  get_midi_ctrl_value(const int pad_idx) { return midi_ctrl_value[pad_idx]; }
  bool get_midi_ctrl_is_open(const int pad_idx) { return midi_ctrl_value[pad_idx] < Pad::hi_hat_is_open_MIDI_threshold; }

  // configure the pads
  void set_pad_type(const int pad_idx, const Pad::Epadtype new_pad_type)
  {
    set_coupled_pad_idx(pad_idx, 0 /* disable possible previous coupling first */);
    pad[pad_idx].set_pad_type(new_pad_type);
  }
  Pad::Epadtype   get_pad_type(const int pad_idx) { return pad[pad_idx].get_pad_type(); }
  void            set_velocity_threshold(const int pad_idx, const int new_threshold) { pad[pad_idx].set_velocity_threshold(new_threshold); }
  int             get_velocity_threshold(const int pad_idx) { return pad[pad_idx].get_velocity_threshold(); }
  void            set_velocity_sensitivity(const int pad_idx, const int new_velocity) { pad[pad_idx].set_velocity_sensitivity(new_velocity); }
  int             get_velocity_sensitivity(const int pad_idx) { return pad[pad_idx].get_velocity_sensitivity(); }
  void            set_pos_threshold(const int pad_idx, const int new_threshold) { pad[pad_idx].set_pos_threshold(new_threshold); }
  int             get_pos_threshold(const int pad_idx) { return pad[pad_idx].get_pos_threshold(); }
  void            set_pos_sensitivity(const int pad_idx, const int new_velocity) { pad[pad_idx].set_pos_sensitivity(new_velocity); }
  int             get_pos_sensitivity(const int pad_idx) { return pad[pad_idx].get_pos_sensitivity(); }
  void            set_rim_pos_threshold(const int pad_idx, const int new_threshold) { pad[pad_idx].set_rim_pos_threshold(new_threshold); }
  int             get_rim_pos_threshold(const int pad_idx) { return pad[pad_idx].get_rim_pos_threshold(); }
  void            set_rim_pos_sensitivity(const int pad_idx, const int new_velocity) { pad[pad_idx].set_rim_pos_sensitivity(new_velocity); }
  int             get_rim_pos_sensitivity(const int pad_idx) { return pad[pad_idx].get_rim_pos_sensitivity(); }
  void            set_mask_time(const int pad_idx, const int new_time) { pad[pad_idx].set_mask_time(new_time); }
  int             get_mask_time(const int pad_idx) { return pad[pad_idx].get_mask_time(); }
  void            set_rim_shot_threshold(const int pad_idx, const int new_threshold) { pad[pad_idx].set_rim_shot_threshold(new_threshold); }
  int             get_rim_shot_threshold(const int pad_idx) { return pad[pad_idx].get_rim_shot_threshold(); }
  void            set_rim_shot_boost(const int pad_idx, const int new_boost) { pad[pad_idx].set_rim_shot_boost(new_boost); }
  int             get_rim_shot_boost(const int pad_idx) { return pad[pad_idx].get_rim_shot_boost(); }
  void            set_curve(const int pad_idx, const Pad::Ecurvetype new_curve) { pad[pad_idx].set_curve(new_curve); }
  Pad::Ecurvetype get_curve(const int pad_idx) { return pad[pad_idx].get_curve(); }
  void            set_cancellation(const int pad_idx, const int new_cancel) { pad[pad_idx].set_cancellation(new_cancel); }
  int             get_cancellation(const int pad_idx) { return pad[pad_idx].get_cancellation(); }
  void            set_coupled_pad_idx(const int pad_idx, const int new_idx);
  int             get_coupled_pad_idx(const int pad_idx) { return pad[pad_idx].get_coupled_pad_idx(); }

  void set_midi_notes(const int pad_idx, const int new_midi_note, const int new_midi_note_rim) { pad[pad_idx].set_midi_notes(new_midi_note, new_midi_note_rim); }
  void set_midi_note_norm(const int pad_idx, const int new_midi_note) { pad[pad_idx].set_midi_note(new_midi_note); }
  void set_midi_note_rim(const int pad_idx, const int new_midi_note_rim) { pad[pad_idx].set_midi_note_rim(new_midi_note_rim); }
  void set_midi_notes_open(const int pad_idx, const int new_midi_note, const int new_midi_note_rim) { pad[pad_idx].set_midi_notes_open(new_midi_note, new_midi_note_rim); }
  void set_midi_note_open_norm(const int pad_idx, const int new_midi_note) { pad[pad_idx].set_midi_note_open(new_midi_note); }
  void set_midi_note_open_rim(const int pad_idx, const int new_midi_note_rim) { pad[pad_idx].set_midi_note_open_rim(new_midi_note_rim); }
  void set_midi_ctrl_ch(const int pad_idx, const int new_midi_ctrl_ch) { pad[pad_idx].set_midi_ctrl_ch(new_midi_ctrl_ch); }
  void set_rim_shot_is_used(const int pad_idx, const bool new_is_used) { pad[pad_idx].set_rim_shot_is_used(new_is_used); }
  bool get_rim_shot_is_used(const int pad_idx) { return pad[pad_idx].get_rim_shot_is_used(); }
  void set_pos_sense_is_used(const int pad_idx, const bool new_is_used) { pad[pad_idx].set_pos_sense_is_used(new_is_used); }
  bool get_pos_sense_is_used(const int pad_idx) { return pad[pad_idx].get_pos_sense_is_used(); }

  void set_spike_cancel_level(const int new_level) { spike_cancel_level = new_level; }
  int  get_spike_cancel_level() { return spike_cancel_level; }

  void set_enable_load_indicator(const int new_status) { load_indicator = new_status != 0; }

  // error and overload handling (implement blinking LED for error using error_LED_blink_time)
  bool get_status_is_error() { return status_is_error && ((error_LED_cnt % error_LED_blink_time) < (error_LED_blink_time / 2)); }
  bool get_status_is_overload() { return status_is_overload; }
  int  get_status_dc_offset_error_channel() { return dc_offset_error_channel; }

  // persistent settings storage
  void write_setting(const int pad_index, const int address, const byte value) { edrumulus_hardware.write_setting(pad_index, address, value); }
  byte read_setting(const int pad_index, const int address) { return edrumulus_hardware.read_setting(pad_index, address); }

 protected:
  // constant definitions
  const int   Fs                       = 8000;  // this is the most fundamental system parameter: system sampling rate
  const float dc_offset_est_len_s      = 1.25f; // length of initial DC offset estimation in seconds
  const int   samplerate_max_cnt_len_s = 1.25f; // time interval for sampling rate estimation in seconds
  const int   samplerate_max_error_Hz  = 200;   // tolerate a sample rate deviation of 200 Hz
  const float dc_offset_max_rel_error  = 0.25f; // DC offset limit from ADC middle position, where offset is defined relative to ADC maximum value
  const int   cancel_time_ms           = 30;    // on same stand approx. 10 ms + some margin (20 ms)
  const float overload_LED_on_time_s   = 0.25f; // minimum overload LED on time (e.g., 250 ms)
  const float error_LED_blink_time_s   = 0.25f; // LED blink time on error (e.g., 250 ms)

#ifdef ESP_PLATFORM
  // for ESP we have a coupling of ADC inputs so that a hi-hat control pedal movement may
  // influence the DC offset of some pad inputs, therefore we need to adapt faster to
  // compensate for this
  const int dc_offset_iir_tau_seconds = 5; // DC offset update IIR filter tau in seconds
#else
  const int dc_offset_iir_tau_seconds = 30; // DC offset update IIR filter tau in seconds
#endif

  Edrumulus_hardware edrumulus_hardware;
  int                number_pads;
  bool               any_coupling_used;
  int                coupled_pad_idx_primary;
  int                coupled_pad_idx_secondary;
  int                coupled_pad_idx_rim_primary;
  int                coupled_pad_idx_rim_secondary;
  int                number_inputs[MAX_NUM_PADS];
  int                analog_pin[MAX_NUM_PADS][MAX_NUM_PAD_INPUTS];
  float              sample[MAX_NUM_PAD_INPUTS];
  float              stored_sample_coupled_head[MAX_NUM_PAD_INPUTS];
  float              stored_sample_coupled_rim[MAX_NUM_PAD_INPUTS];
  int                overload_detected[MAX_NUM_PAD_INPUTS];
  int                stored_overload_detected_coupled_head[MAX_NUM_PAD_INPUTS];
  int                stored_overload_detected_coupled_rim[MAX_NUM_PAD_INPUTS];
  double             dc_offset[MAX_NUM_PADS][MAX_NUM_PAD_INPUTS]; // must be double type for IIR filter
  int                sample_org[MAX_NUM_PADS][MAX_NUM_PAD_INPUTS];
  int                dc_offset_est_len;
  float              dc_offset_iir_gamma;
  float              dc_offset_iir_one_minus_gamma;
  int                spike_cancel_level;
  bool               load_indicator;
  int                overload_LED_cnt;
  int                overload_LED_on_time;
  int                error_LED_cnt;
  int                error_LED_blink_time;
  bool               status_is_overload;
  bool               status_is_error;
  int                dc_offset_error_channel;
  int                samplerate_max_cnt;
  int                samplerate_prev_micros_cnt;
  unsigned long      samplerate_prev_micros;
  int                dc_offset_min_limit;
  int                dc_offset_max_limit;
  Pad                pad[MAX_NUM_PADS];
  bool               peak_found[MAX_NUM_PADS];
  bool               control_found[MAX_NUM_PADS];
  int                midi_velocity[MAX_NUM_PADS];
  int                midi_pos[MAX_NUM_PADS];
  int                midi_ctrl_value[MAX_NUM_PADS];
  Pad::Erimstate     rim_state[MAX_NUM_PADS];
  bool               is_choke_on[MAX_NUM_PADS];
  bool               is_choke_off[MAX_NUM_PADS];
  int                cancel_num_samples;
  int                cancel_cnt;
  int                cancel_MIDI_velocity;
  int                cancel_pad_index;

  // ADC spike cancellation
  void cancel_ADC_spikes(float&    signal,
                         int&      overload_detected,
                         const int pad_index,
                         const int input_channel_index,
                         const int level);

  enum Espikestate
  {
    ST_NOISE,
    ST_SPIKE_HIGH,
    ST_SPIKE_LOW,
    ST_OTHER
  };

  Espikestate prev1_input_state[MAX_NUM_PADS][MAX_NUM_PAD_INPUTS];
  Espikestate prev2_input_state[MAX_NUM_PADS][MAX_NUM_PAD_INPUTS];
  Espikestate prev3_input_state[MAX_NUM_PADS][MAX_NUM_PAD_INPUTS];
  Espikestate prev4_input_state[MAX_NUM_PADS][MAX_NUM_PAD_INPUTS];
  Espikestate prev5_input_state[MAX_NUM_PADS][MAX_NUM_PAD_INPUTS];
  float       prev_input1[MAX_NUM_PADS][MAX_NUM_PAD_INPUTS];
  float       prev_input2[MAX_NUM_PADS][MAX_NUM_PAD_INPUTS];
  float       prev_input3[MAX_NUM_PADS][MAX_NUM_PAD_INPUTS];
  float       prev_input4[MAX_NUM_PADS][MAX_NUM_PAD_INPUTS];
  int         prev_overload1[MAX_NUM_PADS][MAX_NUM_PAD_INPUTS];
  int         prev_overload2[MAX_NUM_PADS][MAX_NUM_PAD_INPUTS];
  int         prev_overload3[MAX_NUM_PADS][MAX_NUM_PAD_INPUTS];
  int         prev_overload4[MAX_NUM_PADS][MAX_NUM_PAD_INPUTS];
};
