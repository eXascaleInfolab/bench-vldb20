//
// Created by zakhar on 14.03.18.
//

#include <cmath>
#include "SVDecomposition.h"

using namespace LinearAlgebra_Basic;

// START CODE FROM: http://svn.lirec.eu/libs/magicsquares/src/SVD.cpp
// Copyright (c) held by Dianne Cook
// modified to integrate into the project structure

namespace LinearAlgebra_Algorithms {

#define SIGN(a, b) ((b) >= 0.0 ? fabs(a) : -fabs(a))
#define MAX(x, y) ((x) > (y) ? (x) : (y))

double PYTHAG(double a, double b)
{
    double at = fabs(a), bt = fabs(b), ct, result;

    if (at > bt)       { ct = bt / at; result = at * sqrt(1.0 + ct * ct); }
    else if (bt > 0.0) { ct = at / bt; result = bt * sqrt(1.0 + ct * ct); }
    else result = 0.0;
    return(result);
}

int64_t SVDecomposition::SVDecompose(Matrix &u, Vector &sigma, Matrix &v)
{
    uint64_t n = u.dimensionN();
    uint64_t m = u.dimensionM();

    bool flag;
    uint64_t l = 0, nm = 0;
    double c, f, h, s, x, y, z;
    double anorm = 0.0, g = 0.0, scale = 0.0;

    if (n < m)
    {
        return -1; // fprintf(stderr, "#rows must be > #cols \n");
    }

    Vector rv1 = Vector(m, true);

    /* Householder reduction to bidiagonal form */
    for (uint64_t i = 0; i < m; i++)
    {
        /* left-hand reduction */
        l = i + 1;
        rv1[i] = scale * g;
        g = s = scale = 0.0;
        if (i < n)
        {
            for (uint64_t k = i; k < n; k++)
            {
                scale += fabs(u(k, i));
            }

            if (scale != 0.0)
            {
                for (uint64_t k = i; k < n; k++)
                {
                    u(k, i) = (u(k, i) / scale);
                    s += (u(k, i) * u(k, i));
                }
                f = u(i, i);
                g = -SIGN(sqrt(s), f);
                h = f * g - s;
                u(i, i) = (f - g);
                if (i != m - 1)
                {
                    for (uint64_t j = l; j < m; j++)
                    {
                        uint64_t k;
                        for (s = 0.0, k = i; k < n; k++)
                        {
                            s += (u(k, i) * u(k, j));
                        }

                        f = s / h;

                        for (k = i; k < n; k++)
                        {
                            u(k, j) += (f * u(k, i));
                        }
                    }
                }
                for (uint64_t k = i; k < n; k++)
                {
                    u(k, i) = (u(k, i) * scale);
                }
            }
        }
        sigma[i] = (scale * g);

        /* right-hand reduction */
        g = s = scale = 0.0;
        if (i < n && i != m - 1)
        {
            for (uint64_t k = l; k < m; k++)
            {
                scale += fabs(u(i, k));
            }

            if (scale != 0.0)
            {
                for (uint64_t k = l; k < m; k++)
                {
                    u(i, k) = (u(i, k) / scale);
                    s += (u(i, k) * u(i, k));
                }
                f = u(i, l);
                g = -SIGN(sqrt(s), f);
                h = f * g - s;
                u(i, l) = (f - g);
                for (uint64_t k = l; k < m; k++)
                {
                    rv1[k] = u(i, k) / h;
                }

                if (i != n - 1)
                {
                    for (uint64_t j = l; j < n; j++)
                    {
                        uint64_t k;
                        for (s = 0.0, k = l; k < m; k++)
                        {
                            s += (u(j, k) * u(i, k));
                        }

                        for (k = l; k < m; k++)
                        {
                            u(j, k) += (s * rv1[k]);
                        }
                    }
                }

                for (uint64_t k = l; k < m; k++)
                {
                    u(i, k) = (u(i, k) * scale);
                }
            }
        }
        anorm = MAX(anorm, (fabs(sigma[i]) + fabs(rv1[i])));
    }

    uint64_t minusone = static_cast<uint64_t >(-1);

    /* accumulate the right-hand transformation */
    for (uint64_t i = m - 1; i != minusone; i--)
    {
        if (i < m - 1)
        {
            if (g != 0.0)
            {
                for (uint64_t j = l; j < m; j++)
                {
                    v(j, i) = ((u(i, j) / u(i, l)) / g);
                }
                /* double division to avoid underflow */
                for (uint64_t j = l; j < m; j++)
                {
                    uint64_t k;

                    for (s = 0.0, k = l; k < m; k++)
                    {
                        s += (u(i, k) * v(k, j));
                    }

                    for (k = l; k < m; k++)
                    {
                        v(k, j) += (s * v(k, i));
                    }
                }
            }

            for (uint64_t j = l; j < m; j++)
            {
                v(i, j) = v(j, i) = 0.0;
            }
        }
        v(i, i) = 1.0;
        g = rv1[i];
        l = i;
    }

    /* accumulate the left-hand transformation */
    for (uint64_t i = m - 1; i != minusone; i--)
    {
        l = i + 1;
        g = sigma[i];
        if (i < m - 1)
        {
            for (uint64_t j = l; j < m; j++)
            {
                u(i, j) = 0.0;
            }
        }

        if (g != 0.0)
        {
            g = 1.0 / g;

            if (i != m - 1)
            {
                for (uint64_t j = l; j < m; j++)
                {
                    uint64_t k;

                    for (s = 0.0, k = l; k < n; k++)
                    {
                        s += (u(k, i) * u(k, j));
                    }
                    f = (s / u(i, i)) * g;
                    for (k = i; k < n; k++)
                    {
                        u(k, j) += (f * u(k, i));
                    }
                }
            }

            for (uint64_t j = i; j < n; j++)
            {
                u(j, i) = (u(j, i) * g);
            }
        }
        else
        {
            for (uint64_t j = i; j < n; j++)
            {
                u(j, i) = 0.0;
            }
        }
        ++u(i, i);
    }

    /* diagonalize the bidiagonal form */
    for (uint64_t k = m - 1; k != minusone; k--)
    {                             /* loop over singular values */
        for (uint16_t its = 0; its < 30; its++)
        {                         /* loop over allowed iterations */
            flag = true;
            for (l = k; l != minusone; l--)
            {                     /* test for splitting */
                nm = l - 1;
                if (fabs(rv1[l]) + anorm == anorm)
                {
                    flag = false;
                    break;
                }
                if (fabs(sigma[nm]) + anorm == anorm)
                {
                    break;
                }
            }
            if (flag)
            {
                c = 0.0;
                s = 1.0;
                for (uint64_t i = l; i <= k; i++)
                {
                    f = s * rv1[i];
                    if (fabs(f) + anorm != anorm)
                    {
                        g = sigma[i];
                        h = PYTHAG(f, g);
                        sigma[i] = h;
                        h = 1.0 / h;
                        c = g * h;
                        s = (-f * h);
                        for (uint64_t j = 0; j < n; j++)
                        {
                            y = u(j, nm);
                            z = u(j, i);
                            u(j, nm) = (y * c + z * s);
                            u(j, i) = (z * c - y * s);
                        }
                    }
                }
            }
            z = sigma[k];
            if (l == k)
            {                  /* convergence */
                if (z < 0.0)
                {              /* make singular value nonnegative */
                    sigma[k] = (-z);
                    for (uint64_t j = 0; j < m; j++)
                    {
                        v(j, k) = (-v(j, k));
                    }
                }
                break;
            }
            if (its >= 30)
            {
                return -2; // fprintf(stderr, "No convergence after 30,000! iterations \n");
            }

            /* shift from bottom 2 x 2 minor */
            x = sigma[l];
            nm = k - 1;
            y = sigma[nm];
            g = rv1[nm];
            h = rv1[k];
            f = ((y - z) * (y + z) + (g - h) * (g + h)) / (2.0 * h * y);
            g = PYTHAG(f, 1.0);
            f = ((x - z) * (x + z) + h * ((y / (f + SIGN(g, f))) - h)) / x;

            /* next QR transformation */
            c = s = 1.0;
            for (uint64_t j = l; j <= nm; j++)
            {
                uint64_t i = j + 1;
                g = rv1[i];
                y = sigma[i];
                h = s * g;
                g = c * g;
                z = PYTHAG(f, h);
                rv1[j] = z;
                c = f / z;
                s = h / z;
                f = x * c + g * s;
                g = g * c - x * s;
                h = y * s;
                y = y * c;
                for (uint64_t jj = 0; jj < m; jj++)
                {
                    x = v(jj, j);
                    z = v(jj, i);
                    v(jj, j) = (x * c + z * s);
                    v(jj, i) = (z * c - x * s);
                }
                z = PYTHAG(f, h);
                sigma[j] = z;
                if (z != 0.0)
                {
                    z = 1.0 / z;
                    c = f * z;
                    s = h * z;
                }
                f = (c * g) + (s * y);
                x = (c * y) - (s * g);
                for (uint64_t jj = 0; jj < n; jj++)
                {
                    y = u(jj, j);
                    z = u(jj, i);
                    u(jj, j) = (y * c + z * s);
                    u(jj, i) = (z * c - y * s);
                }
            }
            rv1[l] = 0.0;
            rv1[k] = f;
            sigma[k] = x;
        }
    }
    return 0;
}

// END CODE FROM: http://svn.lirec.eu/libs/magicsquares/src/SVD.cpp

} // namespace LinearAlgebra_Algorithms