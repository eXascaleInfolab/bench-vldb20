function res = experimentRun(tcase, code, rt)
  # folders
  root = "./";
  fileprefix = strcat("in", "/", code, "_m");
  base = strcat(root, fileprefix);
  out = strcat(root, "out/", "trmf");
  
  # parameters
  lag_idx = [1:10];
  maxit = 100;
    
  # impute
  matname = strcat(base, num2str(tcase));
  [timeSpan, impY] = test_single(matname, lag_idx, maxit);
  
  # write
  matname = strcat(out, num2str(tcase));
  if (rt > 0)
    dlmwrite (strcat("../data/", matname, ".txt"), [timeSpan], " ");
  else
    dlmwrite (strcat("../data/", matname, ".txt"), impY, " "); # rewrites
  endif
  
  res = "OK";
