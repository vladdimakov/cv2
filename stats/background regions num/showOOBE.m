clear; clc; close all;

lims = [1 3000];

OOBE0 = calcOOBE('OOBE0.txt', lims);
OOBE1 = calcOOBE('OOBE1.txt', lims);
OOBE2 = calcOOBE('OOBE2.txt', lims);
OOBE3 = calcOOBE('OOBE3.txt', lims);
OOBE4 = calcOOBE('OOBE4.txt', lims);
OOBE5 = calcOOBE('OOBE5.txt', lims);

figure(1); hold on; grid on;
plot(lims(1):lims(2), OOBE0, 'b');
plot(lims(1):lims(2), OOBE1, 'r');
plot(lims(1):lims(2), OOBE2, 'g');
plot(lims(1):lims(2), OOBE3, 'm');
plot(lims(1):lims(2), OOBE4, 'k');
plot(lims(1):lims(2), OOBE5, 'c');

xlim(lims); legend('0', '1', '2', '3', '4', '5');