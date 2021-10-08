%*******************************************************************************
% Copyright (c) 2020-2021
% Author(s): Volker Fischer
%*******************************************************************************
% This program is free software; you can redistribute it and/or modify it under
% the terms of the GNU General Public License as published by the Free Software
% Foundation; either version 2 of the License, or (at your option) any later
% version.
% This program is distributed in the hope that it will be useful, but WITHOUT
% ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
% FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
% details.
% You should have received a copy of the GNU General Public License along with
% this program; if not, write to the Free Software Foundation, Inc.,
% 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
%*******************************************************************************

% reference code for the C++ implementation on the actual hardware

function edrumulus(x)

global energy_window_len;
global rim_shot_window_len;

close all

% load test data
%x = audioread("signals/pd120_roll.wav");x = x(292410:294749, :);
%x = audioread("signals/pd120_single_hits.wav");
%x = audioread("signals/pd120_pos_sense.wav");x = x(2900:10000, :);%x = x(55400:58000, :);%
%x = audioread("signals/pd120_pos_sense2.wav");x = x(109000:117000, :);%
%x = audioread("signals/pd120_rimshot.wav");x = x(168000:171000, :);%x = x(1:8000, :);%x = x(1:34000, :);%x = x(1:100000, :);
%x = audioread("signals/pd120_rimshot_hardsoft.wav");
x = audioread("signals/pd80r.wav");padtype = 'pd80r';x = x(130000:176000, :);%265000, :);%x = x(57500:59000, :);%


% match the signal level of the ESP32
x = x * 25000;

Setup();

% loop
hil_debug                 = zeros(size(x, 1), 1);
hil_filt                  = zeros(size(x, 1), 1);
hil_filt_decay_debug      = zeros(size(x, 1), 1);
cur_decay_debug           = zeros(size(x, 1), 1);
rim_max_pow_debug         = zeros(size(x, 1), 1);
x_rim_high_debug          = zeros(size(x, 1), 1);
peak_found                = false(size(x, 1), 1);
peak_found_offset         = zeros(size(x, 1), 1);
pos_sense_peak_energy     = zeros(size(x, 1), 1);
pos_sense_peak_energy_low = zeros(size(x, 1), 1);
was_pos_sense_ready       = false(size(x, 1), 1);
pos_sense_metric          = zeros(size(x, 1), 1);
is_rim_shot               = false(size(x, 1), 1);
is_left_main_peak         = false(size(x, 1), 1);

for i = 1:size(x, 1)

  [hil_debug(i), ...
   hil_filt(i), ...
   hil_filt_decay_debug(i), ...
   cur_decay_debug(i), ...
   rim_max_pow_debug(i), ...
   x_rim_high_debug(i), ...
   peak_found(i), ...
   peak_found_offset(i), ...
   pos_sense_peak_energy(i), ...
   pos_sense_peak_energy_low(i), ...
   was_pos_sense_ready(i), ...
   pos_sense_metric(i), ...
   is_rim_shot(i), ...
   is_left_main_peak(i)] = process_sample(x(i, :));

end

% note that caused by the positional sensing/rim shot detection algorithms the peak detection is delayed
peak_found_idx                         = find(peak_found) - peak_found_offset(peak_found);
peak_found_corrected                   = false(size(peak_found));
peak_found_corrected(peak_found_idx)   = true;
is_rim_shot_idx                        = find(is_rim_shot) - peak_found_offset(is_rim_shot);
is_rim_shot_corrected                  = false(size(is_rim_shot));
is_rim_shot_corrected(is_rim_shot_idx) = true;

figure; plot(10 * log10([pos_sense_peak_energy, pos_sense_peak_energy_low * 100])); hold on; grid on;
        plot(find(was_pos_sense_ready), 10 * log10(pos_sense_peak_energy(was_pos_sense_ready)), 'k*');
        ylim([0, 90]); title('checking pos sense high/low signals for metric');

figure; plot(10 * log10(abs([hil_filt, hil_filt_decay_debug, cur_decay_debug, x_rim_high_debug]))); hold on; grid on;
        plot(10 * log10(rim_max_pow_debug), 'y*');
        plot(find(peak_found_corrected),  10 * log10(hil_filt(peak_found_corrected)), 'g*');
        plot(find(is_rim_shot_corrected), 10 * log10(hil_filt(is_rim_shot_corrected)), 'b*');
        plot(find(peak_found_corrected),  10 * log10(pos_sense_metric(peak_found)) + 40, 'k*');
        plot(find(is_left_main_peak),     10 * log10(hil_filt(is_left_main_peak)), 'y*');
        ylim([-10, 90]);

% figure; plot(20 * log10(abs([x, hil_debug, hil_filt])));

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
global a_re a_im;
global hil_filt_len;
global hil_hist;
global b_rim_high a_rim_high;
global rim_high_prev_x;
global rim_x_high;
global energy_window_len;
global pos_energy_window_len;
global pos_unfiltered_delay;
global scan_time;
global scan_time_cnt;
global mov_av_hist;
global mask_time;
global mask_back_cnt;
global threshold;
global first_peak_diff_thresh;
global was_above_threshold;
global first_peak_val;
global prev_hil_filt_val;
global main_peak_dist;
global hist_main_peak_pow_left;
global power_hypo_left;
global power_hypo_right_cnt;
global decay_pow_est_start_cnt;
global decay_pow_est_cnt;
global decay_pow_est_sum;
global decay_est_delay2nd;
global decay_est_len;
global decay_est_fact;
global decay_fact;
global decay_len;
global decay;
global decay_back_cnt;
global decay_scaling;
global pos_low_pass_hist_len;
global pos_low_pass_hist_re pos_low_pass_hist_im;
global pos_hil_hist;
global pos_hil_low_hist;
global pos_sense_unfilt_hist;
global pos_sense_cnt;
global rim_shot_window_len;
global rim_shot_treshold_dB;
global rim_x_high_hist;
global rim_shot_cnt;
global hil_filt_max_pow;
global stored_pos_sense_metric;
global stored_is_rimshot;
global max_hil_filt_val;
global peak_found_offset;
global was_peak_found;
global was_pos_sense_ready;
global was_rim_shot_ready;

Fs           = 8000;
hil_filt_len = 7;
hil_hist     = zeros(hil_filt_len, 1); % memory allocation for hilbert filter history
a_re = [-0.037749783581601, -0.069256807147465, -1.443799477299919,  2.473967088799056, ...
         0.551482327389238, -0.224119735833791, -0.011665324660691]';
a_im = [ 0,                  0.213150535195075, -1.048981722170302, -1.797442302898130, ...
         1.697288080048948,  0,                  0.035902177664014]';

% moving average cut off frequency approximation according to:
% https://dsp.stackexchange.com/questions/9966/what-is-the-cut-off-frequency-of-a-moving-average-filter
pos_low_pass_cutoff        = 250; % Hz
low_pass_cutoff_normalized = pos_low_pass_cutoff / Fs;
pos_low_pass_hist_len      = round(sqrt(0.196202 + low_pass_cutoff_normalized ^ 2) / low_pass_cutoff_normalized);
if mod(pos_low_pass_hist_len, 2) == 1
  pos_low_pass_hist_len = pos_low_pass_hist_len + 1; % make sure we have an even length
endif

rim_high_prev_x         = 0;
rim_x_high              = 0;
b_rim_high              = [0.969531252908746, -0.969531252908746];
a_rim_high              = -0.939062505817492;
energy_window_len       = round(2e-3 * Fs); % hit energy estimation time window length (e.g. 2 ms)
scan_time               = round(2.5e-3 * Fs); % scan time from first detected peak
scan_time_cnt           = 0;
mov_av_hist             = zeros(energy_window_len, 1); % memory for moving average filter history
mask_time               = round(6e-3 * Fs); % mask time (e.g. 10 ms)
mask_back_cnt           = 0;
threshold               = power(10, 28 / 10); % 28 dB threshold
first_peak_diff_thresh  = 10 ^ (20 / 10); % 20 dB difference allowed between first peak and later peak in scan time
was_above_threshold     = false;
first_peak_val          = 0;
prev_hil_filt_val       = 0;
main_peak_dist          = round(2.25e-3 * Fs);
hist_main_peak_pow_left = zeros(main_peak_dist, 1); % memory for left main peak power
power_hypo_left         = 0;
power_hypo_right_cnt    = 0;
decay_pow_est_start_cnt = 0;
decay_pow_est_cnt       = 0;
decay_pow_est_sum       = 0;
decay_est_delay2nd      = round(2.5e-3 * Fs);
decay_est_len           = round(3e-3 * Fs);
decay_est_fact          = 10 ^ (15 / 10);
decay_fact              = power(10, 1 / 10); % decay factor of 1 dB
decay_back_cnt          = 0;
decay_scaling           = 1;
pos_low_pass_hist_re    = zeros(pos_low_pass_hist_len, 1);
pos_low_pass_hist_im    = zeros(pos_low_pass_hist_len, 1);
pos_energy_window_len   = round(0.5e-3 * Fs); % positional sensing energy estimation time window length (e.g. 2 ms)
pos_unfiltered_delay    = pos_low_pass_hist_len / 2;
pos_sense_cnt           = 0;
pos_hil_hist            = zeros(pos_energy_window_len, 1);
pos_hil_low_hist        = zeros(pos_energy_window_len, 1);
pos_sense_unfilt_hist   = zeros(pos_unfiltered_delay, 1); % memory for unfiltered signal for positional sensing metric
rim_shot_window_len     = round(5e-3 * Fs); % window length (e.g. 6 ms)
rim_shot_treshold_dB    = 2.3; % dB
rim_x_high_hist         = zeros(rim_shot_window_len, 1);
rim_shot_cnt            = 0;
hil_filt_max_pow        = 0;
stored_pos_sense_metric = 0;
stored_is_rimshot       = false;
max_hil_filt_val        = 0;
peak_found_offset       = 0;
was_peak_found          = false;
was_pos_sense_ready     = false;
was_rim_shot_ready      = false;
decay_len1              = round(0 * Fs); % not used
decay_grad1             = 200 / Fs;
decay_len2              = round(0.25 * Fs);% decay time (e.g. 250 ms)
decay_grad2             = 200 / Fs; % decay gradient factor
decay_len3              = round(0 * Fs); % not used
decay_grad3             = 200 / Fs;

% calculate the decay curve
decay_len = decay_len1 + decay_len2 + decay_len3;
decay     = zeros(decay_len, 1);
for i = 1:decay_len1
  decay(i) = power(10, -(i - 1) / 10 * decay_grad1);
end
decay_fact1 = power(10, -decay_len1 / 10 * decay_grad1);
for i = 1:decay_len2
  decay(decay_len1 + i) = decay_fact1 * power(10, -(i - 1) / 10 * decay_grad2);
end
decay_fact2 = decay_fact1 * power(10, -decay_len2 / 10 * decay_grad2);
for i = 1:decay_len3
  decay(decay_len1 + decay_len2 + i) = decay_fact2 * power(10, -(i - 1) / 10 * decay_grad3);
end

end


function fifo_memory = update_fifo ( input, ...
                                     fifo_length, ...
                                     fifo_memory )

  % move all values in the history one step back and put new value on the top
  fifo_memory(1:fifo_length - 1) = fifo_memory(2:fifo_length);
  fifo_memory(fifo_length)       = input;

end


function [hil_debug, ...
          hil_filt, ...
          hil_filt_decay_debug, ...
          cur_decay_debug, ...
          rim_max_pow_debug, ...
          x_rim_high_debug, ...
          peak_found, ...
          peak_found_offset, ...
          peak_energy_delayed, ...
          peak_energy_low, ...
          was_pos_sense_ready, ...
          pos_sense_metric, ...
          is_rim_shot, ...
          is_left_main_peak] = process_sample(x)

global Fs;
global a_re a_im;
global hil_filt_len;
global hil_hist;
global b_rim_high a_rim_high;
global rim_high_prev_x;
global rim_x_high;
global energy_window_len;
global pos_energy_window_len;
global pos_unfiltered_delay;
global scan_time;
global scan_time_cnt;
global mov_av_hist;
global mask_time;
global mask_back_cnt;
global threshold;
global first_peak_diff_thresh;
global was_above_threshold;
global first_peak_val;
global prev_hil_filt_val;
global main_peak_dist;
global hist_main_peak_pow_left;
global power_hypo_left;
global power_hypo_right_cnt;
global decay_pow_est_start_cnt;
global decay_pow_est_cnt;
global decay_pow_est_sum;
global decay_est_delay2nd;
global decay_est_len;
global decay_est_fact;
global decay_fact;
global decay_len;
global decay;
global decay_back_cnt;
global decay_scaling;
global pos_low_pass_hist_len;
global pos_low_pass_hist_re pos_low_pass_hist_im;
global pos_hil_hist;
global pos_hil_low_hist;
global pos_sense_unfilt_hist;
global pos_sense_cnt;
global rim_shot_window_len;
global rim_shot_treshold_dB;
global rim_x_high_hist;
global rim_shot_cnt;
global hil_filt_max_pow;
global stored_pos_sense_metric;
global stored_is_rimshot;
global max_hil_filt_val;
global peak_found_offset;
global was_peak_found;
global was_pos_sense_ready;
global was_rim_shot_ready;

% initialize return parameter
peak_found        = false;
pos_sense_metric  = 0;
is_rim_shot       = false;
is_left_main_peak = false;
first_peak_found  = false; % only used internally
pos_sense_is_used = true;  % only used internally to enable/disable positional sensing
rim_shot_is_used  = false; % only used internally
cur_decay_debug   = 0; % just for debugging
rim_max_pow_debug = 0; % just for debugging
x_rim_high_debug  = 0; % just for debugging


% Calculate peak detection -----------------------------------------------------
% hilbert filter
hil_hist  = update_fifo(x(1), hil_filt_len, hil_hist);
hil_re    = sum(hil_hist .* a_re);
hil_im    = sum(hil_hist .* a_im);
hil_debug = complex(hil_re, hil_im); % just for debugging

% moving average filter
hil_sq      = hil_re * hil_re + hil_im * hil_im;
mov_av_hist = update_fifo(hil_sq, energy_window_len, mov_av_hist);
hil_filt    = sum(mov_av_hist) / energy_window_len;

% exponential decay assumption
if decay_back_cnt > 0

  % subtract decay (with clipping at zero)
  cur_decay       = decay_scaling * decay(1 + decay_len - decay_back_cnt);
  cur_decay_debug = cur_decay; % just for debugging
  hil_filt_decay  = hil_filt - cur_decay;
  decay_back_cnt  = decay_back_cnt - 1;

  if hil_filt_decay < 0
    hil_filt_decay = 0;
  end

else
  hil_filt_decay = hil_filt;
end

% threshold test
if ((hil_filt_decay > threshold) || was_above_threshold) && (mask_back_cnt == 0)

  was_above_threshold = true;

  % climb to the maximum of the first peak
  if (first_peak_val < hil_filt) && (scan_time_cnt == 0)
    first_peak_val = hil_filt;
  else

    % check if there is a much larger first peak
    if (prev_hil_filt_val > hil_filt) && (first_peak_val * first_peak_diff_thresh < prev_hil_filt_val)

      % reset first peak detection and restart scan time
      first_peak_val = prev_hil_filt_val;
      scan_time_cnt  = 0;

    end

    % start condition of scan time
    if scan_time_cnt == 0

      % search in a pre-defined scan time for the highest peak
      scan_time_cnt       = scan_time;                  % initialize scan time counter
      max_hil_filt_val    = first_peak_val;             % initialize maximum value with first peak
      peak_found_offset   = scan_time;                  % position of first peak after scan time expired (no "-1" because peak is previous sample)
      power_hypo_left     = hist_main_peak_pow_left(1); % for left/right main peak detection
      first_peak_found    = true;
      pos_sense_cnt       = 0;                          % needed if we reset the first peak
      was_pos_sense_ready = false;                      % needed if we reset the first peak
      rim_shot_cnt        = 0;                          % needed if we reset the first peak
      was_rim_shot_ready  = false;                      % needed if we reset the first peak

    end

    % search for a maximum in the scan time interval
    if hil_filt > max_hil_filt_val

      max_hil_filt_val  = hil_filt;                   % we need to store the origianl Hilbert filtered signal for the decay
      peak_found_offset = scan_time_cnt - 1;          % update position of detected peak ("-1" because peak is current sample not previous)
      power_hypo_left   = hist_main_peak_pow_left(1); % for left/right main peak detection

    end

    scan_time_cnt     = scan_time_cnt - 1;
    prev_hil_filt_val = hil_filt;

    % end condition of scan time
    if scan_time_cnt <= 0

      % scan time expired
      first_peak_val      = 0;
      prev_hil_filt_val   = 0;
      was_above_threshold = false;
      decay_scaling       = max_hil_filt_val * decay_fact;
      decay_back_cnt      = decay_len - scan_time; % start is first peak (i.e. scan_time instead of peak_found_offset)
      mask_back_cnt       = mask_time - scan_time; % start is first peak (i.e. scan_time instead of peak_found_offset)
      was_peak_found      = true;

      % for left/right main peak detection (note that we have to add one because
      % we first decrement and then we check for end condition)
      power_hypo_right_cnt = max(1, main_peak_dist - peak_found_offset + 1);

    end

  end

end

if mask_back_cnt > 0
  mask_back_cnt = mask_back_cnt - 1;
end

% manage left/right main peak detection by power comparision
hist_main_peak_pow_left = update_fifo(hil_filt, main_peak_dist, hist_main_peak_pow_left);

if power_hypo_right_cnt > 0

  power_hypo_right_cnt = power_hypo_right_cnt - 1;

  % end condition
  if power_hypo_right_cnt <= 0

    % now we can detect if the main peak was the left/right main peak and we can
    % now start the counter for the decay power estimation interval start (note
    % that we have to add one because we first decrement and then we check for
    % end condition)
    if power_hypo_left > hil_filt
      decay_pow_est_start_cnt = decay_est_delay2nd - main_peak_dist + 1; % detected peak is right main peak
    else

      % detected peak is left main peak
      is_left_main_peak       = true;
      decay_pow_est_start_cnt = decay_est_delay2nd + 1;

    end

  end

end

% decay power estimation
if decay_pow_est_start_cnt > 0

  decay_pow_est_start_cnt = decay_pow_est_start_cnt - 1;

  % end condition
  if decay_pow_est_start_cnt <= 0
    decay_pow_est_cnt = decay_est_len; % now the power estimation can start
  end

end

if decay_pow_est_cnt > 0

  decay_pow_est_sum = decay_pow_est_sum + hil_filt; % sum up the powers in pre-defined interval
  decay_pow_est_cnt = decay_pow_est_cnt - 1;

  % end condition
  if decay_pow_est_cnt <= 0

    decay_power       = decay_pow_est_sum / decay_est_len;                % calculate average power
    decay_pow_est_sum = 0;                                                % we have to reset the sum for the next calculation
    decay_scaling     = min(decay_scaling, decay_est_fact * decay_power); % adjust the decay curve

  end

end

hil_filt_decay_debug = hil_filt_decay; % just for debugging


% Calculate positional sensing -------------------------------------------------
if pos_sense_is_used

  % low pass filter of the Hilbert signal
  pos_low_pass_hist_re = update_fifo(hil_re, pos_low_pass_hist_len, pos_low_pass_hist_re);
  pos_low_pass_hist_im = update_fifo(hil_im, pos_low_pass_hist_len, pos_low_pass_hist_im);
  hil_low_re           = sum(pos_low_pass_hist_re) / pos_low_pass_hist_len;
  hil_low_im           = sum(pos_low_pass_hist_im) / pos_low_pass_hist_len;

  pos_hil     = hil_re * hil_re + hil_im * hil_im;
  pos_hil_low = hil_low_re * hil_low_re + hil_low_im * hil_low_im;

  pos_hil_hist     = update_fifo(pos_hil,     pos_energy_window_len, pos_hil_hist);
  pos_hil_low_hist = update_fifo(pos_hil_low, pos_energy_window_len, pos_hil_low_hist);

  peak_energy     = sum(pos_hil_hist);
  peak_energy_low = sum(pos_hil_low_hist);

  % delay the unfiltered signal so that it matches delay of low-pass filter
  pos_sense_unfilt_hist = update_fifo(peak_energy, pos_unfiltered_delay, pos_sense_unfilt_hist);
  peak_energy_delayed   = pos_sense_unfilt_hist(1);

  % start condition of delay process to fill up the required buffers
  if first_peak_found && (~was_pos_sense_ready) && (pos_sense_cnt == 0)

    % a peak was found, we now have to start the delay process to fill up the
    % required buffer length for our metric (compensate low-pass filter delay
    % and energy window moving average delay of peak detection)
    pos_sense_cnt = pos_energy_window_len / 2 - 1 + pos_unfiltered_delay - energy_window_len / 2;


% TODO if the energy window length is longer than the positional sensing energy
%      window length plus the unfiltered delay, the counter may get <= 1
pos_sense_cnt = max(pos_sense_cnt, 1);


  end

  if pos_sense_cnt > 0

    pos_sense_cnt = pos_sense_cnt - 1;

    % end condition
    if pos_sense_cnt <= 0

      % the buffers are filled, now calculate the metric
      stored_pos_sense_metric = peak_energy_delayed / peak_energy_low;
      was_pos_sense_ready     = true;

    else

      % we need a further delay for the positional sensing estimation, consider
      % this additional delay for the overall peak found offset
      if was_peak_found
        peak_found_offset = peak_found_offset + 1;
      end

    end

  end

end


% Calculate rim shot detection -------------------------------------------------
if length(x) > 1 % rim piezo signal is in second dimension

  rim_shot_is_used = true;

  % one pole IIR high pass filter (y1 = (b0 * x1 + b1 * x0 - a1 * y0) / a0)
  rim_x_high       = (b_rim_high(1) * x(2) + b_rim_high(2) * rim_high_prev_x - a_rim_high * rim_x_high);
  rim_high_prev_x  = x(2); % store previous x
  x_rim_high_debug = rim_x_high; % just for debugging

  rim_x_high_hist = update_fifo(rim_x_high, rim_shot_window_len, rim_x_high_hist);

  % start condition of delay process to fill up the required buffers
  % note that rim_shot_window_len must be larger than energy_window_len,
  % pos_energy_window_len and scan_time for this to work
  if first_peak_found && (~was_rim_shot_ready) && (rim_shot_cnt == 0)

    % a peak was found, we now have to start the delay process to fill up the
    % required buffer length for our metric
    rim_shot_cnt     = rim_shot_window_len / 2 - 1;
    hil_filt_max_pow = hil_filt;

  end

  if rim_shot_cnt > 0

    rim_shot_cnt = rim_shot_cnt - 1;

    % end condition
    if rim_shot_cnt <= 0

      % the buffers are filled, now calculate the metric
      rim_max_pow        = max(rim_x_high_hist .* rim_x_high_hist);
      rim_max_pow_debug  = rim_max_pow; % just for debugging
      rim_metric_db      = 10 * log10(rim_max_pow / hil_filt_max_pow);
      stored_is_rimshot  = rim_metric_db > rim_shot_treshold_dB;
      rim_shot_cnt       = 0;
      was_rim_shot_ready = true;

    else

      % we need a further delay for the positional sensing estimation, consider
      % this additional delay for the overall peak found offset
      if was_peak_found && (~pos_sense_is_used || was_pos_sense_ready)
        peak_found_offset = peak_found_offset + 1;
      end

    end

  end

end

% check for all estimations are ready and we can set the peak found flag and
% return all results
if was_peak_found && (~pos_sense_is_used || was_pos_sense_ready) && (~rim_shot_is_used || was_rim_shot_ready)

  pos_sense_metric = stored_pos_sense_metric;
  peak_found       = true;
  is_rim_shot      = stored_is_rimshot;

  was_peak_found      = false;
  was_pos_sense_ready = false;
  was_rim_shot_ready  = false;

end

end


