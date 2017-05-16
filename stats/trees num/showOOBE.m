clear; clc; close all;

lims = [1 3000];

OOBE1 = calcOOBE('OOBE1.txt', lims);
OOBE11 = calcOOBE('OOBE11.txt', lims);
OOBE25 = calcOOBE('OOBE25.txt', lims);
OOBE51 = calcOOBE('OOBE51.txt', lims);
OOBE75 = calcOOBE('OOBE75.txt', lims);
OOBE101 = calcOOBE('OOBE101.txt', lims);
OOBE201 = calcOOBE('OOBE201.txt', lims);

figure(1); hold on; grid on;
plot(lims(1):lims(2), OOBE1, 'c');
plot(lims(1):lims(2), OOBE11, 'b');
plot(lims(1):lims(2), OOBE25, 'r');
plot(lims(1):lims(2), OOBE51, 'g');
plot(lims(1):lims(2), OOBE75, 'm');
plot(lims(1):lims(2), OOBE101, 'k');
plot(lims(1):lims(2), OOBE201, 'y');

xlim(lims); legend('1', '11', '25', '51', '75', '101', '201');