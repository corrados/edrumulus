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

function edrumuluscontrol

global GUI;

close all;
pkg load audio

figure_handle = figure;
old_figure_position = get(figure_handle, 'Position');
set(figure_handle, 'Position', [old_figure_position(1), old_figure_position(2), 1500, 600]);
slider_width  = 0.1;
slider_hight  = 0.7;
value_hight   = 0.2;

% MIDI device selection combo box
GUI.midi_dev_list = uicontrol(figure_handle, ...
  'style',    'listbox', ...
  'units',    'normalized', ...
  'position', [0, 0.8, 0.4, 0.2], ...
  'callback', @midi_sel_callback);

midi_devices   = mididevinfo;
midi_in_names  = {};
midi_out_names = {};
for i = 1:length(midi_devices.input)
  midi_in_names = [midi_in_names, midi_devices.input{i}.Name];
end
for i = 1:length(midi_devices.output)
  midi_out_names = [midi_out_names, midi_devices.output{i}.Name];
end
set(GUI.midi_dev_list, 'string', midi_out_names);
GUI.midi_dev = [];

% default settings button
GUI.set_but = uicontrol(figure_handle, ...
  'style',    'pushbutton', ... 
  'string',   'Default Settings', ...
  'units',    'normalized', ...
  'position', [0.7, 0.9, 0.3, 0.1], ...
  'callback', @button_callback);

% spike cancellation checkbox
GUI.spike_chbx = uicontrol(figure_handle, ...
  'style',    'checkbox', ...
  'value',    1, ... % is on per default on the ESP32
  'string',   'Spike Cancellation', ...
  'units',    'normalized', ...
  'position', [0.7, 0.75, 0.3, 0.1], ...
  'callback', @checkbox_callback);

% settings panel
GUI.set_panel = uipanel(figure_handle, ...
  'Title',    'Edrumulus settings', ...
  'Position', [0 0 1 0.6]);

% first slider control with text
uicontrol(GUI.set_panel, ...
  'style',    'text', ...
  'string',   '1:Pad Type', ...
  'units',    'normalized', ...
  'position', [0, slider_hight + value_hight, slider_width, 0.1]);

GUI.val1 = uicontrol(GUI.set_panel, ...
  'style',    'edit', ...
  'units',    'normalized', ...
  'position', [0, slider_hight, slider_width, 0.2], ...
  'Enable',   'off');

GUI.slider1 = uicontrol(GUI.set_panel, ...
  'style',      'slider', ...
  'min',        0, ...
  'max',        9, ...              % change value here if new pad type was added
  'SliderStep', [1 / 9, 1 / 9], ... % change value here if new pad type was added
  'units',      'normalized', ...
  'position',   [0, 0, slider_width, slider_hight], ...
  'callback',   @slider_callback);

% second slider control with text
uicontrol(GUI.set_panel, ...
  'style',    'text', ...
  'string',   '2:Threshold', ...
  'units',    'normalized', ...
  'position', [slider_width, slider_hight + value_hight, slider_width, 0.1]);

GUI.val2 = uicontrol(GUI.set_panel, ...
  'style',    'edit', ...
  'units',    'normalized', ...
  'position', [slider_width, slider_hight, slider_width, 0.2], ...
  'Enable',   'off');

GUI.slider2 = uicontrol(GUI.set_panel, ...
  'style',      'slider', ...
  'units',      'normalized', ...
  'min',        0, ...
  'max',        31, ...
  'SliderStep', [1 / 31, 1 / 31], ...
  'position',   [slider_width, 0, slider_width, slider_hight], ...
  'callback',   @slider_callback);

% third slider control with text
uicontrol(GUI.set_panel, ...
  'style',    'text', ...
  'string',   '3:Sensitivity', ...
  'units',    'normalized', ...
  'position', [2 * slider_width, slider_hight + value_hight, slider_width, 0.1]);

GUI.val3 = uicontrol(GUI.set_panel, ...
  'style',    'edit', ...
  'units',    'normalized', ...
  'position', [2 * slider_width, slider_hight, slider_width, 0.2], ...
  'Enable',   'off');

GUI.slider3 = uicontrol(GUI.set_panel, ...
  'style',      'slider', ...
  'min',        0, ...
  'max',        31, ...
  'SliderStep', [1 / 31, 1 / 31], ...
  'units',      'normalized', ...
  'position',   [2 * slider_width, 0, slider_width, slider_hight], ...
  'callback',   @slider_callback);

% fourth slider control with text
uicontrol(GUI.set_panel, ...
  'style',    'text', ...
  'string',   '4:Pos Threshold', ...
  'units',    'normalized', ...
  'position', [3 * slider_width, slider_hight + value_hight, slider_width, 0.1]);

GUI.val4 = uicontrol(GUI.set_panel, ...
  'style',    'edit', ...
  'units',    'normalized', ...
  'position', [3 * slider_width, slider_hight, slider_width, 0.2], ...
  'Enable',   'off');

GUI.slider4 = uicontrol(GUI.set_panel, ...
  'style',      'slider', ...
  'units',      'normalized', ...
  'min',        0, ...
  'max',        31, ...
  'SliderStep', [1 / 31, 1 / 31], ...
  'position',   [3 * slider_width, 0, slider_width, slider_hight], ...
  'callback',   @slider_callback);

% fifth slider control with text
uicontrol(GUI.set_panel, ...
  'style',    'text', ...
  'string',   '5:Pos Sensitivity', ...
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
  'string',   '6:Rim Shot Threshold', ...
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
  'string',   '7:MIDI Curve', ...
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
  'max',        4, ...
  'SliderStep', [1 / 4, 1 / 4], ...
  'position',   [6 * slider_width, 0, slider_width, slider_hight], ...
  'callback',   @slider_callback);

% eigth slider control with text
uicontrol(GUI.set_panel, ...
  'style',    'text', ...
  'string',   '8:Pad Select', ...
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
  'max',        11, ...
  'SliderStep', [1 / 11, 1 / 11], ...
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


% TEST
midi_in_dev = mididevice("input", 3);

while ishandle(figure_handle)
  midi_message = midireceive(midi_in_dev, 1);
  if ~isempty(midi_message) && (midi_message.type == midimsgtype.NoteOff) && ...
      (midi_message.channel == 1)

    if midi_message.note == 102
      set_slieder_value(GUI.slider1, midi_message.velocity, false);
    elseif midi_message.note == 103
      set_slieder_value(GUI.slider2, midi_message.velocity, false);
    elseif midi_message.note == 104
      set_slieder_value(GUI.slider3, midi_message.velocity, false);
    elseif midi_message.note == 105
      set_slieder_value(GUI.slider4, midi_message.velocity, false);
    elseif midi_message.note == 106
      set_slieder_value(GUI.slider5, midi_message.velocity, false);
    elseif midi_message.note == 107
      set_slieder_value(GUI.slider6, midi_message.velocity, false);
    elseif midi_message.note == 109
      set_slieder_value(GUI.slider7, midi_message.velocity, false);
    elseif midi_message.note == 110
      set(GUI.spike_chbx, 'value', midi_message.velocity);
    elseif midi_message.note == 114
      set_slieder_value(GUI.slider9, midi_message.velocity, false);
    end

  end
  pause(0.01);
end



end


function midi_sel_callback(hObject)

global GUI;
list_entries = get(hObject, 'string');
GUI.midi_dev = mididevice("output", list_entries{get(hObject, 'value')});

end


function reset_sliders

global GUI;
set(GUI.slider2, 'value', 0); set(GUI.val2, 'string', 'Not Set');
set(GUI.slider3, 'value', 0); set(GUI.val3, 'string', 'Not Set');
set(GUI.slider4, 'value', 0); set(GUI.val4, 'string', 'Not Set');
set(GUI.slider5, 'value', 0); set(GUI.val5, 'string', 'Not Set');
set(GUI.slider6, 'value', 0); set(GUI.val6, 'string', 'Not Set');
set(GUI.slider7, 'value', 0); set(GUI.val7, 'string', 'Not Set');
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
    switch value
      case 0
        set(GUI.val1, 'string', 'PD120');
      case 1
        set(GUI.val1, 'string', 'PD80R');
      case 2
        set(GUI.val1, 'string', 'PD8');
      case 3
        set(GUI.val1, 'string', 'FD8');
      case 4
        set(GUI.val1, 'string', 'VH12');
      case 5
        set(GUI.val1, 'string', 'VH12CTRL');
      case 6
        set(GUI.val1, 'string', 'KD7');
      case 7
        set(GUI.val1, 'string', 'TP80');
      case 8
        set(GUI.val1, 'string', 'CY6');
      case 9
        set(GUI.val1, 'string', 'CY8');
    end
    if do_send_midi
      midisend(GUI.midi_dev, midimsg("controlchange", 10, 102, value));
      reset_sliders; % if a pad type is changed, all parameters are reset in the ESP32
    end

  case GUI.slider2
    set(GUI.val2, 'string', num2str(value));
    if do_send_midi
      midisend(GUI.midi_dev, midimsg("controlchange", 10, 103, value));
    end

  case GUI.slider3
    set(GUI.val3, 'string', num2str(value));
    if do_send_midi
      midisend(GUI.midi_dev, midimsg("controlchange", 10, 104, value));
    end

  case GUI.slider4
    set(GUI.val4, 'string', num2str(value));
    if do_send_midi
      midisend(GUI.midi_dev, midimsg("controlchange", 10, 105, value));
    end

  case GUI.slider5
    set(GUI.val5, 'string', num2str(value));
    if do_send_midi
      midisend(GUI.midi_dev, midimsg("controlchange", 10, 106, value));
    end

  case GUI.slider6
    set(GUI.val6, 'string', num2str(value));
    if do_send_midi
      midisend(GUI.midi_dev, midimsg("controlchange", 10, 107, value));
    end

  case GUI.slider7
    switch value
      case 0
        set(GUI.val7, 'string', 'LINEAR');
      case 1
        set(GUI.val7, 'string', 'EXP1');
      case 2
        set(GUI.val7, 'string', 'EXP2');
      case 3
        set(GUI.val7, 'string', 'LOG1');
      case 4
        set(GUI.val7, 'string', 'LOG2');
    end
    if do_send_midi
      midisend(GUI.midi_dev, midimsg("controlchange", 10, 109, value));
    end

  case GUI.slider8
    set(GUI.val8, 'string', num2str(value));
    if do_send_midi
      midisend(GUI.midi_dev, midimsg("controlchange", 10, 108, value));
      reset_sliders; % on a pad change we do not know the current parameters
    end

  case GUI.slider9
    set(GUI.val9, 'string', num2str(value));
    if do_send_midi
      midisend(GUI.midi_dev, midimsg("controlchange", 10, 114, value));
    end
end

end


function button_callback(hObject)

global GUI;

% snare
midisend(GUI.midi_dev, midimsg("controlchange", 10, 108, 0)); % pad 0
midisend(GUI.midi_dev, midimsg("controlchange", 10, 102, 2)); % PD8
midisend(GUI.midi_dev, midimsg("controlchange", 10, 103, 3)); % threshold
midisend(GUI.midi_dev, midimsg("controlchange", 10, 104, 8)); % sensitivity
midisend(GUI.midi_dev, midimsg("controlchange", 10, 107, 16)); % rim shot threshold
midisend(GUI.midi_dev, midimsg("controlchange", 10, 105, 26)); % positional sensing threshold
midisend(GUI.midi_dev, midimsg("controlchange", 10, 106, 11)); % positional sensing sensitivity
midisend(GUI.midi_dev, midimsg("controlchange", 10, 111, 3)); % both, rim shot and positional sensing

% kick
midisend(GUI.midi_dev, midimsg("controlchange", 10, 108, 1)); % pad 1
midisend(GUI.midi_dev, midimsg("controlchange", 10, 102, 6)); % KD7
midisend(GUI.midi_dev, midimsg("controlchange", 10, 103, 9)); % threshold
midisend(GUI.midi_dev, midimsg("controlchange", 10, 104, 9)); % sensitivity

% Hi-Hat
midisend(GUI.midi_dev, midimsg("controlchange", 10, 108, 2)); % pad 2
midisend(GUI.midi_dev, midimsg("controlchange", 10, 102, 2)); % PD8
midisend(GUI.midi_dev, midimsg("controlchange", 10, 103, 4)); % threshold
midisend(GUI.midi_dev, midimsg("controlchange", 10, 104, 8)); % sensitivity
midisend(GUI.midi_dev, midimsg("controlchange", 10, 111, 1)); % enable rim shot

% Hi-Hat control
midisend(GUI.midi_dev, midimsg("controlchange", 10, 108, 3)); % pad 3
midisend(GUI.midi_dev, midimsg("controlchange", 10, 102, 3)); % FD8
midisend(GUI.midi_dev, midimsg("controlchange", 10, 103, 5)); % threshold
midisend(GUI.midi_dev, midimsg("controlchange", 10, 104, 0)); % sensitivity

% crash
midisend(GUI.midi_dev, midimsg("controlchange", 10, 108, 4)); % pad 4
midisend(GUI.midi_dev, midimsg("controlchange", 10, 102, 8)); % CY6
midisend(GUI.midi_dev, midimsg("controlchange", 10, 103, 19)); % threshold
midisend(GUI.midi_dev, midimsg("controlchange", 10, 104, 21)); % sensitivity
midisend(GUI.midi_dev, midimsg("controlchange", 10, 111, 1)); % enable rim shot

% tom 1
midisend(GUI.midi_dev, midimsg("controlchange", 10, 108, 5)); % pad 5
midisend(GUI.midi_dev, midimsg("controlchange", 10, 102, 1)); % PD80R
midisend(GUI.midi_dev, midimsg("controlchange", 10, 103, 9)); % threshold
midisend(GUI.midi_dev, midimsg("controlchange", 10, 104, 0)); % sensitivity

% ride
midisend(GUI.midi_dev, midimsg("controlchange", 10, 108, 6)); % pad 6
midisend(GUI.midi_dev, midimsg("controlchange", 10, 102, 2)); % PD8
midisend(GUI.midi_dev, midimsg("controlchange", 10, 103, 18)); % threshold
midisend(GUI.midi_dev, midimsg("controlchange", 10, 104, 21)); % sensitivity
midisend(GUI.midi_dev, midimsg("controlchange", 10, 111, 1)); % enable rim shot

% tom 2
midisend(GUI.midi_dev, midimsg("controlchange", 10, 108, 7)); % pad 7
midisend(GUI.midi_dev, midimsg("controlchange", 10, 102, 1)); % PD80R
midisend(GUI.midi_dev, midimsg("controlchange", 10, 103, 18)); % threshold
midisend(GUI.midi_dev, midimsg("controlchange", 10, 104, 0)); % sensitivity

% cleanup GUI
midisend(GUI.midi_dev, midimsg("controlchange", 10, 108, 0)); % pad 0
reset_sliders;

end


function checkbox_callback(hObject)

global GUI;

% spike cancellation checkbox
midisend(GUI.midi_dev, midimsg("controlchange", 10, 110, get(hObject, 'value')));

end


