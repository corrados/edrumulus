
% testing clipping compensation

pkg load signal
close all;

test_files = {"../../algorithm/signals/pd120_single_hits.wav", 9917:9931; ...
              "../../algorithm/signals/pd8.wav",               67140:67146};

clip_limit_range = 0.05:0.001:1;
num_clipped_val  = [];

for i = 1:size(test_files, 1)

  % load test data
  x = audioread(test_files{i, 1});

  % pick one peak and normalize
  x_org = x(test_files{i, 2}, :);
  x_org = x_org * 1 / max(x_org);

  for idx = 1:length(clip_limit_range)

    % clip
    clip_limit = clip_limit_range(idx);
    x          = max(-clip_limit, min(clip_limit, x_org));

    % count clipped values
    num_clipped_val(idx, i) = length(find(abs(x - clip_limit) < 5 / 2^12));

  end

%figure; plot(num_clipped_val(:, i), 20 * log10(clip_limit_range)); grid on;

end

figure; plot(num_clipped_val, 20 * log10(clip_limit_range)); grid on;


