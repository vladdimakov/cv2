function methodError = calcMethodError(fileName, lims)
    data = readArray(fileName);
    methodError = cumsum(data);
    methodError = methodError(lims(1):lims(2));
end