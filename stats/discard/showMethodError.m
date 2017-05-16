clear; clc; close all;

lims = [1 3000];

methodError0 = calcMethodError('methodError0.txt', lims);
methodError5000 = calcMethodError('methodError5000.txt', lims);
methodError2000 = calcMethodError('methodError2000.txt', lims);
methodError7000 = calcMethodError('methodError7000.txt', lims);
methodError4000 = calcMethodError('methodError4000.txt', lims);

figure(1); hold on; grid on;
plot(lims(1):lims(2), methodError0, 'b');
plot(lims(1):lims(2), methodError5000, 'r');
plot(lims(1):lims(2), methodError2000, 'g');
plot(lims(1):lims(2), methodError7000, 'm');
plot(lims(1):lims(2), methodError4000, 'k');

xlim(lims); legend('0', '5000', '2000', '7000', '4000');