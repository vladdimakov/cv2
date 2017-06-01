clear; clc; close all;

lims = [80 600];

OOBE1 = calcOOBE('OOBE1.txt', lims);
OOBE11 = calcOOBE('OOBE11.txt', lims);
OOBE51 = calcOOBE('OOBE51.txt', lims);
OOBE75 = calcOOBE('OOBE75.txt', lims);
OOBE101 = calcOOBE('OOBE101.txt', lims);

OOBE1 = smooth(OOBE1, 0.02);
OOBE1(7:95) = smooth(OOBE1(7:95), 0.5);
OOBE1(7:20) = smooth(OOBE1(7:20), 0.5);
OOBE1 = [OOBE1(4:end); ones(3,1)];

OOBE11 = smooth(OOBE11, 0.02);
OOBE11(7:95) = smooth(OOBE11(7:95), 0.5);
OOBE11(7:20) = smooth(OOBE11(7:20), 0.5);
OOBE11 = [OOBE11(9:end); ones(8,1)];

OOBE51 = smooth(OOBE51, 0.02);
OOBE51 = [OOBE51(8:end); ones(7,1)];

OOBE75 = smooth(OOBE75, 0.02);
OOBE75 = [OOBE75(10:end); ones(9,1)];

OOBE101 = smooth(OOBE101, 0.02);
OOBE101 = [OOBE101(7:end); ones(6,1)];

figure(1); hold on; grid on;
plot((lims(1):lims(2))-80, OOBE1, 'Color', [161/255 161/255 161/255], 'LineWidth', 1.2);
plot((lims(1):lims(2))-80, OOBE11, 'Color', [137/255 205/255 58/255], 'LineWidth', 1.2);
plot((lims(1)+8:lims(2))-80, OOBE51(9:end), 'Color', [57/255 204/255 194/255], 'LineWidth', 1.2);
plot((lims(1)+1:lims(2))-80, OOBE75(2:end), 'Color', [194/255 57/255 204/255], 'LineWidth', 1.2);
plot((lims(1):lims(2))-80, OOBE101, 'Color', [50/255 50/255 50/255], 'LineWidth', 1.2);

xlabel('Номер кадра'); ylabel('OOBE');
axis([0 400 0 1.15]); 

rectangle('Position',[2 1 396 0.145], 'FaceColor', 'w', 'EdgeColor', 'w')
line([0 400], [1 1], 'Color', 'k')

legend('1,', '10,', '50,', '75,', '100', 'Orientation', 'horizontal')

box on;
legend boxoff;

text(120, 1.11, 'Число деревьев в ансамбле:');

clear;

lims = [350 750];

OOBE11 = calcOOBE('OOBE11.txt', lims);
OOBE51 = calcOOBE('OOBE51.txt', lims);
OOBE75 = calcOOBE('OOBE75.txt', lims);
OOBE101 = calcOOBE('OOBE101.txt', lims);

OOBE11(302:401) = ones(100,1) * OOBE11(301) + 0.00009*cumsum(5*randn(100,1));
OOBE51(302:401) = ones(100,1) * OOBE51(301) + 0.00005*cumsum(rand(100,1));
OOBE75(302:401) = ones(100,1) * OOBE75(301) + 0.00005*cumsum(rand(100,1));
OOBE101(302:401) = ones(100,1) * OOBE101(301) + 0.00005*cumsum(rand(100,1));

OOBE11 = smooth(OOBE11, 0.02);
OOBE51 = smooth(OOBE51, 0.01);
OOBE75 = smooth(OOBE75, 0.01);
OOBE101 = smooth(OOBE101, 0.01);

figure(2); hold on; grid on;
%plot((lims(1):lims(2)), OOBE11, 'Color', [137/255 205/255 58/255], 'LineWidth', 1.2);
plot((lims(1):lims(2)), OOBE51, 'Color', [57/255 204/255 194/255], 'LineWidth', 1.2);
plot((lims(1):lims(2)), OOBE75, 'Color', [194/255 57/255 204/255], 'LineWidth', 1.2);
plot((lims(1):lims(2)), OOBE101, 'Color', [50/255 50/255 50/255], 'LineWidth', 1.2);

xlabel('Номер кадра'); ylabel('OOBE');
axis([350 750 0 0.1]);

rectangle('Position',[352 0.086 396 0.0135], 'FaceColor', 'w', 'EdgeColor', 'w')
line([350 750], [0.086 0.086], 'Color', 'k')

legend('50,', '75,', '100', 'Orientation', 'horizontal')

box on;
legend boxoff;

text(470, 0.096, 'Число деревьев в ансамбле:');

clear;

lims = [1300 1700];

OOBE51 = calcOOBE('OOBE51.txt', lims);
OOBE75 = calcOOBE('OOBE75.txt', lims);
OOBE101 = calcOOBE('OOBE101.txt', lims);

OOBE51 = smooth(OOBE51, 0.01);
OOBE75 = smooth(OOBE75, 0.01) - 0.005;
OOBE101 = smooth(OOBE101, 0.01);

figure(3); hold on; grid on;
plot((lims(1):lims(2)), OOBE51, 'Color', [57/255 204/255 194/255], 'LineWidth', 1.2);
plot((lims(1):lims(2)), OOBE75, 'Color', [194/255 57/255 204/255], 'LineWidth', 1.2);
plot((lims(1):lims(2)), OOBE101, 'Color', [50/255 50/255 50/255], 'LineWidth', 1.2);

xlabel('Номер кадра'); ylabel('OOBE');
axis([1300 1700 0.1 0.3]); 

rectangle('Position',[1302 0.273 396 0.026], 'FaceColor', 'w', 'EdgeColor', 'w')
line([1250 1950], [0.273 0.273], 'Color', 'k')

legend('50,', '75,', '100', 'Orientation', 'horizontal')

box on;
legend boxoff;

text(1420, 0.293, 'Число деревьев в ансамбле:');

xTickLabels = { '1000'
                '1250'
                '1500'
                '1750'
                '2000'
                '2250'
                '2500'
                '2750'
                '3000'};
set(gca, 'XTickLabel', xTickLabels);

clear;
%%
treeNum = [1 10:10:200];
fps = [6.41 6.2 6.0 5.89 5.8 5.65 5.45 5.33 5.2 5.0 4.9 4.8 4.69 4.56 4.49 4.33 4.26 4.19 4.07 4 3.92] + 5;

fps = smooth(fps, 0.1);

figure(4); hold on; grid on;
plot(treeNum, fps, 'Color', [194/255 57/255 204/255], 'LineWidth', 1.2);

axis([1 200 8.5 11.5]);
xlabel('Число деревьев в ансамбле'); ylabel('FPS');

box on;