function res = meanimpute(matrix)
    msize = size(matrix);
    
    mask = isnan(matrix);
    count = sum(~mask);
    
    matrix(mask) = 0.0;
    mean = sum(matrix) ./ count;
    
    for i = 1:msize(1),
        for j = 1:msize(2),
            if (mask(i,j) == 1)
                matrix(i,j) = mean(j);
            end
        end
    end
    
    res = matrix;
end