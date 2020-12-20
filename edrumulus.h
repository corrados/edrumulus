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
	const int   Fs           = 8000;
	const int   hil_filt_len = 7;
	float*      hil_hist; // memory for Hilbert filter history
	const float a_re[7]                = { -0.037749783581601f, -0.069256807147465f, -1.443799477299919f,  2.473967088799056f,
							                            0.551482327389238f, -0.224119735833791f, -0.011665324660691f };
	const float a_im[7]                = {  0.0f,                0.213150535195075f, -1.048981722170302f, -1.797442302898130f,
							                            1.697288080048948f,  0.0f,                0.035902177664014f };
	const int   energy_window_len      = static_cast<int> ( round ( 2e-3f * Fs ) ); // scan time (e.g. 2 ms)
	float*      mov_av_hist_re;        // real part memory for moving average filter history
	float*      mov_av_hist_im;        // imaginary part memory for moving average filter history
	const int   mask_time              = round ( 10e-3f * Fs ); // mask time (e.g. 10 ms)
	int         mask_back_cnt          = 0;
	const float threshold              = pow ( 10.0f, -64.0f / 20 ); // -64 dB threshold
	bool        was_above_threshold;
	float       prev_hil_filt_val;
	float       prev_hil_filt_new_val;
	const float decay_att              = pow ( 10.0f, -1.0f / 20 ); // decay attenuation of 1 dB
	const int   decay_len              = round ( 0.2f * Fs ); // decay time (e.g. 200 ms)
	const float decay_grad             = 200.0f / Fs; // decay gradient factor
  float*      decay;                 // memory for decay function
	int         decay_back_cnt;
	float       decay_scaling;
	const float alpha                  = 0.025f * 8e3f / Fs;
	float       hil_low_re;
	float       hil_low_im;
  const int   energy_window_len_half = energy_window_len / 2;
	float*      hil_hist_re;           // real part of memory for moving average of Hilbert filtered signal
	float*      hil_hist_im;           // imaginary part of memory for moving average of Hilbert filtered signal
	float*      hil_low_hist_re;       // real part of memory for moving average of low-pass filtered Hilbert signal
	float*      hil_low_hist_im;       // imaginary part of memory for moving average of low-pass filtered Hilbert signal
};
