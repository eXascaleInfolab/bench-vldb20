function [ret] = experimentRun(tcase, code, rt)
  # folders
  root = "data/";
  fileprefix = strcat(code, "_m");
  
  # auto
  base = strcat(root, "in/", fileprefix);
  out = strcat(root, "out/", "spirit");

  # params
  lambda = 1.0;
  k0 = 3;
  w = 6;
  
  i = tcase;
  rangefile = strcat(base, num2str(i), "_range", ".txt");
  rangeMat = dlmread(rangefile);
  missingrange = [rangeMat(1,1):rangeMat(1,2)];
  
  if (missingrange != 0)
    matname = strcat(base, num2str(i), ".txt");
    A = dlmread(matname); # should have zeroes for missing values
    
    A(missingrange,1) = ones(size(missingrange,1)) * A(missingrange(1,1)-1,1);

    [imputed, rmse, timeSpan] = SPIRIT2_fixed(A, w, lambda, missingrange, k0, rt);
    
    A(missingrange,1) = imputed(missingrange,1);
  endif
  
  matname = strcat(out, num2str(i), ".txt");
  if (rt > 0)
    dlmwrite(matname, [timeSpan], " ");
  else
    dlmwrite(matname, A, " ");
  endif

  ret = "OK";
