clear; clc; close all;

lims = [1 1900];

data = readArray('methodError.txt');

methodError = cumsum(data);
methodError = methodError(lims(1):lims(2));

figure(1); hold on; grid on;
plot(lims(1):lims(2), methodError, 'b');

xlim(lims);