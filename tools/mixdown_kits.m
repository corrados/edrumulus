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
out_kit_name = 'edrumuluskit';
out_kit_path = ['/home/corrados/edrumulus/tools/' out_kit_name '/'];
samples_path = 'samples/';
mixed_prefix = 'mixed_';

% optional: instrument select (only process this instrument)
instrument_select = [];%'Snare';% % use "[]" for mixing all instruments

% kit properties, where, e.g., channel names and audio mix matrix are
% defined (left/right channels output gain for each channel)
if 1
kit_path           = '/home/corrados/edrumulus/tools/DRSKit/';
channel_properties = {'AmbL',         'AmbL',         1, 0; ...
                      'AmbR',         'AmbR',         0, 1; ...
                      'Hihat',        'Hihat',        1, 1; ...
                      'Kdrum_back',   'Kdrum_back',   0.5, 0.5; ...
                      'Kdrum_front',  'Kdrum_front',  1, 1; ...
                      'OHL',          'OHL',          1, 0; ...
                      'OHR',          'OHR',          0, 1; ...
                      'Ride',         'Ride',         1, 1; ...
                      'Snare_bottom', 'Snare_bottom', 0.5, 0.5; ...
                      'Snare_top',    'Snare_top',    1, 1; ...
                      'Tom1',         'Tom1',         0.8, 0.8; ...
                      'Tom2',         'Tom2',         0.8, 0.8; ...
                      'Tom3',         'Tom3',         0.8, 0.8};
else
kit_path           = '/home/corrados/edrumulus/tools/aasimonster2/';
channel_properties = {'OH L',          'OHL',           1, 0; ...
                      'OH R',          'OHR',           0, 1; ...
                      'Snare top',     'Snare_top',     1, 1; ...
                      'Snare bottom',  'Snare_bottom',  0.5, 0.5; ...
                      'Snare trigger', 'Snare_trigger', 0, 0; ...
                      'Alesis',        'Trigger',       0, 0; ...
                      'Kick R',        'KdrumR',        1, 1; ...
                      'Kick L',        'KdrumL',        1, 1; ...
                      'Hihat',         'Hihat',         1, 1; ...
                      'Ride',          'Ride',          1, 1; ...
                      'Tom1',          'Tom1',          0.8, 0.8; ...
                      'Tom2',          'Tom2',          0.8, 0.8; ...
                      'Tom3',          'Tom3',          0.8, 0.8; ...
                      'Tom4',          'Tom4',          0.8, 0.8; ...
                      'Amb L',         'AmbL',          1, 0; ...
                      'Amb R',         'AmbR',          0, 1};
end

% initialization
mkdir(out_kit_path);

% get XML file and instrument directory names
instr_dir = dir(kit_path);

% identify "full" kit description (biggest file is assumed to be the "full" one)
xml_file_name_index = find(~[instr_dir.isdir]);
file_size           = zeros(length(instr_dir), 1);
for instrument_index = xml_file_name_index
  file_size(instrument_index) = instr_dir(instrument_index).statinfo.size;
end
[~, biggest_file_index] = max(file_size);

% load and modify kit XML
file_id           = fopen([kit_path instr_dir(biggest_file_index).name], 'r');
end_of_file_found = false;
cnt               = 1;
xml_file          = {};

while ~end_of_file_found
  xml_file{cnt} = fgetl(file_id);

  if xml_file{cnt} < 0
    xml_file          = xml_file(1:end - 1); % cut invalid line
    end_of_file_found = true;
  else

    % exchange names of first two channels (left/right channel)
    if strfind(xml_file{cnt}, ['channel name="' channel_properties{1, 2}])
      xml_file{cnt} = strrep(xml_file{cnt}, ['channel name="' channel_properties{1, 2}], ['channel name="left_channel']);
    end
    if strfind(xml_file{cnt}, ['channel name="' channel_properties{2, 2}])
      xml_file{cnt} = strrep(xml_file{cnt}, ['channel name="' channel_properties{2, 2}], ['channel name="right_channel']);
    end
    if strfind(xml_file{cnt}, ['channelmap in="' channel_properties{1, 1}])
      xml_file{cnt} = strrep(xml_file{cnt}, ['channelmap in="' channel_properties{1, 1}], ['channelmap in="left_channel']);
    end
    if strfind(xml_file{cnt}, ['channelmap in="' channel_properties{2, 1}])
      xml_file{cnt} = strrep(xml_file{cnt}, ['channelmap in="' channel_properties{2, 1}], ['channelmap in="right_channel']);
    end
    if strfind(xml_file{cnt}, ['" out="' channel_properties{1, 2}])
      xml_file{cnt} = strrep(xml_file{cnt}, ['" out="' channel_properties{1, 2}], ['" out="left_channel']);
    end
    if strfind(xml_file{cnt}, ['" out="' channel_properties{2, 2}])
      xml_file{cnt} = strrep(xml_file{cnt}, ['" out="' channel_properties{2, 2}], ['" out="right_channel']);
    end

    % remove all other channels
    for channel_properties_index = 3:size(channel_properties, 1)
      if strfind(xml_file{cnt}, ['channel name="' channel_properties{channel_properties_index, 2}])
        cnt = cnt - 1;
      end
    end
    for channel_properties_index = 3:size(channel_properties, 1)
      if strfind(xml_file{cnt}, ['channelmap in="' channel_properties{channel_properties_index, 1}])
        cnt = cnt - 1;
      end
    end

    cnt = cnt + 1;

  end
end

fclose(file_id);

% write modified kit XML file
file_id = fopen([out_kit_path out_kit_name '.xml'], 'w');
for line_index = 1:length(xml_file)
  fwrite(file_id, xml_file{line_index});
  fwrite(file_id, 10);
end
fclose(file_id);

% loop over all instruments
for instrument_index = 1:length(instr_dir)

  is_valid_instrument = instr_dir(instrument_index).isdir && (length(instr_dir(instrument_index).name) > 2);
  is_instrument_used  = isempty(instrument_select) || strcmp(instr_dir(instrument_index).name, instrument_select);


% TEST exclude the snare-position test instrument for now
is_instrument_used = is_instrument_used && ~strcmp(instr_dir(instrument_index).name, 'snare-position');


  if is_valid_instrument && is_instrument_used

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
        if strfind(xml_file{cnt}, ['audiofile channel="' channel_properties{1, 1}])
          xml_file{cnt}   = strrep(xml_file{cnt}, ['audiofile channel="' channel_properties{1, 1}], ['audiofile channel="left_channel']);
          insert_position = strfind(xml_file{cnt}, ['file="' samples_path]) + length(['file="' samples_path]) - 1;
          xml_file{cnt}   = [xml_file{cnt}(1:insert_position) mixed_prefix xml_file{cnt}(insert_position + 1:end)];
        end
        if strfind(xml_file{cnt}, ['audiofile channel="' channel_properties{2, 1}])
          xml_file{cnt}   = strrep(xml_file{cnt}, ['audiofile channel="' channel_properties{2, 1}], ['audiofile channel="right_channel']);
          insert_position = strfind(xml_file{cnt}, ['file="' samples_path]) + length(['file="' samples_path]) - 1;
          xml_file{cnt}   = [xml_file{cnt}(1:insert_position) mixed_prefix xml_file{cnt}(insert_position + 1:end)];
        end

        % remove all other channels
        for channel_properties_index = 3:size(channel_properties, 1)
          if strfind(xml_file{cnt}, ['audiofile channel="' channel_properties{channel_properties_index, 1}])
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
        mix_matrix = cell2mat(channel_properties(:, 3:4));
        x_left     = x_all * mix_matrix(:, 1);
        x_right    = x_all * mix_matrix(:, 2);
        x          = [x_left, x_right];


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
