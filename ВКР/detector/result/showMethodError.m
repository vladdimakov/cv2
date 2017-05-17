clear; clc; close all;

lims = [700 1700];

methodError0 = calcMethodError('methodError1.txt', lims);
methodError5000 = calcMethodError('methodError2.txt', lims);

figure(1); hold on; grid on;
plot(lims(1):lims(2), methodError0, 'Color', [57/255 204/255 194/255], 'LineWidth', 1.2);
plot(lims(1):lims(2), methodError5000, 'Color', [194/255 57/255 204/255], 'LineWidth', 1.2);

axis([700 1700 0 290])

rectangle('Position',[704 250 992 39], 'FaceColor', 'w', 'EdgeColor', 'w')
line([700 1700], [250 250], 'Color', 'k')

legend('Без использования детектора', 'С использованием детектора');

box on;
legend boxoff;