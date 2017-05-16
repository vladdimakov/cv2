clear; clc; close all;

lims = [1 3000];

methodError1 = calcMethodError('methodError1.txt', lims);
methodError2 = calcMethodError('methodError2.txt', lims);

figure(1); hold on; grid on;
plot(lims(1):lims(2), methodError1, 'b');
plot(lims(1):lims(2), methodError2, 'r');

xlim(lims); legend('1', '2');