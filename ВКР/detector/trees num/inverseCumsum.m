function array = inverseCumsum(array)
    for i = 1:length(array)
        if array(i) > 0
            array(i+1:end) = array(i+1:end) - array(i);
        end
    end
end