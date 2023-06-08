
% testing clipping compensation

% TEST
%d=0.13;x=[0.0, 0.05, 0.08, 0.1, 0.15, 0.2, 0.4, 0.6, 0.8, 0.9, 1.0, 1.1, 1.2];subplot(311),plot(1-(0:d:1).^2,'.-');subplot(312),plot((0:d:1).^2,'.-');subplot(313),plot(x);axis([1, 8, 0, 1])

pkg load signal
close all;

pad = "pd120";
%pad = "pd80r";
%pad = "pd8";
%pad = "pd5";
use_neighbors     = true;
use_new_algorithm = true;

if strcmp(pad, "pd120")
  test_files = {"../../algorithm/signals/pd120_single_hits.wav", {9917:9931, 14974:14985, 22525:22538, 35014:35025}};
  if use_neighbors
    if use_new_algorithm
      ampmap_const_step = 0.075;
    else
      ampmap_const_step = 0.09;
    end
  else
    ampmap_const_step = 0.08;
  end
elseif strcmp(pad, "pd80r")
  test_files = {"../../algorithm/signals/pd80r.wav", {48891:48900, 61075:61086, 202210:202222, 242341:242353}};
  if use_neighbors
    if use_new_algorithm
      ampmap_const_step = 0.09;
    else
      ampmap_const_step = 0.11;
    end
  else
    ampmap_const_step = 0.1;
  end
elseif strcmp(pad, "pd8")
  test_files = {"../../algorithm/signals/pd8.wav", {67140:67146, 70170:70175, 73359:73363, 246312:246317, 252036:252039, 296753:296757}};
  if use_new_algorithm
    ampmap_const_step = 0.2;
  else
    ampmap_const_step = 0.57;
  end
elseif strcmp(pad, "pd5")
  test_files = {"../../algorithm/signals/pd5.wav", {599216:599220, 344800:344804, 765417:765421}};
  if use_new_algorithm
    ampmap_const_step = 0.3;
  else
    ampmap_const_step = 0.9;
  end
end

clip_limit                 = 1900; % approx. for 12 bit ADC
clip_factor_range          = 1 ./ (1:-0.01:0.04);
length_ampmap              = 20;
amplification_mapping      = 10 .^ ([0:ampmap_const_step:ampmap_const_step * length_ampmap] .^ 2);
num_clipped_val            = [];
amplification_compensation = [];
cnt                        = 1;

for i = 1:size(test_files, 1)

  % load test data
  x = audioread(test_files{i, 1});

%figure; plot(x, '.-'); grid on;

  for j = 1:length(test_files{i, 2})

    % pick one peak and normalize
    x_org = x(test_files{i, 2}{j}, 1);
    x_org = x_org / max(abs(x_org)); % normalize original input signal

%figure; plot(x_org, '.-'); grid on;

    for idx = 1:length(clip_factor_range)

      % clip
      x_org_scaled  = x_org * clip_limit * clip_factor_range(idx);
      x_org_clipped = max(-clip_limit, min(clip_limit, x_org_scaled));

%figure; plot(x_org_clipped, '.-'); grid on; ax = axis; hold on; plot([ax(1), ax(2)], [clip_limit, clip_limit], 'r'); plot([ax(1), ax(2)], [-clip_limit, -clip_limit], 'r')

      % count clipped values
      if max(x_org) > -min(x_org)
        clip_indexes = find(abs(x_org_clipped - clip_limit) < 5 / 2 ^ 12);
      else
        clip_indexes = find(abs(x_org_clipped + clip_limit) < 5 / 2 ^ 12);
      end
      num_clipped_val(idx, cnt)            = length(clip_indexes);
      amplification_compensation(idx, cnt) = amplification_mapping(1 + num_clipped_val(idx, cnt));

      % use distance of mean left/right neighbor samples to clipping limit as additional offset
      correction_offset_applied = false;

      if num_clipped_val(idx, cnt) > 0

        left_index  = min(clip_indexes) - 1;
        right_index = max(clip_indexes) + 1;

        if (left_index > 0) && (right_index <= length(x_org_scaled))

          % calculate mean neighbor value
          % - use linear domain for offset calculation
          % - use squared x which is available in Edrumulus Arduino code right now
          neighbor = mean(abs([x_org_scaled(left_index), x_org_scaled(right_index)]));

          % x: point just below the limit (neighbor)
          % a: x / x_max, where x_may is the maximum of the peak
          % l: clip limit (for ESP32 usually ~1800)
          % y = x_max / l
          % a = x_max / x
          % -> y = a * x / l
          if use_neighbors
            if use_new_algorithm


% TEST
% a_delta = a_2 - a_1 -> A_delta = a_delta * C, where C is the clip limit
% D = N - (C - A_delta), where N is the neighbor
% r = D / A_delta
% x = a_1 + r * a_delta = a_2 + (N / C - 1)
% y = x * C = (a_2 - 1) * C + N
a_low                 = amplification_mapping(1 + num_clipped_val(idx, cnt));
a_high                = amplification_mapping(1 + num_clipped_val(idx, cnt) + 1);
a_diff                = a_high - a_low;
a_low_abs             = a_low * clip_limit;
a_high_abs            = a_high * clip_limit;
a_diff_abs            = (a_high_abs - a_low_abs) / a_low;
neighbor_to_limit_abs = (neighbor - (clip_limit - a_diff_abs));
neighbor_to_limit_abs = max(0, min(a_diff_abs, neighbor_to_limit_abs));
r                     = neighbor_to_limit_abs / a_diff_abs;
amplification_compensation(idx, cnt) = amplification_mapping(1 + num_clipped_val(idx, cnt)) + r .* a_diff;

% TEST: derived formula but clipping of  neighbor is not yet considered...
%amplification_compensation(idx, cnt) = ((a_high - 1) * clip_limit + neighbor) / clip_limit;

% TEST: not working...
%%amplification_compensation(idx, cnt) = a_high * neighbor / clip_limit;
%%amplification_compensation(idx, cnt) = a_low;
%amplification_compensation(idx, cnt) = max(a_low, min(a_high, a_high * neighbor / clip_limit));


            else
              amplification_compensation(idx, cnt) = amplification_mapping(1 + num_clipped_val(idx, cnt)) * neighbor / clip_limit;
            end

          else
            amplification_compensation(idx, cnt) = amplification_mapping(1 + num_clipped_val(idx, cnt));
          end
          correction_offset_applied = true;

% TEST
%amplification_compensation(idx, cnt) = min(amplification_compensation(idx, cnt), amplification_mapping(1 + num_clipped_val(idx, cnt) - 1));

% TEST
%amplification_compensation(idx, cnt) = min(amplification_compensation(idx, cnt), amplification_mapping(1 + num_clipped_val(idx, cnt) + 1));

%amplification_compensation(idx, cnt) = amplification_mapping(1 + num_clipped_val(idx, cnt));

        end

      end

      if ~correction_offset_applied

        % if no neighbors are available, use worst case assumption of last attenuation
        amplification_compensation(idx, cnt) = amplification_mapping(1 + num_clipped_val(idx, cnt) - 1);

      end

%figure; plot(x_org_clipped, '.-'); grid on; ax = axis; hold on;
%  plot([ax(1), ax(2)], [clip_limit, clip_limit], 'r');
%  plot([ax(1), ax(2)], [-clip_limit, -clip_limit], 'r');
%  plot(x_org_scaled, '.-k');
%  plot(x_org_clipped * amplification_compensation(idx, cnt), '.-g');

    end

    cnt = cnt + 1;

%figure; plot(num_clipped_val(:, i), 20 * log10(clip_factor_range)); grid on;

  end

end

% results plot
figure; plot(20 * log10(clip_factor_range), 20 * log10(amplification_compensation), '.-'); grid on;
hold on; plot(20 * log10(clip_factor_range), 20 * log10(clip_factor_range), '--k');
axis(20 * log10([min(clip_factor_range), max(clip_factor_range), min(clip_factor_range), max(clip_factor_range)]));
xlabel('actual clipping'); ylabel('estimated clipping amplification');


