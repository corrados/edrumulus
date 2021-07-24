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
out_kit_name  = 'edrumuluskit';
out_kit_path  = [out_kit_name '/'];
samples_path  = 'samples/';
mixed_prefix  = 'mixed_';
midi_map_name = [out_kit_name '_midimap'];

% optional: instrument select (only process this instrument)
instrument_select = [];%'Snare';% % use "[]" for mixing all instruments

% kit properties, where, e.g., channel names and audio mix matrix are
% defined (left/right channels output gain for each channel)
if 1
kit_path           = 'DRSKit/';
channel_properties = {'AmbL',         'AmbL',           0, -90,   -2,  0, 0; ...
                      'AmbR',         'AmbR',         -90,   0,   -2,  0, 0; ...
                      'Hihat',        'Hihat',          0,   0,   -5, -3, 0; ...
                      'Kdrum_back',   'Kdrum_back',    -3,  -3,    0,  0, 0; ...
                      'Kdrum_front',  'Kdrum_front',    0,   0,    3, -1, 2; ...
                      'OHL',          'OHL',            0, -90,   -5,  0, 0; ...
                      'OHR',          'OHR',          -90,   0,   -5,  0, 0; ...
                      'Ride',         'Ride',           0,   0,   -5,  0, 5; ...
                      'Snare_bottom', 'Snare_bottom',  -3,  -3,    0,  0, 0; ...
                      'Snare_top',    'Snare_top',      0,   0,   -5,  3, 5; ...
                      'Tom1',         'Tom1',           0,   0,    0,  0, 5; ...
                      'Tom2',         'Tom2',           0,   0,    0,  0, 5; ...
                      'Tom3',         'Tom3',           0,   0,    0,  0, 5};
midi_map           = {'Crash_left_shank',   55; ...
                      'Hihat_closed_shank', 22; ...
                      'Hihat_semi_open',    26; ...
                      'Kdrum_with_contact', 36; ...
                      'Ride_tip',           51; ...
                      'Ride_shank_bell',    66; ...
                      'Snare',              38; ...
                      'Snare',              40; ...
                      'Tom1',               48; ...
                      'Tom2',               45};
elseif 0
kit_path           = 'aasimonster2/';
channel_properties = {'Alesis',        'Trigger',       -90, -90,   0,  0, 0; ...
                      'Amb L',         'AmbL',           -3, -90,   0,  0, 0; ...
                      'Amb R',         'AmbR',          -90,  -3,   0,  0, 0; ...
                      'Hihat',         'Hihat',           0,   0,   0,  0, 0; ...
                      'Kick L',        'KdrumL',          0,   0,   5, -5, 3; ...
                      'Kick R',        'KdrumR',          0,   0,   0,  0, 0; ...
                      'OH L',          'OHL',            -3, -90,   0,  0, 0; ...
                      'OH R',          'OHR',           -90,  -3,   0,  0, 0; ...
                      'Ride',          'Ride',            0,   0,   0,  0, 0; ...
                      'Snare bottom',  'Snare_bottom',    3,   3,  -3,  0, 0; ...
                      'Snare top',     'Snare_top',       3,   3,   0,  0, 0; ...
                      'Snare trigger', 'Snare_trigger', -90, -90,   0,  0, 0; ...
                      'Tom1',          'Tom1',            0,   0,   0,  0, 0; ...
                      'Tom2',          'Tom2',            0,   0,   0,  0, 0; ...
                      'Tom3',          'Tom3',            0,   0,   0,  0, 0; ...
                      'Tom4',          'Tom4',            0,   0,   0,  0, 0};
midi_map           = {'crash1',          55; ...
                      'hihat_closed1',   22; ...
                      'hihat_open',      26; ...
                      'kick_l',          36; ...
                      'ride',            51; ...
                      'ride_bell1',      66; ...
                      'snare_on_center', 38; ...
                      'snare_rim_shot',  40; ...
                      'tom_1',           48; ...
                      'tom_2',           45};
else
kit_path           = 'CrocellKit/';
channel_properties = {'AmbLeft',      'AmbLeft',        0, -90,   0, 0, 0; ...
                      'AmbRight',     'AmbRight',     -90,   0,   0, 0, 0; ...
                      'OHLeft',       'OHLeft',         0, -90,   0, 0, 0; ...
                      'OHRight',      'OHRight',      -90,   0,   0, 0, 0; ...
                      'OHCenter',     'OHCenter',       0,   0,   0, 0, 0; ...
                      'Hihat',        'Hihat',          0,   0,   0, 0, 0; ...
                      'Ride',         'Ride',           0,   0,   0, 0, 0; ...
                      'SnareTop',     'SnareTop',       0,   0,   0, 0, 0; ...
                      'SnareBottom',  'SnareBottom',   -3,  -3,   0, 0, 0; ...
                      'Tom1',         'Tom1',           0,   0,   0, 0, 0; ...
                      'Tom2',         'Tom2',           0,   0,   0, 0, 0; ...
                      'FTom1',        'FTom1',          0,   0,   0, 0, 0; ...
                      'FTom2',        'FTom2',          0,   0,   0, 0, 0; ...
                      'KDrumInside',  'KDrumInside',    0,   0,   0, 0, 0; ...
                      'KDrumOutside', 'KDrumOutside',  -3,  -3,   0, 0, 0};
midi_map           = {'CrashL',       55; ...
                      'HihatClosed',  22; ...
                      'HihatOpen',    26; ...
                      'KDrumL',       36; ...
                      'RideR',        51; ...
                      'RideRBell',    66; ...
                      'Snare',        38; ...
                      'SnareRimShot', 40; ...
                      'Tom1',         48; ...
                      'Tom2',         45};
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
  fwrite(file_id, [xml_file{line_index} char(10)]);
end
fclose(file_id);

% write MIDI map
file_id = fopen([out_kit_path midi_map_name '.xml'], 'w');
fwrite(file_id, ['<?xml version="1.0" encoding="UTF-8"?>' char(10)]);
fwrite(file_id, ['<midimap>' char(10)]);
for midi_map_index = 1:size(midi_map, 1)
  fwrite(file_id, ['  <map note="' num2str(midi_map{midi_map_index, 2}) '" instr="' midi_map{midi_map_index, 1} '"/>' char(10)]); 
end
fwrite(file_id, ['</midimap>' char(10)]);
fclose(file_id);

% loop over all instruments
for instrument_index = 1:length(instr_dir)

  is_valid_instrument = instr_dir(instrument_index).isdir && (length(instr_dir(instrument_index).name) > 2);
  is_instrument_used  = isempty(instrument_select) || strcmp(instr_dir(instrument_index).name, instrument_select);

  % exclude invalid directories
  is_instrument_used = is_instrument_used && ~strcmp(instr_dir(instrument_index).name, '_pictures');


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

        % equalizer
        for channel_index = 1:size(x_all, 2)

          eq_low  = cell2mat(channel_properties(channel_index, 5));
          eq_mid  = cell2mat(channel_properties(channel_index, 6));
          eq_high = cell2mat(channel_properties(channel_index, 7));

          % low filter
          if eq_low ~= 0
            if eq_low > 0
              [b, a] = butter(3, [50, 200] / sampling_rate * 2);
            else
              [b, a] = butter(3, [50, 200] / sampling_rate * 2, 'stop');
            end
            x_all(:, channel_index) = x_all(:, channel_index) + 10 ^ (eq_low / 20) * ...
              filter(b, a, x_all(:, channel_index));
          end

          % mid filter
          if eq_mid ~= 0
            if eq_mid > 0
              [b, a] = butter(3, [1000, 2500] / sampling_rate * 2);
            else
              [b, a] = butter(3, [1000, 2500] / sampling_rate * 2, 'stop');
            end
            x_all(:, channel_index) = x_all(:, channel_index) + 10 ^ (eq_mid / 20) * ...
              filter(b, a, x_all(:, channel_index));
          end

          % high filter
          if eq_high ~= 0
            if eq_high > 0
              [b, a] = butter(3, [3000, 9000] / sampling_rate * 2);
            else
              [b, a] = butter(3, [3000, 9000] / sampling_rate * 2, 'stop');
            end
            x_all(:, channel_index) = x_all(:, channel_index) + 10 ^ (eq_high / 20) * ...
              filter(b, a, x_all(:, channel_index));
          end

        end

        % mix channels
        mix_matrix = 10 .^ (cell2mat(channel_properties(:, 3:4)) / 20);
        x_left     = x_all * mix_matrix(:, 1);
        x_right    = x_all * mix_matrix(:, 2);
        x          = [x_left, x_right];


% TEST
% play the resulting wave form
if ~isempty(instrument_select) && (sample_index == 4)
  player = audioplayer(x, sampling_rate, 16);
  play(player);
  pause(1);
end


        % store the resulting wave file
        mkdir([out_kit_path file_path samples_path]);
        audiowrite([out_kit_path file_path samples_path mixed_prefix file_name '.wav'], x, sampling_rate);

      end

    end

  end

end
