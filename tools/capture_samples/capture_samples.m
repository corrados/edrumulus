%*******************************************************************************
% Copyright (c) 2020-2021
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

% capture samples which were recorded on the ESP32 device

close all
pkg load instrument-control
pkg load statistics

% prepare serial port
try
  a = serialport("COM7", 250000);
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


