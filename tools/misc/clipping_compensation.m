
% testing clipping compensation

pkg load signal

% load test data
x = audioread("../../algorithm/signals/pd120_single_hits.wav");

% pick one peak
x = x(9880:9960, :);


figure; plot(x);


