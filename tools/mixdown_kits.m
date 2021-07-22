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

pkg load signal

% load wave file
kit_path  = '/home/corrados/edrumulus/tools/DRSKit/';
file_name = [kit_path 'Tom1_whisker/samples/11-Tom1_whisker.wav'];
x_all     = audioread(file_name);

% select one channel
x = x_all(:, 3);

% filter one channel
b = firls(255, [0 0.15 0.2 1], [1 1 0.8 0.8]);
a = 1;
freqz(b, a);
x = filter(b, a, x);

% play the resulting wave form
player = audioplayer(x, 44100, 16);
play(player);
