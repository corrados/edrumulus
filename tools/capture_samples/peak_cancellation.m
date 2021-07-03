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


% TEST
figure;
plot(x);



