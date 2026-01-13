% Copyright (c) 2020-2026 Volker Fischer
% SPDX-License-Identifier: GPL-2.0-or-later

% testing sub-sample peak detection algorithm

%range = 0.1;%0.75; % single test where the interpolation curve is plotted
range = 0:0.001:2; % range test for evaluating the approximation error

for i = 1:length(range)

  y = [2, 4, 2 + range(i)];
  x = 1:3;

  % peak characterization metric 0 < m < 1
  m(i) = (y(3) - y(1)) / (y(2) - y(1));

  x1             = linspace(1, 3, 1000);
  y1             = polyval(polyfit(1:3, y, 2), x1);
  [dummy, x_max] = max(y1);

  % approximation to be evaluated
%  x_est(i) = mean(x .* y) / mean(y);
% TEST
r = y(3) / y(1);
x_est(i) = 1 + sqrt(r);

% TEST
x_est2(i) = m(i) * m(i) / 2;
error(i)  = x_est2(i) - (x1(x_max) - 2);


  [dummy, x_test_idx] = min(abs(x1 - x_est(i)));
  pol_curve(i)        = x1(x_max) - 2;
  est_curve(i)        = x1(x_test_idx) - 2;
  %error(i)            = x_max - x_test_idx;

end

close all;

if length(range) == 1

  plot(x, y, 'k*');
  hold on;
  plot(x1, y1);
  plot(x1(x_max),      y1(x_max), '*r');
  plot(x1(x_test_idx), y1(x_test_idx), 'ob', 'MarkerSize', 10);

else

  subplot(3, 1, 1), plot(range, [pol_curve; est_curve]); title('detected maxima (blue:ref, red:approx.)')
  subplot(3, 1, 2), plot(m, error); title('error curve')
  subplot(3, 1, 3), plot(m, [pol_curve; x_est2]); title('curve versus metric')

end


