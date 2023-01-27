
% testing three sensor padecoef

close all;
pkg load signal

s_low   = load('-ascii', 'recording_sensor_low.txt');
s_left  = load('-ascii', 'recording_sensor_left.txt');
s_right = load('-ascii', 'recording_sensor_right.txt');

s_low   = s_low(:, [1, 9, 12]);   s_low   = s_low - ones(size(s_low, 1), 1) * mean(s_low);
s_left  = s_left(:, [1, 9, 12]);  s_left  = s_left - ones(size(s_left, 1), 1) * mean(s_left);
s_right = s_right(:, [1, 9, 12]); s_right = s_right - ones(size(s_right, 1), 1) * mean(s_right);

%subplot(3, 1, 1), plot(s_low);   grid on; title('sensor low');
%subplot(3, 1, 2), plot(s_left);  grid on; title('sensor left');
%subplot(3, 1, 3), plot(s_right); grid on; title('sensor right');

subplot(3, 1, 1), plot(20 * log10(abs(s_low)));   grid on; title('sensor low');
subplot(3, 1, 2), plot(20 * log10(abs(s_left)));  grid on; title('sensor left');
subplot(3, 1, 3), plot(20 * log10(abs(s_right))); grid on; title('sensor right');


