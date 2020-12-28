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
% x = audioread("signals/pd120_pos_sense.wav");
% x = audioread("signals/pd120_pos_sense2.wav");
% x = audioread("signals/pd120_single_hits.wav");
% x = audioread("signals/pd120_roll.wav");
% x = audioread("signals/pd120_middle_velocity.wav");
% x = audioread("signals/pd120_hot_spot.wav");
x = audioread("signals/pd120_rimshot.wav");
% x = audioread("signals/pd6.wav");
% org = audioread("signals/snare.wav"); x = resample(org(:, 1), 1, 6); % PD-120
% org = audioread("signals/snare.wav"); x = org(:, 1); Fs = 48e3; % PD-120

x = x(1:100000, :);

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

  % climb to the maximum of the current peak
  peak_idx = peak_start(1);
  max_idx  = find(hil_filt(1 + peak_idx:end) - hil_filt(peak_idx:end - 1) < 0);

  % second exit condition
  if isempty(max_idx)
    no_more_peak = true;
    continue;
  end

  peak_idx      = peak_idx + max_idx(1) - 1;
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


function rim_x_low = detect_rim_shot(rim_x, hil, all_peaks, Fs)

[rim_x_low, rim_hil_filt] = filter_input_signal(rim_x, Fs);

%[b, a]    = butter(3, 0.05, 'high');%figure;freqz(b,a,1024,8e3);
%rim_x_low = filter(b, a, rim_x);

%alpha     = 200 / Fs;
%rim_x_low = filter(alpha, [1, alpha - 1], rim_x);

%[b,a]=ellip(2, 3, 20, 100/4e3+[-.001 .001]);
%rim_x_low = filter(b, a, rim_x);

if ~isempty(rim_x_low)

  % TEST copy from above!!!!!
  energy_window_len  = round(6e-3 * Fs); % scan time (e.g. 2 ms)
  for i = 1:length(all_peaks)
    win_idx        = (all_peaks(i):all_peaks(i) + energy_window_len - 1) - energy_window_len / 2;
    win_idx        = win_idx((win_idx <= length(rim_x_low)) & (win_idx > 0));
    rim_max_pow(i) = max(abs(rim_x_low(win_idx)) .^ 2);
    hil_max_pow(i) = max(abs(hil(win_idx)) .^ 2);
  end

figure;
plot(20 * log10(abs(rim_x_low))); hold on; grid on;
plot(all_peaks, 10 * log10(rim_max_pow), '*-');
plot(all_peaks, ones(length(all_peaks), 1) * 87.5, 'r--'); % possible threshold
%plot(all_peaks, 10 * log10(hil_max_pow), '*-');
%plot(all_peaks, 10 * log10(rim_max_pow ./ hil_max_pow) + 60, '*-');
axis([-9.8041e+02   9.9146e+04   7.5604e+01   9.6315e+01]);

end

end


function processing(x, Fs, do_realtime)


% TEST rim shot support
rim_hil_filt = [];
rim_x        = [];
if size(x, 2) > 1
  rim_x = x(:, 2);
  x     = x(:, 1);
  [rim_hil, rim_hil_filt] = filter_input_signal(rim_x, Fs);
end


% calculate peak detection and positional sensing
[hil, hil_filt]  = filter_input_signal(x, Fs);
all_peaks        = calc_peak_detection(hil_filt, Fs);
rim_x_low        = detect_rim_shot(rim_x, hil, all_peaks, Fs);
pos_sense_metric = calc_pos_sense_metric(hil, Fs, all_peaks);

%if ~do_realtime
%  figure % open figure to keep previous plots (not desired for real-time)
%end
%
%% plot results
%cla
%plot(10 * log10([abs(x) .^ 2, hil_filt])); grid on; hold on;
%plot(all_peaks, 10 * log10(hil_filt(all_peaks)), 'g*');
%plot(all_peaks, pos_sense_metric + 40, 'k*');
%%if ~isempty(rim_hil_filt)
%%  plot(10 * log10(abs(rim_hil_filt))); hold on
%%  plot(all_peaks, 10 * log10(rim_hil_filt(all_peaks) ./ hil_filt(all_peaks)) + 60, 'b*-')
%%  plot(all_peaks, 10 * log10(rim_hil_filt(all_peaks)), 'y*')
%%end
%title('Green marker: level; Black marker: position');
%xlabel('samples'); ylabel('dB');
%ylim([-10, 90]);
%drawnow;


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


