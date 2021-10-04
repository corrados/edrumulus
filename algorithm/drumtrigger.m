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
pad.threshold_db          = 28;
pad.mask_time_ms          = 6;
pad.energy_win_len_ms     = 0.5;
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
pad.pos_energy_win_len_ms = 2;%0.5;%2;%0.5;%2;
pad.pos_iir_alpha         = 200;
pad.pos_invert            = false;

switch padtype
  case 'pd120'
    % note: the PRESET settings are from the PD120 pad
    pad.decay_len_ms1         = 10;
    pad.decay_grad_fact1      = 30;
    pad.decay_len_ms2         = 250;
    pad.decay_grad_fact2      = 220;
    pad.decay_len_ms3         = 0; % not used
  case 'pd80r'
    pad.scan_time_ms          = 3;
    pad.main_peak_dist_ms     = 2.4;
    pad.decay_len_ms1         = 10;
    pad.decay_grad_fact1      = 30;
    pad.decay_len_ms2         = 75;
    pad.decay_grad_fact2      = 300;
    pad.decay_len_ms3         = 300;
    pad.decay_grad_fact3      = 100;
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


function hil = myhilbert(x)

a   = fir1(6, 0.4);
a   = a .* exp(1j * 2 * pi * (0:length(a) - 1) * 0.3) * length(a);
hil = filter(a, 1, x);

% figure; freqz(a);
% figure;
% subplot(2, 1, 1), pwelch(x,[],[],[],[],'twosided','db');
% subplot(2, 1, 2), pwelch(hil,[],[],[],[],'twosided','db');

% TEST use built-in hilbert filter instead of my own implementation for reference
% hil = hilbert(x);

end


function [hil, hil_filt] = filter_input_signal(x, Fs)
global pad;

energy_window_len = round(pad.energy_win_len_ms * 1e-3 * Fs); % hit energy estimation time window length (e.g. 2 ms)

% Hilbert filter
hil = myhilbert(x);

% moving average filter
hil_filt = abs(filter(ones(energy_window_len, 1) / energy_window_len, 1, abs(hil) .^ 2)); % moving average

end


function [all_peaks, all_first_peaks, scan_region] = calc_peak_detection(hil_filt, Fs)
global pad;

scan_region = nan(size(hil_filt));

first_peak_diff_thresh = 10 ^ (20 / 10); % 20 dB difference allowed
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
i               = 1;
no_more_peak    = false;
hil_filt_decay  = hil_filt;
decay_all       = nan(size(hil_filt)); % only for debugging
decay_est_rng   = nan(size(hil_filt)); % only for debugging

while ~no_more_peak

  % find values above threshold, masking regions which are already done
  above_thresh = (hil_filt_decay > 10 ^ (pad.threshold_db / 10)) & [zeros(last_peak_idx, 1); ones(length(hil_filt_decay) - last_peak_idx, 1)];
  peak_start   = find(diff(above_thresh) > 0);

  % exit condition
  if isempty(peak_start)
    no_more_peak = true;
    continue;
  end

  % climb to the maximum of the first peak
  peak_idx = peak_start(1);
  max_idx  = find(hil_filt(1 + peak_idx:end) - hil_filt(peak_idx:end - 1) < 0);

  if ~isempty(max_idx)
    peak_idx = peak_idx + max_idx(1) - 1;
  end

  % find all peaks after the initial peak
  peak_idx_after_initial = find((hil_filt(2 + peak_idx:end) < hil_filt(1 + peak_idx:end - 1)) & ...
    (hil_filt(1 + peak_idx:end - 1) > hil_filt(peak_idx:end - 2)));

  scan_peaks_idx = peak_idx + peak_idx_after_initial(peak_idx_after_initial <= scan_time);

  % if a peak in the scan time is much higher than the initial peak, use that one
  much_higher_peaks = find(hil_filt(peak_idx) * first_peak_diff_thresh < hil_filt(scan_peaks_idx));

  if ~isempty(much_higher_peaks)
    peak_idx = scan_peaks_idx(much_higher_peaks(1));
  end

  all_first_peaks = [all_first_peaks; peak_idx];

  % search in a pre-defined scan time for the highest peak
  scan_indexes              = peak_idx:min(1 + peak_idx + scan_time - 1, length(hil_filt));
  [~, max_idx]              = max(hil_filt(scan_indexes));
  peak_idx                  = peak_idx + max_idx - 1;
  scan_region(scan_indexes) = hil_filt(peak_idx); % mark scan time region

  % calculate power left/right of detected peak for second main peak position detection
  first_peak_idx = peak_idx; % initialization

  if peak_idx - main_peak_dist < 1
    all_sec_peaks = [all_sec_peaks; 1];
  elseif peak_idx + main_peak_dist > length(hil_filt)
    all_sec_peaks = [all_sec_peaks; length(hil_filt)];
  else

    power_hypo_left  = hil_filt(peak_idx - main_peak_dist);
    power_hypo_right = hil_filt(peak_idx + main_peak_dist);

    if power_hypo_left > power_hypo_right

      all_sec_peaks  = [all_sec_peaks; peak_idx - main_peak_dist];
      first_peak_idx = peak_idx - main_peak_dist;

    else
      all_sec_peaks = [all_sec_peaks; peak_idx + main_peak_dist];
    end

  end

  % estimate current decay power
  decay_factor = hil_filt(peak_idx);

  if first_peak_idx + main_peak_dist + decay_est_delay2nd + decay_est_len - 1 <= length(hil_filt)

    % average power measured right after the two main peaks (it showed for high level hits
    % close to the pad center the decay has much lower power right after the main peaks) in
    % a predefined time intervall, but never use a higher decay factor than derived from the
    % main peak (in case a second hit is right behind our main peaks to avoid very high
    % decay curve placement)
    decay_power  = mean(hil_filt(first_peak_idx + main_peak_dist + decay_est_delay2nd + (0:decay_est_len - 1)));
    decay_factor = min(decay_factor, decay_est_fact * decay_power);

    decay_est_rng(first_peak_idx + main_peak_dist + decay_est_delay2nd + (0:decay_est_len - 1)) = decay_power; % only for debugging

  end

  % store the new detected peak
  all_peaks     = [all_peaks; peak_idx];
  last_peak_idx = min(peak_idx + mask_time, length(hil_filt));

  % exponential decay assumption
  decay           = decay_factor * decay_curve;
  decay_x         = first_peak_idx + (0:decay_len - 1) + 2; % NOTE "+ 2" delay needed for sample-wise processing
  valid_decay_idx = decay_x <= length(hil_filt_decay);
  decay           = decay(valid_decay_idx);
  decay_x         = decay_x(valid_decay_idx);

  % subtract decay (with clipping at zero)
  hil_filt_new                   = hil_filt_decay(decay_x) - decay.';
  hil_filt_new(hil_filt_new < 0) = 0;

  % update filtered signal
  hil_filt_decay(decay_x) = hil_filt_new;
  i                       = i + 1;

  decay_all(decay_x) = decay; % only for debugging

end

figure; plot(10 * log10([hil_filt, hil_filt_decay, decay_all, decay_est_rng])); hold on;
plot(all_peaks, 10 * log10(hil_filt(all_peaks)), 'k*');
plot(all_sec_peaks, 10 * log10(hil_filt(all_sec_peaks)), 'y*');

% TODO What is this zoom area for?
%axis([2.835616531556589e+05   2.856098468655325e+05  -1.994749771562022e+01   4.962270061651073e+01]);

% incorrect triggering: low level hit prior to high level hit
%axis([2.924131435344061e+05   2.947458419243922e+05  -1.015529367435146e+00   5.738834385758622e+01]);
%axis([2.421239842841798e+05   2.444566826741658e+05   5.379662774744027e+00   6.378353599976542e+01]);

% difficult region with low level hits (at the edge?)
%axis([3.116618850384215e+05   3.139945834284076e+05  -8.199169855910323e+00   5.020470336911105e+01]);

% second hit has almost only one peak, not two as usual
%axis([3.205408049935086e+05   3.228735033834946e+05   1.221288177597649e+01   7.061675500099790e+01]);

% three missed hits at low level hits in the middle position
%axis([3.325856532044706e+05   3.349183515944567e+05   1.177485491692314e+01   7.017872814194450e+01]);
%axis([3.351685187444009e+05   3.375012171343870e+05   8.358245416306897e+00   6.676211864132827e+01]);
%axis([3.411346824123786e+05   3.434673808023647e+05   1.011035285252035e+01   6.851422607754172e+01]);

% difficult to identify hits by eye (low level hits at the edge)
%axis([ 2.834549475082915e+05   2.857876458982775e+05  -6.009035560643461e+00   5.239483766437795e+01]);
%axis([2.510387063358597e+05   2.533714047258457e+05  -1.628766970109801e+00   5.677510625491159e+01]);
%axis([2.492000129465431e+05   2.515327113365291e+05  -5.220587214347390e+00   5.318328601067400e+01]);
%axis([2.459957253014613e+05   2.483284236914473e+05  -3.818901265376631e+00   5.458497195964476e+01]);
%axis([2.217960652971447e+05   2.241287636871307e+05  -3.118058290891270e+00   5.528581493413012e+01]);
%axis([2.195405332117799e+05   2.218732316017659e+05  -1.278345482867138e+00   5.712552774215425e+01]);
%axis([2.053756894239842e+05   2.077083878139702e+05   2.985512097249909e-01   5.870242443474638e+01]);
%axis([1.634437624356316e+05   1.657764608256176e+05  -1.803977713731133e+00   5.659989551129024e+01]);

% are these real hits?
%axis([1.014064009176438e+05   1.037390993076298e+05  -5.045376470726055e+00   5.335849675429534e+01]);


end


function pos_sense_metric = calc_pos_sense_metric(hil, hil_filt, Fs, all_peaks)
global pad;

pos_energy_window_len = round(pad.pos_energy_win_len_ms * 1e-3 * Fs); % positional sensing energy estimation time window length (e.g. 2 ms)

% low pass filter of the Hilbert signal
% lp_ir_len = 80; % low-pass filter length
% lp_cutoff = 0.02; % normalized cut-off of low-pass filter
% a         = fir1(lp_ir_len, lp_cutoff);
% hil_low   = filter(a, 1, hil);
% hil_low   = hil_low(lp_ir_len / 2:end);
% use a simple one-pole IIR filter for less CPU processing and shorter delay
alpha   = pad.pos_iir_alpha / Fs;
hil_low = filter(alpha, [1, alpha - 1], hil);

% TEST
all_peaks = all_peaks + 6;
%all_peaks = all_peaks - 2;
%hil_low = circshift(hil_low, -3);%-7);

figure; plot(20 * log10(abs([hil(1:length(hil_low)), hil_low]))); hold on;
        plot(all_peaks, 20 * log10(hil(all_peaks)), 'k*');
        plot(all_peaks, 20 * log10(hil_low(all_peaks)), 'k*');

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
  peak_energy(i)     = sum(abs(hil(win_idx)) .^ 2);
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
%plot(10 * log10([abs(hil(x_peaks)) .^ 2, abs(hil_low(x_peaks)) .^ 2])); grid on; hold on;
%%plot(21:80:length(all_peaks) * 80, 20 * log10(abs(hil(all_peaks))), 'y*');


%figure; plot(20 * log10(abs([hil, hil_low, hil_filt]))); hold on;
%plot(win_idx_all', 20 * log10(abs(hil(win_idx_all'))), 'k.-');
%grid on; axis([2978.533   3072.863    -20.398    131.296]);
%
%figure; plot(20 * log10(abs([hil, hil_low, hil_filt]))); hold on;
%plot(win_idx_all', 20 * log10(abs(hil(win_idx_all'))), 'k.-');
%grid on; axis([2.3806e+04   2.3900e+04  -1.4506e+01   1.1036e+02]);

%figure; plot(20 * log10(abs(hil))); hold on;
%for i = 1:size(win_idx_all, 1)
%  plot(win_idx_all(i, :), 20 * log10(abs(hil(win_idx_all(i, :)))), 'k.-');
%  plot(win_idx_all(i, :), 20 * log10(abs(hil_low(win_idx_all(i, :)))), 'b.-');
%  plot(all_peaks, 10 * log10(peak_energy), 'k');
%  plot(all_peaks, 10 * log10(peak_energy_low), 'b');
%end

end


function is_rim_shot = detect_rim_shot(x, hil_filt, all_first_peaks, Fs)

is_rim_shot          = false(size(all_first_peaks));
rim_shot_window_len  = round(5e-3 * Fs); % scan time (e.g. 6 ms)
rim_shot_treshold_dB = 2.3; % dB

if size(x, 2) > 1

  % one pole IIR high pass filter
  [b, a]     = butter(1, 0.02, 'high');
  rim_x_high = filter(b, a, x(:, 2));

  for i = 1:length(all_first_peaks)

    win_idx             = (all_first_peaks(i):all_first_peaks(i) + rim_shot_window_len - 1) - rim_shot_window_len / 2;
    win_idx             = win_idx((win_idx <= length(rim_x_high)) & (win_idx > 0));
    rim_max_pow(i)      = max(abs(rim_x_high(win_idx)) .^ 2);
    hil_filt_max_pow(i) = hil_filt(all_first_peaks(i));

  end

  rim_metric_db = 10 * log10(rim_max_pow ./ hil_filt_max_pow);
  is_rim_shot   = rim_metric_db > rim_shot_treshold_dB;

%figure;
%plot(10 * log10(abs([hil_filt, rim_x_high]))); hold on; grid on;
%plot(all_first_peaks, rim_metric_db, '*-');
%plot(all_first_peaks(is_rim_shot), rim_metric_db(is_rim_shot), '*');
%plot(all_first_peaks(~is_rim_shot), rim_metric_db(~is_rim_shot), '*');

end

end


function processing(x, Fs)

% calculate peak detection and positional sensing
[hil, hil_filt]                           = filter_input_signal(x(:, 1), Fs);
[all_peaks, all_first_peaks, scan_region] = calc_peak_detection(hil_filt, Fs);
is_rim_shot                               = detect_rim_shot(x, hil_filt, all_first_peaks, Fs);
pos_sense_metric                          = calc_pos_sense_metric(hil, hil_filt, Fs, all_first_peaks);


% plot results
figure
plot(10 * log10([abs(x(:, 1)) .^ 2, hil_filt, scan_region])); grid on; hold on;
plot(all_first_peaks, 10 * log10(hil_filt(all_first_peaks)), 'y*');
plot(all_peaks, 10 * log10(hil_filt(all_peaks)), 'g*');
plot(all_first_peaks, pos_sense_metric + 40, 'k*');
title('Green marker: level; Black marker: position');
xlabel('samples'); ylabel('dB');
ylim([-10, 90]);


% TEST
% velocity/positional sensing mapping and play MIDI notes
velocity            = (10 * log10(hil_filt(all_peaks)) / 39) * 127 - 73;
velocity_clipped    = max(1, min(127, velocity));
pos_sensing         = (pos_sense_metric / 4) * 127 - 510;
pos_sensing_clipped = max(1, min(127, pos_sensing));
% figure; subplot(2, 1, 1), plot(velocity); title('velocity'); subplot(2, 1, 2), plot(pos_sensing); title('pos');

end


