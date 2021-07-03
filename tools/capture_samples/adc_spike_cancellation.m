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

% setup and load test signal
close all;
%load win_recording1.txt
%x = win_recording1;

%load win_recording_long1.txt
%x = win_recording_long1;

load win_recording_problematic.txt
x = win_recording_problematic;


% extract individual processing blocks marked with all zeros
y          = find(all(x == 0, 2));
num_blocks = length(y) - 1;

%figure; plot(diff(y));

for i = 1:num_blocks
  z{i} = x(y(i) + 1:y(i + 1) - 1, :);
end

% select block to analyze
% interesting blocks: 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 23, 24
% interesting blocks long: 20, 22, 106, 170, 194, 201, 202, 239
%block_index_range = 13:15;%[20, 22, 106, 170, 194, 201, 202, 239];%1:3;%6:200;%
block_index_range = 1;%1:length(z);


% % TEST store block data in original format (assuming a block size of 200)
%out = zeros(200 * length(block_index_range) + length(block_index_range) + 1, size(x, 2));
%cnt = 1;
%for block_index = block_index_range
%  out(1 + (cnt - 1) + (cnt - 1) * 200 + (1:200), :) = z{block_index};
%  cnt = cnt + 1;
%end
%save -ascii 'recording.txt' out
%return;


for block_index = block_index_range

  x = z{block_index};

  % DC offset estimation and compensation
  % we assume here that the DC offset is constand over the entire block
  dc_offset = mean(x);
  x         = x - dc_offset;


%% TEST for testing we select just one signal
%x = x(:, 1);


  for j = 1:size(x, 2)

    input = x(:, j);


    % ADC spike cancellation algorithm -----------------------------------------
    ADC_MAX_RANGE      = 4096; % Teensy 4.0 ADC has 12 bits -> 0..4095
    ADC_MAX_NOISE_AMPL = 8;    % highest assumed ADC noise amplitude in the ADC input range unit (measured)

    ST_OTHER = 0;
    ST_NOISE = 1;
    ST_SPIKE = 2;

    prev_input1       = 0;
    prev_input2       = 0;
    prev1_input_state = 0;
    prev2_input_state = 0;
    prev3_input_state = 0;

    % remove single/dual sample spikes by checking if right before and right after the
    % detected spike(s) we only have noise and no useful signal (since the ESP32 spikes
    % mostly are on just one or two sample(s))
    max_peak_threshold = 150; % maximum assumed ESP32 spike amplitude

    for i = 1:length(input)

      return_value = prev_input2; % normal return value in case no spike was detected
      input_abs    = abs(input(i));
      input_state  = ST_OTHER; % initialization value, might be overwritten

      if input_abs < ADC_MAX_NOISE_AMPL
        input_state = ST_NOISE;
      elseif input_abs < max_peak_threshold
        input_state = ST_SPIKE;
      end

      % check for single spike sample case
      if (prev3_input_state == ST_NOISE) && ...
         (prev2_input_state == ST_SPIKE) && ...
         (prev1_input_state == ST_NOISE)

        return_value = 0; % remove single spike
      end

      % check for two sample spike case
      if (prev3_input_state == ST_NOISE) && ...
         (prev2_input_state == ST_SPIKE) && ...
         (prev1_input_state == ST_SPIKE) && ...
         (input_state       == ST_NOISE)

        prev_input1  = 0; % remove two sample spike
        return_value = 0; % remove two sample spike

      end

      % update three-step input signal memory where we store the last three states of
      % the input signal and two previous untouched input samples
      prev3_input_state = prev2_input_state;
      prev2_input_state = prev1_input_state;
      prev1_input_state = input_state;
      prev_input2       = prev_input1;
      prev_input1       = x(i);

      x(i, j) = return_value;

    end

  end

  % cut out algorithm settling time
  x = x(3:end, :);

  % plot results
  plot(x); title(num2str(block_index));


% TEST
if length(block_index_range) > 1
  pause;
end


end


