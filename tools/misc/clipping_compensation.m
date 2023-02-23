
% testing clipping compensation

pkg load signal
close all;

% load test data
x = audioread("../../algorithm/signals/pd120_single_hits.wav");

% pick one peak
x_org = x(9917:9931, :);

clip_limit_range = 0.01:0.001:0.1;
num_clipped_val  = [];

for idx = 1:length(clip_limit_range)

  % clip
  clip_limit = clip_limit_range(idx);
  x          = max(-clip_limit, min(clip_limit, x_org));

  % count clipped values
  num_clipped_val(idx) = length(find(abs(x - clip_limit) < 5 / 2^12));

end

figure; plot(num_clipped_val, 20 * log10(clip_limit_range)); grid on;


