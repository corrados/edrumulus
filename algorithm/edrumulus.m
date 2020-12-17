%*******************************************************************************
%* Copyright (c) 2020-2020
%*
%* Author(s):
%*  Volker Fischer
%*
%*******************************************************************************
%*
%* This program is free software; you can redistribute it and/or modify it under
%* the terms of the GNU General Public License as published by the Free Software
%* Foundation; either version 2 of the License, or (at your option) any later
%* version.
%*
%* This program is distributed in the hope that it will be useful, but WITHOUT
%* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
%* FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
%* details.
%*
%* You should have received a copy of the GNU General Public License along with
%* this program; if not, write to the Free Software Foundation, Inc.,
%* 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
%*
%*******************************************************************************

% reference code for the C++ implementation on the actual hardware

function edrumulus(x)


% TEST
pkg load instrument-control

% prepare serial port
try
  a = serialport("COM7", 115200);
catch
end
flush(a);


% load test data
x = audioread("signals/pd120_pos_sense2.wav");
x = x(1300:5000);

% send the input data vector
for i = 1:length(x)
  write(a, sprintf('%f.6\n', x(i)), 'char');
end

% receive the return data vector
for i = 1:length(x)

  % get number from string
  readready = false;
  bytearray = uint8([]);

  while ~readready

    val = fread(a, 1);

    if val == 13
      readready = true;
    end

    bytearray = [bytearray, uint8(val)];

  end

  y(i) = str2double(char(bytearray));
  
  if isnan(y(i))
    disp(char(bytearray));
  endif

end


% figure; plot(abs(x.' - y));



return;







% Setup ------------------------------------------------------------------------
global Fs;
global a_re;
global a_im;
global hil_filt_len;
global hil_hist;
global energy_window_len;
global mov_av_hist_re;
global mov_av_hist_im;

Fs           = 8000;
hil_filt_len = 7;
hil_hist     = zeros(hil_filt_len, 1); % memory allocation for hilbert filter history
a_re = [-0.037749783581601, -0.069256807147465, -1.443799477299919,  2.473967088799056, ...
         0.551482327389238, -0.224119735833791, -0.011665324660691]';
a_im = [ 0,                  0.213150535195075, -1.048981722170302, -1.797442302898130, ...
         1.697288080048948,  0,                  0.035902177664014]';
energy_window_len = round(2e-3 * Fs); % scan time (e.g. 2 ms)
mov_av_hist_re    = zeros(energy_window_len, 1); % real part memory for moving average filter history
mov_av_hist_im    = zeros(energy_window_len, 1); % imaginary part memory for moving average filter history


% Loop (loop over all samples -> per sample processing as on the hardware) -----
hil_debug      = zeros(length(x), 1);
hil_filt_debug = zeros(length(x), 1);

for i = 1:length(x)
  [hil_debug(i), hil_filt_debug(i)] = process_sample(x(i));
end

figure;
plot(20 * log10(abs([x, hil_debug, hil_filt_debug])));

end


function [hil_debug, hil_filt_debug] = process_sample(x)

global Fs;
global a_re;
global a_im;
global hil_filt_len;
global hil_hist;
global energy_window_len;
global mov_av_hist_re;
global mov_av_hist_im;


% Calculate peak detection -----------------------------------------------------
% hilbert filter
hil_hist(1:hil_filt_len - 1) = hil_hist(2:hil_filt_len);
hil_hist(hil_filt_len)       = x;
hil_re                       = sum(hil_hist .* a_re);
hil_im                       = sum(hil_hist .* a_im);

hil_debug = complex(hil_re, hil_im); % just for debugging


% moving average filter
mov_av_hist_re(1:energy_window_len - 1) = mov_av_hist_re(2:energy_window_len);
mov_av_hist_im(1:energy_window_len - 1) = mov_av_hist_im(2:energy_window_len);
mov_av_hist_re(energy_window_len)       = hil_re;
mov_av_hist_im(energy_window_len)       = hil_im;
mov_av_re                               = sum(mov_av_hist_re) / energy_window_len;
mov_av_im                               = sum(mov_av_hist_im) / energy_window_len;

hil_filt = sqrt(mov_av_re * mov_av_re + mov_av_im * mov_av_im);

hil_filt_debug = hil_filt; % just for debugging







end


