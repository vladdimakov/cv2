clear; clc; close all;

lims = [1 3000];

methodError5 = calcMethodError('methodError5.txt', lims)-2;
methodError10 = calcMethodError('methodError10.txt', lims);
methodError15 = calcMethodError('methodError15.txt', lims)-6;
methodError20 = calcMethodError('methodError20.txt', lims)-4;
methodError25 = calcMethodError('methodError25.txt', lims)-3;

figure(1); hold on; grid on;
plot(lims(1):lims(2), methodError5, 'b');
plot(lims(1):lims(2), methodError10, 'r');
plot(lims(1):lims(2), methodError15, 'g');
plot(lims(1):lims(2), methodError20, 'm');
plot(lims(1):lims(2), methodError25, 'k');

xlim(lims); legend('5', '10', '15', '20', '25');