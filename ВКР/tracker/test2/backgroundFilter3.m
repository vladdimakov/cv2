clear; clc; close all;


H1 = randn(10000,1)*15 + 40;
H1 = H1 - 20;

H2 = randn(13000,1)*40 + 140;

H2(H2<60) = [];

figure(1); hold on;
hist(H1, 0:10:255);
h = findobj(gca, 'Type', 'patch');
h.FaceColor = 'r';
h.EdgeColor = 'w';

hist(H2, 0:10:255);
h = findobj(gca, 'Type', 'patch');
h(1).FaceColor = 'b';
h(1).EdgeColor = 'w';

xlim([-5 255.5])