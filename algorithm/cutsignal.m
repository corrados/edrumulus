


ax = axis

% TEST overwrite axis from previous zoom area
%ax = [23358   23772      14      72];

ax = [round(ax(1)), round(ax(2)), round(ax(3)), round(ax(4))];
axis(ax);

%x = audioread("signals/pd120_pos_sense.wav");
x=audioread("signals/pd120_middle_velocity.wav");x=[x;audioread("signals/pd120_pos_sense2.wav")];x=[x;audioread("signals/pd120_hot_spot.wav")];
%x = audioread("signals/pd120_roll.wav");

x = x(ax(1) - 1000 + 1:ax(2) + 1000);

edrumulus(x)

axis([1000, 1000 + ax(2) - ax(1), ax(3), ax(4)]);


