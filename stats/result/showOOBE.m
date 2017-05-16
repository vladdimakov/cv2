clear; clc; close all;

lims = [1 3000];

OOBE1 = calcOOBE('OOBE1.txt', lims);
OOBE2 = calcOOBE('OOBE2.txt', lims);

figure(1); hold on; grid on;
plot(lims(1):lims(2), OOBE1, 'b');
plot(lims(1):lims(2), OOBE2, 'r');

xlim(lims); legend('1', '2');