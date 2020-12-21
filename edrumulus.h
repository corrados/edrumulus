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

class Edrumulus
{
public:
  Edrumulus() { initialize(); }

  void process_sample ( const float fIn,
                        bool&       peak_found,
                        int&        midi_velocity,
                        int&        midi_pos,
                        float&      debug );


protected:
  void initialize();

  // Hilbert filter coefficients (they are constant and must not be changed)
  const int   hil_filt_len = 7;
  const float a_re[7]      = { -0.037749783581601f, -0.069256807147465f, -1.443799477299919f,  2.473967088799056f,
                                0.551482327389238f, -0.224119735833791f, -0.011665324660691f };
  const float a_im[7]      = {  0.0f,                0.213150535195075f, -1.048981722170302f, -1.797442302898130f,
                                1.697288080048948f,  0.0f,                0.035902177664014f };

  float* hil_hist        = nullptr;
  float* mov_av_hist_re  = nullptr;
  float* mov_av_hist_im  = nullptr;
  float* decay           = nullptr;
  float* hil_hist_re     = nullptr;
  float* hil_hist_im     = nullptr;
  float* hil_low_hist_re = nullptr;
  float* hil_low_hist_im = nullptr;

  int    Fs;
  int    energy_window_len;
  int    decay_len;
  int    mask_time;
  int    mask_back_cnt;
  float  threshold;
  bool   was_above_threshold;
  float  prev_hil_filt_val;
  float  prev_hil_filt_new_val;
  float  decay_att;
  int    decay_back_cnt;
  float  decay_scaling;
  float  alpha;
  int    pos_sense_cnt;
  int    stored_midi_velocity;
  float  hil_low_re;
  float  hil_low_im;
};
