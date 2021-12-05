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

% Edrumulus algorithm development

close all;
pkg load signal

Fs      = 8000; % Hz
padtype = 'pd120'; % default

% select the recording to process:
%x = audioread("signals/teensy4_0_noise_test.wav");x=(x-mean(x))*4;padtype = 'pd80r';
%x = audioread("signals/teensy4_0_pd80r.wav");x=(x-mean(x))*4;padtype = 'pd80r';%x = x(1:390000, :);%
%x = audioread("signals/esp32_pd120.wav");x=x/8;
%x = audioread("signals/esp32_pd8.wav");x=x/8;padtype = 'pd8';
%x = audioread("signals/pd120_pos_sense.wav");%x=x(10600:15000);%x = x(2900:10000, :);%x = x(55400:58000, :);%
%x = audioread("signals/pd120_pos_sense2.wav");
%x = audioread("signals/pd120_single_hits.wav");
%x = audioread("signals/pd120_roll.wav");%x = x(1:20000, :);%x = x(292410:294749, :);%x = x(311500:317600, :);
%x = audioread("signals/pd120_middle_velocity.wav");
%x = audioread("signals/pd120_hot_spot.wav");
%x = audioread("signals/pd120_rimshot.wav");%x=x(7000:15000,:);%x = x(1:100000, :);%x = x(168000:171000, :);%x = x(1:34000, :);%
%x = audioread("signals/pd120_rimshot_hardsoft.wav");
x=audioread("signals/pd120_middle_velocity.wav");x=[x;audioread("signals/pd120_pos_sense2.wav")];x=[x;audioread("signals/pd120_hot_spot.wav")];
%x = audioread("signals/pd80r.wav");x=x(:,1);padtype='pd80r';x = x(1:265000, :);%x = x(264000:320000, :);%
%x = audioread("signals/pd6.wav");
%x = audioread("signals/pd8.wav");padtype = 'pd8';%x = x(1:300000, :);%x = x(420000:470000, :);%x = x(1:100000, :);
%x = audioread("signals/pd8_rimshot.wav");padtype = 'pd8';
%x = audioread("signals/cy6.wav");padtype = 'cy6';%x = x(480000:590000, :);%x = x(250000:450000, :);%x = x(1:150000, :);
%x = audioread("signals/cy8.wav");padtype = 'cy8';%x = x(1:200000, :);
%x = audioread("signals/kd8.wav");
%x = audioread("signals/kd7.wav");padtype = 'kd7';%x = x(1:170000, :);
%x = audioread("signals/tp80.wav");padtype = 'tp80';
%x = audioread("signals/vh12.wav");padtype = 'vh12';%x = x(900000:end, :);%x = x(376000:420000, :);%x = x(1:140000, :);


% pad PRESET settings first, then overwrite these with pad specific properties
pad.threshold_db              = 17;
pad.mask_time_ms              = 6;
pad.first_peak_diff_thresh_db = 8;
pad.scan_time_ms              = 2.5;
pad.pre_scan_time_ms          = 3;
pad.decay_est_delay_ms        = 8;
pad.decay_est_len_ms          = 3;
pad.decay_est_fact_db         = 16;
pad.decay_fact_db             = 1;
pad.decay_len_ms1             = 0; % not used
pad.decay_len_ms2             = 250;
pad.decay_len_ms3             = 0; % not used
pad.decay_grad_fact1          = 200;
pad.decay_grad_fact2          = 200;
pad.decay_grad_fact3          = 200;
pad.pos_low_pass_cutoff       = 150; % Hz
pad.pos_invert                = false;

switch padtype
  case 'pd120'
    % note: the PRESET settings are from the PD120 pad
  case 'pd80r'
    pad.scan_time_ms       = 3;
    pad.decay_len_ms2      = 75;
    pad.decay_grad_fact2   = 300;
    pad.decay_len_ms3      = 300;
    pad.decay_grad_fact3   = 100;
  case 'pd8'
    pad.scan_time_ms       = 1.3;
    pad.decay_est_delay_ms = 6;
    pad.mask_time_ms       = 7;
    pad.decay_fact_db      = 5;
    pad.decay_len_ms2      = 30;
    pad.decay_grad_fact2   = 600;
    pad.decay_len_ms3      = 150;
    pad.decay_grad_fact3   = 120;
  case 'tp80'
    pad.scan_time_ms       = 2.75;
    pad.decay_est_delay_ms = 11;
    pad.decay_len_ms2      = 60;
    pad.decay_grad_fact2   = 400;
    pad.decay_len_ms3      = 700;
    pad.decay_grad_fact3   = 60;
    pad.pos_invert         = true;
  case 'vh12'
% TODO if the Hi-Hat is open just a little bit, we get double triggers
    pad.threshold_db       = 16;
    pad.scan_time_ms       = 4;
    pad.decay_est_delay_ms = 9;
    pad.decay_fact_db      = 5;
    pad.decay_len_ms2      = 27;
    pad.decay_grad_fact2   = 700;
    pad.decay_len_ms3      = 600; % must be long because of open Hi-Hat ringing
    pad.decay_grad_fact3   = 75;
  case 'kd7'
    pad.scan_time_ms       = 3.5;
    pad.decay_est_delay_ms = 8;
    pad.decay_fact_db      = 5;
    pad.decay_len_ms1      = 4;
    pad.decay_grad_fact1   = 30;
    pad.decay_len_ms2      = 30;
    pad.decay_grad_fact2   = 450;
    pad.decay_len_ms3      = 500;
    pad.decay_grad_fact3   = 45;
  case 'cy6'
    pad.scan_time_ms       = 6;
    pad.decay_len_ms2      = 150;
    pad.decay_grad_fact2   = 120;
    pad.decay_len_ms3      = 450;
    pad.decay_grad_fact3   = 30;
  case 'cy8'
    pad.scan_time_ms       = 6;
    pad.decay_len_ms1      = 10;
    pad.decay_grad_fact1   = 10;
    pad.decay_len_ms2      = 100;
    pad.decay_grad_fact2   = 200;
    pad.decay_len_ms3      = 450;
    pad.decay_grad_fact3   = 30;
end

% % TEST call reference mode for C++ implementation
% edrumulus(x);
% % TEST use 4 kHz sampling rate
% x = resample(x, 1, 2); Fs = Fs / 2;
% % TEST quantize to 12 bit resolution as available in ESP32 micro controller
% iNumBits = 10;%12; % reserve 2 bits for overload headroom -> 10 bits
% max_val  = max(abs(x));
% quant    = max_val / (2 ^ (iNumBits - 1) - 1);
% x        = round(x / quant);
% x        = x / max(abs(x)) * max_val;

processing(x * 25000, Fs); % scale to the ESP32 input range

end


function [x, x_filt, x_filt_delay] = filter_input_signal(x, Fs)
global pad;

% different band-pass filter designs for testing
%[b, a] = ellip(2, 2, 15, [40 400] / 4e3); % optimized for speed, i.e., quick rise time
%[b, a] = ellip(2, 0.9, 40, [40 400] / 4e3); % optimized for detecting peaks in noise
[b, a] = butter(2, [40 400] / 4e3); % seems to be a good trade-off

% TEST adjust filtered signal amplification so that the noise floor matches the unfiltered signal (depends on hardware noise spectrum)
%f = 3; b = b * f; pad.threshold_db = pad.threshold_db + 20 * log10(f);

% TEST to export coefficients to edrumulus.m:
%format long g; fliplr(a(2:end))
%fliplr(b)

x_filt = filter(b, a, x(:, 1)) .^ 2;
%close all;freqz(b, a, 512, 8000);f(3)
%subplot(2,1,1), plot(20 * log10(abs([x(:, 1) y]))); axis([-1809.80310, 142862.72867, -130.11254, 96.47492]);

% estimate the filter delay which is used to define the scan time for the
% unfiltered signal
[~, x_filt_delay] = max(impz(b, a));
x_filt_delay      = x_filt_delay;

end


function [all_peaks, all_first_peaks, all_peaks_filt, scan_region, mask_region, pre_scan_region, decay_all, decay_est_rng, x_filt_decay] = ...
           calc_peak_detection(x, x_filt, x_filt_delay, Fs)
global pad;

scan_region     = nan(size(x_filt));
mask_region     = nan(size(x_filt));
pre_scan_region = nan(size(x_filt));

first_peak_diff_thresh = 10 ^ (pad.first_peak_diff_thresh_db / 10); % difference between peaks to find first peak
mask_time              = round(pad.mask_time_ms * 1e-3 * Fs); % mask time (e.g. 10 ms)
scan_time              = round(pad.scan_time_ms * 1e-3 * Fs); % scan time from above threshold
pre_scan_time          = round(pad.pre_scan_time_ms * 1e-3 * Fs); % scan time before above threshold for detecting first peak
total_scan_time        = scan_time + pre_scan_time; % includes pre-scan time

% the following settings are trigger pad-specific
decay_len1      = round(pad.decay_len_ms1 * 1e-3 * Fs); % decay time (e.g. 250 ms)
decay_grad1     = pad.decay_grad_fact1 / Fs;            % decay gradient factor
decay_len2      = round(pad.decay_len_ms2 * 1e-3 * Fs);
decay_grad2     = pad.decay_grad_fact2 / Fs;
decay_len3      = round(pad.decay_len_ms3 * 1e-3 * Fs);
decay_grad3     = pad.decay_grad_fact3 / Fs;
decay_est_delay = round(pad.decay_est_delay_ms * 1e-3 * Fs);
decay_est_len   = round(pad.decay_est_len_ms * 1e-3 * Fs);
decay_est_fact  = 10 ^ (pad.decay_est_fact_db / 10);
decay_fact      = 10 ^ (pad.decay_fact_db / 10);

decay_curve1 = 10 .^ (-(0:decay_len1) / 10 * decay_grad1);
decay_curve2 = 10 .^ (-(0:decay_len2) / 10 * decay_grad2);
decay_curve3 = 10 .^ (-(0:decay_len3 - 1) / 10 * decay_grad3);
decay_curve  = [decay_curve1(1:end - 1), decay_curve1(end) * decay_curve2(1:end - 1), decay_curve1(end) * decay_curve2(end) * decay_curve3];
decay_len    = decay_len1 + decay_len2 + decay_len3;

last_peak_idx      = pre_scan_time;
all_peaks          = [];
all_first_peaks    = [];
all_peaks_filt     = [];
no_more_peak       = false;
x_sq               = x .^ 2;
x_filt_decay       = x_filt;
all_scan_peaks_idx = [];                % only for debugging
decay_all          = nan(size(x_filt)); % only for debugging
decay_est_rng      = nan(size(x_filt)); % only for debugging

while ~no_more_peak

  % find values above threshold, masking regions which are already done
  above_thresh       = (x_filt_decay > 10 ^ (pad.threshold_db / 10)) & [zeros(last_peak_idx, 1); ones(length(x_filt_decay) - last_peak_idx, 1)];
  above_thresh_start = find(diff(above_thresh) > 0) + 1;

  % exit condition
  if isempty(above_thresh_start) || (above_thresh_start(1) + decay_est_delay + decay_est_len >= length(x))
    no_more_peak = true;
    continue;
  end

  org_above_thresh_start = above_thresh_start(1); % store original unmodifed value
  above_thresh_start     = org_above_thresh_start - x_filt_delay; % consider filter delay

  % It has shown that using the filtered signal for velocity
  % estimation, the detected velocity drops significantly if a mesh pad is hit
  % close to the edge. This is because the main lope is much smaller at the edge
  % and therefore the collected energy is much smaller. To solve this issue, we
  % have to use the unfiltered signal.
  %
  % Caused by the impulse response of the band-pass filter, the peaks usually
  % look like three peaks: two smaller peaks left/right and a large in the
  % middle. For fast rolls only the middle peak might be over the threshold.
  % Therefore we introduce a pre-scan region to make sure we can detect the
  % correct firt peak (which is important for the positional sensing).

  % climb to the maximum of the first peak (using the unfiltered signal)
  first_peak_idx = above_thresh_start - pre_scan_time;
  max_idx        = find(x_sq(1 + first_peak_idx:end) - x_sq(first_peak_idx:end - 1) < 0);

  if ~isempty(max_idx)
    first_peak_idx = first_peak_idx + max_idx(1) - 1;
  end

  % find all peaks after the initial peak (using the unfiltered signal)
  peak_idx_after_initial = find((x_sq(2 + first_peak_idx:end) < x_sq(1 + first_peak_idx:end - 1)) & ...
    (x_sq(1 + first_peak_idx:end - 1) >= x_sq(first_peak_idx:end - 2)));

  scan_peaks_idx     = first_peak_idx + peak_idx_after_initial(peak_idx_after_initial <= total_scan_time);
  all_scan_peaks_idx = [all_scan_peaks_idx; scan_peaks_idx]; % only for debugging

  % if a peak in the scan time is much higher than the initial peak, use that one
  for i = 1:length(scan_peaks_idx)

    if x_sq(first_peak_idx) * first_peak_diff_thresh < x_sq(scan_peaks_idx(i))
      first_peak_idx = scan_peaks_idx(i);
    end

  end

  all_first_peaks = [all_first_peaks; first_peak_idx];

  % search in a pre-defined scan time for the highest peak in unfiltered signal
  scan_indexes = above_thresh_start + (0:scan_time - 1);
  [~, max_idx] = max(x_sq(scan_indexes));
  peak_idx     = above_thresh_start + max_idx - 1;

  % search from above threshold to corrected scan+mask time for highest peak in
  % filtered signal, needed for decay power estimation
  scan_indexes_filt = org_above_thresh_start:org_above_thresh_start + scan_time + mask_time;
  [~, max_idx]      = max(x_filt(scan_indexes_filt));
  peak_idx_filt     = org_above_thresh_start + max_idx - 1;

  % estimate current decay power
  decay_scaling = decay_fact * x_filt(peak_idx_filt);

  % average power measured right after the two main peaks (it showed for high
  % level hits close to the pad center the decay has much lower power right
  % after the main peaks) in a predefined time intervall, but never use a higher
  % decay factor than derived from the main peak (in case a second hit is right
  % behind our main peaks to avoid very high decay curve placement)
  decay_power_win = above_thresh_start + decay_est_delay + (0:decay_est_len - 1);
  decay_power     = mean(x_filt(decay_power_win));
  decay_scaling   = min(decay_scaling, decay_est_fact * decay_power);

  % store the new detected peaks
  all_peaks      = [all_peaks; peak_idx];
  all_peaks_filt = [all_peaks_filt; peak_idx_filt];
  last_peak_idx  = org_above_thresh_start + scan_time + mask_time;

  % exponential decay assumption
  decay           = decay_scaling * decay_curve;
  decay_x         = org_above_thresh_start + scan_time + mask_time + (0:decay_len - 1);
  valid_decay_idx = decay_x <= length(x_filt_decay);
  decay           = decay(valid_decay_idx);
  decay_x         = decay_x(valid_decay_idx);

  % subtract decay (with clipping at zero)
  x_filt_new                 = x_filt(decay_x) - decay.';
  x_filt_new(x_filt_new < 0) = 0;

  % update filtered signal
  x_filt_decay(decay_x) = x_filt_new;

  % debugging outputs
  scan_region(scan_indexes)                                                    = x_sq(first_peak_idx); % mark scan time region
  pre_scan_region(above_thresh_start - pre_scan_time + (0:pre_scan_time - 1))  = x_sq(first_peak_idx); % mark pre-scan time region
  mask_region(last_peak_idx - 1 + (-mask_time - x_filt_delay + 1:0))           = x_sq(first_peak_idx); % mark mask region
  decay_est_rng(decay_power_win)                                               = decay_power;          % mark decay power estimation region
  decay_all(decay_x)                                                           = decay;                % store decay curve
  decay_all(above_thresh_start + (0:scan_time + mask_time + x_filt_delay - 1)) = nan;                  % remove previous decay curve during observation region

end

end


function pos_sense_metric = calc_pos_sense_metric(x, Fs, all_first_peaks)
global pad;

% low pass filter of the input signal
% moving average cut off frequency approximation according to:
% https://dsp.stackexchange.com/questions/9966/what-is-the-cut-off-frequency-of-a-moving-average-filter
lp_cutoff_norm = pad.pos_low_pass_cutoff / Fs;
lp_filt_len    = round(sqrt(0.196202 + lp_cutoff_norm ^ 2) / lp_cutoff_norm);
if mod(lp_filt_len, 2) == 0
  lp_filt_len = lp_filt_len + 1; % make sure we have an odd length
endif
lp_half_len = (lp_filt_len - 1) / 2;
lp_filt_b   = [0.5:0.5 / lp_half_len:(1 - 0.5 / lp_half_len) 1 (1 - 0.5 / lp_half_len):-0.5 / lp_half_len:0.5] / lp_filt_len;
%lp_filt_b = ones(lp_filt_len, 1) / lp_filt_len; % TEST
x_low = filter(lp_filt_b, 1, x) .^ 2; % moving average

%disp(['low-pass filter delay: ' num2str(lp_filt_len / 2 / 8) ' ms, ' num2str(lp_filt_len) ' samples']);

x_sq            = x .^ 2;
num_peaks       = length(all_first_peaks);
peak_energy     = zeros(num_peaks, 1);
peak_energy_low = zeros(num_peaks, 1);
all_peaks_low   = zeros(num_peaks, 1);

win_len     = lp_filt_len;
win_low_all = nan(length(x_low), 1);

for i = 1:num_peaks

  % find first peak of low-pass filtered signal searching for the maximum in the
  % range of the low-pass moving average window length

% TODO find the correct offset (0 seems to work good)
win_offset           = 0;%-(win_len - 1) / 2;

  win_idx              = (all_first_peaks(i):all_first_peaks(i) + win_len - 1) + win_offset;
  win_idx              = win_idx((win_idx <= length(x_sq)) & (win_idx > 0));
  [~, x_low_max_index] = max(x_low(win_idx));
  all_peaks_low(i)     = all_first_peaks(i) + win_offset + x_low_max_index - 1;
  win_low_all(win_idx) = x_low(win_idx); % only for debugging

  % use the original signal as the reference power for the positional
  % sensing metric where the first peak position is used
  peak_energy(i)     = x_sq(all_first_peaks(i));
  peak_energy_low(i) = x_low(all_peaks_low(i));

end

if pad.pos_invert
  % add offset to get to similar range as non-inverted metric
  pos_sense_metric = 10 * log10(peak_energy_low) - 10 * log10(peak_energy) + 40;
else
  pos_sense_metric = 10 * log10(peak_energy) - 10 * log10(peak_energy_low);
end

%figure; plot(10 * log10([x_sq(1:length(x_low)), x_low])); hold on;
%        plot(10 * log10(win_low_all), 'g.', 'MarkerSize', 10);
%        plot(all_first_peaks, 10 * log10(x_sq(all_first_peaks)), 'k*');
%        plot(all_peaks_low, 10 * log10(x_low(all_peaks_low)), 'g*');

end


function [is_rim_shot, rim_metric_db] = detect_rim_shot(x, all_first_peaks, Fs)

is_rim_shot          = false(size(all_first_peaks));
rim_metric_db        = nan(size(all_first_peaks));
rim_shot_window_len  = round(3.5e-3 * Fs); % scan time (e.g. 6 ms)
rim_shot_treshold_dB = -5; % dB
rim_max_pow_index    = zeros(size(all_first_peaks));
rim_win_region       = nan(size(x));

if size(x, 2) > 1

  x_rim = x(:, 2);

  for i = 1:length(all_first_peaks)

    win_idx                     = (all_first_peaks(i):all_first_peaks(i) + rim_shot_window_len - 1);
    win_idx                     = win_idx((win_idx <= length(x_rim)) & (win_idx > 0));
    [rim_max_pow(i), max_index] = max(x_rim(win_idx) .^ 2);
    x_max_pow(i)                = x(all_first_peaks(i), 1) .^ 2;
    rim_max_pow_index(i)        = win_idx(1) + max_index - 1; % only for debugging
    rim_win_region(win_idx)     = rim_max_pow(i);             % only for debugging

  end

  rim_metric_db = 10 * log10(rim_max_pow ./ x_max_pow);
  is_rim_shot   = rim_metric_db > rim_shot_treshold_dB;

%figure; plot(10 * log10([x(:, 1) .^ 2, x_rim .^ 2, rim_win_region])); hold on; grid on;
%        plot(all_first_peaks, 10 * log10(x(all_first_peaks, 1) .^ 2), 'y*');
%        plot(rim_max_pow_index, 10 * log10(x_rim(rim_max_pow_index) .^ 2), 'b*');
%        plot(all_first_peaks, rim_metric_db, '*-');
%        plot(all_first_peaks(is_rim_shot), rim_metric_db(is_rim_shot), '*');
%        plot(all_first_peaks(~is_rim_shot), rim_metric_db(~is_rim_shot), '*');

end

end


function processing(x, Fs)
global pad;

% calculate peak detection and positional sensing
[x, x_filt, x_filt_delay] = filter_input_signal(x, Fs);
[all_peaks, all_first_peaks, all_peaks_filt, scan_region, mask_region, pre_scan_region, decay_all, decay_est_rng, x_filt_decay] = ...
  calc_peak_detection(x(:, 1), x_filt, x_filt_delay, Fs);
[is_rim_shot, rim_metric_db] = detect_rim_shot(x, all_first_peaks, Fs);
pos_sense_metric             = calc_pos_sense_metric(x(:, 1), Fs, all_first_peaks);

% plot results
figure
plot(10 * log10([mask_region, scan_region, pre_scan_region, decay_est_rng]), 'LineWidth', 20);
grid on; hold on; set(gca, 'ColorOrderIndex', 1); % reset color order so that x trace is blue and so on
plot(10 * log10([x(:, 1) .^ 2, x_filt, decay_all, x_filt_decay]));
plot(all_first_peaks, 10 * log10(x(all_first_peaks, 1) .^ 2), 'b*');
plot(all_peaks, 10 * log10(x(all_peaks, 1) .^ 2), 'g*');
plot(all_peaks_filt, 10 * log10(x_filt(all_peaks_filt)), 'y*');
plot(all_first_peaks, pos_sense_metric + 40, 'k*');
plot(all_first_peaks, rim_metric_db + 70, '*-');
plot(all_first_peaks(is_rim_shot), rim_metric_db(is_rim_shot) + 70, '*');
plot(all_first_peaks(~is_rim_shot), rim_metric_db(~is_rim_shot) + 70, '*');
plot([1, length(x_filt)], [pad.threshold_db, pad.threshold_db], '--');
title('Green marker: level; Black marker: position; Blue marker: first peak'); xlabel('samples'); ylabel('dB');
ylim([-10, 90]);

end


