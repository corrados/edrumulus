


% run drumtrigger and zoom into the area to be observed -> then call cutsignal
ax = axis

ax = [round(ax(1)), round(ax(2)), round(ax(3)), round(ax(4))];
axis(ax);

[x, pad] = signalsandsettings;

x = x(ax(1) - 1000 + 1:ax(2) + 1000, :);

edrumulus(x, pad)

axis([1000, 1000 + ax(2) - ax(1), ax(3), ax(4)]);


