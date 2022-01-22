%*******************************************************************************
% Copyright (c) 2020-2022
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

function edrumuluscontrol

global GUI;

close all;
pkg load audio

figure_handle = figure;
old_figure_position = get(figure_handle, 'Position');
set(figure_handle, 'Position', [old_figure_position(1), old_figure_position(2), 1500, 600]);
slider_width = 0.1;
slider_hight = 0.7;
value_hight  = 0.2;

% MIDI device selection combo boxes
GUI.midi_out_dev_list = uicontrol(figure_handle, ...
  'style',    'listbox', ...
  'units',    'normalized', ...
  'position', [0.1, 0.8, 0.4, 0.2], ...
  'callback', @midi_out_sel_callback);
uicontrol(figure_handle, ...
  'style',    'text', ...
  'string',   'MIDI out:', ...
  'units',    'normalized', ...
  'position', [0, 0.8, 0.1, 0.2]);
GUI.midi_in_dev_list = uicontrol(figure_handle, ...
  'style',    'listbox', ...
  'units',    'normalized', ...
  'position', [0.1, 0.6, 0.4, 0.2], ...
  'callback', @midi_in_sel_callback);
uicontrol(figure_handle, ...
  'style',    'text', ...
  'string',   'MIDI in:', ...
  'units',    'normalized', ...
  'position', [0, 0.6, 0.1, 0.2]);

midi_devices        = mididevinfo;
midi_in_names       = {};
midi_out_names      = {};
edrumulus_in_index  = [];
edrumulus_out_index = [];
for i = 1:length(midi_devices.input)
  midi_in_names = [midi_in_names, midi_devices.input{i}.Name];
  if ~isempty(strfind(midi_devices.input{i}.Name, 'Edrumulus')) || ...
      ~isempty(strfind(midi_devices.input{i}.Name, 'ttymidi'))
    edrumulus_in_index = i;
  end
end
for i = 1:length(midi_devices.output)
  midi_out_names = [midi_out_names, midi_devices.output{i}.Name];
  if ~isempty(strfind(midi_devices.output{i}.Name, 'Edrumulus')) || ...
      ~isempty(strfind(midi_devices.output{i}.Name, 'ttymidi'))
    edrumulus_out_index = i;
  end
end
set(GUI.midi_out_dev_list, 'string', midi_out_names);
set(GUI.midi_in_dev_list,  'string', midi_in_names);
GUI.midi_out_dev = [];
GUI.midi_in_dev  = [];

% default settings button
GUI.set_but = uicontrol(figure_handle, ...
  'style',    'pushbutton', ... 
  'string',   'Reset All Settings', ...
  'units',    'normalized', ...
  'position', [0.7, 0.9, 0.3, 0.1], ...
  'callback', @button_callback);

% spike cancellation dropdown
GUI.spike_dropdown = uicontrol(figure_handle, ...
  'style',    'popupmenu', ...
  'value',    1, ... % is on per default on the ESP32
  'string',   {'Off', 'Level 1', 'Level 2', 'Level 3', 'Level 4'}, ...
  'units',    'normalized', ...
  'position', [0.7, 0.75, 0.3, 0.1], ...
  'callback', @popupmenu_callback);

% auto pad select checkbox
GUI.autopad      = false;
GUI.autopad_chbx = uicontrol(figure_handle, ...
  'style',    'checkbox', ...
  'value',    0, ...
  'string',   'Auto Pad Select', ...
  'units',    'normalized', ...
  'position', [0.7, 0.65, 0.3, 0.1], ...
  'callback', @checkbox_callback);

% settings panel
GUI.set_panel = uipanel(figure_handle, ...
  'Title',    'Edrumulus settings', ...
  'Position', [0 0 1 0.6]);

% first slider control with text
uicontrol(GUI.set_panel, ...
  'style',    'text', ...
  'string',   '1:Pad Select', ...
  'units',    'normalized', ...
  'position', [0 * slider_width, slider_hight + value_hight, slider_width, 0.1]);

GUI.val1 = uicontrol(GUI.set_panel, ...
  'style',    'edit', ...
  'units',    'normalized', ...
  'position', [0 * slider_width, slider_hight, slider_width, 0.2], ...
  'Enable',   'off');

GUI.slider1 = uicontrol(GUI.set_panel, ...
  'style',      'slider', ...
  'units',      'normalized', ...
  'min',        0, ...
  'max',        11, ...
  'SliderStep', [1 / 11, 1 / 11], ...
  'position',   [0 * slider_width, 0, slider_width, slider_hight], ...
  'callback',   @slider_callback);

% second slider control with text
uicontrol(GUI.set_panel, ...
  'style',    'text', ...
  'string',   '2:Pad Type', ...
  'units',    'normalized', ...
  'position', [1 * slider_width, slider_hight + value_hight, slider_width, 0.1]);

GUI.val2 = uicontrol(GUI.set_panel, ...
  'style',    'edit', ...
  'units',    'normalized', ...
  'position', [1 * slider_width, slider_hight, slider_width, 0.2], ...
  'Enable',   'off');

GUI.slider2 = uicontrol(GUI.set_panel, ...
  'style',      'slider', ...
  'min',        0, ...
  'max',        17, ...               % change value here if new pad type was added
  'SliderStep', [1 / 17, 1 / 17], ... % change value here if new pad type was added
  'units',      'normalized', ...
  'position',   [1 * slider_width, 0, slider_width, slider_hight], ...
  'callback',   @slider_callback);

% third slider control with text
uicontrol(GUI.set_panel, ...
  'style',    'text', ...
  'string',   '3:Threshold', ...
  'units',    'normalized', ...
  'position', [2 * slider_width, slider_hight + value_hight, slider_width, 0.1]);

GUI.val3 = uicontrol(GUI.set_panel, ...
  'style',    'edit', ...
  'units',    'normalized', ...
  'position', [2 * slider_width, slider_hight, slider_width, 0.2], ...
  'Enable',   'off');

GUI.slider3 = uicontrol(GUI.set_panel, ...
  'style',      'slider', ...
  'units',      'normalized', ...
  'min',        0, ...
  'max',        31, ...
  'SliderStep', [1 / 31, 1 / 31], ...
  'position',   [2 * slider_width, 0, slider_width, slider_hight], ...
  'callback',   @slider_callback);

% fourth slider control with text
uicontrol(GUI.set_panel, ...
  'style',    'text', ...
  'string',   '4:Sensitivity', ...
  'units',    'normalized', ...
  'position', [3 * slider_width, slider_hight + value_hight, slider_width, 0.1]);

GUI.val4 = uicontrol(GUI.set_panel, ...
  'style',    'edit', ...
  'units',    'normalized', ...
  'position', [3 * slider_width, slider_hight, slider_width, 0.2], ...
  'Enable',   'off');

GUI.slider4 = uicontrol(GUI.set_panel, ...
  'style',      'slider', ...
  'min',        0, ...
  'max',        31, ...
  'SliderStep', [1 / 31, 1 / 31], ...
  'units',      'normalized', ...
  'position',   [3 * slider_width, 0, slider_width, slider_hight], ...
  'callback',   @slider_callback);

% fifth slider control with text
uicontrol(GUI.set_panel, ...
  'style',    'text', ...
  'string',   '5:Pos Threshold', ...
  'units',    'normalized', ...
  'position', [4 * slider_width, slider_hight + value_hight, slider_width, 0.1]);

GUI.val5 = uicontrol(GUI.set_panel, ...
  'style',    'edit', ...
  'units',    'normalized', ...
  'position', [4 * slider_width, slider_hight, slider_width, 0.2], ...
  'Enable',   'off');

GUI.slider5 = uicontrol(GUI.set_panel, ...
  'style',      'slider', ...
  'units',      'normalized', ...
  'min',        0, ...
  'max',        31, ...
  'SliderStep', [1 / 31, 1 / 31], ...
  'position',   [4 * slider_width, 0, slider_width, slider_hight], ...
  'callback',   @slider_callback);

% sixth slider control with text
uicontrol(GUI.set_panel, ...
  'style',    'text', ...
  'string',   '6:Pos Sensitivity', ...
  'units',    'normalized', ...
  'position', [5 * slider_width, slider_hight + value_hight, slider_width, 0.1]);

GUI.val6 = uicontrol(GUI.set_panel, ...
  'style',    'edit', ...
  'units',    'normalized', ...
  'position', [5 * slider_width, slider_hight, slider_width, 0.2], ...
  'Enable',   'off');

GUI.slider6 = uicontrol(GUI.set_panel, ...
  'style',      'slider', ...
  'units',      'normalized', ...
  'min',        0, ...
  'max',        31, ...
  'SliderStep', [1 / 31, 1 / 31], ...
  'position',   [5 * slider_width, 0, slider_width, slider_hight], ...
  'callback',   @slider_callback);

% seventh slider control with text
uicontrol(GUI.set_panel, ...
  'style',    'text', ...
  'string',   '7:Rim Shot Threshold', ...
  'units',    'normalized', ...
  'position', [6 * slider_width, slider_hight + value_hight, slider_width, 0.1]);

GUI.val7 = uicontrol(GUI.set_panel, ...
  'style',    'edit', ...
  'units',    'normalized', ...
  'position', [6 * slider_width, slider_hight, slider_width, 0.2], ...
  'Enable',   'off');

GUI.slider7 = uicontrol(GUI.set_panel, ...
  'style',      'slider', ...
  'units',      'normalized', ...
  'min',        0, ...
  'max',        31, ...
  'SliderStep', [1 / 31, 1 / 31], ...
  'position',   [6 * slider_width, 0, slider_width, slider_hight], ...
  'callback',   @slider_callback);

% eigth slider control with text
uicontrol(GUI.set_panel, ...
  'style',    'text', ...
  'string',   '8:MIDI Curve', ...
  'units',    'normalized', ...
  'position', [7 * slider_width, slider_hight + value_hight, slider_width, 0.1]);

GUI.val8 = uicontrol(GUI.set_panel, ...
  'style',    'edit', ...
  'units',    'normalized', ...
  'position', [7 * slider_width, slider_hight, slider_width, 0.2], ...
  'Enable',   'off');

GUI.slider8 = uicontrol(GUI.set_panel, ...
  'style',      'slider', ...
  'units',      'normalized', ...
  'min',        0, ...
  'max',        4, ...
  'SliderStep', [1 / 4, 1 / 4], ...
  'position',   [7 * slider_width, 0, slider_width, slider_hight], ...
  'callback',   @slider_callback);

% ninth slider control with text
uicontrol(GUI.set_panel, ...
  'style',    'text', ...
  'string',   '9:Crosstalk Cancel', ...
  'units',    'normalized', ...
  'position', [8 * slider_width, slider_hight + value_hight, slider_width, 0.1]);

GUI.val9 = uicontrol(GUI.set_panel, ...
  'style',    'edit', ...
  'units',    'normalized', ...
  'position', [8 * slider_width, slider_hight, slider_width, 0.2], ...
  'Enable',   'off');

GUI.slider9 = uicontrol(GUI.set_panel, ...
  'style',      'slider', ...
  'units',      'normalized', ...
  'min',        0, ...
  'max',        31, ...
  'SliderStep', [1 / 31, 1 / 31], ...
  'position',   [8 * slider_width, 0, slider_width, slider_hight], ...
  'callback',   @slider_callback);

reset_sliders;

% if Edrumulus devices for input and output were found, select them
if ~isempty(edrumulus_in_index) && ~isempty(edrumulus_out_index)
  set(GUI.midi_out_dev_list, 'value', edrumulus_out_index);
  set(GUI.midi_in_dev_list,  'value', edrumulus_in_index);
  GUI.midi_out_dev = mididevice("output", midi_devices.output{edrumulus_out_index}.Name);
  GUI.midi_in_dev  = mididevice("input",  midi_devices.input{edrumulus_in_index}.Name);

  % let Edrumulus know which pad is currently selected (to retrieve parameters)
  midisend(GUI.midi_out_dev, midimsg("controlchange", 10, 108, 0)); % pad 0
end

% parse MIDI input to receive pad parameters and apply them to the GUI controls
version_major = -1;
version_minor = -1;
while ishandle(figure_handle)
  midi_message = midireceive(GUI.midi_in_dev, 1);
  if ~isempty(midi_message) && (midi_message.type == midimsgtype.NoteOff) && ...
      (midi_message.channel == 1)

    if midi_message.note == 102
      set_slieder_value(GUI.slider2, midi_message.velocity, false);
    elseif midi_message.note == 103
      set_slieder_value(GUI.slider3, midi_message.velocity, false);
    elseif midi_message.note == 104
      set_slieder_value(GUI.slider4, midi_message.velocity, false);
    elseif midi_message.note == 105
      set_slieder_value(GUI.slider5, midi_message.velocity, false);
    elseif midi_message.note == 106
      set_slieder_value(GUI.slider6, midi_message.velocity, false);
    elseif midi_message.note == 107
      set_slieder_value(GUI.slider7, midi_message.velocity, false);
    elseif midi_message.note == 109
      set_slieder_value(GUI.slider8, midi_message.velocity, false);
    elseif midi_message.note == 110
      set(GUI.spike_dropdown, 'value', midi_message.velocity + 1);
    elseif midi_message.note == 114
      set_slieder_value(GUI.slider9, midi_message.velocity, false);
    elseif midi_message.note == 126
      version_minor = midi_message.velocity;
    elseif midi_message.note == 127
      version_major = midi_message.velocity;
    end

  end

  % auto pad selection
  if GUI.autopad

    if ~isempty(midi_message) && (midi_message.type == midimsgtype.NoteOn) && ...
        (midi_message.channel == 10)

      update_pad_selection(midi_message, 38, 40, 0) % snare
      update_pad_selection(midi_message, 36, 36, 1) % kick
      update_pad_selection(midi_message, 22, 26, 2) % hi-hat
      update_pad_selection(midi_message, 49, 55, 4) % crash
      update_pad_selection(midi_message, 48, 50, 5) % tom1

    end

  end

  % if version number is available, set the window title
  if (version_major >= 0) && (version_minor >= 0) && isempty(strfind(get(figure_handle, 'Name'), 'Edrumulus'))
    set(figure_handle, 'Name', ['Edrumulus Version ' num2str(version_major) '.' num2str(version_minor)], 'NumberTitle', 'off');
  end

  pause(0.01); % do not block the CPU all the time
end

end


function update_pad_selection(midi_message, midi_note1, midi_note2, pad_index)

global GUI;
if ((midi_message.note == midi_note1) || (midi_message.note == midi_note2)) && ...
    (midi_message.velocity > 10) && ... % introduce velocity limit until we switch the pad
    (round(get(GUI.slider1, 'value')) ~= pad_index)

  set(GUI.slider1, 'value', pad_index);
  set_slieder_value(GUI.slider1, pad_index, true)

end

end

function midi_out_sel_callback(hObject)

global GUI;
list_entries = get(hObject, 'string');
GUI.midi_out_dev = mididevice("output", list_entries{get(hObject, 'value')});

end


function midi_in_sel_callback(hObject)

global GUI;
list_entries = get(hObject, 'string');
GUI.midi_in_dev = mididevice("input", list_entries{get(hObject, 'value')});

end


function reset_sliders

global GUI;
set(GUI.slider3, 'value', 0); set(GUI.val3, 'string', 'Not Set');
set(GUI.slider4, 'value', 0); set(GUI.val4, 'string', 'Not Set');
set(GUI.slider5, 'value', 0); set(GUI.val5, 'string', 'Not Set');
set(GUI.slider6, 'value', 0); set(GUI.val6, 'string', 'Not Set');
set(GUI.slider7, 'value', 0); set(GUI.val7, 'string', 'Not Set');
set(GUI.slider8, 'value', 0); set(GUI.val8, 'string', 'Not Set');
set(GUI.slider9, 'value', 0); set(GUI.val9, 'string', 'Not Set');

end


function slider_callback(hObject)
set_slieder_value(hObject, round(get(hObject, 'value')), true)
end


function set_slieder_value(hObject, value, do_send_midi)

global GUI;

if ~do_send_midi
  set(hObject, 'value', value);
end

switch hObject
  case GUI.slider1
    set(GUI.val1, 'string', num2str(value));
    if do_send_midi
      midisend(GUI.midi_out_dev, midimsg("controlchange", 10, 108, value));
      reset_sliders; % on a pad change we do not know the current parameters
    end

  case GUI.slider2
    switch value
      case 0
        set(GUI.val2, 'string', 'PD120');
      case 1
        set(GUI.val2, 'string', 'PD80R');
      case 2
        set(GUI.val2, 'string', 'PD8');
      case 3
        set(GUI.val2, 'string', 'FD8');
      case 4
        set(GUI.val2, 'string', 'VH12');
      case 5
        set(GUI.val2, 'string', 'VH12CTRL');
      case 6
        set(GUI.val2, 'string', 'KD7');
      case 7
        set(GUI.val2, 'string', 'TP80');
      case 8
        set(GUI.val2, 'string', 'CY6');
      case 9
        set(GUI.val2, 'string', 'CY8');
      case 10
        set(GUI.val2, 'string', 'DIABOLO12');
      case 11
        set(GUI.val2, 'string', 'CY5');
      case 12
        set(GUI.val2, 'string', 'HD1TOM');
      case 13
        set(GUI.val2, 'string', 'PD6');
      case 14
        set(GUI.val2, 'string', 'KD8');
      case 15
        set(GUI.val2, 'string', 'PDX8');
      case 16
        set(GUI.val2, 'string', 'KD120');
      case 17
        set(GUI.val2, 'string', 'PD5');
    end
    if do_send_midi
      midisend(GUI.midi_out_dev, midimsg("controlchange", 10, 102, value));
      reset_sliders; % if a pad type is changed, all parameters are reset in the ESP32
    end

  case GUI.slider3
    set(GUI.val3, 'string', num2str(value));
    if do_send_midi
      midisend(GUI.midi_out_dev, midimsg("controlchange", 10, 103, value));
    end

  case GUI.slider4
    set(GUI.val4, 'string', num2str(value));
    if do_send_midi
      midisend(GUI.midi_out_dev, midimsg("controlchange", 10, 104, value));
    end

  case GUI.slider5
    set(GUI.val5, 'string', num2str(value));
    if do_send_midi
      midisend(GUI.midi_out_dev, midimsg("controlchange", 10, 105, value));
    end

  case GUI.slider6
    set(GUI.val6, 'string', num2str(value));
    if do_send_midi
      midisend(GUI.midi_out_dev, midimsg("controlchange", 10, 106, value));
    end

  case GUI.slider7
    set(GUI.val7, 'string', num2str(value));
    if do_send_midi
      midisend(GUI.midi_out_dev, midimsg("controlchange", 10, 107, value));
    end

  case GUI.slider8
    switch value
      case 0
        set(GUI.val8, 'string', 'LINEAR');
      case 1
        set(GUI.val8, 'string', 'EXP1');
      case 2
        set(GUI.val8, 'string', 'EXP2');
      case 3
        set(GUI.val8, 'string', 'LOG1');
      case 4
        set(GUI.val8, 'string', 'LOG2');
    end
    if do_send_midi
      midisend(GUI.midi_out_dev, midimsg("controlchange", 10, 109, value));
    end

  case GUI.slider9
    set(GUI.val9, 'string', num2str(value));
    if do_send_midi
      midisend(GUI.midi_out_dev, midimsg("controlchange", 10, 114, value));
    end
end

end


function button_callback(hObject)

global GUI;

selected_button = questdlg('Do you really want to reset all Edrumulus parameters to the default values?', ...
                           'Reset All Parameters', 'OK', 'Cancel', 'Cancel');

if selected_button == 'OK'
  midisend(GUI.midi_out_dev, midimsg("controlchange", 10, 115, 0)); % PRESET
  reset_sliders;
  midisend(GUI.midi_out_dev, midimsg("controlchange", 10, 108, 0)); % select pad 0
end

end


function popupmenu_callback(hObject)

global GUI;
switch hObject
  case GUI.spike_dropdown
    % spike cancellation dropdown
    midisend(GUI.midi_out_dev, midimsg("controlchange", 10, 110, get(hObject, 'value') - 1));
end

end


function checkbox_callback(hObject)

global GUI;
switch hObject
  case GUI.autopad_chbx
    % auto pad select
    GUI.autopad = get(hObject, 'value');
end

end


