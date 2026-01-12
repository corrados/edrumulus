%*******************************************************************************
% Copyright (c) 2020-2026
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

% capture samples which were recorded on the ESP32 device

close all
pkg load instrument-control
pkg load statistics


do_input_capture = false;


% #ifdef DO_INPUT_CAPTURE ------------------------------------------------------
if do_input_capture

  % prepare serial port
  try
    a = serialport("/dev/ttyUSB0", 500000); % note that we increased the transfer rate, now it is different from the default
  catch
  end
  flush(a);

  val = double(fread(a, 1 * 8000));
  marker_pos = find(val == 255);

  val = val(marker_pos(1):end);
  val(1 + marker_pos - marker_pos(1)) = [];
  if mod(length(val), 2) ~= 0
    val = val(1:end - 1);
  end
  x = val(1:2:end) * 255 + val(2:2:end);

  % the data seems to be corrupt, remove obviously incorrect data
  x(x > 2^12) = nan;

  figure; subplot(2, 1, 1), plot(x); grid on; title('raw linear sample data');
  axis([0, length(x), 0, 2^12]);

  subplot(2, 1, 2), plot(20 * log10(abs(x - nanmean(x)))); grid on; ylabel('dB'); title('power');

  %audiowrite('testout.wav',(x-1893)/4096,8000);


% #ifdef DO_INPUT_BUFFER_CAPTURE -----------------------------------------------
else

  % prepare serial port
  try
    a = serialport("/dev/ttyUSB0", 115200);
    set(a, 'bytesize', 8);
    set(a, 'parity', 'n');
    set(a, 'stopbits', 1);
  catch
  end

% Windows: Enable the flush once and then disable it to get correct results:
%flush(a);

% TEST
number_samples = 500;%50000;

  out = zeros(number_samples, 12);

  for i = 1:number_samples

    % carriage return is 13 + 10 -> use 10 as start and 13 as end marker
    while fread(a, 1) ~= 10
    end

    end_found = false;
    samples   = '';
test=[];
    while ~end_found

      x = fread(a, 1);
test = [test;x];
      samples = [samples, char(x)];

      if x == 13
        end_found = true;
      end

    end

    % convert from string to numbers for all channels
    try
      y = strsplit(samples, '\t');
    catch
      disp(samples)
      disp(test)
    end_try_catch

    for j = 1:length(y)
      out(i, j) = str2double(y{j});
    end

  end

  figure; plot(out, '.-')
  %disp(out)

  save -ascii 'recording.txt' out

end


