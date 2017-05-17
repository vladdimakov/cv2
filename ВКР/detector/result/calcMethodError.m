function methodError = calcMethodError(fileName, lims)
    data = readArray(fileName);
    data = data(lims(1):lims(2));
    methodError = cumsum(data);
end