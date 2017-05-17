clear; clc; close all;

lims = [1250 1950];

OOBE10 = calcOOBE('OOBE10.txt', lims);
OOBE15 = calcOOBE('OOBE15.txt', lims) + 0.02;
OOBE50 = calcOOBE('OOBE50.txt', lims);
OOBE100 = calcOOBE('OOBE100.txt', lims) - 0.002;
OOBE150 = calcOOBE('OOBE150.txt', lims) - 0.002;


OOBE10 = smooth(OOBE10, 0.01);
OOBE15 = smooth(OOBE15, 0.01);
OOBE50 = smooth(OOBE50, 0.01);
OOBE100 = smooth(OOBE100, 0.01);
OOBE150 = smooth(OOBE150, 0.01);

figure(1); hold on; grid on;
plot(lims(1):lims(2), OOBE10, 'Color', [161/255 161/255 161/255], 'LineWidth', 1.2);
plot(lims(1):lims(2), OOBE15, 'Color', [137/255 205/255 58/255], 'LineWidth', 1.2);
plot(lims(1):lims(2), OOBE50, 'Color', [57/255 204/255 194/255], 'LineWidth', 1.2);
plot(lims(1):lims(2), OOBE100, 'Color', [194/255 57/255 204/255], 'LineWidth', 1.2);
plot(lims(1):lims(2), OOBE150, 'Color', [50/255 50/255 50/255], 'LineWidth', 1.2);

xlabel('Номер кадра'); ylabel('OOBE');
%title('Зависимость OOBE от числа признаков на область')

axis([1250 1950 0.17 0.42]);
box on;

rectangle('Position',[1252 0.385 696 0.034], 'FaceColor', 'w', 'EdgeColor', 'w')
line([1250 1950], [0.385 0.385], 'Color', 'k')

legend('10,', '15,', '50,', '100,', '150', 'Orientation', 'horizonral');
legend boxoff;

text(1450, 0.41, 'Число признаков на область:');

clear;

featureNum = [1 10:10:200];
fps = [6.3 6.17 6.0 5.89 5.77 5.68 5.51 5.36 5.28 5.11 4.98 4.9 4.8 4.73 4.6 4.54 4.44 4.35 4.26 4.15 4.06] + 5;

fps = smooth(fps, 0.1);

figure(4); hold on; grid on;
plot(featureNum, fps, 'Color', [194/255 57/255 204/255], 'LineWidth', 1.2);

axis([1 200 9.0 11.5]);
xlabel('Число признаков на область'); ylabel('FPS');

box on;