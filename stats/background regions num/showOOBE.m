clear; clc; close all;

lims = [1 1900];

data = readArray('OOBE.txt');
correctlyClassifiedNum = inverseCumsum(data(1:2:end));
incorrectlyClassifiedNum = inverseCumsum(data(2:2:end));

OOBE = cumsum(incorrectlyClassifiedNum) ./ ...
    (cumsum(correctlyClassifiedNum) + cumsum(incorrectlyClassifiedNum));

OOBE = OOBE(lims(1):lims(2));

figure(1); hold on; grid on;
plot(lims(1):lims(2), OOBE, 'b');

xlim(lims);