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
load win_recording1.txt
x = win_recording1;

% extract individual processing blocks marked with all zeros
y          = find(all(x == 0, 2));
num_blocks = length(y) - 1;

for i = 1:num_blocks
  z{i} = x(y(i) + 1:y(i + 1) - 1, :);
end

% select block to analyze
% interesting blocks: 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 23, 24
block_index = 13;
x           = z{block_index};


% TEST for testing we select just one signal
x = x(:, 1);


% DC offset estimation and compensation
% we assume here that the DC offset is constand over the entire block
dc_offset = mean(x);
x         = x - dc_offset;



% ADC spike cancellation algorithm ---------------------------------------------

%  // remove single/dual sample spikes by checking if right before and right after the
%  // detected spike(s) we only have noise and no useful signal (since the ESP32 spikes
%  // mostly are on just one or two sample(s))
%  const int max_peak_threshold = 150; // maximum assumed ESP32 spike amplitude
%
%  float       return_value = prev_input2[pad_index][input_channel_index]; // normal return value in case no spike was detected
%  const float input_abs    = abs ( input );
%  Espikestate input_state  = ST_OTHER; // initialization value, might be overwritten
%
%  if ( input_abs < ADC_MAX_NOISE_AMPL )
%  {
%    input_state = ST_NOISE;
%  }
%  else if ( input_abs < max_peak_threshold )
%  {
%    input_state = ST_SPIKE;
%  }
%
%  // check for single spike sample case
%  if ( ( prev3_input_state[pad_index][input_channel_index] == ST_NOISE ) &&
%       ( prev2_input_state[pad_index][input_channel_index] == ST_SPIKE ) &&
%       ( prev1_input_state[pad_index][input_channel_index] == ST_NOISE ) )
%  {
%    return_value = 0.0f; // remove single spike
%  }
%
%  // check for two sample spike case
%  if ( ( prev3_input_state[pad_index][input_channel_index] == ST_NOISE ) &&
%       ( prev2_input_state[pad_index][input_channel_index] == ST_SPIKE ) &&
%       ( prev1_input_state[pad_index][input_channel_index] == ST_SPIKE ) &&
%       ( input_state                                       == ST_NOISE ) )
%  {
%    prev_input1[pad_index][input_channel_index] = 0.0f; // remove two sample spike
%    return_value                                = 0.0f; // remove two sample spike
%  }
%
%  // update three-step input signal memory where we store the last three states of
%  // the input signal and two previous untouched input samples
%  prev3_input_state[pad_index][input_channel_index] = prev2_input_state[pad_index][input_channel_index];
%  prev2_input_state[pad_index][input_channel_index] = prev1_input_state[pad_index][input_channel_index];
%  prev1_input_state[pad_index][input_channel_index] = input_state;
%  prev_input2[pad_index][input_channel_index]       = prev_input1[pad_index][input_channel_index];
%  prev_input1[pad_index][input_channel_index]       = input;






% plot results
figure;
plot(x);


