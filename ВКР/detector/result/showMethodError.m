clear; clc; close all;

lims = [700 1700];

methodError0 = calcMethodError('methodError1.txt', lims);
methodError5000 = calcMethodError('methodError2.txt', lims);


point1 = [952 0];
point2 = [1457 58];
point3 = [1510 58];
point4 = [1510 0];

figure(1); hold on; grid on;
plot(lims(1):lims(2), methodError0, 'Color', [57/255 204/255 194/255], 'LineWidth', 1.2);
plot(lims(1):lims(2), methodError5000, 'Color', [194/255 57/255 204/255], 'LineWidth', 1.2);

plot(point1(1), point1(2), '.k', 'MarkerSize', 20); text(936, 12, '1', 'FontSize', 11);
plot(point2(1), point2(2), '.k', 'MarkerSize', 20); text(1442, 70, '2', 'FontSize', 11);
plot(point3(1), point3(2), '.k', 'MarkerSize', 20); text(1495, 70, '3', 'FontSize', 11);
plot(point4(1), point4(2), '.k', 'MarkerSize', 20); text(1495, 12, '4', 'FontSize', 11);

axis([700 1700 0 290])
xlabel('Номер кадра'); ylabel('Число кадров с неправильно найденной целью');

rectangle('Position',[704 250 992 39], 'FaceColor', 'w', 'EdgeColor', 'w')
line([700 1700], [250 250], 'Color', 'k')

legend('Без использования детектора', 'С использованием детектора');

box on;
legend boxoff;