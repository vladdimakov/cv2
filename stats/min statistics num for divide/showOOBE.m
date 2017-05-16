clear; clc; close all;

lims = [1 3000];

OOBE5 = calcOOBE('OOBE5.txt', lims);
OOBE10 = calcOOBE('OOBE10.txt', lims);
OOBE15 = calcOOBE('OOBE15.txt', lims);
OOBE20 = calcOOBE('OOBE20.txt', lims);
OOBE25 = calcOOBE('OOBE25.txt', lims);

figure(1); hold on; grid on;
plot(lims(1):lims(2), OOBE5, 'b');
plot(lims(1):lims(2), OOBE10, 'r');
plot(lims(1):lims(2), OOBE15, 'g');
plot(lims(1):lims(2), OOBE20, 'm');
plot(lims(1):lims(2), OOBE25, 'k');

xlim(lims); legend('5', '10', '15', '20', '25');