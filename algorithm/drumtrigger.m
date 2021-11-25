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
%x = audioread("signals/pd120_single_hits.wav");
%x = audioread("signals/pd120_roll.wav");%x = x(292410:294749, :);%x = x(311500:317600, :);
%x = audioread("signals/pd120_middle_velocity.wav");
%x = audioread("signals/pd120_hot_spot.wav");
%x = audioread("signals/pd120_rimshot.wav");%x = x(168000:171000, :);%x = x(1:34000, :);%x = x(1:100000, :);
%x = audioread("signals/pd120_rimshot_hardsoft.wav");
%x=audioread("signals/pd120_middle_velocity.wav");x=[x;audioread("signals/pd120_pos_sense2.wav")];x=[x;audioread("signals/pd120_hot_spot.wav")];
x = audioread("signals/pd80r.wav");padtype = 'pd80r';x = x(1:265000, :);%x = x(52000:60000, :);
%x = audioread("signals/pd6.wav");
%x = audioread("signals/pd8.wav");padtype = 'pd8';%x = x(1:300000, :);%x = x(420000:470000, :);%x = x(1:100000, :);
%x = audioread("signals/pd8_rimshot.wav");padtype = 'pd8';
%x = audioread("signals/cy6.wav");padtype = 'cy6';x = x(480000:590000, :);%x = x(250000:450000, :);%x = x(1:150000, :);
%x = audioread("signals/cy8.wav");padtype = 'cy8';%x = x(1:200000, :);
%x = audioread("signals/kd8.wav");
%x = audioread("signals/kd7.wav");padtype = 'kd7';%x = x(1:170000, :);
%x = audioread("signals/tp80.wav");padtype = 'tp80';
%x = audioread("signals/vh12.wav");padtype = 'vh12';%x = x(900000:end, :);%x = x(376000:420000, :);%x = x(1:140000, :);
%org = audioread("signals/snare.wav"); x = resample(org(:, 1), 1, 6); % PD-120
%org = audioread("signals/snare.wav"); x = org(:, 1); Fs = 48e3; % PD-120



% pad PRESET settings first, then overwrite these with pad specific properties
pad.threshold_db          = 17;%6;%
pad.mask_time_ms          = 6;
pad.energy_win_len_ms     = 2;
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
pad.pos_energy_win_len_ms = 2;
pad.pos_iir_alpha         = 200;
pad.pos_invert            = false;

switch padtype
  case 'pd120'
    % note: the PRESET settings are from the PD120 pad
  case 'pd80r'
    pad.scan_time_ms          = 3;
    pad.main_peak_dist_ms     = 2.4;
    pad.decay_len_ms2         = 75;
    pad.decay_grad_fact2      = 300;
    pad.decay_len_ms3         = 300;
    pad.decay_grad_fact3      = 100;
    pad.pos_energy_win_len_ms = 0.5;

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


function [x, x_filt] = filter_input_signal(x, Fs)
global pad;

% different band-pass filter designs for testing
%[b, a] = ellip(2, 2, 15, [40 400] / 4e3); % optimized for speed, i.e., quick rise time
%[b, a] = ellip(2, 0.9, 40, [40 400] / 4e3); % optimized for detecting peaks in noise
[b, a] = butter(2, [40 400] / 4e3); % seems to be a good trade-off

x_filt = abs(filter(b, a, x(:, 1))) .^ 2;
%close all;freqz(b, a, 512, 8000);f(3)
%subplot(2,1,1), plot(20 * log10(abs([x(:, 1) y]))); axis([-1809.80310, 142862.72867, -130.11254, 96.47492]);

% estimate the filter delay which is used to define the scan time for the
% unfiltered signal
[~, x_filt_delay] = max(impz(b, a));
x_filt_delay      = x_filt_delay;

% TODO Latency optimization: To have a simple implementation, we delay the input
%      signal by the latency of the band-pass filter so that the peaks are
%      matched (which is important for, e.g., the threshold check for the
%      filtered signal which is applied to the unfiltered signal as well.
x = circshift(x, x_filt_delay); % note that we delay both channels of x

end


function [all_peaks, all_first_peaks, scan_region] = calc_peak_detection(x, x_filt, Fs)
global pad;

scan_region = nan(size(x_filt));

energy_window_len      = round(pad.energy_win_len_ms * 1e-3 * Fs); % hit energy estimation time window length (e.g. 2 ms)
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

last_peak_idx      = 0;
all_peaks          = [];
all_first_peaks    = [];
i                  = 1;
no_more_peak       = false;
x_sq               = abs(x) .^ 2;
x_filt_decay       = x_filt;
all_scan_peaks_idx = [];                % only for debugging
decay_all          = nan(size(x_filt)); % only for debugging
decay_est_rng      = nan(size(x_filt)); % only for debugging

while ~no_more_peak

  % find values above threshold, masking regions which are already done
  above_thresh = (x_filt_decay > 10 ^ (pad.threshold_db / 10)) & [zeros(last_peak_idx, 1); ones(length(x_filt_decay) - last_peak_idx, 1)];
  peak_start   = find(diff(above_thresh) > 0);

  % exit condition
  if isempty(peak_start)
    no_more_peak = true;
    continue;
  end

  % It has shown that using the filtered signal for velocity
  % estimation, the detected velocity drops significantly if a mesh pad is hit
  % close to the edge. This is because the main lope is much smaller at the edge
  % and therefore the collected energy is much smaller. To solve this issue, we
  % have to use the unfiltered signal.

  % climb to the maximum of the first peak
  peak_idx = peak_start(1);
  max_idx  = find(x_sq(1 + peak_idx:end) - x_sq(peak_idx:end - 1) < 0);

  if ~isempty(max_idx)
    peak_idx = peak_idx + max_idx(1) - 1;
  end

  % find all peaks after the initial peak
  peak_idx_after_initial = find((x_sq(2 + peak_idx:end) < x_sq(1 + peak_idx:end - 1)) & ...
    (x_sq(1 + peak_idx:end - 1) > x_sq(peak_idx:end - 2)));

  scan_peaks_idx     = peak_idx + peak_idx_after_initial(peak_idx_after_initial <= scan_time);
  all_scan_peaks_idx = [all_scan_peaks_idx; scan_peaks_idx]; % only for debugging

  % if a peak in the scan time is much higher than the initial peak, use that one
  for i = 1:length(scan_peaks_idx)

    if x_sq(peak_idx) * first_peak_diff_thresh < x_sq(scan_peaks_idx(i))
      peak_idx = scan_peaks_idx(i);
    end

  end

  first_peak_idx  = peak_idx;
  all_first_peaks = [all_first_peaks; first_peak_idx];

  % search in a pre-defined scan time for the highest peak
  scan_indexes              = peak_idx:min(1 + peak_idx + scan_time - 1, length(x_sq));
  [~, max_idx]              = max(x_sq(scan_indexes));
  peak_idx                  = peak_idx + max_idx - 1;
  scan_region(scan_indexes) = x_sq(peak_idx); % mark scan time region

  % estimate current decay power

% TODO use the maximum of x_filt in scantime+masktime region instead
decay_factor = x_sq(peak_idx);

  if first_peak_idx + main_peak_dist + decay_est_delay2nd + decay_est_len - 1 <= length(x_filt)

    % average power measured right after the two main peaks (it showed for high level hits
    % close to the pad center the decay has much lower power right after the main peaks) in
    % a predefined time intervall, but never use a higher decay factor than derived from the
    % main peak (in case a second hit is right behind our main peaks to avoid very high
    % decay curve placement)
    decay_power  = mean(x_filt(first_peak_idx + main_peak_dist + decay_est_delay2nd + (0:decay_est_len - 1)));
    decay_factor = min(decay_factor, decay_est_fact * decay_power);

    decay_est_rng(first_peak_idx + main_peak_dist + decay_est_delay2nd + (0:decay_est_len - 1)) = decay_power; % only for debugging

  end

  % store the new detected peak
  all_peaks     = [all_peaks; peak_idx];
  last_peak_idx = min(first_peak_idx + mask_time, length(x_filt));

  % exponential decay assumption
  decay           = decay_factor * decay_curve;
  decay_x         = first_peak_idx + (0:decay_len - 1) + 2; % NOTE "+ 2" delay needed for sample-wise processing
  valid_decay_idx = decay_x <= length(x_filt_decay);
  decay           = decay(valid_decay_idx);
  decay_x         = decay_x(valid_decay_idx);

  % subtract decay (with clipping at zero)
  x_filt_new                 = x_filt_decay(decay_x) - decay.';
  x_filt_new(x_filt_new < 0) = 0;

  % update filtered signal
  x_filt_decay(decay_x) = x_filt_new;
  i                     = i + 1;

  decay_all(decay_x) = decay; % only for debugging

end

figure; plot(10 * log10([x_sq, x_filt, x_filt_decay, decay_all, decay_est_rng])); hold on;
plot(all_scan_peaks_idx, 10 * log10(x_sq(all_scan_peaks_idx)), '.');
plot(all_peaks, 10 * log10(x_sq(all_peaks)), 'k*');
plot(all_first_peaks, 10 * log10(x_sq(all_first_peaks)), 'y*');

end


function pos_sense_metric = calc_pos_sense_metric(x, x_filt, Fs, all_peaks)
global pad;

pos_energy_window_len = round(pad.pos_energy_win_len_ms * 1e-3 * Fs); % positional sensing energy estimation time window length (e.g. 2 ms)

% low pass filter of the Hilbert signal
% lp_ir_len = 80; % low-pass filter length
% lp_cutoff = 0.02; % normalized cut-off of low-pass filter
% a         = fir1(lp_ir_len, lp_cutoff);
% hil_low   = filter(a, 1, x);
% hil_low   = hil_low(lp_ir_len / 2:end);
% use a simple one-pole IIR filter for less CPU processing and shorter delay
alpha   = pad.pos_iir_alpha / Fs;
hil_low = filter(alpha, [1, alpha - 1], x);

% figure; plot(20 * log10(abs([x(1:length(hil_low)), hil_low]))); hold on;

peak_energy     = [];
peak_energy_low = [];
win_idx_all     = []; % only for debugging

for i = 1:length(all_peaks)

  % The peak detection was performed on the moving averaged filtered signal but
  % for positional sensing we need to use the original Hilbert transformed signal
  % since we have to calculate a separate low-pass filter. Since the detected
  % peak position in the moving averaged filtered signal might be in an attenuated
  % region of the original Hilbert transformed signal, we average the powers of
  % the filtered and un-filtered signals around the detected peak position.
  win_idx            = (all_peaks(i):all_peaks(i) + pos_energy_window_len - 1) - pos_energy_window_len / 2;
  win_idx            = win_idx((win_idx <= length(hil_low)) & (win_idx > 0));
  peak_energy(i)     = sum(abs(x(win_idx)) .^ 2);
  peak_energy_low(i) = sum(abs(hil_low(win_idx)) .^ 2);

  win_idx_all = [win_idx_all; win_idx]; % only for debugging

end

if pad.pos_invert
  % add offset to get to similar range as non-inverted metric
  pos_sense_metric = 10 * log10(peak_energy_low) - 10 * log10(peak_energy) + 40;
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
%plot(10 * log10([abs(x(x_peaks)) .^ 2, abs(hil_low(x_peaks)) .^ 2])); grid on; hold on;
%%plot(21:80:length(all_peaks) * 80, 20 * log10(abs(x(all_peaks))), 'y*');

%figure; plot(20 * log10(abs([x, hil_low, x_filt]))); hold on;
%plot(win_idx_all', 20 * log10(abs(x(win_idx_all'))), 'k.-');
%grid on; axis([2978.533   3072.863    -20.398    131.296]);
%
%figure; plot(20 * log10(abs([x, hil_low, x_filt]))); hold on;
%plot(win_idx_all', 20 * log10(abs(x(win_idx_all'))), 'k.-');
%grid on; axis([2.3806e+04   2.3900e+04  -1.4506e+01   1.1036e+02]);

%figure; plot(20 * log10(abs(x))); hold on;
%for i = 1:size(win_idx_all, 1)
%  plot(win_idx_all(i, :), 20 * log10(abs(x(win_idx_all(i, :)))), 'k.-');
%  plot(win_idx_all(i, :), 20 * log10(abs(hil_low(win_idx_all(i, :)))), 'b.-');
%  plot(all_peaks, 10 * log10(peak_energy), 'k');
%  plot(all_peaks, 10 * log10(peak_energy_low), 'b');
%end

end


function is_rim_shot = detect_rim_shot(x, all_peaks_x, Fs)

is_rim_shot          = false(size(all_peaks_x));
rim_shot_window_len  = round(5e-3 * Fs); % scan time (e.g. 6 ms)
rim_shot_treshold_dB = 2.3; % dB

if size(x, 2) > 1

  % one pole IIR high pass filter
  [b, a]     = butter(1, 0.02, 'high');
  rim_x_high = filter(b, a, x(:, 2));

  for i = 1:length(all_peaks_x)

    win_idx           = (all_peaks_x(i):all_peaks_x(i) + rim_shot_window_len - 1) - rim_shot_window_len / 2;
    win_idx           = win_idx((win_idx <= length(rim_x_high)) & (win_idx > 0));
    rim_max_pow(i)    = max(abs(rim_x_high(win_idx)) .^ 2);
    x_filt_max_pow(i) = abs(x(all_peaks_x(i), 1)) .^ 2;

  end

  rim_metric_db = 10 * log10(rim_max_pow ./ x_filt_max_pow);
  is_rim_shot   = rim_metric_db > rim_shot_treshold_dB;

%figure;
%plot(10 * log10(abs([abs(x(:, 1)) .^ 2, rim_x_high]))); hold on; grid on;
%plot(all_peaks_x, 20 * log10(abs(x(all_peaks_x, 1))), 'y*');
%plot(all_peaks_x, rim_metric_db, '*-');
%plot(all_peaks_x(is_rim_shot), rim_metric_db(is_rim_shot), '*');
%plot(all_peaks_x(~is_rim_shot), rim_metric_db(~is_rim_shot), '*');

end

end


function processing(x, Fs)

% calculate peak detection and positional sensing
[x, x_filt]                               = filter_input_signal(x, Fs);
[all_peaks, all_first_peaks, scan_region] = calc_peak_detection(x(:, 1), x_filt, Fs);
is_rim_shot                               = detect_rim_shot(x, all_peaks, Fs);
pos_sense_metric                          = calc_pos_sense_metric(x(:, 1), x_filt, Fs, all_first_peaks);


% plot results
figure
plot(10 * log10([abs(x(:, 1)) .^ 2, x_filt, scan_region])); grid on; hold on;
plot(all_first_peaks, 10 * log10(x_filt(all_first_peaks)), 'y*');
plot(all_peaks, 10 * log10(x_filt(all_peaks)), 'r*');
plot(all_peaks, 10 * log10(abs(x(all_peaks, 1)) .^ 2), 'g*');
plot(all_first_peaks, pos_sense_metric + 40, 'k*');
title('Green marker: level; Black marker: position');
xlabel('samples'); ylabel('dB');
ylim([-10, 90]);

% TEST
% velocity/positional sensing mapping and play MIDI notes
velocity            = (10 * log10(x_filt(all_peaks)) / 39) * 127 - 73;
velocity_clipped    = max(1, min(127, velocity));
pos_sensing         = (pos_sense_metric / 4) * 127 - 510;
pos_sensing_clipped = max(1, min(127, pos_sensing));
% figure; subplot(2, 1, 1), plot(velocity); title('velocity'); subplot(2, 1, 2), plot(pos_sensing); title('pos');

end


