/******************************************************************************\
 * Copyright (c) 2020-2020
 *
 * Author(s):
 *  Volker Fischer
 *
 ******************************************************************************
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 *
\******************************************************************************/

#pragma once

#include "Arduino.h"

class Edrumulus
{
public:
  Edrumulus();

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

// TODO these are algorithm parameter and should be moved to the initialized function -> problem with memory allocation to be solved
const int Fs                     = 8000; // sampling rate of 8 kHz
const int energy_window_len      = static_cast<int> ( round ( 2e-3f * Fs ) ); // scan time (e.g. 2 ms)
const int decay_len              = round ( 0.2f * Fs ); // decay time (e.g. 200 ms)
const int energy_window_len_half = energy_window_len / 2;

  float* hil_hist;
  float* mov_av_hist_re;
  float* mov_av_hist_im;
  int    mask_time;
  int    mask_back_cnt;
  float  threshold;
  bool   was_above_threshold;
  float  prev_hil_filt_val;
  float  prev_hil_filt_new_val;
  float  decay_att;
  float* decay;
  int    decay_back_cnt;
  float  decay_scaling;
  float  alpha;
  float  hil_low_re;
  float  hil_low_im;
  float* hil_hist_re;
  float* hil_hist_im;
  float* hil_low_hist_re;
  float* hil_low_hist_im;
};
