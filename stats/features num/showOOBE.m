clear; clc; close all;

lims = [1 3000];

OOBE10_3 = calcOOBE('OOBE10_3.txt', lims);
OOBE50_10 = calcOOBE('OOBE50_10.txt', lims);
OOBE100_10 = calcOOBE('OOBE100_10.txt', lims);
OOBE100_new = calcOOBE('OOBE100_new.txt', lims);

figure(1); hold on; grid on;
plot(lims(1):lims(2), OOBE10_3, 'b');
plot(lims(1):lims(2), OOBE50_10, 'r');
plot(lims(1):lims(2), OOBE100_10, 'g');
plot(lims(1):lims(2), OOBE100_new, 'm');

xlim(lims); legend('10_{3}', '50_{10}', '100_{10}', '100_{new}');