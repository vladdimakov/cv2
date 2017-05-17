function array = readArray(fileName)
    fileId = fopen(fileName, 'r');
    
    array = fscanf(fileId, '%f');
    
    fclose(fileId);
end