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

% sampling rate depends on the kit
sampling_rate = 44100;

% base paths
kit_path     = '/home/corrados/edrumulus/tools/DRSKit/';
out_kit_path = '/home/corrados/edrumulus/tools/EdrumulusKit/';
samples_path = 'samples/';


% TODO loop over all instruments of a kit

% current instrument path
file_path = 'Tom1_whisker/';

% get instrument XML file name
instr_root_dir      = dir([kit_path file_path]);
xml_file_name_index = find(~[instr_root_dir.isdir]);
[~, xml_file_name]  = fileparts(instr_root_dir(xml_file_name_index).name);

% get instrument samples file names
instr_samples_dir = dir([kit_path file_path samples_path]);

% initialization
mkdir(out_kit_path);

% load instrument XML
file_id           = fopen([kit_path file_path xml_file_name '.xml'], 'r');
end_of_file_found = false;
cnt               = 1;
xml_file          = {};

while ~end_of_file_found
  xml_file{cnt} = fgetl(file_id);

  if xml_file{cnt} < 0
    xml_file = xml_file(1:end - 1);
    end_of_file_found = true;
  end

  cnt = cnt + 1;
end

fclose(file_id);

for sample_index = 1:length(instr_samples_dir)

  if ~instr_samples_dir(sample_index).isdir

    % get current sample file name
    [~, file_name] = fileparts(instr_samples_dir(sample_index).name);

    % load wave file
    x_all = audioread([kit_path file_path samples_path file_name '.wav']);

    % select one channel
    x = x_all(:, 1:2);

    % filter one channel
    b = firls(255, [0 0.15 0.2 1], [1 1 0.8 0.8]);
    a = 1;
    freqz(b, a);
    x = filter(b, a, x);

    % play the resulting wave form
    player = audioplayer(x, sampling_rate, 16);
    play(player);

    % store the resulting wave file
    mkdir([out_kit_path file_path samples_path]);
    audiowrite([out_kit_path file_path samples_path 'mixed_' file_name '.wav'], x, sampling_rate);

  end

end
