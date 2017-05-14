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
hist(H2, 0:11);
h = findobj(gca, 'Type', 'patch');
h.FaceColor = 'k';
h.EdgeColor = 'w';
axis([-0.5 10.5 0 14*10^4]);
text(k0-0.25, H1(k0+1)+5000, 'k_0');

figure(2);
hist(H2, 9:21);
h = findobj(gca, 'Type', 'patch');
h.FaceColor = 'k';
h.EdgeColor = 'w';
axis([9.5 20.5 0 250]);
text(k3-0.25, H1(k3+1)+10, 'k_3');

clear H1 H2;

scaledD = imread('current_deviation_image.jpg');
scaledD(D < k3) = 0;

figure(3);
imshow(scaledD);