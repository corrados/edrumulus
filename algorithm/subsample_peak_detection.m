%*******************************************************************************
% Copyright (c) 2020-2022
% Author(s): Volker Fischer
%*******************************************************************************
% This program is free software; you can redistribute it and/or modify it under
% the terms of the GNU General Public License as published by the Free Software
% Foundation; either version 2 of the License, or (at your option) any later
% version.
% This program is distributed in the hope that it will be useful, but WITHOUT
% ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
% FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
% details.
% You should have received a copy of the GNU General Public License along with
% this program; if not, write to the Free Software Foundation, Inc.,
% 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
%*******************************************************************************

% testing sub-sample peak detection algorithm

%range = 0.1;%0.75; % single test where the interpolation curve is plotted
range = 0:0.001:1; % range test for evaluating the approximation error

for i = 1:length(range)

  y = [1, 2, 1 + range(i)];

  x  = 1:3;
  x1 = linspace(1, 3, 100);
  y1 = polyval(polyfit(1:3, y, 2), x1);

  % approximation to be evaluated
%  x_est(i) = mean(x .* y) / mean(y);
% TEST
r = y(3) / y(1);
x_est(i) = 1 + sqrt(r);

  [dummy, x_max]      = max(y1);
  [dummy, x_test_idx] = min(abs(x1 - x_est(i)));
  pol_curve(i)        = x1(x_max) - 2;
  est_curve(i)        = x1(x_test_idx) - 2;
  error(i)            = x_max - x_test_idx;

end

close all;

if length(range) == 1

  plot(x, y, 'k*');
  hold on;
  plot(x1, y1);
  plot(x1(x_max),      y1(x_max), '*r');
  plot(x1(x_test_idx), y1(x_test_idx), 'ob', 'MarkerSize', 10);

else

  subplot(2, 1, 1), plot(range, [pol_curve; est_curve]); title('detected maxima (blue:ref, red:approx.)')
  subplot(2, 1, 2), plot(range, error); title('error curve')

end


