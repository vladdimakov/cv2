clear; clc; close all;

N = 1000;
gain = 100;

OOBE_t = [1/10 1/3 1/5 1];

res = [];
for i = 1:N
    for j = 1:length(OOBE_t)
        if rand * gain < OOBE_t(j)
            res = [res j];
        end
    end
end

res

