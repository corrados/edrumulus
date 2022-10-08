%*******************************************************************************
% Copyright (c) 2020-2022
% Author(s): Matthias Roth
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

function find_position_3sensors()

  close all;

  sensor_pos_norm = [-0.5, 0; 0.5, 0; 0, sqrt(3) / 2];
  fLen            = 20;

% TEST
range = [-1.5:0.1:1.5];

  for d1 = range

% TEST
d2 = 0.1;%d1;

    vfP = find_position(0.1, sensor_pos_norm, fLen, fLen * [d1, d2]);
    plot(fLen * sensor_pos_norm(:, 1), fLen * sensor_pos_norm(:, 2), 'rx', 'MarkerSize', 20,'LineWidth', 4);
    axis(fLen * [-0.8, 0.8, -0.5, 1.3]); grid on; hold on;
    plot(vfP(1), vfP(2), 'ko', 'MarkerSize', 10,'LineWidth', 4)

  end

end


function vfP = find_position(fEps, sensor_pos_norm, fLen, vfcD)
%  vfcD = [fD21, fD31]=[fL2-fL1, fL3-fL1]

  mfcP = fLen * sensor_pos_norm;

  vfcP0 = [0, sqrt(3) / 4] * fLen;

  vfQ0     = vfcP0;
  bIterate = true;
  iCount   = 0;

  while bIterate

    [vfVal, mfDiff] = Fcalc(mfcP, vfQ0, vfcD);

    fDet      =  mfDiff(1, 1) * mfDiff(2, 2) - mfDiff(1, 2) * mfDiff(2, 1);
    mfInvDiff = [ mfDiff(2, 2), -mfDiff(1, 2); ...
                 -mfDiff(2, 1),  mfDiff(1, 1)] / fDet;

    vfQ1   = vfQ0 - vfVal * mfInvDiff;
    fDelta = mydist([0, 0], vfVal);

    %disp(sprintf('mydist([0,0], vfVal) %10.8f', fDelta))

    if (fDelta < fEps) || (iCount >= 20)
      bIterate = false;
    end

    vfQ0   = vfQ1;
    iCount = iCount + 1;

  end

  vfP = vfQ1;

end


function fDist = mydist(vP, vQ)

  vfDiff = vP - vQ;
  fDist  = sqrt(vfDiff * vfDiff');

end 

function [vfVal, mfDiff] = Fcalc(mfcP, vfQ, vfcD)
  
  vfDist = [mydist(vfQ, mfcP(1, :)), mydist(vfQ, mfcP(2, :)) , mydist(vfQ, mfcP(3, :))]; % =:[L1,L2,L3]
  vfVal  = [vfDist(2) - vfDist(1), vfDist(3) - vfDist(1)] - vfcD;
  mfDiff = [(vfQ-mfcP(2, :)) / vfDist(2) - (vfQ - mfcP(1, :)) / vfDist(1);
            (vfQ-mfcP(3, :)) / vfDist(3) - (vfQ - mfcP(1, :)) / vfDist(1)];

end
