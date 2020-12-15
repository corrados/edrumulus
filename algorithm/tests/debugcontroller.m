

pkg load signal
pkg load instrument-control

close all

% serialportlist("available")'

try
  a = serialport("COM6", 9600);
catch
end

flush(a);

tic
x = double(fread(a, 1000));
toc

% y = zeros(length(x) / 2, 1);
% for i = 0:length(x) / 2 - 1
%   y(1 + i) = x(1 + 2 * i) + 256 * x(2 + 2 * i);
% end


figure; subplot(2, 1, 1), plot(x - 127, '.-'); subplot(2, 1, 2), pwelch(x, [], [], [], [], [], 'db')

% fclose(a);

% serialportlist("available")'


