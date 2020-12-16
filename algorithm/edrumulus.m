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

% Setup ------------------------------------------------------------------------
global Fs;
global a_re;
global a_im;
global hil_filt_len;
global hil_hist;

Fs           = 8000;
hil_filt_len = 7;
hil_hist     = zeros(hil_filt_len, 1); % memory allocation for hilbert filter history
a_re = [-0.037749783581601, -0.069256807147465, -1.443799477299919,  2.473967088799056, ...
         0.551482327389238, -0.224119735833791, -0.011665324660691]';
a_im = [ 0,                  0.213150535195075, -1.048981722170302, -1.797442302898130, ...
         1.697288080048948,  0,                  0.035902177664014]';


% Loop (loop over all samples -> per sample processing as on the hardware) -----
hil_debug = zeros(length(x), 1);

for i = 1:length(x)
  [hil_debug(i)] = process_sample(x(i));
end

figure;
plot(20 * log10(abs([x, hil_debug])));

end


function [hil_debug] = process_sample(x)

global Fs;
global a_re;
global a_im;
global hil_filt_len;
global hil_hist;


% Calculate peak detection -----------------------------------------------------
% hilbert filter
hil_hist(1:hil_filt_len - 1) = hil_hist(2:hil_filt_len);
hil_hist(hil_filt_len)       = x;
hil_re                       = sum(hil_hist .* a_re);
hil_im                       = sum(hil_hist .* a_im);

hil_debug = complex(hil_re, hil_im); % just for debugging














end


