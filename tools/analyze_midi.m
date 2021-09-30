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

pkg load audio

midi_channel_filter   = 10;
midi_note_filter      = [38, 40, 48, 50]; % snare/tom1
midi_pos_sense_filter = 16;
midi_device_name      = 'Edrumulus';% 'MidiLink Mini';% 


% Get Edrumulus input device ---------------------------------------------------
devinfo = mididevinfo;

edrumulus_in_device_index = 1;

for i = 1:length(devinfo.input)
  if strfind(devinfo.input{i}.Name, midi_device_name)
    edrumulus_in_device_index = i;
  end
end

edrumulus_in_device = mididevice(devinfo.input{edrumulus_in_device_index}.ID);


% Plot MIDI messages -----------------------------------------------------------
max_num_values       = 100;
midi_velocity_values = zeros(max_num_values, 1);
midi_control_values  = zeros(max_num_values, 1);

while true

  midi_message = midireceive(edrumulus_in_device, 1);

  if !isempty(midi_message)

    % apply MIDI filters for NoteOn message
    if (midi_message.type == midimsgtype.NoteOn) && ...
      (midi_message.channel == midi_channel_filter) && ...
      (any(midi_message.note == midi_note_filter))

      midi_velocity_values(1:max_num_values - 1) = midi_velocity_values(2:max_num_values);
      midi_velocity_values(max_num_values)       = midi_message.velocity;

      % display note value
      subplot(2, 1, 1), plot(midi_velocity_values, '*-');
      ax = axis; axis([ax(1), ax(2), 1, 127]); title('MIDI value');
      drawnow;

    end

    % apply MIDI filters for ControlChange message
    if (midi_message.type == midimsgtype.ControlChange) && ...
      (midi_message.channel == midi_channel_filter) && ...
      (midi_message.ccnumber == midi_pos_sense_filter)

      midi_control_values(1:max_num_values - 1) = midi_control_values(2:max_num_values);
      midi_control_values(max_num_values)       = midi_message.ccvalue;

      % display control value
      subplot(2, 1, 2), plot(midi_control_values, '*-');
      ax = axis; axis([ax(1), ax(2), 1, 127]); title('positional sensing');
      drawnow;

    end

  end

end
