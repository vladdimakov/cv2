function OOBE = calcOOBE(fileName, lims)
    data = readArray(fileName);
    correctlyClassifiedNum = inverseCumsum(data(1:2:end));
    incorrectlyClassifiedNum = inverseCumsum(data(2:2:end));

    OOBE = cumsum(incorrectlyClassifiedNum) ./ ...
        (cumsum(correctlyClassifiedNum) + cumsum(incorrectlyClassifiedNum));

    OOBE = OOBE(lims(1):lims(2));
end