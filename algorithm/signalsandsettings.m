%*******************************************************************************
% Copyright (c) 2020-2023
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

function [x, pad] = signalsandsettings(is_called_from_other_function)

% special case: if this function is called, we assume drumtrigger should be called
if ~exist('is_called_from_other_function', 'var')
  drumtrigger;
  return;
end

padtype = 'pd120'; % default

% load test data

%x = audioread("signals/teensy4_0_noise_test.wav");x=(x-mean(x))*4;padtype='pd80r';
%x = audioread("signals/teensy4_0_pd80r.wav");x=(x-mean(x))*4;padtype='pd80r';%x = x(1:390000, :);%
%x = audioread("signals/teensy4_0_pd80r_hot_spot.wav");x=(x-mean(x))*4;padtype='pd80r';
%x = audioread("signals/teensy4_0_pd120_hot_spot.wav");x=(x-mean(x))*4;
%x = audioread("signals/esp32_pd120.wav");x=x/8;
%x = audioread("signals/esp32_pd8.wav");x=x/8;padtype='pd8';
%x = audioread("signals/pd120_pos_sense.wav");%x=x(10600:15000);%x = x(2900:10000, :);%x = x(55400:58000, :);%
%x = audioread("signals/pd120_pos_sense2.wav");
%x = audioread("signals/pd120_single_hits.wav");
%x = audioread("signals/pd120_roll.wav");%x = x(1:20000, :);%x = x(292410:294749, :);%x = x(311500:317600, :);
%x = audioread("signals/pd120_middle_velocity.wav");
%x = audioread("signals/pd120_hot_spot.wav");%x = x(1:5000);%x = x(41500:42200);%
%x = audioread("signals/pd120_rimshot.wav");%x=x(7000:15000,:);%x = x(1:100000, :);%x = x(168000:171000, :);%x = x(1:34000, :);%
%x = audioread("signals/pd120_rimshot_hardsoft.wav");
%x=audioread("signals/pd120_middle_velocity.wav");x=[x;audioread("signals/pd120_pos_sense2.wav")];x=[x;audioread("signals/pd120_hot_spot.wav")];
%x = audioread("signals/pd80r.wav");x=x(:,1);padtype='pd80r';x = x(1:265000, :);%x = x(264000:320000, :);%
%x = audioread("signals/pd80r_hot_spot.wav");padtype='pd80r';%x = x(191700:192400, :);%
%x = audioread("signals/pd80r_no_hot_spot.wav");padtype='pd80r';
%x = audioread("signals/pd80r_rimshot_issue.wav");padtype='pd80r';
x = audioread("signals/pd85rimshotpossense.wav");padtype='pd80r';
%x = audioread("signals/pda120ls.wav");x=x(:,1);padtype='pda120ls';x = x(1:630000, :);%x = x(1.06e6:end, :);%x = x(840000:930000, :);%
%x = audioread("signals/pda120ls_2.wav");x=x(:,1);padtype='pda120ls';x = x(1:210000, :);
%x = audioread("signals/pda120ls_multpiezotest.wav");padtype='pda120ls';
%x = audioread("signals/pda120ls_multpiezotest2.wav");padtype='pda120ls';
%x = audioread("signals/pdx100.wav");padtype='pdx100';x = x(1:420000, :);
%x = audioread("signals/pdx100_rimshot.wav");padtype='pdx100';
%x = audioread("signals/pdx8.wav");padtype='pdx8';
%x = audioread("signals/pd5.wav");padtype='pd5';
%x = audioread("signals/pd6.wav");padtype='pd6';
%x = audioread("signals/pd8.wav");padtype='pd8';%x = x(1:300000, :);%x = x(420000:470000, :);%x = x(1:100000, :);
%x = audioread("signals/pd8_rimshot.wav");padtype='pd8';
%x = audioread("signals/hd1tom.wav");padtype='hd1tom';
%x = audioread("signals/cy5.wav");padtype='cy5';
%x = audioread("signals/cy6.wav");padtype='cy6';%x = x(480000:590000, :);%x = x(250000:450000, :);%x = x(1:150000, :);
%x = audioread("signals/cy8.wav");padtype='cy8';%x = x(1:200000, :);
%x = audioread("signals/kd8.wav");padtype='kd8';%x = x(177050:178200, :);%
%x = audioread("signals/kd7.wav");padtype='kd7';%x = x(1:170000, :);
%x = audioread("signals/kd7_hard_hits.wav");padtype='kd7';x = x(1:3000, :);
%x = audioread("signals/kt10.wav");padtype='kt10';
%x = audioread("signals/kd120.wav");padtype='kd120';
%x = audioread("signals/tp80.wav");padtype='tp80';
%x = audioread("signals/vh12.wav");padtype='vh12';%x = x(900000:end, :);%x = x(376000:420000, :);%x = x(1:140000, :);
%x = audioread("signals/drumtec_diabolo12.wav");x=x(:,1);padtype='diabolo12';
%x = audioread("signals/mps750x_tom.wav");padtype='mps750x_tom';x = x(156000:end, :); %until press rolls: 1:156000, press rolls: 156000:end
%x = audioread("signals/mps750x_snare.wav");padtype='mps750x_tom';x = x(1:172000, :); %until press rolls: 1:172000, press rolls: 172000:end
%x = audioread("signals/mps750x_snare_rim.wav");padtype='mps750x_tom';
%x = audioread("signals/mps750x_kick.wav");padtype='mps750x_kick';
%x = audioread("signals/mps750x_ride_all.wav");padtype='mps750x_cymbal';
%x = audioread("signals/mps750x_crash_all.wav");padtype='mps750x_cymbal';
%x = audioread("signals/lehhs12c_all.wav");padtype='lehhs12c';


% scale to the ESP32 input range to match the signal level of the ESP32
x = x * 25000;


  
% pad PRESET settings first, then overwrite these with pad specific properties
pad.threshold_db                    = 17;
pad.mask_time_ms                    = 6;
pad.first_peak_diff_thresh_db       = 8;
pad.mask_time_decay_fact_db         = 15;
pad.scan_time_ms                    = 2.5;
pad.pre_scan_time_ms                = 2.5;
pad.decay_est_delay_ms              = 7;
pad.decay_est_len_ms                = 4;
pad.decay_est_fact_db               = 16;
pad.decay_fact_db                   = 1;
pad.decay_len_ms1                   = 0; % not used
pad.decay_len_ms2                   = 350;
pad.decay_len_ms3                   = 0; % not used
pad.decay_grad_fact1                = 200;
pad.decay_grad_fact2                = 200;
pad.decay_grad_fact3                = 200;
pad.pos_low_pass_cutoff             = 150; % Hz
pad.pos_invert                      = false;
pad.rim_shot_window_len_ms          = 3.5;
pad.rim_use_low_freq_bp             = true;
pad.second_peak_diff_ms             = 2.55;
pad.hot_spot_sec_peak_win_len_ms    = 0.5;
pad.hot_spot_peak_diff_limit_min_db = 0.4; % dB minimum difference between first and second peak
pad.hot_spot_middle_diff_db         = 14;
pad.hot_spot_attenuation_db         = 0; % 0 dB attenuation means that hot spot suppression is turned off

switch padtype
  case 'pd120'
    % note: the PRESET settings are from the PD120 pad
    pad.hot_spot_attenuation_db = 3;
  case 'lehhs12c'
    pad.scan_time_ms              = 4;
    pad.decay_fact_db             = 5;
    pad.decay_len_ms2             = 600;
    pad.decay_grad_fact2          = 100;
  case 'mps750x_tom'
    pad.scan_time_ms              = 6;
    pad.pre_scan_time_ms          = 3.5;
    pad.decay_grad_fact2          = 150;
    pad.decay_len_ms2             = 450;
    pad.rim_use_low_freq_bp       = false;
  case 'mps750x_kick'
    pad.first_peak_diff_thresh_db = 3;
    pad.scan_time_ms              = 6;
    pad.pre_scan_time_ms          = 3.5;
    pad.decay_grad_fact2          = 150;
    pad.decay_len_ms2             = 450;
    pad.threshold_db              = 30;
  case 'mps750x_cymbal'
    pad.scan_time_ms              = 3;
    pad.decay_est_delay_ms        = 1;
    pad.decay_grad_fact2          = 150;
    pad.decay_len_ms2             = 450;
    pad.threshold_db              = 30;
    pad.rim_use_low_freq_bp       = false;
  case 'pda120ls'
    pad.decay_grad_fact2          = 250;
    pad.decay_fact_db             = 5;
    pad.pre_scan_time_ms          = 3.5;
    pad.first_peak_diff_thresh_db = 7;
    pad.pos_invert                = true;
  case 'pd80r'
    pad.scan_time_ms                    = 3;
    pad.decay_len_ms2                   = 75;
    pad.decay_grad_fact2                = 300;
    pad.decay_len_ms3                   = 300;
    pad.decay_grad_fact3                = 100;
    pad.rim_use_low_freq_bp             = false;
    pad.hot_spot_attenuation_db         = 3;
    pad.hot_spot_peak_diff_limit_min_db = 4;
    pad.hot_spot_middle_diff_db         = 5;
  case 'pdx100'
    pad.decay_fact_db       = 2;
    pad.decay_len_ms2       = 65;
    pad.decay_grad_fact2    = 300;
    pad.decay_len_ms3       = 350;
    pad.decay_grad_fact3    = 150;
    pad.pos_low_pass_cutoff = 120;
  case 'pdx8'
    % note: no changes needed to the default values
  case 'pd5'
    pad.scan_time_ms     = 4;
    pad.mask_time_ms     = 9;
    pad.decay_grad_fact2 = 400;
  case 'pd6'
    pad.scan_time_ms     = 1.5;
    pad.decay_grad_fact2 = 400;
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
    pad.scan_time_ms       = 2;
    pad.decay_est_delay_ms = 8;
    pad.decay_fact_db      = 5;
    pad.decay_len_ms1      = 4;
    pad.decay_grad_fact1   = 30;
    pad.decay_len_ms2      = 30;
    pad.decay_grad_fact2   = 450;
    pad.decay_len_ms3      = 500;
    pad.decay_grad_fact3   = 45;
  case 'kd8'
    pad.scan_time_ms            = 3;
    pad.mask_time_decay_fact_db = 10;
    pad.decay_grad_fact2        = 450;
    pad.decay_len_ms3           = 500;
    pad.decay_grad_fact3        = 45;
  case 'kt10'
    pad.scan_time_ms       = 2;
    pad.decay_est_delay_ms = 8;
    pad.decay_fact_db      = 5;
    pad.decay_len_ms1      = 4;
    pad.decay_grad_fact1   = 30;
    pad.decay_len_ms2      = 30;
    pad.decay_grad_fact2   = 350;
    pad.decay_len_ms3      = 500;
    pad.decay_grad_fact3   = 100;
  case 'kd120'
    pad.scan_time_ms      = 6;
    pad.decay_fact_db     = 12;
    pad.decay_est_fact_db = 20;
  case 'cy5'
    pad.scan_time_ms  = 3;
    pad.mask_time_ms  = 8;
    pad.decay_fact_db = 3;
  case 'cy6'
    pad.scan_time_ms     = 6;
    pad.decay_len_ms2    = 150;
    pad.decay_grad_fact2 = 120;
    pad.decay_len_ms3    = 450;
    pad.decay_grad_fact3 = 30;
  case 'cy8'
    pad.scan_time_ms     = 6;
    pad.decay_len_ms1    = 10;
    pad.decay_grad_fact1 = 10;
    pad.decay_len_ms2    = 100;
    pad.decay_grad_fact2 = 200;
    pad.decay_len_ms3    = 450;
    pad.decay_grad_fact3 = 30;
  case 'diabolo12'
    pad.scan_time_ms              = 2;
    pad.mask_time_ms              = 8;
    pad.first_peak_diff_thresh_db = 13;
    pad.mask_time_decay_fact_db   = 20;
    pad.decay_grad_fact2          = 270;
    pad.decay_fact_db             = 6;
    pad.decay_est_delay_ms        = 20;
    pad.pos_low_pass_cutoff       = 50; % positional sensing seems not to work correctly
  case 'hd1tom'
    pad.scan_time_ms        = 1.5;
    pad.decay_grad_fact2    = 300;
    pad.pos_low_pass_cutoff = 300;
end

end


