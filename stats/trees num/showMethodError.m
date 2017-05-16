clear; clc; close all;

lims = [1 3000];

methodError1 = calcMethodError('methodError1.txt', lims);
methodError11 = calcMethodError('methodError11.txt', lims);
methodError25 = calcMethodError('methodError25.txt', lims);
methodError51 = calcMethodError('methodError51.txt', lims);
methodError75 = calcMethodError('methodError75.txt', lims);
methodError101 = calcMethodError('methodError101.txt', lims);
methodError201 = calcMethodError('methodError201.txt', lims);

figure(1); hold on; grid on;
plot(lims(1):lims(2), methodError1, 'c');
plot(lims(1):lims(2), methodError11, 'b');
plot(lims(1):lims(2), methodError25, 'r');
plot(lims(1):lims(2), methodError51, 'g');
plot(lims(1):lims(2), methodError75, 'm');
plot(lims(1):lims(2), methodError101, 'k');
plot(lims(1):lims(2), methodError101, 'y');

xlim(lims); legend('1', '11', '25', '51', '75', '101', '201');