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

function drumtrigger
global pad;

% Drum trigger tests

close all;
pkg load signal

Fs      = 8000; % Hz
padtype = 'pd120'; % default

% TEST process recordings
%x = audioread("signals/teensy4_0_noise_test.wav");x=x-mean(x);padtype = 'pd80r';
%x = audioread("signals/teensy4_0_pd80r.wav");x=x-mean(x);padtype = 'pd80r';x = x(1:390000, :);%
%x = audioread("signals/esp32_pd120.wav");
%x = audioread("signals/esp32_pd8.wav");padtype = 'pd8';
%x = audioread("signals/pd120_pos_sense.wav");%x = x(2900:10000, :);%x = x(55400:58000, :);%
%x = audioread("signals/pd120_pos_sense2.wav");
x = audioread("signals/pd120_single_hits.wav");
%x = audioread("signals/pd120_roll.wav");%x = x(292410:294749, :);%x = x(311500:317600, :);
%x = audioread("signals/pd120_middle_velocity.wav");
%x = audioread("signals/pd120_hot_spot.wav");
%x = audioread("signals/pd120_rimshot.wav");%x = x(168000:171000, :);%x = x(1:34000, :);%x = x(1:100000, :);
%x = audioread("signals/pd120_rimshot_hardsoft.wav");
%x=audioread("signals/pd120_middle_velocity.wav");x=[x;audioread("signals/pd120_pos_sense2.wav")];x=[x;audioread("signals/pd120_hot_spot.wav")];
%x = audioread("signals/pd80r.wav");padtype = 'pd80r';x = x(1:265000, :);%x = x(52000:60000, :);%x = x(260000:360000, :);%x = x(130000:176000, :);%
%x = audioread("signals/pd6.wav");
%x = audioread("signals/pd8.wav");padtype = 'pd8';%x = x(1:300000, :);%x = x(420000:470000, :);%x = x(1:100000, :);
%x = audioread("signals/pd8_rimshot.wav");padtype = 'pd8';
%x = audioread("signals/cy6.wav");padtype = 'cy6';x = x(480000:590000, :);%x = x(250000:450000, :);%x = x(1:150000, :);
%x = audioread("signals/cy8.wav");padtype = 'cy8';%x = x(1:200000, :);
%x = audioread("signals/kd8.wav");
%x = audioread("signals/kd7.wav");padtype = 'kd7';%x = x(1:170000, :);
%x = audioread("signals/tp80.wav");padtype = 'tp80';x = x(1:318000, :);
%x = audioread("signals/vh12.wav");padtype = 'vh12';%x = x(900000:end, :);%x = x(376000:420000, :);%x = x(1:140000, :);
%org = audioread("signals/snare.wav"); x = resample(org(:, 1), 1, 6); % PD-120
%org = audioread("signals/snare.wav"); x = org(:, 1); Fs = 48e3; % PD-120



% pad PRESET settings first, then overwrite these with pad specific properties
pad.threshold_db          = 0;
pad.mask_time_ms          = 6;
pad.energy_win_len_ms     = 0.3;
pad.scan_time_ms          = 2.5;
pad.main_peak_dist_ms     = 2.25;
pad.decay_est_delay2nd_ms = 2.5;
pad.decay_est_len_ms      = 3;
pad.decay_est_fact_db     = 15;
pad.decay_fact_db         = 1;
pad.decay_len_ms1         = 0; % not used
pad.decay_len_ms2         = 250;
pad.decay_len_ms3         = 0; % not used
pad.decay_grad_fact1      = 200;
pad.decay_grad_fact2      = 200;
pad.decay_grad_fact3      = 200;
pad.pos_low_pass_cutoff   = 150; % Hz
pad.pos_invert            = false;

switch padtype
  case 'pd120'
    % note: the PRESET settings are from the PD120 pad
    pad.decay_len_ms1         = 10;
    pad.decay_grad_fact1      = 30;
    pad.decay_len_ms2         = 250;
    pad.decay_grad_fact2      = 220;
    pad.decay_len_ms3         = 0; % not used
    pad.pos_low_pass_cutoff   = 100; % Hz
case 'pd80r'
    pad.scan_time_ms          = 3;
    pad.main_peak_dist_ms     = 2.4;
    pad.decay_len_ms1         = 10;
    pad.decay_grad_fact1      = 30;
    pad.decay_len_ms2         = 75;
    pad.decay_grad_fact2      = 300;
    pad.decay_len_ms3         = 300;
    pad.decay_grad_fact3      = 100;
    pad.pos_low_pass_cutoff   = 150; % Hz
  case 'pd8'
    pad.scan_time_ms          = 1.3;
    pad.main_peak_dist_ms     = 0.75;
    pad.decay_est_delay2nd_ms = 6;
    pad.mask_time_ms          = 7;
    pad.decay_fact_db         = 5;
    pad.decay_len_ms1         = 10;
    pad.decay_grad_fact1      = 30;
    pad.decay_len_ms2         = 30;
    pad.decay_grad_fact2      = 600;
    pad.decay_len_ms3         = 150;
    pad.decay_grad_fact3      = 120;
  case 'tp80'
    pad.scan_time_ms          = 2.75;
    pad.main_peak_dist_ms     = 2;
    pad.decay_est_delay2nd_ms = 7;
    pad.decay_len_ms1         = 10;
    pad.decay_grad_fact1      = 30;
    pad.decay_len_ms2         = 30;
    pad.decay_grad_fact2      = 600;
    pad.decay_len_ms3         = 700;
    pad.decay_grad_fact3      = 60;
    pad.pos_low_pass_cutoff   = 150; % Hz
    pad.pos_invert            = true;
  case 'vh12'
% TODO if the Hi-Hat is open just a little bit, we get double triggers
    pad.threshold_db          = 16;
    pad.scan_time_ms          = 4;
    pad.main_peak_dist_ms     = 0.75;
    pad.decay_est_delay2nd_ms = 5;
    pad.decay_fact_db         = 5;
    pad.decay_len_ms1         = 4;
    pad.decay_grad_fact1      = 30;
    pad.decay_len_ms2         = 27;
    pad.decay_grad_fact2      = 700;
    pad.decay_len_ms3         = 600; % must be long because of open Hi-Hat ringing
    pad.decay_grad_fact3      = 75;
  case 'kd7'
    pad.scan_time_ms          = 3.5;
    pad.main_peak_dist_ms     = 2;
    pad.decay_est_delay2nd_ms = 4;
    pad.decay_fact_db         = 5;
    pad.decay_len_ms1         = 4;
    pad.decay_grad_fact1      = 30;
    pad.decay_len_ms2         = 30;
    pad.decay_grad_fact2      = 450;
    pad.decay_len_ms3         = 500;
    pad.decay_grad_fact3      = 45;
  case 'cy6'
    pad.scan_time_ms          = 6;
    pad.main_peak_dist_ms     = 2;
    pad.decay_fact_db         = 4;
    pad.decay_len_ms1         = 20;
    pad.decay_grad_fact1      = 400;
    pad.decay_len_ms2         = 150;
    pad.decay_grad_fact2      = 120;
    pad.decay_len_ms3         = 450;
    pad.decay_grad_fact3      = 30;
  case 'cy8'
    pad.scan_time_ms          = 6;
    pad.main_peak_dist_ms     = 2;
    pad.decay_fact_db         = 7;
    pad.decay_len_ms1         = 40;
    pad.decay_grad_fact1      = 10;
    pad.decay_len_ms2         = 100;
    pad.decay_grad_fact2      = 120;
    pad.decay_len_ms3         = 450;
    pad.decay_grad_fact3      = 30;
end


% % TEST call reference mode for C++ implementation
% edrumulus(x);

% % TEST use 4 kHz sampling rate
% x = resample(x, 1, 2); Fs = Fs / 2;

% % TEST simulate a DC offset -> TODO the algorithms needs a DC offset compensation
% x = x + 0.01;

% % TEST quantize to 12 bit resolution as available in ESP32 micro controller
% iNumBits = 10;%12; % reserve 2 bits for overload headroom -> 10 bits
% max_val  = max(abs(x));
% quant    = max_val / (2 ^ (iNumBits - 1) - 1);
% x        = round(x / quant);
% x        = x / max(abs(x)) * max_val;

processing(x * 25000, Fs); % scale to the ESP32 input range

end


function [x_movav, x_lpfilt] = filter_input_signal(x, Fs)
global pad;

energy_window_len = round(pad.energy_win_len_ms * 1e-3 * Fs); % hit energy estimation time window length (e.g. 2 ms)

x_movav = abs(filter(ones(energy_window_len, 1) / energy_window_len, 1, x(:, 1))) .^ 2; % moving average

alpha    = 400 / Fs;
x_lpfilt = abs(filter(alpha, [1, alpha - 1], x(:, 1))) .^ 2;

end


function [all_peaks, all_first_peaks, scan_region] = calc_peak_detection(x_movav, x_lpfilt, Fs)
global pad;

scan_region = nan(size(x_lpfilt));

first_peak_diff_thresh = 10 ^ (8 / 10); % 8 dB difference allowed
mask_time              = round(pad.mask_time_ms * 1e-3 * Fs); % mask time (e.g. 10 ms)
scan_time              = round(pad.scan_time_ms * 1e-3 * Fs); % scan time from first detected peak

% the following settings are trigger pad-specific
decay_len1         = round(pad.decay_len_ms1 * 1e-3 * Fs); % decay time (e.g. 250 ms)
decay_grad1        = pad.decay_grad_fact1 / Fs;            % decay gradient factor
decay_len2         = round(pad.decay_len_ms2 * 1e-3 * Fs);
decay_grad2        = pad.decay_grad_fact2 / Fs;
decay_len3         = round(pad.decay_len_ms3 * 1e-3 * Fs);
decay_grad3        = pad.decay_grad_fact3 / Fs;
main_peak_dist     = round(pad.main_peak_dist_ms * 1e-3 * Fs);
decay_est_delay2nd = round(pad.decay_est_delay2nd_ms * 1e-3 * Fs);
decay_est_len      = round(pad.decay_est_len_ms * 1e-3 * Fs);
decay_est_fact     = 10 ^ (pad.decay_est_fact_db / 10);

decay_curve1 = 10 ^ (pad.decay_fact_db / 10) * 10 .^ (-(0:decay_len1) / 10 * decay_grad1);
decay_curve2 = 10 .^ (-(0:decay_len2) / 10 * decay_grad2);
decay_curve3 = 10 .^ (-(0:decay_len3 - 1) / 10 * decay_grad3);
decay_curve  = [decay_curve1(1:end - 1), decay_curve1(end) * decay_curve2(1:end - 1), decay_curve1(end) * decay_curve2(end) * decay_curve3];
decay_len    = decay_len1 + decay_len2 + decay_len3;

last_peak_idx   = 0;
all_peaks       = [];
all_first_peaks = [];
all_sec_peaks   = [];
no_more_peak    = false;
x_lpfilt_decay  = x_lpfilt;
decay_all       = nan(size(x_lpfilt)); % only for debugging
decay_est_rng   = nan(size(x_lpfilt)); % only for debugging

while ~no_more_peak

  % find values above threshold, masking regions which are already done
  above_thresh = (x_lpfilt_decay > 10 ^ (pad.threshold_db / 10)) & [zeros(last_peak_idx, 1); ones(length(x_lpfilt_decay) - last_peak_idx, 1)];
  peak_start   = find(diff(above_thresh) > 0);

  % exit condition
  if isempty(peak_start)
    no_more_peak = true;
    continue;
  end

  % climb to the maximum of the first peak
  peak_idx = peak_start(1);
  max_idx  = find(x_lpfilt(1 + peak_idx:end) - x_lpfilt(peak_idx:end - 1) < 0);

  if ~isempty(max_idx)
    peak_idx = peak_idx + max_idx(1) - 1;
  end

  % find all peaks after the initial peak
  peak_idx_after_initial = find((x_lpfilt(2 + peak_idx:end) < x_lpfilt(1 + peak_idx:end - 1)) & ...
    (x_lpfilt(1 + peak_idx:end - 1) > x_lpfilt(peak_idx:end - 2)));

  scan_peaks_idx = peak_idx + peak_idx_after_initial(peak_idx_after_initial <= scan_time);

  % if a peak in the scan time is much higher than the initial peak, use that one
  much_higher_peaks = find(x_lpfilt(peak_idx) * first_peak_diff_thresh < x_lpfilt(scan_peaks_idx));

  if ~isempty(much_higher_peaks)
    peak_idx = scan_peaks_idx(much_higher_peaks(1));
  end

  all_first_peaks = [all_first_peaks; peak_idx];

  % search in a pre-defined scan time for the highest peak
  scan_indexes              = peak_idx:min(1 + peak_idx + scan_time - 1, length(x_lpfilt));
  [~, max_idx]              = max(x_lpfilt(scan_indexes));
  peak_idx                  = peak_idx + max_idx - 1;
  scan_region(scan_indexes) = x_lpfilt(peak_idx); % mark scan time region

  % calculate power left/right of detected peak for second main peak position detection
  first_peak_idx = peak_idx; % initialization

  if peak_idx - main_peak_dist < 1
    all_sec_peaks = [all_sec_peaks; 1];
  elseif peak_idx + main_peak_dist > length(x_lpfilt)
    all_sec_peaks = [all_sec_peaks; length(x_lpfilt)];
  else

    power_hypo_left  = x_lpfilt(peak_idx - main_peak_dist);
    power_hypo_right = x_lpfilt(peak_idx + main_peak_dist);

    if power_hypo_left > power_hypo_right

      all_sec_peaks  = [all_sec_peaks; peak_idx - main_peak_dist];
      first_peak_idx = peak_idx - main_peak_dist;

    else
      all_sec_peaks = [all_sec_peaks; peak_idx + main_peak_dist];
    end

  end

  % estimate current decay power
  decay_factor = x_lpfilt(peak_idx);

  if first_peak_idx + main_peak_dist + decay_est_delay2nd + decay_est_len - 1 <= length(x_lpfilt)

    % average power measured right after the two main peaks (it showed for high level hits
    % close to the pad center the decay has much lower power right after the main peaks) in
    % a predefined time intervall, but never use a higher decay factor than derived from the
    % main peak (in case a second hit is right behind our main peaks to avoid very high
    % decay curve placement)
    decay_power  = mean(x_lpfilt(first_peak_idx + main_peak_dist + decay_est_delay2nd + (0:decay_est_len - 1)));
    decay_factor = min(decay_factor, decay_est_fact * decay_power);

    decay_est_rng(first_peak_idx + main_peak_dist + decay_est_delay2nd + (0:decay_est_len - 1)) = decay_power; % only for debugging

  end

  % store the new detected peak
  all_peaks     = [all_peaks; peak_idx];
  last_peak_idx = min(first_peak_idx + mask_time, length(x_lpfilt));

  % exponential decay assumption
  decay           = decay_factor * decay_curve;
  decay_x         = first_peak_idx + (0:decay_len - 1) + 2; % NOTE "+ 2" delay needed for sample-wise processing
  valid_decay_idx = decay_x <= length(x_lpfilt_decay);
  decay           = decay(valid_decay_idx);
  decay_x         = decay_x(valid_decay_idx);

  % subtract decay (with clipping at zero)
  x_lpfilt_new                   = x_lpfilt_decay(decay_x) - decay.';
  x_lpfilt_new(x_lpfilt_new < 0) = 0;

  % update filtered signal
  x_lpfilt_decay(decay_x) = x_lpfilt_new;

  decay_all(decay_x) = decay; % only for debugging

end

figure; plot(10 * log10([x_lpfilt, x_lpfilt_decay, decay_all, decay_est_rng])); hold on;
plot(all_peaks, 10 * log10(x_lpfilt(all_peaks)), 'k*');
plot(all_sec_peaks, 10 * log10(x_lpfilt(all_sec_peaks)), 'y*');

end


function pos_sense_metric = calc_pos_sense_metric(x, x_movav, Fs, all_peaks)
global pad;

% low pass filter of the signal
% moving average cut off frequency approximation according to:
% https://dsp.stackexchange.com/questions/9966/what-is-the-cut-off-frequency-of-a-moving-average-filter
low_pass_cutoff_normalized  = pad.pos_low_pass_cutoff / Fs;
low_pass_moving_average_len = round(sqrt(0.196202 + low_pass_cutoff_normalized ^ 2) / low_pass_cutoff_normalized);
if mod(low_pass_moving_average_len, 2) == 1
  low_pass_moving_average_len = low_pass_moving_average_len + 1; % make sure we have an even length
endif
low_pass_moving_average_len
disp(['low-pass filter delay: ' num2str(low_pass_moving_average_len / 2 / 8) ' ms']);

l = low_pass_moving_average_len / 2 - 1;
b = [0.5:0.5 / l:1 1:-0.5 / l:0.5] / low_pass_moving_average_len;
%b = ones(low_pass_moving_average_len, 1) / low_pass_moving_average_len; % TEST
x_low = abs(filter(b, 1, x(:, 1))) .^ 2; % moving average

peak_energy     = [];
peak_energy_low = [];
all_peaks_low   = all_peaks;

for i = 1:length(all_peaks)

  % find first peak of low-pass filtered signal searching for the maximum in the
  % range of the low-pass moving average window length
  test_win_offset  = low_pass_moving_average_len / 2 - round(low_pass_moving_average_len / 2);
  test_win_idx     = (all_peaks(i):all_peaks(i) + low_pass_moving_average_len - 1) + test_win_offset;
  [~, test_max]    = max(x_low(test_win_idx));
  all_peaks_low(i) = all_peaks(i) + test_win_offset + test_max - 1;

  % use the filtered signal with energy window moving average as the
  % reference power for the positional sensing metric where the first peak
  % position is used
  peak_energy(i)     = x_movav(all_peaks(i));
  peak_energy_low(i) = x_low(all_peaks_low(i));

end

figure; plot(10 * log10([x_movav(1:length(x_low)), x_low])); hold on;
        plot(all_peaks, 10 * log10(abs(x_movav(all_peaks))), 'k*');
        plot(all_peaks_low, 20 * log10(abs(x_low(all_peaks_low))), 'g*');

if pad.pos_invert
  % add offset to get to similar range as non-inverted metric
  pos_sense_metric = 10 * log10(peak_energy_low) - 10 * log10(peak_energy);
else
  pos_sense_metric = 10 * log10(peak_energy) - 10 * log10(peak_energy_low);
end

% TODO only show peaks
%x_peaks = [];
%for i = 1:length(all_peaks)
%  x_peaks = [x_peaks, all_peaks(i) - 20:all_peaks(i) + 60];
%end
%x_peaks_inv = find(x_peaks);
%figure
%plot(10 * log10([abs(x(x_peaks, 1)) .^ 2, abs(x_low(x_peaks)) .^ 2])); grid on; hold on;
%%plot(21:80:length(all_peaks) * 80, 20 * log10(abs(x(all_peaks, 1))), 'y*');

end


function is_rim_shot = detect_rim_shot(x, x_movav, all_first_peaks, Fs)

is_rim_shot          = false(size(all_first_peaks));
rim_shot_window_len  = round(5e-3 * Fs); % scan time (e.g. 6 ms)
rim_shot_treshold_dB = 2.3; % dB

if size(x, 2) > 1

  % one pole IIR high pass filter
  [b, a]     = butter(1, 0.02, 'high');
  rim_x_high = filter(b, a, x(:, 2));

  for i = 1:length(all_first_peaks)

    win_idx            = (all_first_peaks(i):all_first_peaks(i) + rim_shot_window_len - 1) - rim_shot_window_len / 2;
    win_idx            = win_idx((win_idx <= length(rim_x_high)) & (win_idx > 0));
    rim_max_pow(i)     = max(abs(rim_x_high(win_idx)) .^ 2);
    x_movav_max_pow(i) = x_movav(all_first_peaks(i));

  end

  rim_metric_db = 10 * log10(rim_max_pow ./ x_movav_max_pow);
  is_rim_shot   = rim_metric_db > rim_shot_treshold_dB;

%figure;
%plot(10 * log10(abs([x_movav, rim_x_high]))); hold on; grid on;
%plot(all_first_peaks, rim_metric_db, '*-');
%plot(all_first_peaks(is_rim_shot), rim_metric_db(is_rim_shot), '*');
%plot(all_first_peaks(~is_rim_shot), rim_metric_db(~is_rim_shot), '*');

end

end


function processing(x, Fs)

% calculate peak detection and positional sensing
[x_movav, x_lpfilt]                       = filter_input_signal(x(:, 1), Fs);
[all_peaks, all_first_peaks, scan_region] = calc_peak_detection(x_movav, x_lpfilt, Fs);
is_rim_shot                               = detect_rim_shot(x, x_movav, all_first_peaks, Fs);
pos_sense_metric                          = calc_pos_sense_metric(x, x_movav, Fs, all_first_peaks);


% plot results
figure
plot(10 * log10([abs(x(:, 1)) .^ 2, x_movav, scan_region])); grid on; hold on;
plot(all_first_peaks, 10 * log10(x_movav(all_first_peaks)), 'y*');
plot(all_peaks, 10 * log10(x_movav(all_peaks)), 'g*');
plot(all_first_peaks, pos_sense_metric + 40, 'k*');
title('Green marker: level; Black marker: position');
xlabel('samples'); ylabel('dB');
ylim([-10, 90]);


% TEST
% velocity/positional sensing mapping and play MIDI notes
velocity            = (10 * log10(x_movav(all_peaks)) / 39) * 127 - 73;
velocity_clipped    = max(1, min(127, velocity));
pos_sensing         = (pos_sense_metric / 4) * 127 - 510;
pos_sensing_clipped = max(1, min(127, pos_sensing));
% figure; subplot(2, 1, 1), plot(velocity); title('velocity'); subplot(2, 1, 2), plot(pos_sensing); title('pos');

end


