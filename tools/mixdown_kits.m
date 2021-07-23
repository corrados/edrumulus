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
out_kit_path = '/home/corrados/edrumulus/tools/EdrumulusKit/';
samples_path = 'samples/';
mixed_prefix = 'mixed_';
kit_path     = '/home/corrados/edrumulus/tools/DRSKit/';

% channel names of the microphone signals
channel_names = {'AmbL', 'AmbR', 'Hihat', 'Kdrum_back', 'Kdrum_front', 'OHL', ...
  'OHR', 'Ride', 'Snare_bottom', 'Snare_top', 'Tom1', 'Tom2', 'Tom3'};

% audio mix matrix (left/right channels output gain for each channel)
mix_matrix = [1, 0; ...
              0, 1; ...
              1, 1; ...
              0, 0; ...
              1, 1; ...
              1, 0; ...
              0, 1; ...
              1, 1; ...
              0, 0; ...
              1, 1; ...
              1, 1; ...
              1, 1; ...
              1, 1];

% initialization
mkdir(out_kit_path);

% get instrument directory names
instr_dir = dir(kit_path);

for instrument_index = 1:length(instr_dir)

  if instr_dir(instrument_index).isdir && (length(instr_dir(instrument_index).name) > 2)

    disp(['Current instrument: ' instr_dir(instrument_index).name]);

    % get current instrument path
    file_path = [instr_dir(instrument_index).name '/'];

    % get instrument XML file name
    instr_root_dir      = dir([kit_path file_path]);
    xml_file_name_index = find(~[instr_root_dir.isdir]);
    [~, xml_file_name]  = fileparts(instr_root_dir(xml_file_name_index).name);

    % get instrument samples file names
    instr_samples_dir = dir([kit_path file_path samples_path]);

    % load and modify instrument XML
    file_id           = fopen([kit_path file_path xml_file_name '.xml'], 'r');
    end_of_file_found = false;
    cnt               = 1;
    xml_file          = {};

    while ~end_of_file_found
      xml_file{cnt} = fgetl(file_id);

      if xml_file{cnt} < 0
        xml_file          = xml_file(1:end - 1); % cut invalid line
        end_of_file_found = true;
      else

        % add prefix for mixed signal to the sample name
        if strfind(xml_file{cnt}, 'sample name="')
          insert_position = strfind(xml_file{cnt}, 'sample name="') + length('sample name="') - 1;
          xml_file{cnt}   = [xml_file{cnt}(1:insert_position) mixed_prefix xml_file{cnt}(insert_position + 1:end)];
        end

        % exchange names of first two channels (left/right channel)
        if strfind(xml_file{cnt}, ['audiofile channel="' channel_names{1}])
          xml_file{cnt} = strrep(xml_file{cnt}, ['audiofile channel="' channel_names{1}], ['audiofile channel="left_channel']);
        end
        if strfind(xml_file{cnt}, ['audiofile channel="' channel_names{2}])
          xml_file{cnt} = strrep(xml_file{cnt}, ['audiofile channel="' channel_names{2}], ['audiofile channel="right_channel']);
        end

        % remove all other channels
        for channel_names_index = 3:length(channel_names)
          if strfind(xml_file{cnt}, ['audiofile channel="' channel_names{channel_names_index}])
            cnt = cnt - 1;
          end
        end

        cnt = cnt + 1;

      end
    end

    fclose(file_id);

    % write modified instrument XML file
    mkdir([out_kit_path file_path]);
    file_id = fopen([out_kit_path file_path xml_file_name '.xml'], 'w');
    for line_index = 1:length(xml_file)
      fwrite(file_id, xml_file{line_index});
      fwrite(file_id, 10);
    end
    fclose(file_id);

    % mix and process instrument samples
    for sample_index = 1:length(instr_samples_dir)

      if ~instr_samples_dir(sample_index).isdir

        % get current sample file name
        [~, file_name] = fileparts(instr_samples_dir(sample_index).name);

        % load wave file
        x_all = audioread([kit_path file_path samples_path file_name '.wav']);

        % mix channels
        x_left  = x_all * mix_matrix(:, 1);
        x_right = x_all * mix_matrix(:, 2);
        x       = [x_left, x_right];


% TEST
%% filter one channel
%b = firls(255, [0 0.15 0.2 1], [1 1 0.8 0.8]);
%a = 1;
%freqz(b, a);
%x = filter(b, a, x);

% TEST
%% play the resulting wave form
%player = audioplayer(x, sampling_rate, 16);
%play(player);


        % store the resulting wave file
        mkdir([out_kit_path file_path samples_path]);
        audiowrite([out_kit_path file_path samples_path mixed_prefix file_name '.wav'], x, sampling_rate);

      end

    end

  end

end
