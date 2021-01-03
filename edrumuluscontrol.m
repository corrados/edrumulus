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

close all;
figure_handle = figure;

GUI.set_panel = uipanel( ...
  'Title',    'Edrumulus settings', ...
  'Position', [0 0 1 .6]);

GUI.slider1 = uicontrol(GUI.set_panel, ...
  'style',    'slider', ...
  'units',    'normalized', ...
  'position', [0, 0, 0.1, 0.8], ...
  'callback', @slider_callback);

GUI.val1 = uicontrol(GUI.set_panel, ...
  'style',    'edit', ...
  'units',    'normalized', ...
  'position', [0, 0.8, 0.1, 0.2], ...
  'Enable',   'off', ...
  'callback', @slider_callback);

GUI.slider1 = uicontrol(GUI.set_panel, ...
  'style',    'slider', ...
  'units',    'normalized', ...
  'position', [0.2, 0, 0.1, 0.8], ...
  'callback', @slider_callback);


  function slider_callback(hObject)

    num = get(hObject, 'Value');
    disp(num);
    %GUI.val1.Value = num;%num2str(num);

  end

end


