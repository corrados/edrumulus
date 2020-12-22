

pkg load instrument-control

close all

try
  a = serialport("COM7", 115200);
catch
end

flush(a);

for i = 1:1500%15000%20000

  % get number from string
  readready = false;
  bytearray = uint8([]);

  while ~readready

    val = fread(a, 1);

    if val == 13
      readready = true;
    end

    bytearray = [bytearray, uint8(val)];

  end

  x(i) = str2double(char(bytearray));
  
  if isnan(x(i))
    disp(char(bytearray));
  endif

end

% the very first value is garbage
x = x(2:end);

% % compensate DC offset
% x = x - mean(x(~isnan(x)));

% % normalize
% x = x / max(abs(x));

figure; plot(x, '.-');
% figure; plot(20 * log10(abs(x)));

% audiowrite("out.wav", x, 8000, "BitsPerSample", 16);
% wavwrite(x, 8000, 16, "out.wav");

% 1/((x(14000)-x(4000))/1e6/(14000-4000))

return;








pkg load instrument-control

close all

try
  a = serialport("COM7", 115200);
catch
end

flush(a);


x = double(fread(a, 1000));

pos       = find(x == 255);
markerpos = pos(find(diff(pos) == 1));



figure; plot(x);

% audiowrite("out.wav", x, 8000, "BitsPerSample", 16);



