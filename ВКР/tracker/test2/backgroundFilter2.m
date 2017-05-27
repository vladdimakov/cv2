clear; clc; close all;

D = imread('current_deviation_image.bmp');

[H1,~] = imhist(D);
[k0,k3] = getBackgroundBound(H1);

H2 = [];
for i = 1:size(D,1)
    for j = 1:size(D,2)
        H2(end+1) = D(i,j);
    end
end

figure(1);
hist(H2, 0:10);
h = findobj(gca, 'Type', 'patch');
h.FaceColor = 'k';
h.EdgeColor = 'w';
axis([-0.5 7.5 0 14*10^4]);
text(k0-0.25, H1(k0+1)+7000, '\nu_0', 'FontSize', 14);
yTickLabels = { '0'
                '20000'
                '40000'
                '60000'
                '80000'
                '100000'
                '120000'
                '140000'};
set(gca, 'YTickLabel', yTickLabels);


H2 = [H2 16*ones(1,10)];
H2 = [H2 17*ones(1,10)];
H2 = [H2 18*ones(1,35)];
H2 = [H2 19*ones(1,35)];
H2 = [H2 20*ones(1,40)];

figure(2);
hist(H2, 9:21);
h = findobj(gca, 'Type', 'patch');
h.FaceColor = 'k';
h.EdgeColor = 'w';
axis([9.5 20.5 0 250]);
text(k3-0.2, H1(k3+1)+15, '\omega', 'FontSize', 17);

clear H1 H2;