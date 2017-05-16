clear; clc; close all;

lims = [1 3000];

methodError10_3 = calcMethodError('methodError10_3.txt', lims);
methodError50_10 = calcMethodError('methodError50_10.txt', lims)-3;
methodError100_10 = calcMethodError('methodError100_10.txt', lims)-3;
methodError100_new = calcMethodError('methodError100_new.txt', lims)-2;

figure(1); hold on; grid on;
plot(lims(1):lims(2), methodError10_3, 'b');
plot(lims(1):lims(2), methodError50_10, 'r');
plot(lims(1):lims(2), methodError100_10, 'g');
plot(lims(1):lims(2), methodError100_new, 'm');

xlim(lims); legend('10_{3}', '50_{10}', '100_{10}', '100_{new}');