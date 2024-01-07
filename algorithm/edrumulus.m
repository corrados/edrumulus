%*******************************************************************************
% Copyright (c) 2020-2024
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

function edrumulus(x, pad_input)

global pad;

%close all

% load signal and pad settings
if ~exist('x', 'var') || ~exist('pad_input', 'var')
  [x, pad] = signalsandsettings(true);
else
  pad = pad_input;
end

Setup();

% loop
x_filt           = nan(size(x, 1), 1);
pre_scan_region  = nan(size(x, 1), 1);
scan_region      = nan(size(x, 1), 1);
mask_region      = nan(size(x, 1), 1);
decay_est_rng    = nan(size(x, 1), 1);
decay_all        = nan(size(x, 1), 1);
x_filt_decay     = nan(size(x, 1), 1);
all_peaks        = [];
all_first_peaks  = [];
all_peaks_filt   = [];
pos_sense_metric = [];
is_rim_shot      = logical([]);
rim_metric_db    = [];

for i = 1:size(x, 1)

  [x_filt, ...
   pre_scan_region, ...
   scan_region, ...
   mask_region, ...
   decay_est_rng, ...
   decay_all, ...
   x_filt_decay, ...
   all_peaks, ...
   all_first_peaks, ...
   all_peaks_filt, ...
   pos_sense_metric, ...
   is_rim_shot, ...
   rim_metric_db] = process_sample(x(i, :), i, ...
                                   x_filt, ...
                                   pre_scan_region, ...
                                   scan_region, ...
                                   mask_region, ...
                                   decay_est_rng, ...
                                   decay_all, ...
                                   x_filt_decay, ...
                                   all_peaks, ...
                                   all_first_peaks, ...
                                   all_peaks_filt, ...
                                   pos_sense_metric, ...
                                   is_rim_shot, ...
                                   rim_metric_db);

end

figure;
plot(10 * log10([mask_region, scan_region, pre_scan_region, decay_est_rng]), 'LineWidth', 20);
grid on; hold on; set(gca, 'ColorOrderIndex', 1); % reset color order so that x trace is blue and so on
plot(10 * log10([x(:, 1) .^ 2, x_filt, decay_all, x_filt_decay]));
plot(all_first_peaks, 10 * log10(x(all_first_peaks, 1) .^ 2), 'b*');
plot(all_peaks,  10 * log10(x(all_peaks, 1) .^ 2), 'g*');
plot(all_peaks_filt, 10 * log10(x_filt(all_peaks_filt)), 'y*');
if ~isempty(rim_metric_db) && (length(all_peaks) == length(rim_metric_db))
  plot(all_peaks, rim_metric_db + 40, '*-');
  plot(all_peaks(is_rim_shot), rim_metric_db(is_rim_shot) + 40, '*');
  plot(all_peaks(~is_rim_shot), rim_metric_db(~is_rim_shot) + 40, '*');
end
if length(all_first_peaks) == length(pos_sense_metric)
  plot(all_first_peaks,  10 * log10(pos_sense_metric) + 40, 'k*');
end
plot([1, length(x_filt)], [pad.threshold_db, pad.threshold_db], '--');
title('Green marker: level; Black marker: position; Blue marker: first peak'); xlabel('samples'); ylabel('dB');
ylim([-10, 90]);

% TEST for edrumulus porting...
%axis([61, 293, 14, 71]);
%axis([2871, 3098, 12, 67]);

return;

% TEST
% prepare serial port
pkg load instrument-control
try
  a = serialport("/dev/ttyACM0", 115200);
catch
end
flush(a);
% send the input data vector
for i = 1:length(x)
  % write sample
  write(a, sprintf('%.5f\n', x(i, 1)), 'char');
  if ( size(x, 2) > 1 )
    write(a, sprintf('%.5f\n', x(i, 2)), 'char');
  end
  % receive the return sample, get number from string
  readready = false; bytearray = uint8([]);
  while ~readready
    val = fread(a, 1);
    if val == 13
      readready = true;
    end
    bytearray = [bytearray, uint8(val)];
  end
  y(i) = str2double(char(bytearray));
end
%figure; plot(10 * log10(abs(circshift(y, -27)))+40,'*'); grid on;
%figure; plot(10 * log10(y)); grid on;
figure; plot(10 * log10(y),'*'); grid on;
%figure; plot(20 * log10(abs(y))); grid on;
%figure; plot(y+40, '*'); grid on;
%figure; plot(y, '*'); grid on;
ylim([-10, 90]);

end


function Setup

global pad Fs bp_filt_a bp_filt_b bp_filt_len bp_filt_hist_x bp_filt_hist_y x_filt_delay;
global scan_time scan_time_cnt pre_scan_time total_scan_time;
global mask_time mask_back_cnt threshold first_peak_diff_thresh was_above_threshold;
global peak_val first_peak_val decay_mask_fact;
global decay_pow_est_start_cnt decay_pow_est_cnt decay_pow_est_sum;
global decay_est_delay decay_est_len decay_est_fact decay_fact decay_len;
global decay decay_back_cnt decay_scaling;
global x_sq_hist x_sq_hist_len lp_filt_b lp_filt_hist lp_filt_len;
global x_low_hist x_low_hist_len pos_sense_cnt x_low_hist_idx;
global rim_shot_window_len rim_shot_treshold_dB x_rim_hist x_rim_hist_len rim_shot_cnt x_rim_hist_idx;
global stored_pos_sense_metric stored_is_rimshot rim_bp_filt_a rim_bp_filt_b rim_bp_hist_x rim_bp_hist_y;
global max_x_filt_val max_mask_x_filt_val max_x_filt_idx_debug;
global was_peak_found was_pos_sense_ready was_rim_shot_ready;

Fs                       = 8000;
bp_filt_len              = 5;
bp_filt_a                = [6.704579059531744e-01, -2.930427216820138, 4.846289804288025, -3.586239808116909]';
bp_filt_b                = [1.658193166930305e-02, 0, -3.316386333860610e-02, 0, 1.658193166930305e-02]';
rim_bp_low_freq_a        = [0.8008026466657076, -3.348944421626415, 5.292099516163272, -3.743650976941178]';
rim_bp_low_freq_b        = [0.005542717210280682, 0, -0.01108543442056136, 0, 0.005542717210280682]';
rim_bp_high_freq_a       = [0.8008026466657077, -3.021126408169798, 4.637919662489649, -3.377196335768073]';
rim_bp_high_freq_b       = [0.00554271721028068, 0, -0.01108543442056136, 0, 0.00554271721028068]';
x_filt_delay             = 5;
scan_time                = round(pad.scan_time_ms * 1e-3 * Fs);
scan_time_cnt            = 0;
pre_scan_time            = round(pad.pre_scan_time_ms * 1e-3 * Fs);
total_scan_time          = scan_time + pre_scan_time; % includes pre-scan time
mask_time                = round(pad.mask_time_ms * 1e-3 * Fs);
mask_back_cnt            = 0;
threshold                = 10 ^ (pad.threshold_db / 10);
first_peak_diff_thresh   = 10 ^ (pad.first_peak_diff_thresh_db / 10);
was_above_threshold      = false;
peak_val                 = 0;
first_peak_val           = 0;
decay_pow_est_start_cnt  = 0;
decay_pow_est_cnt        = 0;
decay_pow_est_sum        = 0;
decay_est_delay          = round(pad.decay_est_delay_ms * 1e-3 * Fs);
decay_est_len            = round(pad.decay_est_len_ms * 1e-3 * Fs);
decay_est_fact           = 10 ^ (pad.decay_est_fact_db / 10);
decay_fact               = 10 ^ (pad.decay_fact_db / 10);
decay_back_cnt           = 0;
decay_scaling            = 1;
decay_mask_fact          = 10 ^ (pad.mask_time_decay_fact_db / 10);
pos_sense_cnt            = 0;
x_low_hist_idx           = 1;
bp_filt_hist_x           = zeros(bp_filt_len, 1);
bp_filt_hist_y           = zeros(bp_filt_len - 1, 1);
x_sq_hist_len            = total_scan_time;
x_sq_hist                = zeros(x_sq_hist_len, 1);
rim_bp_hist_x            = zeros(bp_filt_len, 1);
rim_bp_hist_y            = zeros(bp_filt_len - 1, 1);
rim_shot_window_len      = round(pad.rim_shot_window_len_ms * 1e-3 * Fs);
x_rim_hist_len           = x_sq_hist_len + rim_shot_window_len;
x_rim_hist_idx           = 1;
rim_shot_treshold_dB     = -19; % dB
x_rim_hist               = zeros(x_rim_hist_len, 1);
rim_shot_cnt             = 0;
stored_pos_sense_metric  = 0;
stored_is_rimshot        = false;
max_x_filt_val           = 0;
max_mask_x_filt_val      = 0;
max_x_filt_idx_debug     = 0;
was_peak_found           = false;
was_pos_sense_ready      = false;
was_rim_shot_ready       = false;
decay_len1               = round(pad.decay_len_ms1 * 1e-3 * Fs);
decay_grad1              = pad.decay_grad_fact1 / Fs;
decay_len2               = round(pad.decay_len_ms2 * 1e-3 * Fs);
decay_grad2              = pad.decay_grad_fact2 / Fs; % decay gradient factor
decay_len3               = round(pad.decay_len_ms3 * 1e-3 * Fs);
decay_grad3              = pad.decay_grad_fact3 / Fs;

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

% positional sensing low-pass filter coefficients and result histories
% moving average cut off frequency approximation according to:
% https://dsp.stackexchange.com/questions/9966/what-is-the-cut-off-frequency-of-a-moving-average-filter
lp_cutoff_norm = pad.pos_low_pass_cutoff / Fs;
lp_filt_len    = round(sqrt(0.196202 + lp_cutoff_norm * lp_cutoff_norm) / lp_cutoff_norm);
if mod(lp_filt_len, 2) == 0
  lp_filt_len = lp_filt_len + 1; % make sure we have an odd length
endif
lp_half_len = (lp_filt_len - 1) / 2;
lp_filt_b   = zeros(lp_filt_len, 1);
for i = 1:lp_filt_len
  if i <= lp_half_len
    lp_filt_b(i) = (0.5 + (i - 1) * 0.5 / lp_half_len) / lp_filt_len;
  elseif i == lp_half_len + 1
    lp_filt_b(i) = 1 / lp_filt_len;
  else
    lp_filt_b(i) = lp_filt_b(lp_filt_len - i + 1);
  end
end
lp_filt_hist   = zeros(lp_filt_len, 1);
x_low_hist_len = x_sq_hist_len + lp_filt_len;
x_low_hist     = zeros(x_low_hist_len, 1);

% select rim shot signal band-pass filter coefficients
if pad.rim_use_low_freq_bp
  rim_bp_filt_a = rim_bp_low_freq_a;
  rim_bp_filt_b = rim_bp_low_freq_b;
else
  rim_bp_filt_a = rim_bp_high_freq_a;
  rim_bp_filt_b = rim_bp_high_freq_b;
end

end


function fifo_memory = update_fifo(input, ...
                                   fifo_length, ...
                                   fifo_memory)

  % move all values in the history one step back and put new value on the top
  fifo_memory(1:fifo_length - 1) = fifo_memory(2:fifo_length);
  fifo_memory(fifo_length)       = input;

end


function [x_filt_debug, ...
          pre_scan_region_debug, ...
          scan_region_debug, ...
          mask_region_debug, ...
          decay_est_rng_debug, ...
          decay_all_debug, ...
          x_filt_decay_debug, ...
          all_peaks_debug, ...
          all_first_peaks_debug, ...
          all_peaks_filt_debug, ...
          pos_sense_metric, ...
          is_rim_shot_debug, ...
          rim_metric_db_debug] = process_sample(x, i, ...
                                                x_filt_debug, ...
                                                pre_scan_region_debug, ...
                                                scan_region_debug, ...
                                                mask_region_debug, ...
                                                decay_est_rng_debug, ...
                                                decay_all_debug, ...
                                                x_filt_decay_debug, ...
                                                all_peaks_debug, ...
                                                all_first_peaks_debug, ...
                                                all_peaks_filt_debug, ...
                                                pos_sense_metric, ...
                                                is_rim_shot_debug, ...
                                                rim_metric_db_debug)

global Fs bp_filt_a bp_filt_b bp_filt_len bp_filt_hist_x bp_filt_hist_y x_filt_delay;
global scan_time scan_time_cnt pre_scan_time total_scan_time;
global mask_time mask_back_cnt threshold first_peak_diff_thresh was_above_threshold;
global peak_val first_peak_val decay_mask_fact;
global decay_pow_est_start_cnt decay_pow_est_cnt decay_pow_est_sum;
global decay_est_delay decay_est_len decay_est_fact decay_fact decay_len;
global decay decay_back_cnt decay_scaling;
global x_sq_hist x_sq_hist_len lp_filt_b lp_filt_hist lp_filt_len;
global x_low_hist x_low_hist_len pos_sense_cnt x_low_hist_idx;
global rim_shot_window_len rim_shot_treshold_dB x_rim_hist x_rim_hist_len rim_shot_cnt x_rim_hist_idx;
global stored_pos_sense_metric stored_is_rimshot rim_bp_filt_a rim_bp_filt_b rim_bp_hist_x rim_bp_hist_y;
global max_x_filt_val max_mask_x_filt_val max_x_filt_idx_debug;
global was_peak_found was_pos_sense_ready was_rim_shot_ready;

% initialize return parameter
peak_found        = false;
is_rim_shot       = false;
first_peak_found  = false; % only used internally
peak_delay        = 0;     % only used internally
first_peak_delay  = 0;     % only used internally
pos_sense_is_used = true;  % only used internally to enable/disable positional sensing
rim_shot_is_used  = false; % only used internally

% square input signal and store in FIFO buffer
x_sq      = x .^ 2;
x_sq_hist = update_fifo(x_sq(1), x_sq_hist_len, x_sq_hist);


% Calculate peak detection -----------------------------------------------------
% IIR band-pass filter
bp_filt_hist_x  = update_fifo(x(1), bp_filt_len, bp_filt_hist_x);
x_filt          = sum(bp_filt_hist_x .* bp_filt_b) - sum(bp_filt_hist_y .* bp_filt_a);
bp_filt_hist_y  = update_fifo(x_filt, bp_filt_len - 1, bp_filt_hist_y);
x_filt          = x_filt * x_filt; % calculate power of filter result
x_filt_debug(i) = x_filt; % just for debugging

% exponential decay assumption
if decay_back_cnt > 0

  % subtract decay (with clipping at zero)
  cur_decay          = decay_scaling * decay(1 + decay_len - decay_back_cnt);
  x_filt_decay       = x_filt - cur_decay;
  decay_back_cnt     = decay_back_cnt - 1;
  decay_all_debug(i) = cur_decay; % just for debugging

  if x_filt_decay < 0
    x_filt_decay = 0;
  end

else
  x_filt_decay = x_filt;
end
x_filt_decay_debug(i) = x_filt_decay; % just for debugging

% during the mask time we apply a constant value to the decay way above the
% detected peak to avoid missing a loud hit which is preceeded with a very
% low volume hit which mask period would delete the loud hit
if (mask_back_cnt > 0) && (mask_back_cnt <= mask_time)

  decay_all_debug(i) = max_mask_x_filt_val * decay_mask_fact; % just for debugging

  if x_filt > max_mask_x_filt_val * decay_mask_fact

    was_above_threshold = false;  % reset the peak detection (note that x_filt_decay is always > threshold now)
    x_filt_decay        = x_filt; % remove decay subtraction
    pos_sense_cnt       = 0;      % needed since we reset the peak detection
    was_pos_sense_ready = false;  % needed since we reset the peak detection
    rim_shot_cnt        = 0;      % needed since we reset the peak detection
    was_rim_shot_ready  = false;  % needed since we reset the peak detection
    decay_all_debug(i)  = nan;    % invalided debug value for this special case, just for debugging

  end

end

% threshold test
if (x_filt_decay > threshold) || was_above_threshold

  % initializations at the time when the signal was above threshold for the
  % first time for the current peak
  if ~was_above_threshold

    decay_pow_est_start_cnt = max(1, decay_est_delay - x_filt_delay + 1);
    scan_time_cnt           = max(1, scan_time - x_filt_delay);
    mask_back_cnt           = scan_time + mask_time;
    decay_back_cnt          = 0;      % reset in case it was active from previous peak
    max_x_filt_val          = x_filt; % initialize maximum value with first value
    max_mask_x_filt_val     = x_filt; % initialize maximum value with first value
    max_x_filt_idx_debug    = i;      % only for debugging

  end

  % this flag ensures that we always enter the if condition after the very first
  % time the signal was above the threshold (this flag is then reset when the
  % scan time is expired)
  was_above_threshold = true;

  % search from above threshold to corrected scan+mask time for highest peak in
  % filtered signal (needed for decay power estimation)
  if x_filt > max_x_filt_val

    max_x_filt_val       = x_filt;
    max_x_filt_idx_debug = i; % only for debugging

  end

  % search from above threshold in scan time region needed for decay mask factor
  if (mask_back_cnt > mask_time) && (x_filt > max_mask_x_filt_val)
    max_mask_x_filt_val = x_filt;
  end

  scan_time_cnt = scan_time_cnt - 1;
  mask_back_cnt = mask_back_cnt - 1;

  % end condition of scan time
  if scan_time_cnt == 0

    % climb to the maximum of the first peak (using the unfiltered signal)
    first_peak_found = false;
    first_peak_val   = x_sq_hist(x_sq_hist_len - total_scan_time + 1);
    first_peak_idx   = 1;

    for idx = 2:total_scan_time

      cur_x_sq_hist_val  = x_sq_hist(x_sq_hist_len - total_scan_time + idx);
      prev_x_sq_hist_val = x_sq_hist(x_sq_hist_len - total_scan_time + idx - 1);

      if (first_peak_val < cur_x_sq_hist_val) && ~first_peak_found

        first_peak_val = cur_x_sq_hist_val;
        first_peak_idx = idx;

      else

        first_peak_found = true;

        % check if there is a much larger first peak
        if (prev_x_sq_hist_val > cur_x_sq_hist_val) && (first_peak_val * first_peak_diff_thresh < prev_x_sq_hist_val)

          first_peak_val = prev_x_sq_hist_val;
          first_peak_idx = idx - 1;

        end

      end

    end

    % get the maximum velocity in the scan time using the unfiltered signal
    [peak_val, peak_velocity_idx] = max(x_sq_hist(x_sq_hist_len + (-scan_time + 1:0)));

    % peak detection results
    peak_delay       = scan_time - peak_velocity_idx;
    first_peak_delay = total_scan_time - first_peak_idx;
    first_peak_found = true; % for special case signal only increments, the peak found would be false -> correct this
    was_peak_found   = true;

    % debugging outputs
    if i - scan_time + peak_velocity_idx > 0
      all_peaks_debug = [all_peaks_debug; i - scan_time + peak_velocity_idx];
    end
    if i - total_scan_time > 0
      pre_scan_region_debug(i - total_scan_time + (1:pre_scan_time)) = first_peak_val;
      scan_region_debug(i + (-scan_time + 1:0))                      = first_peak_val;
    end
    if i - total_scan_time + first_peak_idx > 0
      all_first_peaks_debug = [all_first_peaks_debug; i - total_scan_time + first_peak_idx];
    end

  end

  % end condition of mask time
  if mask_back_cnt == 0

    decay_back_cnt      = decay_len; % per definition decay starts right after mask time
    decay_scaling       = decay_fact * max_x_filt_val; % take maximum of filtered signal in scan+mask time
    was_above_threshold = false;

    % debugging outputs
    all_peaks_filt_debug = [all_peaks_filt_debug; max_x_filt_idx_debug];
    mask_region_debug(i - mask_time - x_filt_delay + (1:mask_time + x_filt_delay)) = first_peak_val;

  end

end

% decay power estimation
if decay_pow_est_start_cnt > 0

  decay_pow_est_start_cnt = decay_pow_est_start_cnt - 1;

  % end condition
  if decay_pow_est_start_cnt == 0
    decay_pow_est_cnt = decay_est_len; % now the power estimation can start
  end

end

if decay_pow_est_cnt > 0

  decay_pow_est_sum = decay_pow_est_sum + x_filt; % sum up the powers in pre-defined interval
  decay_pow_est_cnt = decay_pow_est_cnt - 1;

  % end condition
  if decay_pow_est_cnt == 0

    decay_power       = decay_pow_est_sum / decay_est_len;                % calculate average power
    decay_pow_est_sum = 0;                                                % we have to reset the sum for the next calculation
    decay_scaling     = min(decay_scaling, decay_est_fact * decay_power); % adjust the decay curve

    decay_est_rng_debug(i + (-decay_est_len + 1:0)) = decay_power; % only for debugging

  end

end


% Calculate positional sensing -------------------------------------------------
if pos_sense_is_used

  % low pass filter of the input signal and store results in a FIFO
  lp_filt_hist = update_fifo(x(1), lp_filt_len, lp_filt_hist);
  x_low        = sum(lp_filt_hist .* lp_filt_b);
  x_low_hist   = update_fifo(x_low * x_low, x_low_hist_len, x_low_hist);

  % start condition of delay process to fill up the required buffers
  if first_peak_found && (~was_pos_sense_ready) && (pos_sense_cnt == 0)

    % a peak was found, we now have to start the delay process to fill up the
    % required buffer length for our metric
    pos_sense_cnt  = max(1, lp_filt_len - first_peak_delay);
    x_low_hist_idx = x_low_hist_len - lp_filt_len - max(0, first_peak_delay - lp_filt_len + 1) + 1;

  end

  if pos_sense_cnt > 0

    pos_sense_cnt = pos_sense_cnt - 1;

    % end condition
    if pos_sense_cnt == 0

      % the buffers are filled, now calculate the metric
      peak_energy_low         = max(x_low_hist(x_low_hist_idx + (0:lp_filt_len - 1)));
      stored_pos_sense_metric = first_peak_val / peak_energy_low;
      was_pos_sense_ready     = true;

    end

  end

end


% Calculate rim shot detection -------------------------------------------------
if length(x) > 1 % rim piezo signal is in second dimension

  rim_shot_is_used = true;

  % band-pass filter the rim signal (two types are supported)
  rim_bp_hist_x = update_fifo(x(2), bp_filt_len, rim_bp_hist_x);
  x_rim_bp      = sum(rim_bp_hist_x .* rim_bp_filt_b) - sum(rim_bp_hist_y .* rim_bp_filt_a);
  rim_bp_hist_y = update_fifo(x_rim_bp, bp_filt_len - 1, rim_bp_hist_y);
  x_rim_bp      = x_rim_bp * x_rim_bp; % calculate power of filter result
  x_rim_hist    = update_fifo(x_rim_bp, x_rim_hist_len, x_rim_hist);

  % start condition of delay process to fill up the required buffers
  if was_peak_found && (~was_rim_shot_ready) && (rim_shot_cnt == 0)

    % a peak was found, we now have to start the delay process to fill up the
    % required buffer length for our metric
    rim_shot_cnt   = max(1, rim_shot_window_len - peak_delay);
    x_rim_hist_idx = x_rim_hist_len - rim_shot_window_len - max(0, peak_delay - rim_shot_window_len + 1) + 1;

  end

  if rim_shot_cnt > 0

    rim_shot_cnt = rim_shot_cnt - 1;

    % end condition
    if rim_shot_cnt == 0

      % the buffers are filled, now calculate the metric
      rim_max_pow         = max(x_rim_hist(x_rim_hist_idx + (0:rim_shot_window_len - 1)));
      rim_metric_db       = 10 * log10(rim_max_pow / peak_val);
      rim_metric_db_debug = [rim_metric_db_debug; rim_metric_db]; % just for debugging
      stored_is_rimshot   = rim_metric_db > rim_shot_treshold_dB;
      rim_shot_cnt        = 0;
      was_rim_shot_ready  = true;

    end

  end

end

% check for all estimations are ready and we can set the peak found flag and
% return all results
if was_peak_found && (~pos_sense_is_used || was_pos_sense_ready) && (~rim_shot_is_used || was_rim_shot_ready)

  pos_sense_metric  = [pos_sense_metric; stored_pos_sense_metric];
  peak_found        = true;
  is_rim_shot       = stored_is_rimshot;
  is_rim_shot_debug = [is_rim_shot_debug; is_rim_shot]; % just for debugging

  was_peak_found      = false;
  was_pos_sense_ready = false;
  was_rim_shot_ready  = false;

end

end


