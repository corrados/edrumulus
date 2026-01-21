% Copyright (c) 2020-2026 Volker Fischer
% SPDX-License-Identifier: GPL-2.0-or-later

% capture samples which were recorded on the ESP32 device

close all
pkg load instrument-control


% optionally, post process recorded data
if false

  load('recording2026-01-20-08-34-33.mat');
  o{1} = out;
  load('recording2026-01-20-08-37-55.mat');
  o{2} = out;
  load('recording2026-01-20-08-41-06.mat');
  o{3} = out;
  load('recording2026-01-20-08-42-15.mat');
  o{4} = out;
  load('recording2026-01-20-08-43-25.mat');
  o{5} = out;
  load('recording2026-01-20-08-44-28.mat');
  o{6} = out;

  % o1:
  id{1} = [     4,        6,      8,      9,     10];
  rg{1} = {1:8426, 400:8100, 1:8000, 1:8000, 1:7800};
  % o2:
  id{2} = [     5,       10];
  rg{2} = {1:7400, 600:8300};
  % o3:
  id{3} = [     2,      6,      7,      9,     10];
  rg{3} = {1:7300, 1:8390, 1:7400, 1:7600, 1:7700};
  % o4:
  id{4} = [       4,      5,        9];
  rg{4} = {400:7800, 1:8400, 600:8000};
  % o5:
  id{5} = [        3,      4,      5,      7,      8,       10];
  rg{5} = {1000:8000, 1:8400, 1:8000, 1:8000, 1:8400, 600:8200};
  % o6:
  id{6} = [     3,        4,      6,      7,      9];
  rg{6} = {1:7700, 800:7900, 1:8000, 1:7300, 1:7300};

  %n = 1;
  %for i = 1:length(id{n})
  %  cur_id = id{n}(i);
  %  figure; plot(o{n}{cur_id}); title(num2str(cur_id));
  %  %figure; plot(o{n}{cur_id}(rg{n}{i})); title(num2str(cur_id));
  %end

  out = [];
  for n = 1:length(id)
    for i = 1:length(id{n})
      cur_id = id{n}(i);
      out    = [out; o{n}{cur_id}(rg{n}{i})];
    end
  end
  out = out(~isnan(out));
  figure; plot(out);
  audiowrite('testout.wav', (out - mean(out)) / 4096, 8000);

  return;
end





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

N = 10;
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

%disp(out)
%save -ascii 'recording.txt' out


fn = strcat('recording', datestr(now, 'yyyy-mm-dd-HH-MM-SS'), '.mat');
save(fn, 'out');

system('play -q -n synth 0.2 sine 1000');

clear a



