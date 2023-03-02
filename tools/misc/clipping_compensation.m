
% testing clipping compensation

pkg load signal
close all;

test_files = {"../../algorithm/signals/pd120_single_hits.wav", {9917:9931, 14974:14985, 22525:22538, 35014:35025}; ...
              "../../algorithm/signals/pd8.wav",               {67140:67146, 70170:70175, 73359:73363, 246312:246317, 252036:252039, 296753:296757}};

%attenuation_mapping = 0:20; % 1 dB per number of clipping samples -> current implementation in C++
attenuation_mapping = [0.18, 0.22, 0.8, 1.3, 2, 3, 5, 6.5, 9, 12, 16, 23, 30, 40, 50];

clip_limit_range         = 0.05:0.001:1;%0.031623:9.6838e-03:1;
num_clipped_val          = [];
attenuation_compensation = [];
cnt                      = 1;

for i = 1:size(test_files, 1)

  % load test data
  x = audioread(test_files{i, 1});

%figure; plot(x)

  for j = 1:length(test_files{i, 2})

    % pick one peak and normalize
    x_org = x(test_files{i, 2}{j}, :);
    x_org = x_org * 1 / max(x_org);

    for idx = 1:length(clip_limit_range)

      % clip
      clip_limit = clip_limit_range(idx);
      y          = max(-clip_limit, min(clip_limit, x_org));

      % count clipped values
      clip_indexes                       = find(abs(y - clip_limit) < 5 / 2^12);
      num_clipped_val(idx, cnt)          = length(clip_indexes);
      attenuation_compensation(idx, cnt) = -attenuation_mapping(1 + num_clipped_val(idx, cnt));


% TEST
left_index  = min(clip_indexes) - 1;
right_index = max(clip_indexes) + 1;
if ( left_index > 0 ) && ( right_index <= length(x_org) )
  max_neighbor = max(x_org(left_index), x_org(right_index));
end


    end

    cnt = cnt + 1;

%figure; plot(num_clipped_val(:, i), 20 * log10(clip_limit_range)); grid on;

  end

end

%figure; plot(num_clipped_val, 20 * log10(clip_limit_range)); grid on;
figure; plot(20 * log10(clip_limit_range), attenuation_compensation); grid on;
hold on; plot(20 * log10(clip_limit_range), 20 * log10(clip_limit_range), '--k')


