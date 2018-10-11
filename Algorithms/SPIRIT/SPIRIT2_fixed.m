% This is my adaption of the script SPIRIT_fixed.m obtained from
% https://www.cs.cmu.edu/afs/cs/project/spirit-1/www/
%
% Direct download:
% https://www.cs.cmu.edu/afs/cs/project/spirit-1/www/code/SPIRIT.zip
%
% This is the original copyright notice:
%----------------------------------------------------------------
% Copyright: 2006,
%         Spiros Papadimitriou, Jimeng Sun, Christos Faloutsos.
%         All rights reserved.
% Please address questions or comments to: jimeng@cs.cmu.edu
%----------------------------------------------------------------
%
% Parameters:
%   A: input matrix
%   w: order of the autogressive models
%   lambda: exponential forgetting factor
%   missing: an array [a,b] to simulate missing values from time a to b
%   k0: the number of hidden variables
%
% Return values:
%   imputed: the imputed time series
%   rmse: the root mean square error
%   W: eigvector matrix such as 1st eigvector W(:,1), kth eigvector W(:,k)
%   Proj: the lowe-dimensionality projections;
%   errs: the errors
%   imputed: all reconstructed time series

function [imputed, rmse, runtime, W, Proj, errs, recon] = SPIRIT2_fixed(A, w, lambda, missing, k0, stream)
myA = A;

n = size(A,2); % number of time series
totalTime = size(A, 1);
Proj = zeros(totalTime,n);
recon = zeros(totalTime,n);

%initialize w_i to unit vectors
W = eye(n);
d = 0.01*ones(n, 1);
m = k0; % number of eigencomponents

relErrors = zeros(totalTime, 1);

prevW = W;
Yvalues = zeros(totalTime, m);
ARc = zeros(w, m); % AR coefficients, one for each hidden variable
G = zeros(w,w,m);  % "Gain-Matrix", one for each hidden variable

% initialize the "Gain-Matrix" with the identity matrix
for j = 1:m
  G(:,:,j) = eye(w) * inv(0.004);
end

blockStart = missing(1);
blockEnd = missing(2);

%incremental update W
if (stream < 2)
  tic_id = tic();
endif

for t = 1:totalTime
  #[WARNING] this is quite fucky when it comes to incremental stuff
  #          the measurement position below is suitable for incremental stuff, not not "from scratch"
  #          brace yourself

  if stream > 1 && t == blockStart
    tic_id_str = tic();
  end

  % Simulate a missing block
  if blockStart <= t && t <= blockEnd

    % one-step forecast for each y-value
    Y = zeros(m,1);
    for j = 1:m
      xj = Yvalues(t-w:t-1,j)';
      aj = ARc(:,j);
      Y(j) = xj * aj; % Eq 1 in Muscles paper
    end

    % estimate the missing value
    xProj = prevW(:,1:m) * Y; % reconstruction of the current time
    A(t,1) = xProj(1); % feed back imputed value
  end

  % update W for each y_t
  x = A(t,:)';
  for j = 1:m
     [W(:,j), d(j), x] = updateW(x, W(:,j), d(j), lambda);
  end
  W(:,1:m) = grams(W(:,1:m));
  %compute low-D projection, reconstruction and relative error
  Y = W(:,1:m)' * A(t,:)'; %project to m-dimensional space

  prevW = W;
  Yvalues(t,:) = Y;

  xActual = A(t,:)'; %actual vector of the current time
  xProj = W(:,1:m) * Y; %reconstruction of the current time
  Proj(t,1:m) = Y;
  recon(t,:) = xProj;
  xOrth = xActual - xProj;
  relErrors(t) = sum(xOrth.^2)/sum(xActual.^2);

  % update the AR coefficients for each hidden variable
  if t > w
    % we can start only when we have seen w measurements
    for j = 1:m
      xj = Yvalues(t-w+1:t,j)';
      yj = Yvalues(t,j);
      aj = ARc(:,j);
      Gj = G(:,:,j);

      Gj = (1/lambda)*Gj - (1/lambda)*inv(lambda + xj * Gj * xj') * (Gj * xj') * (xj * Gj);
      aj = aj - Gj * xj' * (xj * aj - yj);

      G(:,:,j) = Gj;
      ARc(:,j) = aj;
    end
  end

end

if (stream < 2)
  runtime = toc(tic_id);
  runtime = (runtime * 1000.0); #/ double(blockEnd - blockStart);
  fprintf('(spirit) runtime: %f\n', runtime);
else
  runtime = toc(tic_id_str);
  runtime = (runtime * 1000.0); #/ double(blockEnd - blockStart);
  fprintf('(spirit) streaming runtime: %f\n', runtime);
endif

% set outputs
W(:,1:m) = grams(W(:,1:m));
W = W(:,1:m);
errs = relErrors;
rmse = sqrt(mean((recon(blockStart:blockEnd,1) - myA(blockStart:blockEnd,1)).^2));
imputed = recon(:,1);
