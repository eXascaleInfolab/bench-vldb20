function res = recovery(algcode, filename_input, filename_output, runtime)
    
    % calling recovery function
    function mx_rec = recover_matrix(mx_mis)
    
        mx_rec = meanimpute(mx_mis);
        
    end
    
    % read input matrix
    matrix = dlmread(filename_input);
    
    % beginning of imputation process - start time measurement
    id = tic();
    
    matrix_imputed = recover_matrix(matrix);
    
    % imputation is complete - stop time measurement and calculate the time elapsed in [!] microseconds
    exec_time = toc(id) * 1000 * 1000;
        
    % verification
    nan_mask = isnan(matrix_imputed);
    %matrix_imputed[nan_mask] = sqrt(np.finfo('d').max / 100000.0);
    
    printf(strcat("Time (", algcode, "): ", num2str(exec_time), "\n"));
    
    % we use a binary flag to indicate whether we need runtime results or algorithm output
    if (runtime > 0)
        % if we need runtime, we only save one value with the time in microseconds
        dlmwrite (filename_output, [exec_time], " ");
    else
        % if we need the output, we instead save the matrix with imputed values to the same file
        dlmwrite (filename_output, matrix_imputed, " ");
    endif
  
    res = "OK";
end