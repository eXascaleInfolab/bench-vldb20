//
// Created on 04/01/19.
//

//
// Code translated to C++ from the original: https://github.com/lileicc/dynammo
//

#include "DynaMMo.h"
#include "../Algebra/Auxiliary.h"

namespace Algorithms
{

void DynaMMo::doDynaMMo(arma::mat &X, uint64_t H, uint64_t maxIter, bool FAST)
{
    arma::arma_rng::set_seed(18931);
    
    uint64_t N = X.n_cols;
    uint64_t M = X.n_rows;
    
    // lambda for stopping condition
    auto isTiny = [](const arma::mat &sigma) -> bool {
        return (arma::norm(sigma, 1) < 1e-10) || (arma::any(sigma.diag() < 1e-10));
    };
    
    // get number of hidden variables
    
    if (H == 0)
    {
        H = M;
    }
    
    // get the initial model
    DynaMMoModel model(
            arma::eye<arma::mat>(H, H) + arma::randn<arma::mat>(H, H),
            arma::eye<arma::mat>(M, H) + arma::randn<arma::mat>(M, H),
            arma::eye<arma::mat>(H, H),
            arma::eye<arma::mat>(M, M),
            arma::randn<arma::vec>(H),
            arma::eye<arma::mat>(H, H)
    );
    
    std::vector<arma::uvec> non_observed;
    for (uint64_t i = 0; i < X.n_rows; ++i)
    {
        non_observed.emplace_back(arma::find_nonfinite(X.row(i)));
    }
    
    arma::mat Y;
    
    Algebra::Algorithms::interpolate(X, true, &non_observed);
    
    uint64_t iter = 0;
    
    while ((iter < maxIter) && (!(isTiny(model.Q0) || isTiny(model.Q) || isTiny(model.R))))
    {
        iter = iter + 1;
        
        std::vector<arma::vec> mu;
        std::vector<arma::mat> V;
        std::vector<arma::mat> P;
        
        std::tie(mu, V, P) = forward(X, model, N, H, FAST);
        
        std::vector<arma::vec> Ez;
        std::vector<arma::mat> Ezz;
        std::vector<arma::mat> Ez1z;
        
        std::tie(Ez, Ezz, Ez1z) = backward(mu, V, P, model, N);
        
        MLE_lds(N, M, H, X, model, Ez, Ezz, Ez1z);
        
        Y.set_size(M, 0);
        estimate_missing(Y, N, Ez, model);
        
        for (uint64_t i = 0; i < X.n_rows; ++i)
        {
            for (uint64_t j : non_observed[i])
            {
                X.at(i, j) = Y.at(i, j);
            }
        }
    }
}

std::tuple<std::vector<arma::vec>, std::vector<arma::mat>, std::vector<arma::mat>>
DynaMMo::forward(const arma::mat &X, const DynaMMoModel &model, uint64_t N, uint64_t H, bool FAST)
{
    arma::mat Ih = arma::eye<arma::mat>(H, H);
    
    //predicted mean for hidden variable z
    std::vector<arma::vec> mu(N);
    std::vector<arma::mat> V(N);
    std::vector<arma::mat> P(N);
    
    mu[0] = model.mu0;
    V[0] = model.Q0;
    
    arma::mat invR;
    arma::mat invRC;
    arma::mat invCRC;
    if (FAST)
    {
        invR = arma::inv(model.R);
        invRC = invR * model.C;
        invCRC = model.C.t() * invRC;
    }
    
    for (uint64_t i = 0; i < N; ++i)
    {
        arma::mat KP;
        if (i == 0)
        {
            KP = model.Q0;
            mu[i] = model.mu0;
        }
        else
        {
            P[i - 1] = model.A * V[i - 1] * model.A.t() + model.Q;
            KP = P[i - 1];
            mu[i] = model.A * mu[i - 1];
        }
        
        arma::mat invSig;
        if (FAST)
        {
            arma::mat sol = arma::solve((arma::inv(KP) + invCRC).t(), invRC.t()).t();
            invSig = invR - sol * invRC.t();
        }
        else
        {
            arma::mat sigma_c = model.C * KP * model.C.t() + model.R;
            invSig = inv(sigma_c);
        }
        
        arma::mat K = KP * model.C.t() * invSig;
        arma::vec u_c = model.C * mu[i];
        arma::vec delta = X.col(i) - u_c;
        mu[i] = mu[i] + K * delta;
        V[i] = (Ih - K * model.C) * KP;
    }
    
    return std::make_tuple(mu, V, P);
}

std::tuple<std::vector<arma::vec>, std::vector<arma::mat>, std::vector<arma::mat>>
DynaMMo::backward(const std::vector<arma::vec> &mu, const std::vector<arma::mat> &V, const std::vector<arma::mat> &P,
                  const DynaMMoModel &model, uint64_t N)
{
    std::vector<arma::vec> Ez(N);
    std::vector<arma::mat> Ezz(N);
    std::vector<arma::mat> Ez1z(N);
    
    Ez[N - 1] = mu[N - 1];
    arma::mat Vhat(V[N - 1]);
    Ezz[N - 1] = Vhat + Ez[N - 1] * Ez[N - 1].t();
    
    for (uint64_t ii = N - 1; ii > 0; --ii)
    {
        uint64_t i = ii - 1;
        
        arma::mat J = arma::solve(P[i].t(), (V[i] * model.A.t()).t()).t();
        Ez[i] = mu[i] + J * (Ez[i + 1] - model.A * mu[i]);
        Ez1z[i] = Vhat * J.t() + Ez[i + 1] * Ez[i].t();
        Vhat = V[i] + J * (Vhat - P[i]) * J.t();
        Ezz[i] = Vhat + Ez[i] * Ez[i].t();
    }
    
    return std::make_tuple(Ez, Ezz, Ez1z);
}

void DynaMMo::MLE_lds(uint64_t N, uint64_t M, uint64_t H, const arma::mat &X, DynaMMoModel &model,
             std::vector<arma::vec> Ez, std::vector<arma::mat> Ezz, std::vector<arma::mat> Ez1z)
{
    arma::mat Sz1z = arma::zeros<arma::mat>(H, H);
    arma::mat Szz = arma::zeros<arma::mat>(H, H);
    arma::mat Sxz = arma::zeros<arma::mat>(M, H);
    
    for (uint64_t i = 0; i < N-1; ++i)
    {
        Sz1z = Sz1z + Ez1z[i];
    }
    
    for (uint64_t i = 0; i < N; ++i)
    {
        Szz += Ezz[i];
        Sxz += (X.col(i) * Ez[i].t());
    }
    
    arma::mat SzzN = Szz - Ezz[N - 1]; // sum of E[z, z] from 1 to n-1
    
    model.mu0 = Ez[0];
    
    model.Q0 = Ezz[0] - Ez[0] * Ez[0].t();
    model.Q0 = (arma::eye<arma::mat>(H, H) * (arma::trace(model.Q0) / (double)H));
    
    model.A = arma::solve(SzzN.t(), Sz1z.t()).t();
    
    {
        double delta = ((
                arma::trace(Szz)
                - arma::trace(Ezz[0])
                - 2 * arma::trace(model.A * Sz1z.t())
                + arma::trace(model.A * SzzN * model.A.t())
                        ) / (double)(N - 1)) / (double)H;
        model.Q = arma::eye<arma::mat>(H, H) * delta;
    }
    
    model.C = arma::solve(Szz.t(), Sxz.t()).t();
    
    {
        double delta = (
                arma::trace(X * X.t())
                - 2 * arma::trace(model.C * Sxz.t())
                + arma::trace(model.C * Szz * model.C.t())
                       ) / (double)N / (double)M;
        
        model.R = arma::eye<arma::mat>(M, M) * delta;
    }
}

void DynaMMo::estimate_missing(arma::mat &Y, uint64_t N, const std::vector<arma::vec> &Ez, const DynaMMoModel &model)
{
    for (uint64_t i = 0; i < N; ++i)
    {
        Y.insert_cols(i, model.C * Ez[i]);
    }
}

} // namespace Algorithms
