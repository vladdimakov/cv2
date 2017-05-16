clear; clc; close all;

lims = [1 3000];

methodError01 = calcMethodError('methodError01.txt', lims);
methodError2 = calcMethodError('methodError2.txt', lims)-2;
methodError5 = calcMethodError('methodError5.txt', lims)-3;
methodError7 = calcMethodError('methodError7.txt', lims)-3;
methodError10 = calcMethodError('methodError10.txt', lims)-4;

figure(1); hold on; grid on;
plot(lims(1):lims(2), methodError01, 'b');
plot(lims(1):lims(2), methodError2, 'r');
plot(lims(1):lims(2), methodError5, 'g');
plot(lims(1):lims(2), methodError7, 'm');
plot(lims(1):lims(2), methodError10, 'k');

xlim(lims); legend('0.1', '2', '5', '7', '10');