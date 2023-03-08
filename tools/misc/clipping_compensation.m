
% testing clipping compensation

pkg load signal
close all;

testsignal = 1;

%test_files = {"../../algorithm/signals/pd120_single_hits.wav", {9917:9931, 14974:14985, 22525:22538, 35014:35025}; ...
%              "../../algorithm/signals/pd8.wav",               {67140:67146, 70170:70175, 73359:73363, 246312:246317, 252036:252039, 296753:296757}};
%attenuation_mapping = 0:20; % 1 dB per number of clipping samples -> current implementation in C++

if testsignal == 0
  test_files = {"../../algorithm/signals/pd120_single_hits.wav", {9917:9931, 14974:14985, 22525:22538, 35014:35025}};
  attenuation_mapping = [0, 0.22, 0.8, 1.3, 2, 3, 5, 6.5, 9, 12, 16, 23, 30, 40, 50]; % optimized for PD120
else
  test_files = {"../../algorithm/signals/pd8.wav", {67140:67146, 70170:70175, 73359:73363, 246312:246317, 252036:252039, 296753:296757}};
  attenuation_mapping = [0, 6.5, 10, 17:40];%[0, 9, 39, 13:40]; % optimized for PD8
end

clip_limit_range         = 0.05:0.04:1;%0.05:0.001:1;%0.031623:9.6838e-03:1;
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

%figure; subplot(211), plot(x_org, '.-'); grid on; subplot(212), plot(20 * log10(abs(x_org)), '.-'); grid on;

    for idx = 1:length(clip_limit_range)

      % clip
      clip_limit    = clip_limit_range(idx);
      x_org_clipped = max(-clip_limit, min(clip_limit, x_org));

      % count clipped values
      clip_indexes                       = find(abs(x_org_clipped - clip_limit) < 5 / 2^12);
      num_clipped_val(idx, cnt)          = length(clip_indexes);
      attenuation_compensation(idx, cnt) = -attenuation_mapping(1 + num_clipped_val(idx, cnt));


% TEST use distance of max neighbor sample to clipping limit as additional offset
if num_clipped_val(idx, cnt) > 0

  left_index  = min(clip_indexes) - 1;
  right_index = max(clip_indexes) + 1;
  if ( left_index > 0 ) && ( right_index <= length(x_org) )

    max_offset = attenuation_mapping(1 + num_clipped_val(idx, cnt)) - attenuation_mapping(1 + num_clipped_val(idx, cnt) - 1);

    neighbor = max(0, mean([x_org(left_index), x_org(right_index)]));

    %neighbor = max(0, max(x_org(left_index), x_org(right_index)));

%%20 * log10(abs(x_org(left_index) / x_org(right_index)))
%if abs(20 * log10(abs(x_org(left_index) / x_org(right_index)))) > 20
%  neighbor = max(0, max(x_org(left_index), x_org(right_index)));
%else
%  neighbor = max(0, mean([x_org(left_index), x_org(right_index)]));
%end

    %neighbor = max(0, sqrt(mean([x_org(left_index) ^ 2, x_org(right_index) ^ 2])));
    %neighbor = max(0, mean([sqrt(x_org(left_index)), sqrt(x_org(right_index))]) ^ 2);

    %neighbor = max(0, mean([min(x_org(left_index), max_offset), min(x_org(right_index), max_offset)]));
    %neighbor = max(0, min(x_org(left_index), x_org(right_index)));

%disp([num2str(clip_limit) ', ' num2str(neighbor)])

    offset = 20 * log10(clip_limit / neighbor);
    offset = min(offset, max_offset);

%disp([num2str(attenuation_compensation(idx, cnt)) ', ' num2str(offset)])

    attenuation_compensation(idx, cnt) = attenuation_compensation(idx, cnt) + offset;

  end

end

    end

    cnt = cnt + 1;

%figure; plot(num_clipped_val(:, i), 20 * log10(clip_limit_range)); grid on;

  end

end

%figure; plot(num_clipped_val, 20 * log10(clip_limit_range)); grid on;
figure; plot(20 * log10(clip_limit_range), attenuation_compensation, '.-'); grid on;
hold on; plot(20 * log10(clip_limit_range), 20 * log10(clip_limit_range), '--k')


