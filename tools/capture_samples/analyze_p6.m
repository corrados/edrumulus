
% noise testing with prototype 6
% - no caps test
% - using ESP32 3.3 V

close all;
pkg load signal

x1 = load('-ascii', 'recording_p6_nocaps_esp3v_4knoise_1.txt');
x2 = load('-ascii', 'recording_p6_nocaps_esp3v_4knoise_2.txt');
x3 = load('-ascii', 'recording_p6_nocaps_esp3v_4knoise_3.txt');

y1 = load('-ascii', 'recording_p6_4knoise_1.txt');
y2 = load('-ascii', 'recording_p6_4knoise_2.txt');
y3 = load('-ascii', 'recording_p6_4knoise_3.txt');


% x ----------------------------------------------------------------------------
x1 = x1(:,6);
x1 = [x1(1:76); x1(78:277); x1(279:478); x1(480:end)];

x2 = x2(:,6);
x2 = [x2(1:78); x2(80:279); x2(281:480); x2(482:end)];

x3 = x3(:,6);
x3 = [x3(1:65); x3(67:266); x3(268:467)];

x = [x1; x2; x3];

%figure; subplot(211), plot(x1); subplot(212), pwelch(x1); title('x1');
%figure; subplot(211), plot(x2); subplot(212), pwelch(x2); title('x2');
%figure; subplot(211), plot(x3); subplot(212), pwelch(x3); title('x3');
%figure; subplot(211), plot(x); subplot(212), pwelch(x); title('x');


% y ----------------------------------------------------------------------------

y1 = y1(:,6);
y1 = [y1(1:55); y1(57:256); y1(258:457); y1(459:end)];

y2 = y2(:,6);
y2 = [y2(1:66); y2(68:267); y2(269:468); y2(470:end)];

y3 = y3(:,6);
y3 = [y3(1:263); y3(265:464); y3(466:end)];

y = [y1; y2; y3];

%figure; subplot(211), plot(y1); subplot(212), pwelch(y1); title('y1');
%figure; subplot(211), plot(y2); subplot(212), pwelch(y2); title('y2');
%figure; subplot(211), plot(y3); subplot(212), pwelch(y3); title('y3');
figure; subplot(211), plot(y); subplot(212), pwelch(y); title('y');


