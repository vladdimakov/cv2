clear; clc; close all;

lims = [1 3000];

OOBE01 = calcOOBE('OOBE01.txt', lims);
OOBE2 = calcOOBE('OOBE2.txt', lims);
OOBE5 = calcOOBE('OOBE5.txt', lims);
OOBE7 = calcOOBE('OOBE7.txt', lims);
OOBE10 = calcOOBE('OOBE10.txt', lims);

figure(1); hold on; grid on;
plot(lims(1):lims(2), OOBE01, 'b');
plot(lims(1):lims(2), OOBE2, 'r');
plot(lims(1):lims(2), OOBE5, 'g');
plot(lims(1):lims(2), OOBE7, 'm');
plot(lims(1):lims(2), OOBE10, 'k');

xlim(lims); legend('0.1', '2', '5', '7', '10');