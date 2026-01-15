% Copyright (c) 2020-2026 Volker Fischer
% SPDX-License-Identifier: GPL-2.0-or-later

% capture samples which were recorded on the ESP32 device

close all
pkg load instrument-control

% prepare serial port
try
  a = serialport("/dev/ttyUSB0", 115200);
  %set(a, 'bytesize', 8);
  %set(a, 'parity', 'n');
  %set(a, 'stopbits', 1);
catch
  disp('error');
end


figure;

N = 3;
out = cell(N, 1);

for k = 1:N

  block_end_found = false;
  out{k} = [];

  while ~block_end_found

    % carriage return is 13 + 10 -> use 10 as start and 13 as end marker
    while fread(a, 1) ~= 10
    end

    end_found = false;
    samples   = '';
    while ~end_found

      x = fread(a, 1);
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

    if length(y) == 1
      out{k} = [out{k}; str2double(y{1})];
    else
      block_end_found = true;
    end

  end

  plot(out{k}, '.-');
  drawnow;

end

disp(out)
%save -ascii 'recording.txt' out

clear a



