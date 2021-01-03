%*******************************************************************************
% Copyright (c) 2020-2020
% Author: Volker Fischer
%*******************************************************************************
% Permission is hereby granted, free of charge, to any person obtaining a copy
% of this software and associated documentation files (the "Software"), to deal
% in the Software without restriction, including without limitation the rights
% to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
% copies of the Software, and to permit persons to whom the Software is
% furnished to do so, subject to the following conditions:
%
% The above copyright notice and this permission notice shall be included in all
% copies or substantial portions of the Software.
%
% THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
% IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
% FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
% AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
% LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
% OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
% SOFTWARE.
%*******************************************************************************

function edrumuluscontrol

global GUI;

close all;
pkg load audio

figure_handle = figure;
slider_width  = 0.15;
slider_hight  = 0.7;
value_hight   = 0.2;

% MIDI device selection combo box
GUI.midi_dev_list = uicontrol(figure_handle, ...
  'style',    'listbox', ...
  'units',    'normalized', ...
  'position', [0, 0.8, 0.4, 0.2], ...
  'callback', @midi_sel_callback);

midi_devices = mididevinfo;
for i = 1:length(midi_devices.output)
  set(GUI.midi_dev_list, 'string', midi_devices.output{i}.Name);
end
GUI.midi_dev = [];


GUI.set_panel = uipanel(figure_handle, ...
  'Title',    'Edrumulus settings', ...
  'Position', [0 0 1 0.6]);

% first slider control with text
uicontrol(GUI.set_panel, ...
  'style',    'text', ...
  'string',   'Threshold', ...
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
  'max',        31, ...
  'SliderStep', [1 / 31, 1 / 31], ...
  'units',      'normalized', ...
  'position',   [0, 0, slider_width, slider_hight], ...
  'callback',   @slider_callback);

% second slider control with text
uicontrol(GUI.set_panel, ...
  'style',    'text', ...
  'string',   'Sensitivity', ...
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

end


function midi_sel_callback(hObject)

global GUI;
GUI.midi_dev = mididevice("output", get(hObject, 'string'));

end


function slider_callback(hObject)

global GUI;

value = round(get(hObject, 'value'));

switch hObject
   case GUI.slider1
     set(GUI.val1, 'string', num2str(value));
     midisend(GUI.midi_dev, midimsg("controlchange", 1, 1, value));

   case GUI.slider2
     set(GUI.val2, 'string', num2str(value));
     midisend(GUI.midi_dev, midimsg("controlchange", 1, 2, value));
end

end
