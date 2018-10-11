%Simulation on updating w (works better for data with mean-zero)
%function [W, k, Proj, recon] = SPIRIT(A, lambda, energy, k0, holdOffTime)
%dynamic maintain the number of output units according to the energy level
%Inputs:
%A: data matrix
%-lambda: forgetting factor between [0,1], 1 means putting equal weight on
%         the past, otherwise the past is exponentially decayed by lambda
%-energy: an interval within  [0,1], which specifies the upper and lower bounds of energy level
%-k0: initial number of hidden variables, default=3
%-holdOffTime: the minimal timestamps before next change on the
%              number of hidden variables
%
%Output: 
%-W: eigvector matrix such as 1st eigvector W(:,1), kth eigvector W(:,k)
%-k: the max number of eigenvectors,
%-Proj: the lowe-dim projections;
%-recon: the reconstructions of A; 
%Example:
% >> A = sin((1:1000)/20)'*rand(1,10);
% >> plot(A)
% >> A = [sin((1:1000)/10) sin((1:1000)/50)]'*rand(1,10);
% >> plot(A)
% >> [W,k,Proj,recon] = SPIRIT(A,1,[0.95,0.98]);
% Decreasing m to 2 at time 12 (ratio 205.84)
% Decreasing m to 1 at time 23 (ratio 175.68)
% >> plot(recon)
%----------------------------------------------------------------
% Copyright: 2006,
%         Spiros Papadimitriou, Jimeng Sun, Christos Faloutsos.
%         All rights reserved.
% Please address questions or comments to: jimeng@cs.cmu.edu
%----------------------------------------------------------------
function [W,  k, Proj, recon] = SPIRIT(A, lambda, energy, k0, holdOffTime)

if nargin < 5, holdOffTime = 10; end
if nargin < 4, k0 = 3; end

n = size(A,2);
totalTime = size(A, 1);
Proj = zeros(totalTime,n); 
recon = zeros(totalTime,n);
%initialize w_i to unit vectors
W = eye(n);
d = 0.01*ones(n, 1);
m = k0; % number of eigencomponents

relErrors = zeros(totalTime, 1);

sumYSq=0;
sumXSq=0;

%incremental update W
lastChangeAt = 1;
for t = 1:totalTime
  % update W for each y_t
  x = A(t,:)';
  for j = 1:m
     [W(:,j), d(j), x] = updateW(x, W(:,j), d(j), lambda);
     Wj = W(:,j);
  end
  W(:,1:m) = grams(W(:,1:m));
  %compute low-D projection, reconstruction and relative error
  Y = W(:,1:m)' * A(t,:)'; %project to m-dimensional space
  xActual = A(t,:)'; %actual vector of the current time
  xProj = W(:,1:m) * Y; %reconstruction of the current time
  Proj(t,1:m) = Y; 
  recon(t,:) = xProj;
  xOrth = xActual - xProj;
  relErrors(t) = sum(xOrth.^2)/sum(xActual.^2);

  %update energy
  sumYSq = lambda * sumYSq + sum(Y.^2);
  sumXSq = lambda * sumXSq + sum(A(t,:).^2);
  % check the lower bound of energy level
  if(sumYSq < energy(1)*sumXSq && lastChangeAt < t - holdOffTime && m < n)
    lastChangeAt = t;
    m = m+1;
    fprintf('Increasing m to %d at time %d (ratio %6.2f)\n', m, t, 100*sumYSq/sumXSq);
  % check the upper bound of energy level
  else if (sumYSq > energy(2)*sumXSq && lastChangeAt < t - holdOffTime && m < n && m>1)
    lastChangeAt = t;
    m = m-1;
    fprintf('Decreasing m to %d at time %d (ratio %6.2f)\n', m, t, 100*sumYSq/sumXSq);  
    end
  end
  
end

% set outputs
W(:,1:m) = grams(W(:,1:m));
W = W(:,1:m);
k = m;
errs = relErrors;

