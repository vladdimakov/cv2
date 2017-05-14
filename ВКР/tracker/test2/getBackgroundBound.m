function [k0,k3] = getBackgroundBound(H)
	startInd = 2;
	if H(1) <= H(2)
        for startInd = 2:256
            if H(startInd+1) < H(startInd)
				break;
            end
        end
    end
    k0 = startInd - 1;
    
    endInd = 2;
	for endInd = startInd:256
		if H(endInd+1) >= H(endInd)
            break;
		end
    end
    k3 = endInd - 1;
end