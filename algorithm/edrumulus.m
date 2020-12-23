%*******************************************************************************
% Copyright (c) 2020-2020
% Author: Volker Fischer
%*******************************************************************************
% Permission is hereby granted, free of charge, to any person obtaining a copy
% of this software and associated documentation files (the "Software"), to deal
% in the Software without restriction, including without limitation the rights
% to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
% copies of the Software, and to permit persons to whom the Software is
% furnished to do so, subject to the following conditions:
%
% The above copyright notice and this permission notice shall be included in all
% copies or substantial portions of the Software.
%
% THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
% IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
% FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
% AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
% LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
% OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
% SOFTWARE.
%*******************************************************************************

% reference code for the C++ implementation on the actual hardware

function edrumulus(x)

global energy_window_len;

close all

% load test data
% x = audioread("signals/pd120_roll.wav");
x = audioread("signals/pd120_single_hits.wav");
% x = audioread("signals/pd120_pos_sense2.wav");
% x = x(1300:5000); % * 1000;

% match the signal level of the ESP32
x = x * 25000;

Setup();

% loop
hil_debug          = zeros(length(x), 1);
hil_filt_debug     = zeros(length(x), 1);
hil_filt_new_debug = zeros(length(x), 1);
cur_decay_debug    = zeros(length(x), 1);
peak_found         = false(length(x), 1);
pos_sense_metric   = zeros(length(x), 1);

for i = 1:length(x)

  [hil_debug(i), ...
   hil_filt_debug(i), ...
   hil_filt_new_debug(i), ...
   cur_decay_debug(i), ...
   peak_found(i), ...
   pos_sense_metric(i)] = process_sample(x(i));

end

% note that caused by the positional sensing algorithm the peak detection is delayed
peak_found_idx                       = find(peak_found) - energy_window_len / 2;
peak_found_corrected                 = false(size(peak_found));
peak_found_corrected(peak_found_idx) = true;

figure; plot(20 * log10(abs([hil_filt_debug, hil_filt_new_debug, cur_decay_debug]))); hold on;
        plot(find(peak_found_corrected), 20 * log10(hil_filt_debug(peak_found_corrected)), 'g*');
        plot(find(peak_found_corrected), 10 * log10(pos_sense_metric(peak_found)) + 40, 'k*');
        ylim([-10, 90]);
% figure; plot(20 * log10(abs([x, hil_debug, hil_filt_debug])));

return;


% TEST
pkg load instrument-control

% prepare serial port
try
  a = serialport("COM7", 115200);
catch
end
flush(a);

bReturnIsComplex = false;

% send the input data vector
for i = 1:length(x)

  % write sample
  write(a, sprintf('%f.6\n', x(i)), 'char');

  % receive the return sample
  if bReturnIsComplex

    for j = 1:2

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

      y(2 * (i - 1) + j) = str2double(char(bytearray));

    end

  else

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

  end

end

if bReturnIsComplex
  y = complex(y(1:2:2 * length(x)), y(2:2:2 * length(x)));
end


% figure; plot([peak_found, y.'], '*');
figure; plot(20 * log10(abs([hil_filt_debug, y.'])));
% figure; plot(20 * log10(abs([x, y.'])));
% figure; plot(abs(x.' - y));


end





function Setup

global Fs;
global a_re;
global a_im;
global hil_filt_len;
global hil_hist;
global energy_window_len;
global mov_av_hist_re;
global mov_av_hist_im;
global mask_time;
global mask_back_cnt;
global threshold;
global was_above_threshold;
global prev_hil_filt_val;
global prev_hil_filt_new_val;
global decay_att;
global decay_len;
global decay;
global decay_back_cnt;
global decay_scaling;
global alpha;
global hil_low_re;
global hil_low_im;
global hil_hist_re;
global hil_hist_im;
global hil_low_hist_re;
global hil_low_hist_im;
global pos_sense_cnt;

Fs           = 8000;
hil_filt_len = 7;
hil_hist     = zeros(hil_filt_len, 1); % memory allocation for hilbert filter history
a_re = [-0.037749783581601, -0.069256807147465, -1.443799477299919,  2.473967088799056, ...
         0.551482327389238, -0.224119735833791, -0.011665324660691]';
a_im = [ 0,                  0.213150535195075, -1.048981722170302, -1.797442302898130, ...
         1.697288080048948,  0,                  0.035902177664014]';
energy_window_len      = round(2e-3 * Fs); % scan time (e.g. 2 ms)
mov_av_hist_re         = zeros(energy_window_len, 1); % real part memory for moving average filter history
mov_av_hist_im         = zeros(energy_window_len, 1); % imaginary part memory for moving average filter history
mask_time              = round(10e-3 * Fs); % mask time (e.g. 10 ms)
mask_back_cnt          = 0;
threshold              = power(10, 23 / 20); % 23 dB threshold
was_above_threshold    = false;
prev_hil_filt_val      = 0;
prev_hil_filt_new_val  = 0;
decay_att              = power(10, -1 / 20); % decay attenuation of 1 dB
decay_len              = round(0.2 * Fs); % decay time (e.g. 200 ms)
decay_grad             = 200 / Fs; % decay gradient factor
decay                  = power(10, -(0:decay_len - 1) / 20 * decay_grad);
decay_back_cnt         = 0;
decay_scaling          = 1;
alpha                  = 200 / Fs;
hil_low_re             = 0;
hil_low_im             = 0;
pos_sense_cnt          = 0;
hil_hist_re            = zeros(energy_window_len, 1);
hil_hist_im            = zeros(energy_window_len, 1);
hil_low_hist_re        = zeros(energy_window_len, 1);
hil_low_hist_im        = zeros(energy_window_len, 1);

end


function fifo_memory = update_fifo ( input, ...
                                     fifo_length, ...
                                     fifo_memory )

  % move all values in the history one step back and put new value on the top
  fifo_memory(1:fifo_length - 1) = fifo_memory(2:fifo_length);
  fifo_memory(fifo_length)       = input;

end


function [hil_debug, hil_filt_debug, ...
          hil_filt_new_debug, ...
          cur_decay_debug, ...
          peak_found, ...
          pos_sense_metric] = process_sample(x)

global Fs;
global a_re;
global a_im;
global hil_filt_len;
global hil_hist;
global energy_window_len;
global mov_av_hist_re;
global mov_av_hist_im;
global mask_time;
global mask_back_cnt;
global threshold;
global was_above_threshold;
global prev_hil_filt_val;
global prev_hil_filt_new_val;
global decay_att;
global decay_len;
global decay;
global decay_back_cnt;
global decay_scaling;
global alpha;
global hil_low_re;
global hil_low_im;
global hil_hist_re;
global hil_hist_im;
global hil_low_hist_re;
global hil_low_hist_im;
global pos_sense_cnt;

% initialize return parameter
peak_found       = false;
pos_sense_metric = 0;
cur_decay_debug  = 0; % just for debugging


% Calculate peak detection -----------------------------------------------------
% hilbert filter
hil_hist = update_fifo ( x, hil_filt_len, hil_hist );
hil_re   = sum(hil_hist .* a_re);
hil_im   = sum(hil_hist .* a_im);

hil_debug = complex(hil_re, hil_im); % just for debugging


% moving average filter
mov_av_hist_re = update_fifo ( hil_re, energy_window_len, mov_av_hist_re );
mov_av_hist_im = update_fifo ( hil_im, energy_window_len, mov_av_hist_im );
mov_av_re      = sum(mov_av_hist_re) / energy_window_len;
mov_av_im      = sum(mov_av_hist_im) / energy_window_len;

hil_filt = sqrt(mov_av_re * mov_av_re + mov_av_im * mov_av_im);

hil_filt_debug = hil_filt; % just for debugging


% exponential decay assumption (note that we must not use hil_filt_org since a
% previous peak might not be faded out and the peak detection works on hil_filt)
% subtract decay (with clipping at zero)
if decay_back_cnt > 0

  cur_decay       = decay_scaling * decay(1 + decay_len - decay_back_cnt);
  cur_decay_debug = cur_decay; % just for debugging
  hil_filt_new    = hil_filt - cur_decay;
  decay_back_cnt  = decay_back_cnt - 1;

  if hil_filt_new < 0

    hil_filt_new = 0;

  end

else

  hil_filt_new = hil_filt;

end


% threshold test
if ((hil_filt_new > threshold) || was_above_threshold) && (mask_back_cnt == 0)

  was_above_threshold = true;

  % climb to the maximum of the current peak
  if prev_hil_filt_new_val < hil_filt_new

    prev_hil_filt_new_val = hil_filt_new;
    prev_hil_filt_val     = hil_filt; % needed for further processing

  else

    % maximum found
    prev_hil_filt_new_val = 0;
    was_above_threshold   = false;
    decay_back_cnt        = decay_len;
    decay_scaling         = prev_hil_filt_val * decay_att;
    mask_back_cnt         = mask_time;
    peak_found            = true;

  end

end

if mask_back_cnt > 0

  mask_back_cnt = mask_back_cnt - 1;

end

hil_filt_new_debug = hil_filt_new; % just for debugging


% Calculate positional sensing -------------------------------------------------
% low pass filter of the Hilbert signal
hil_low_re = (1 - alpha) * hil_low_re + alpha * hil_re;
hil_low_im = (1 - alpha) * hil_low_im + alpha * hil_im;

hil_hist_re     = update_fifo ( hil_re, energy_window_len, hil_hist_re );
hil_hist_im     = update_fifo ( hil_im, energy_window_len, hil_hist_im );
hil_low_hist_re = update_fifo ( hil_low_re, energy_window_len, hil_low_hist_re );
hil_low_hist_im = update_fifo ( hil_low_im, energy_window_len, hil_low_hist_im );

if peak_found || (pos_sense_cnt > 0)

  if peak_found && (pos_sense_cnt == 0)

    % a peak was found, we now have to start the delay process to fill up the
    % required buffer length for our metric
    pos_sense_cnt = energy_window_len / 2 - 2;
    peak_found    = false; % will be set after delay process is done

  elseif pos_sense_cnt == 1

    % the buffers are filled, now calculate the metric
    peak_energy     = sum(hil_hist_re .* hil_hist_re + hil_hist_im .* hil_hist_im);
    peak_energy_low = sum(hil_low_hist_re .* hil_low_hist_re + hil_low_hist_im .* hil_low_hist_im);

    pos_sense_metric = peak_energy / peak_energy_low;
    pos_sense_cnt    = 0;
    peak_found       = true;

  else

    % we still need to wait for the buffers to fill up
    pos_sense_cnt = pos_sense_cnt - 1;

  end

end

end


