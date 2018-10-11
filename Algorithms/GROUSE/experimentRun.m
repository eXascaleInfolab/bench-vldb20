function res = experimentRun(tcase, code, numr, numc, rt)
  # folders
  root = "./data/";
  fileprefix = strcat("in", "/", code, "_m");
  base = strcat(root, fileprefix);
  out = strcat(root, "out/", "grouse");
  
  # parameters
  truerank = 3;
  maxit = 100;
  maxrank = truerank;
  maxCycles = 5;
  step_size = 0.1;
  
  # impute
  matname = strcat(base, num2str(tcase), "_sparce.txt");
  A = dlmread(matname);
  
  I = A(:,1);
  J = A(:,2);
  S = A(:,3);
  
  id = tic();
  [Usg, Vsg, err_reg] = grouse(I,J,S,numr,numc,maxrank,step_size,maxCycles);
  timeSpan = toc(id) * 1000;
  
  impY = Usg * Vsg';
  
  # write
  matname = strcat(out, num2str(tcase));
  if (rt > 0)
    dlmwrite (strcat(matname, ".txt"), [timeSpan], " ");
  else
    dlmwrite (strcat(matname, ".txt"), impY', " "); # rewrites
  endif
  
  res = "OK";