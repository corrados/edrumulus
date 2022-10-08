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

function [vfP] = find_position_3sensors(fEps, fLen, vfcD)
%  vfcD = [fD21, fD31]=[fL2-fL1, fL3-fL1]

  global  mfcP

  mfcP =  fLen*[0,-0.5; 0.5, 0; 0, sqrt(3)/2];
  
  vfcP0 = [0, sqrt(3)/4]*fLen;

  vfQ0 = vfcP0;
  bIterate = true;
  while(bIterate) 
    [vfVal, mfDiff] = Fcalc(vfQ0, vfcD);
    fDet      =  mfDiff(1,1)*mfDiff(2,2)-mfDiff(1,2)*mfDiff(2,1);
    mfInvDiff = [ mfDiff(2,2), -mfDiff(1,2); ...
                 -mfDiff(2,1),  mfDiff(1,1)]/fDet;
    vfQ1      = vfQ0 - vfVal*mfInvDiff;
    
    
    fDelta = mydist(vfQ0, vfQ1); 
    disp(sprintf('mydist(vfQ0, vfQ1) %10.8f', fDelta))
    if (fDelta < fEps) break; end
    vfQ0 = vfQ1;
  end 
  vfP = vfQ1;
end


function [fDist] = mydist(vP,vQ)
  vfDiff = vP-vQ;
  fDist  = sqrt(vfDiff*vfDiff'); 
end 

function [vfVal, mfDiff] = Fcalc(vfQ, vfcD)
  global mfcP
  
  vfDist = [mydist(vfQ, mfcP(1,:)), mydist(vfQ, mfcP(2,:)) , mydist(vfQ, mfcP(3,:))];
  vfVal  = [vfDist(2)-vfDist(1), vfDist(3)-vfDist(1)] - vfcD;
  mfDiff = [(vfQ-mfcP(2,:))/vfDist(2) - (vfQ-mfcP(1,:))/vfDist(1);
            (vfQ-mfcP(3,:))/vfDist(3) - (vfQ-mfcP(1,:))/vfDist(1) ];

%  vfVal  = [ mydist(vfQ, mfcP(2,:)) - mydist(vfQ, mfcP(1,:)); ...
%            mydist(vfQ, mfcP(3,:)) - mydist(vfQ, mfcP(1,:))] - vfcD;
end

