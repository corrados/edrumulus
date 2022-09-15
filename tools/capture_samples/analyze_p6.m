
% noise testing with prototype 6
% - no caps test
% - using ESP32 3.3 V

close all;
pkg load signal

x1 = load('-ascii', 'recording_p6_nocaps_esp3v_4knoise_1.txt');
x2 = load('-ascii', 'recording_p6_nocaps_esp3v_4knoise_2.txt');
x3 = load('-ascii', 'recording_p6_nocaps_esp3v_4knoise_3.txt');

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
figure; subplot(211), plot(x); subplot(212), pwelch(x); title('x');


