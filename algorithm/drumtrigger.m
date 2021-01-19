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

function drumtrigger

% Drum trigger tests

close all;
pkg load signal
pkg load audio

Fs = 8000; % Hz

% TEST for continuous audio data capturing and processing
% continuous_recording(1, Fs, @(x, Fs, do_realtime) processing(x, Fs, do_realtime));

% TEST process recordings
% x = audioread("signals/esp32_pd120.wav");
% x = audioread("signals/pd120_pos_sense.wav");%x = x(1:5000, :);%x = x(55400:58000, :);%
% x = audioread("signals/pd120_pos_sense2.wav");
% x = audioread("signals/pd120_single_hits.wav");
% x = audioread("signals/pd120_roll.wav");
% x = audioread("signals/pd120_middle_velocity.wav");
% x = audioread("signals/pd120_hot_spot.wav");
x = audioread("signals/pd120_rimshot.wav");%x = x(1200:1700, :);%x = x(168000:171000, :);%x = x(1:34000, :);%x = x(1:100000, :);
%x = audioread("signals/pd120_rimshot_hardsoft.wav");
% x = audioread("signals/pd6.wav");
% org = audioread("signals/snare.wav"); x = resample(org(:, 1), 1, 6); % PD-120
% org = audioread("signals/snare.wav"); x = org(:, 1); Fs = 48e3; % PD-120


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

%x_edge   = x(26200:28000);
%x_middle = x(3000:4200);
%figure; subplot(2, 1, 1), pwelch(x_middle,[],[],[],[],'twosided','db'); title('middle');
%subplot(2, 1, 2), pwelch(x_edge,[],[],[],[],'twosided','db'); title('edge');
%figure; freqz(fir1(80, 0.02));

% hil = myhilbert(x);
% figure; plot(20 * log10(abs([x, hilbert(x)])));
% figure; plot(20 * log10(abs([x, myhilbert(x)]))); title('myhilbert');

processing(x * 25000, Fs, false); % scale to the ESP32 input range

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

energy_window_len = round(2e-3 * Fs); % scan time (e.g. 2 ms)

% Hilbert filter
hil = myhilbert(x);

% moving average filter
hil_filt = abs(filter(ones(energy_window_len, 1) / energy_window_len, 1, hil)) .^ 2; % moving average

end


function all_peaks = calc_peak_detection(hil_filt, Fs)

threshold_db = 23; % TEST: figure;plot(10.^((15:(30/31):45)/20),'.-')
mask_time    = round(10e-3 * Fs); % mask time (e.g. 10 ms)

% the following settings are trigger pad-specific (here, a PD-120 is used)
decay_len     = round(0.25 * Fs); % decay time (e.g. 250 ms)
decay_fact_db = 1; % decay factor in dB
decay_grad    = 200 / Fs; % decay gradient factor

last_peak_idx = 0;
all_peaks     = [];
i             = 1;
no_more_peak  = false;
decay_all     = nan(size(hil_filt)); % only for debugging
hil_filt_org  = hil_filt;            % only for debugging

while ~no_more_peak

  % find values above threshold, masking regions which are already done
  above_thresh = (hil_filt > 10 ^ (threshold_db / 10)) & [zeros(last_peak_idx, 1); ones(length(hil_filt) - last_peak_idx, 1)];
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

  % search in a pre-defined scan time for the highest peak
  scan_time    = round(1e-3 * Fs); % scan time from first detected peak
  [~, max_idx] = max(hil_filt(peak_idx:min(1 + peak_idx + scan_time - 1, length(hil_filt))));
  peak_idx     = peak_idx + max_idx - 1;

  % store the new detected peak
  all_peaks     = [all_peaks; peak_idx];
  last_peak_idx = min(peak_idx + mask_time, length(hil_filt));

  % exponential decay assumption (note that we must not use hil_filt_org since a
  % previous peak might not be faded out and the peak detection works on hil_filt)
  decay           = hil_filt(peak_idx) * 10 ^ (decay_fact_db / 10) * 10 .^ (-(0:decay_len - 1) / 10 * decay_grad);
  decay_x         = peak_idx + (0:decay_len - 1) + 2; % NOTE "+ 2" delay needed for sample-wise processing
  valid_decay_idx = decay_x <= length(hil_filt);
  decay           = decay(valid_decay_idx);
  decay_x         = decay_x(valid_decay_idx);

  % subtract decay (with clipping at zero)
  hil_filt_new                   = hil_filt(decay_x) - decay.';
  hil_filt_new(hil_filt_new < 0) = 0;

  % update filtered signal
  hil_filt(decay_x) = hil_filt_new;
  i                 = i + 1;

  decay_all(decay_x) = decay; % only for debugging

end

%figure; plot(10 * log10([hil_filt_org, hil_filt, decay_all])); hold on;
%plot(all_peaks, 10 * log10(hil_filt_org(all_peaks)), 'k*');

end


function pos_sense_metric = calc_pos_sense_metric(hil, Fs, all_peaks)

energy_window_len = round(2e-3 * Fs); % scan time (e.g. 2 ms)

% low pass filter of the Hilbert signal
% lp_ir_len = 80; % low-pass filter length
% lp_cutoff = 0.02; % normalized cut-off of low-pass filter
% a         = fir1(lp_ir_len, lp_cutoff);
% hil_low   = filter(a, 1, hil);
% hil_low   = hil_low(lp_ir_len / 2:end);
% use a simple one-pole IIR filter for less CPU processing and shorter delay
alpha   = 0.025 * 8e3 / Fs;
hil_low = filter(alpha, [1, alpha - 1], hil);

% figure; plot(20 * log10(abs([hil(1:length(hil_low)), hil_low]))); hold on;

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
  win_idx            = (all_peaks(i):all_peaks(i) + energy_window_len - 1) - energy_window_len / 2;
  win_idx            = win_idx((win_idx <= length(hil_low)) & (win_idx > 0));
  peak_energy(i)     = sum(abs(hil(win_idx)) .^ 2);
  peak_energy_low(i) = sum(abs(hil_low(win_idx)) .^ 2);

  win_idx_all = [win_idx_all; win_idx]; % only for debugging

end

pos_sense_metric = 10 * log10(peak_energy) - 10 * log10(peak_energy_low);

% % figure; plot(20 * log10(abs([hil, hil_low]))); hold on;
% % plot(win_idx_all', 20 * log10(abs(hil(win_idx_all'))), 'k.-');
% figure; plot(20 * log10(abs(hil))); hold on;
% for i = 1:size(win_idx_all, 1)
%   plot(win_idx_all(i, :), 20 * log10(abs(hil(win_idx_all(i, :)))), 'k.-');
%   plot(win_idx_all(i, :), 20 * log10(abs(hil_low(win_idx_all(i, :)))), 'b.-');
%   plot(all_peaks, 10 * log10(peak_energy), 'k');
%   plot(all_peaks, 10 * log10(peak_energy_low), 'b');
% end

end


function is_rim_shot = detect_rim_shot(x, hil, hil_filt, all_peaks, Fs)

is_rim_shot           = false(size(all_peaks));
rim_shot_window_len   = round(6e-3 * Fs); % scan time (e.g. 6 ms)
rim_shot_threshold_dB = 87.5;
rim_shot_threshold    = 10 ^ (rim_shot_threshold_dB / 10);

if size(x, 2) > 1

% TEST
%[b, a]    = butter(3, 0.05, 'high');%figure;freqz(b,a,1024,8e3);
%rim_x_low = filter(b, a, rim_x);

%alpha     = 200 / Fs;
%rim_x_low = filter(alpha, [1, alpha - 1], rim_x);

%[b,a]=ellip(2, 3, 20, 100/4e3+[-.001 .001]);
%rim_x_low = filter(b, a, rim_x);

%% TEST
%figure; plot(20 * log10(abs([x, rim_x]))); title('unfiltered'); hold on;
%        plot(all_peaks, 10 * log10(hil_filt(all_peaks)), 'g*');
%figure; plot(20 * log10(abs([hil_filt, rim_hil_filt]))); title('filtered');

%% TEST
%% [b,a]=ellip(2, 3, 20, 100/4e3+[-.001 .001]);figure;freqz(b,a,1024,8e3);
%% [b,a]=ellip(2, 3, 20, 25/4e3+[-.001 .001]);figure;freqz(b,a,1024,8e3)
%
%[b,a]=ellip(2, 3, 20, 100/4e3+[-.001 .001]);
%rim_x_100 = filter(b, a, rim_x);
%
%[b,a]=ellip(2, 3, 20, 25/4e3+[-.001 .001]);
%rim_x_25 = filter(b, a, rim_x);
%
%alpha     = 0.025 * 8e3 / Fs;
%rim_x_low = filter(alpha, [1, alpha - 1], rim_x);
%
%figure;
%plot(20 * log10(abs([rim_x_low, rim_x])));
%
%figure;
%N = 500;
%rim_x_mov = filter(ones(N, 1) / N, 1, rim_x); % moving average
%plot(20 * log10(abs(rim_x_mov)));


  x_rim_hil = filter_input_signal(x(:, 2), Fs);%hilbert(x(:, 2));%x(:, 2);%

lin_reg_debug = nan(size(x_rim_hil));

  for i = 1:length(all_peaks)

    win_idx        = (all_peaks(i):all_peaks(i) + rim_shot_window_len - 1) - rim_shot_window_len / 2;
    win_idx        = win_idx((win_idx <= length(x_rim_hil)) & (win_idx > 0));
    rim_max_pow(i) = max(abs(x_rim_hil(win_idx)) .^ 2);

% TEST
hil_max_pow(i)      = max(abs(hil(win_idx)) .^ 2);
hil_filt_max_pow(i) = max(hil_filt(win_idx));


%% TEST
%rim_max_pow(i)      = mean(abs(x_rim_hil(win_idx)) .^ 2);
%hil_max_pow(i)      = mean(abs(hil(win_idx)) .^ 2);
%hil_filt_max_pow(i) = mean(hil_filt(win_idx));


% TEST linear regression of rim signal at detected peak position
test_window_len = round(3e-3 * Fs);
win_idx2        = (all_peaks(i):all_peaks(i) + test_window_len - 1) - test_window_len / 2;
win_idx2        = win_idx2((win_idx2 <= length(x_rim_hil)) & (win_idx2 > 0));
a = 20 * log10(abs(x_rim_hil(win_idx2)));
b = (1:length(a))';
m(i)  = sum((b - mean(b)) .* (a - mean(a))) / sum((b - mean(b)) .^ 2);
b0(i) = mean(a) - m(i) * mean(b);
lin_reg_debug(win_idx2) = b0(i) + m(i) * b;

s = 20 * log10(abs(x(win_idx2, 2))) - lin_reg_debug(win_idx2);
[~, index(i)] = max(s);

% TEST peak-to-average
metric(i) = max(lin_reg_debug(win_idx2)) / mean(lin_reg_debug(win_idx2));

  end

  is_rim_shot = rim_max_pow > rim_shot_threshold;

%% TEST
%window_len = 10;
%x2_filt = sqrt(filter(ones(window_len, 1) / window_len, 1, abs(x(:, 2)) .^ 2)); % moving average

figure;
%plot(20 * log10(abs([x(:, 2), x(:, 1), x2_filt]))); hold on; grid on;
plot(20 * log10(abs([x(:, 2), x(:, 1), sqrt(hil_filt)]))); hold on; grid on;
plot(all_peaks(is_rim_shot), 10 * log10(rim_max_pow(is_rim_shot)), '*');
plot(all_peaks(~is_rim_shot), 10 * log10(rim_max_pow(~is_rim_shot)), '*');
%plot(all_peaks, ones(length(all_peaks), 1) * rim_shot_threshold_dB, 'r--'); % possible threshold

plot(all_peaks, 10 * log10(hil_max_pow), '*-');
plot(all_peaks, 10 * log10(hil_filt_max_pow), '*-');
%plot(all_peaks, 10 * log10(rim_max_pow ./ hil_max_pow) + 60, '*-');
plot(all_peaks, 10 * log10(rim_max_pow ./ hil_filt_max_pow) + 60, '*-');
%%axis([-9.8041e+02   9.9146e+04   7.5604e+01   9.6315e+01]);

plot(all_peaks, 10 * log10(metric), 'k*-');
plot(lin_reg_debug, 'k');
plot(20 * log10(abs(x(:, 2))) - lin_reg_debug, 'k');

test = 20 * log10(abs(x(:, 2))) - lin_reg_debug;
figure;x2 = test(~isnan(test));plot(x2);hold on;
x1 = test_window_len:test_window_len:length(x2);
y1 = ones(1, length(x2) / test_window_len);
plot(x1, y1 * -10, 'k*-');

%axis([1085.009   1282.718     11.541     84.837]);
%a1 = gca; f2 = figure; a2 = copyobj(a1, f2);
%axis([3.4512e+04   3.4663e+04   1.2406e+01   7.5105e+01]);



%a1 = gca; f2 = figure; a2 = copyobj(a1, f2); legend('no rim 1'); axis([1.3126e+03   1.4691e+03  -2.3480e+00   8.0788e+01]);
%f2 = figure; a2 = copyobj(a1, f2); legend('no rim 2'); axis([4.7730e+03   4.9209e+03  -3.5131e+00   8.1233e+01]);
%f2 = figure; a2 = copyobj(a1, f2); legend('no rim 3'); axis([8.1841e+03   8.3706e+03  -8.0507e+00   8.2218e+01]);
%f2 = figure; a2 = copyobj(a1, f2); legend('no rim 4'); axis([1.1576e+04   1.1785e+04  -3.0539e+00   7.9802e+01]);
%
%f2 = figure; a2 = copyobj(a1, f2); legend('with rim 1'); axis([1.8161e+04   1.8322e+04  -1.6087e+00   8.7223e+01]);
%f2 = figure; a2 = copyobj(a1, f2); legend('with rim 2'); axis([2.2250e+04   2.2513e+04  -7.6793e+00   8.9130e+01]);
%f2 = figure; a2 = copyobj(a1, f2); legend('with rim 3'); axis([2.5873e+04   2.6007e+04  -8.0813e+00   8.8582e+01]);
%f2 = figure; a2 = copyobj(a1, f2); legend('with rim 4'); axis([2.9413e+04   2.9636e+04  -2.7471e+00   8.6617e+01]);



%% TEST linear regression
%figure;
%b = (1:100)';
%a = randn(100, 1) + 0.4 * b + 10;
%
%m  = sum((b - mean(b)) .* (a - mean(a))) / sum((b - mean(b)) .^ 2);
%b0 = mean(a) - m * mean(b)
%
%plot(a); hold on; plot(b0 + m * b);


end

end


function processing(x, Fs, do_realtime)

% calculate peak detection and positional sensing
[hil, hil_filt]  = filter_input_signal(x(:, 1), Fs);
all_peaks        = calc_peak_detection(hil_filt, Fs);
is_rim_shot      = detect_rim_shot(x, hil, hil_filt, all_peaks, Fs);
pos_sense_metric = calc_pos_sense_metric(hil, Fs, all_peaks);

%if ~do_realtime
%  figure % open figure to keep previous plots (not desired for real-time)
%end

%% plot results
%cla
%plot(10 * log10([abs(x(:, 1)) .^ 2, hil_filt])); grid on; hold on;
%plot(all_peaks, 10 * log10(hil_filt(all_peaks)), 'g*');
%plot(all_peaks, pos_sense_metric + 40, 'k*');
%title('Green marker: level; Black marker: position');
%xlabel('samples'); ylabel('dB');
%ylim([-10, 90]);
%drawnow;


% TEST
% velocity/positional sensing mapping and play MIDI notes
velocity            = (10 * log10(hil_filt(all_peaks)) / 39) * 127 - 73;
velocity_clipped    = max(1, min(127, velocity));
pos_sensing         = (pos_sense_metric / 4) * 127 - 510;
pos_sensing_clipped = max(1, min(127, pos_sensing));
% play_midi(all_peaks, velocity_clipped, pos_sensing_clipped);
% figure; subplot(2, 1, 1), plot(velocity); title('velocity'); subplot(2, 1, 2), plot(pos_sensing); title('pos');

end


function continuous_recording(blocklen, Fs, callbackfkt)

% continuous recording of audio data and processing in a callback function
recorder   = audiorecorder(Fs, 16, 1);
bDataReady = false;

while true

  while isrecording(recorder)
    pause(blocklen / 1000);
  end

  if bDataReady
    x = getaudiodata(recorder);
  end

  record(recorder, blocklen);

  if bDataReady
    callbackfkt(x, Fs, true);
  end

  bDataReady = true;

end

end


function play_midi(all_peaks, velocity, pos_sensing)

dev = mididevice("output", "Lexicon Mac USB 1"); % Lexicon Omega -> TDW-20
x   = now;
y   = x;

for i = 1:length(all_peaks)

  while y < x + all_peaks(i) / 8000 / 1e5
    pause(0.00001);
    y = now;
  end

  midisend(dev, midimsg("controlchange", 10, 16, pos_sensing(i))); % positional sensing
  midisend(dev, midimsg("note",          10, 38, velocity(i), 0.02));

end

end


