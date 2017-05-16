clear; clc; close all;

lims = [1 3000];

OOBE0 = calcOOBE('OOBE0.txt', lims);
OOBE5000 = calcOOBE('OOBE5000.txt', lims); %79
OOBE2000 = calcOOBE('OOBE2000.txt', lims); %67
OOBE7000 = calcOOBE('OOBE7000.txt', lims); %91
OOBE4000 = calcOOBE('OOBE4000.txt', lims); %80


figure(1); hold on; grid on;
plot(lims(1):lims(2), OOBE0, 'b');
plot(lims(1):lims(2), OOBE5000, 'r');
plot(lims(1):lims(2), OOBE2000, 'g');
plot(lims(1):lims(2), OOBE7000, 'm');
plot(lims(1):lims(2), OOBE4000, 'k');

xlim(lims); legend('0', '5000', '2000', '7000', '4000');