clear; clc; close all;

lims = [1 2000];

methodError0 = calcMethodError('methodError1.txt', lims);
methodError5000 = calcMethodError('methodError2.txt', lims);

error1Method0 = (methodError0(1133) - methodError0(952)) / (1133 - 952);
error1Method5000 = (methodError0(1510) - methodError0(1133)) / (1510 - 1133);
error2Method0 = (methodError0(2000) - methodError0(1240)) / (2000 - 1240);
error2Method5000 = (methodError5000(2000) - methodError5000(1240)) / (2000 - 1240);
error3Method5000 = (methodError5000(1811) - methodError5000(1528)) / (1811 - 1528);

error1Method0 = error1Method0 * 100;
error1Method5000 = error1Method5000 * 100;
error2Method0 = error2Method0 * 100;
error2Method5000 = error2Method5000 * 100;
error3Method0 = 100;
error3Method5000 = error3Method5000 * 100;

fprintf('| %.2f | %.2f | %.2f |\n', error1Method0, error2Method0, error3Method0);
fprintf('| %.2f | %.2f | %.2f |\n', error1Method5000, error2Method5000, error3Method5000);
 


