clear; clc; close all;

lims = [1 3000];

methodError0 = calcMethodError('methodError0.txt', lims);
methodError1 = calcMethodError('methodError1.txt', lims);
methodError2 = calcMethodError('methodError2.txt', lims);
methodError3 = calcMethodError('methodError3.txt', lims);
methodError4 = calcMethodError('methodError4.txt', lims);
methodError5 = calcMethodError('methodError5.txt', lims);

figure(1); hold on; grid on;
plot(lims(1):lims(2), methodError0, 'b');
plot(lims(1):lims(2), methodError1, 'r');
plot(lims(1):lims(2), methodError2, 'g');
plot(lims(1):lims(2), methodError3, 'm');
plot(lims(1):lims(2), methodError4, 'k');
plot(lims(1):lims(2), methodError5, 'c');

xlim(lims); legend('0', '1', '2', '3', '4', '5');