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
#include "soc/sens_reg.h"

#define MAX_NUM_PADS         12   // a maximum of 12 pads are supported
#define MAX_NUM_PAD_INPUTS   2    // a maximum of 2 sensors per pad is supported
#define ADC_MAX_RANGE        4096 // ESP32 ADC has 12 bits -> 0..4095
#define ADC_MAX_NOISE_AMPL   8    // highest assumed ADC noise amplitude in the ADC input range unit

class Edrumulus
{
public:
  enum Epadtype
  {
// TODO if new pads are added, check if get_is_control() and get_is_rim_switch() must be updated
    PD120,
    PD80R,
    PD8,
    FD8
  };

  enum Ecurvetype
  {
    LINEAR,
    EXP1,
    EXP2,
    LOG1,
    LOG2
  };

  Edrumulus();

  // call this function during the Setup function of the main program
  void setup ( const int  conf_num_pads,
               const int* conf_analog_pins,
               const int* conf_analog_pins_rim_shot );

  // call the process function during the main loop
  void process();

  // after calling the process function, query the results for each configured pad
  bool get_peak_found      ( const int pad_idx ) { return !pad[pad_idx].get_is_control() && peak_found[pad_idx]; }
  bool get_control_found   ( const int pad_idx ) { return pad[pad_idx].get_is_control() && control_found[pad_idx]; }
  int  get_midi_velocity   ( const int pad_idx ) { return midi_velocity[pad_idx]; }
  int  get_midi_pos        ( const int pad_idx ) { return midi_pos[pad_idx]; }
  int  get_midi_note       ( const int pad_idx ) { return is_rim_shot[pad_idx] ? pad[pad_idx].get_midi_note_rim() : pad[pad_idx].get_midi_note(); }
  int  get_midi_ctrl_ch    ( const int pad_idx ) { return pad[pad_idx].get_midi_ctrl_ch(); }
  int  get_midi_ctrl_value ( const int pad_idx ) { return midi_ctrl_value[pad_idx]; }

  // configure the pads
  void set_pad_type             ( const int pad_idx, const Epadtype   new_pad_type )  { pad[pad_idx].set_pad_type ( new_pad_type ); }
  void set_velocity_threshold   ( const int pad_idx, const int        new_threshold ) { pad[pad_idx].set_velocity_threshold ( new_threshold ); }
  void set_velocity_sensitivity ( const int pad_idx, const int        new_velocity )  { pad[pad_idx].set_velocity_sensitivity ( new_velocity ); }
  void set_pos_threshold        ( const int pad_idx, const int        new_threshold ) { pad[pad_idx].set_pos_threshold ( new_threshold ); }
  void set_pos_sensitivity      ( const int pad_idx, const int        new_velocity )  { pad[pad_idx].set_pos_sensitivity ( new_velocity ); }
  void set_mask_time            ( const int pad_idx, const int        new_time )      { pad[pad_idx].set_mask_time ( new_time ); }
  void set_rim_shot_treshold    ( const int pad_idx, const int        new_threshold ) { pad[pad_idx].set_rim_shot_treshold ( new_threshold ); }
  void set_curve                ( const int pad_idx, const Ecurvetype new_curve )     { pad[pad_idx].set_curve ( new_curve ); }

  void set_midi_notes           ( const int pad_idx, const int new_midi_note, const int new_midi_note_rim ) { pad[pad_idx].set_midi_notes ( new_midi_note, new_midi_note_rim ); }
  void set_midi_ctrl_ch         ( const int pad_idx, const int new_midi_ctrl_ch )                           { pad[pad_idx].set_midi_ctrl_ch ( new_midi_ctrl_ch ); }
  void set_rim_shot_is_used     ( const int pad_idx, const bool new_is_used ) { pad[pad_idx].set_rim_shot_is_used ( new_is_used ); }
  void set_pos_sense_is_used    ( const int pad_idx, const bool new_is_used ) { pad[pad_idx].set_pos_sense_is_used ( new_is_used ); }
  void set_spike_cancel_is_used ( const bool new_is_used )                    { spike_cancel_is_used = new_is_used; }

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

      void process_sample ( const float* input,
                            bool&        peak_found,
                            int&         midi_velocity,
                            int&         midi_pos,
                            bool&        is_rim_shot,
                            float&       debug );

      void process_control_sample ( const int* input,
                                    bool&      change_found,
                                    int&       midi_ctrl_value );

      void set_pad_type          ( const Epadtype new_pad_type );
      void set_midi_notes        ( const int new_midi_note, const int new_midi_note_rim ) { midi_note = new_midi_note; midi_note_rim = new_midi_note_rim; }
      void set_midi_ctrl_ch      ( const int new_midi_ctrl_ch )                           { midi_ctrl_ch = new_midi_ctrl_ch; }
      void set_rim_shot_is_used  ( const bool new_is_used ) { pad_settings.rim_shot_is_used = new_is_used; }
      void set_pos_sense_is_used ( const bool new_is_used ) { pad_settings.pos_sense_is_used = new_is_used; }

      void set_velocity_threshold   ( const int        new_threshold ) { pad_settings.velocity_threshold   = new_threshold; initialize(); }
      void set_velocity_sensitivity ( const int        new_velocity )  { pad_settings.velocity_sensitivity = new_velocity;  initialize(); }
      void set_pos_threshold        ( const int        new_threshold ) { pad_settings.pos_threshold        = new_threshold; initialize(); }
      void set_pos_sensitivity      ( const int        new_velocity )  { pad_settings.pos_sensitivity      = new_velocity;  initialize(); }
      void set_mask_time            ( const int        new_time_ms )   { pad_settings.mask_time_ms         = new_time_ms;   initialize(); }
      void set_rim_shot_treshold    ( const int        new_threshold ) { pad_settings.rim_shot_treshold    = new_threshold; initialize(); }
      void set_curve                ( const Ecurvetype new_curve )     { pad_settings.curve_type           = new_curve;     initialize(); }

      int  get_midi_note()         { return midi_note; }
      int  get_midi_note_rim()     { return midi_note_rim; }
      int  get_midi_ctrl_ch()      { return midi_ctrl_ch; }
      bool get_is_control()        { return pad_settings.pad_type == FD8; } // TODO check if new pads must be added here
      bool get_is_rim_switch()     { return pad_settings.pad_type == PD8; } // TODO check if new pads must be added here
      bool get_pos_sense_is_used() { return pad_settings.pos_sense_is_used; }

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
        bool       pos_sense_is_used;    // switches positional sensing support on or off
        bool       rim_shot_is_used;     // switches rim shot detection on or off
        Ecurvetype curve_type;
        float      energy_win_len_ms;
        float      scan_time_ms;
        float      main_peak_dist_ms;
        float      decay_est_delay2nd_ms;
        float      decay_est_len_ms;
        float      decay_est_fact_db;
        float      decay_fact_db;
        float      decay_len1_ms,    decay_len2_ms,    decay_len3_ms;
        float      decay_grad_fact1, decay_grad_fact2, decay_grad_fact3;
        float      pos_energy_win_len_ms;
        float      pos_iir_alpha;
        float      rim_shot_window_len_ms;
      };

      void initialize();

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
      float* midi_curve              = nullptr;

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
      float        rim_switch_treshold;
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
      int          midi_note;
      int          midi_note_rim;
      int          midi_ctrl_ch;
      int          prev_ctrl_value;
  };

  // constant definitions
  const int dc_offset_est_len       = 5000; // samples
  const int samplerate_max_cnt      = 10000; // samples
  const int samplerate_max_error_Hz = 100; // tolerate a sample rate deviation of 100 Hz
  const int ctrl_subsampling        = 10; // the sampling rate of the control can be much lower

  enum Espikestate
  {
    ST_NOISE,
    ST_SPIKE,
    ST_OTHER
  };

  int           Fs;
  int           number_pads;
  int           number_inputs[MAX_NUM_PADS];
  int           analog_pin[MAX_NUM_PADS][MAX_NUM_PAD_INPUTS];
  float         dc_offset[MAX_NUM_PADS][MAX_NUM_PAD_INPUTS];
  Espikestate   prev1_input_state[MAX_NUM_PADS][MAX_NUM_PAD_INPUTS];
  Espikestate   prev2_input_state[MAX_NUM_PADS][MAX_NUM_PAD_INPUTS];
  Espikestate   prev3_input_state[MAX_NUM_PADS][MAX_NUM_PAD_INPUTS];
  float         prev_input1[MAX_NUM_PADS][MAX_NUM_PAD_INPUTS];
  float         prev_input2[MAX_NUM_PADS][MAX_NUM_PAD_INPUTS];
  bool          spike_cancel_is_used;
  int           overload_LED_cnt;
  int           overload_LED_on_time;
  bool          status_is_overload;
  bool          status_is_error;
  int           ctrl_sample_cnt;
  int           samplerate_prev_micros_cnt;
  unsigned long samplerate_prev_micros;
  Pad           pad[MAX_NUM_PADS];
  bool          peak_found[MAX_NUM_PADS];
  bool          control_found[MAX_NUM_PADS];
  int           midi_velocity[MAX_NUM_PADS];
  int           midi_pos[MAX_NUM_PADS];
  int           midi_ctrl_value[MAX_NUM_PADS];
  bool          is_rim_shot[MAX_NUM_PADS];

  volatile SemaphoreHandle_t timer_semaphore;
  hw_timer_t*                timer = nullptr;
  static void IRAM_ATTR      on_timer();
  TaskHandle_t task_handle;


  // -----------------------------------------------------------------------------
  // ESP32 Specific Functions ----------------------------------------------------
  // -----------------------------------------------------------------------------
  
  float cancel_ADC_spikes ( const float input,
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
  void my_init_analogRead()
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
};


// Utility functions -----------------------------------------------------------------

static void update_fifo ( const float input,
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

static void allocate_initialize ( float**   array_memory,
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
