function [timeSpan, impY] = test_single(matname, lag_idx, maxit)
  #usage example
  #impY = test_single("bball/bball_trmf", [1:10], 100);
  M_full = dlmread(strcat("../data/", matname, "_obs.txt"));
  M_obs = dlmread(strcat("../data/", matname, "_indic.txt"));
  
  n = columns(M_obs);
  T = rows(M_obs);
  k = 3;
  
  id = tic();
  ret = imputation_trmf(M_full, M_obs, lag_idx, k, [0.75, 0.75, 0.75], maxit);
  timeSpan = toc(id) * 1000 * 1000;
  
  printf(num2str(timeSpan)); printf("\n");
  
  impF = ret.model.F;
  impX = ret.model.X;
  impY = impF * impX;
