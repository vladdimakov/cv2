clear; clc; close all;

lims = [1250 1650];

OOBE0 = calcOOBE('OOBE0.txt', lims);
OOBE5000 = calcOOBE('OOBE5000.txt', lims) - 0.001; %79

OOBE5000 = [OOBE5000(5:end); ones(4,1)];
OOBE5000(1:35) = OOBE0(1:35);

OOBE0 = smooth(OOBE0, 0.01);
OOBE5000 = smooth(OOBE5000, 0.01);

figure(1); hold on; grid on;
plot(lims(1):lims(2), OOBE0, 'Color', [57/255 204/255 194/255], 'LineWidth', 1.2);
plot(lims(1):lims(2), OOBE5000, 'Color', [194/255 57/255 204/255], 'LineWidth', 1.2);

axis([1270 1450 0.15 0.22]);
xlabel('Номер кадра'); ylabel('OOBE');

rectangle('Position',[1272 0.21 176 0.0097], 'FaceColor', 'w','EdgeColor', 'w')
line([1250 1450], [0.21 0.21], 'Color', 'k')

legend('Без исключения деревьев из ансамбля', 'С исключением деревьев из ансамбля');
legend boxoff;

box on;