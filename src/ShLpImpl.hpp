// Sh: A GPU metaprogramming language.
//
// Copyright (c) 2003 University of Waterloo Computer Graphics Laboratory
// Project administrator: Michael D. McCool
// Authors: Zheng Qin, Stefanus Du Toit, Kevin Moule, Tiberiu S. Popa,
//          Michael D. McCool
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must
// not claim that you wrote the original software. If you use this
// software in a product, an acknowledgment in the product documentation
// would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must
// not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
// distribution.
//////////////////////////////////////////////////////////////////////////////
#ifndef SHLPIMPL_HPP
#define SHLPIMPL_HPP

#include "ShDebug.hpp"
#include "ShAttrib.hpp"
#include "ShUtility.hpp"
#include "ShMultiArray.hpp"
#include "ShLp.hpp"

//#define SH_DBG_LP

#ifdef SH_DBG_LP
#define SH_DEBUG_PRINT_LP(x) { SH_DEBUG_PRINT(x); }
#else
#define SH_DEBUG_PRINT_LP(x) {}
#endif

namespace SH {

// @todo range eps
#define _SH_LP_EPS 1e-7 

// @todo range the reason why the A input to the publically visible functions
// are valarray<vector<T>> is because in general use valarray<valarray<T>>
// is not actually supposed to work (since valarray's default ctor + operator= 
// does not act the same as its copy ctor).  Internally though we are
// very careful to never create a copy of A or any of its rows.  
// Note that while this works with libstdc++, it may not be portable.
//
// This could be fixed if gslice_arrays could also handle mutating assignment
// ops (/=, *=), because then A could just be a 2D array packed into a
// valarray<T>.
//
// (or if Sh types and swizzles were more efficient) 

template<typename T>
std::ostream& operator<<(std::ostream& out, const std::valarray<std::valarray<T> >& a)
{
  for(size_t i = 0; i < a.size(); ++i) {
    out << a[i] << std::endl;
  }
  return out;
}


/* Pivots non-basic variable e with basic variable l. 
 * nonBasic[i] stores non-basic index in column i of A 
 * basic[j] stores the basic variable index with constrasize_t in row j of A 
 *
 * Straight from CLRS (but a little cleaner).  Although they don't state it
 * explicitly, their pseudocode is very careful about allowing an in-place
 * algorithm to work.
 *
 * All of the functions can work with a matrix A with NN >= N columns.
 * (It just ignores the extra columns)
 * 
 * @param A MxP matrix where P > N (constraint equations) 
 * @param b M-vector (constants in constraint equations)
 * @param c N-vector (objective function coefficients)
 * @param v constant in objective function
 * @param l, e basic and non-basic variables to swap (0 <= l < M, 0 <= e < N)
 */
template<typename T>
void shLpPivot(size_t M, size_t N, std::valarray<size_t>& nonBasic, std::valarray<size_t>& basic, 
    std::valarray<std::valarray<T> >& A, std::valarray<T> &b, 
    std::valarray<T> &c, T& v, size_t l, size_t e)
{
  // Update equation for x_l by solving for x_e 
  T invAle = 1.0 / A[l][e];
  b[l] *= invAle; 
  A[l] *= invAle; 
  A[l][e] = invAle; 

  // Update coefficients by substituting new x_e = ... equation from above 
  for(size_t i = 0; i < M; ++i) {
    if(i != l) {
      T Aie = A[i][e];
      b[i] -= Aie * b[l];
      A[i] -= Aie * A[l];
      A[i][e] = -Aie * A[l][e]; 
    }
  }

  // Revise objective function
  T ce = c[e];
  v += ce * b[l];
  c -= ce * A[l];
  c[e] = -ce * A[l][e];

  std::swap(nonBasic[e], basic[l]);
}

/* Core while loop of the simplex algorithm.
 */
template<typename T>
void shLpSimplexCore(size_t M, size_t N, std::valarray<size_t>& nonBasic, std::valarray<size_t>& basic, 
    std::valarray<std::valarray<T> >& A, std::valarray<T> &b, 
    std::valarray<T> &c, T& v)
{
  size_t i;
  for(;;) {
    // Select the largest indexed nonbasic var to pivot (Bland's rule to prevent cycling)
    size_t e, maxIdx;
    e = N;
    maxIdx = 0; 
    for(i = 0; i < N; ++i) {
      if(c[i] > _SH_LP_EPS && nonBasic[i] >= maxIdx) {
        e = i;
        maxIdx = nonBasic[i];
      }
    }
    if(e == N) return; 

    // select the basic var to pivot (once again selecting max index to break
    // ties)
    T min = 0; // doesn't matter, since l == -1 checked below, but this removes warning
    int l = -1;
    for(i = 0; i < M; ++i) {
      if(A[i][e] < _SH_LP_EPS) continue; // @todo range should be <= EPS
      T deltaie = b[i] / A[i][e];
      if(l == -1 || deltaie < min - _SH_LP_EPS || 
        (deltaie < min + _SH_LP_EPS && basic[i] > maxIdx)) {
        min = deltaie;
        maxIdx = basic[i];
        l = i;
      }
    }
    if(l == -1) throw ShUnboundedLpException();
    shLpPivot(M, N, nonBasic, basic, A, b, c, v, l, e); 

    SH_DEBUG_PRINT_LP("After core iteration (swapped l=" << l << " e=" << e << "):\n A = \n" << A << "\n b = " << b << "\n c = " << c << "\n v = " << v 
        << std::endl << "\n nonBasic =" << nonBasic << "\n basic = " << basic); 
  }
}

template<typename T>
void shLpSimplexInit(size_t M, size_t N, std::valarray<size_t>& nonBasic, std::valarray<size_t>& basic, 
    std::valarray<std::valarray<T> >& A, std::valarray<T> &b, 
    std::valarray<T> &c, T& v)
{
  size_t i, j, k;

  // initialize nonBasic, basic
  nonBasic.resize(N);
  basic.resize(M);
  for(i = 0; i < N; ++i) nonBasic[i] = i; 
  for(i = 0; i < M; ++i) basic[i] = i + N;
  v = 0; 

  SH_DEBUG_PRINT_LP("Initializing lp \n A = \n" << A << "\n b = " << b << "\n c = " << c << "\n v = " << v 
      << std::endl << "\n nonBasic =" << nonBasic << " basic = " << basic); 

  // check if initial solution is feasible?
  int l = 0;
  for(i = 1; i < M; ++i) if(b[i] < b[l]) l = i;
  if(b[l] >= -_SH_LP_EPS) {
  //if(b[l] >= ) {
    SH_DEBUG_PRINT_LP("Initially feasible");
    return;  // @todo range should be >= -EPS
  }

  // Form auxilliary problem, and solve 
  std::valarray<T> cp(static_cast<T>(0), N+1);
  cp[N] = -1;

  size_t SPECIAL = M + N;

  std::valarray<size_t> nonBasicp(SPECIAL, N+1);
  nonBasicp[std::slice(0, N, 1)] = nonBasic;

  SH_DEBUG_PRINT_LP("Set up auxiliarry problem:\n A = \n" << A << "\n b = " << b << "\n cp = " << cp << "\n v = " << v 
      << std::endl << "\n nonBasicp =" << nonBasicp << " basic = " << basic); 

  shLpPivot(M, N + 1, nonBasicp, basic, A, b, cp, v, l, N);

  SH_DEBUG_PRINT_LP("After initial pivot:\n A = \n" << A << "\n b = " << b << "\n cp = " << cp << "\n v = " << v 
      << std::endl << "\n nonBasicp =" << nonBasicp << " basic = " << basic); 

  shLpSimplexCore(M, N + 1, nonBasicp, basic, A, b, cp, v);

  // check if the special variable is set to 0 in the solution 
  for(i = 0; i < N + 1 && nonBasicp[i] != SPECIAL; ++i); 
  if(i == N + 1) throw ShInfeasibleLpException();

  // map results back to original 
  
  // Fix A
  for(j = 0; j < M; ++j) {
    std::swap(A[j][i], A[j][N]);
  }

  // Fix b
  for(j = 0; j < N; ++j) {
    if(i == j) {
      nonBasic[j] = nonBasicp[N];
    } else {
      nonBasic[j] = nonBasicp[j];
    }
  }

  // fix objective function, c
  // For non-basic vars that are still nonbasic, we might just have to shift the coeff
  // For non-basic vars that have become basic, we have to substitute in their constraint equation 
  std::valarray<T> oldc = c;
  c = 0; 
  for(j = 0; j < N; ++j) {
    if(nonBasic[j] < N) {
      c[j] = oldc[nonBasic[j]]; 
    }
  }
  ShSwizzle firstN(N);
  for(j = 0; j < M; ++j) {
    if(basic[j] < N) {
      for(k = 0; k < N; ++k) {
        c[k] -= oldc[basic[j]] * A[j][k];
      }
      v += oldc[basic[j]] * b[j];
    }
  }
  SH_DEBUG_PRINT_LP("Done init:\n A = \n" << A << "\n b = " << b << "\n c = " << c << "\n v = " << v 
      << std::endl << "\n nonBasic =" << nonBasic << " basic = " << basic); 
}

template<typename T>
std::valarray<T> shLpSimplex(size_t M, size_t N, const std::valarray<T>& A, 
    const std::valarray<T>& b, const std::valarray<T>& c)
{
  SH_DEBUG_PRINT_LP("Starting Simplex LP");
  SH_DEBUG_ASSERT(b.size() == M && c.size() == N && A.size() == M * N);
  std::valarray<size_t> nonBasic, basic;
  T v;
  size_t i;

  // make working copies 
  std::valarray<std::valarray<T> > Ap(M);
  for(i = 0; i < M; ++i) {
    Ap[i].resize(N+1);
    Ap[i][std::slice(0, N, 1)] = A[std::slice(i * N, N, 1)];
    Ap[i][N] = -1;
  }
  std::valarray<T> bp(b);
  std::valarray<T> cp(c);

  shLpSimplexInit(M, N, nonBasic, basic, Ap, bp, cp, v); 
  shLpSimplexCore(M, N, nonBasic, basic, Ap, bp, cp, v);

  // extract return values
  std::valarray<T> x(static_cast<T>(0), N);
  for(i = 0; i < M; ++i) {
    if(basic[i] < N) {
      x[basic[i]] = bp[i]; 
    }
  }
  SH_DEBUG_PRINT_LP("x=" << x);

  SH_DEBUG_PRINT_LP("obj: " << (x * c).sum());
  SH_DEBUG_PRINT_LP("verification: " );
  for(i = 0; i < M; ++i) {
    std::valarray<T> temp(A[std::slice(i*N, N, 1)]);
    SH_DEBUG_PRINT_LP("lhs = " << (x * temp).sum() << " rhs=" << b[i]);
  }
  return x;
}

template<typename T>
std::valarray<T> shLpSimplexRelaxed(size_t M, size_t N, const std::valarray<T>& A, 
    const std::valarray<T>& b, const std::valarray<T>& c, 
    const std::valarray<bool>& bounded)
{
  SH_DEBUG_PRINT_LP("Starting relaxed Simplex LP");
  SH_DEBUG_ASSERT(b.size() == M && c.size() == N && A.size() == M * N 
                  && bounded.size() == N);
  size_t i, j, k;

  SH_DEBUG_PRINT_LP("A, b");
  for(i = 0; i < M; ++i) {
    std::valarray<T> temp(A[std::slice(i*N, N, 1)]);
    SH_DEBUG_PRINT_LP(temp << " (" << b[i] << ")");
  }

  // Determine how many unbounded variables there are and add on to N 
  size_t Np = N;
  for(i = 0; i < N; Np += !bounded[i], ++i);
  
  // Turn each variable x_i into x_i - xn_i and update A, c accordingly
  std::valarray<T> Ap(M * Np); 

  for(i = 0; i < M; ++i) {
    int base = i * N;
    int basep = i * Np; 
    for(j = k = 0; j < N; ++j, ++k) { 
      Ap[basep + k] = A[base + j];
      if(!bounded[j]) {
        Ap[basep + (++k)] = -A[base + j];
      } 
    }
  }

  std::valarray<T> cp(Np);
  for(i = j = 0; i < N; ++i, ++j) {
    cp[j] = c[i];
    if(!bounded[i]) {
      cp[++j] = -c[i];
    } 
  }

  SH_DEBUG_PRINT_LP("After mangling relaxed problem: \nA=\n" << Ap << "\n b= " << b << " cp= " << cp);

  std::valarray<T> xp = shLpSimplex(M, Np, Ap, b, cp);

  SH_DEBUG_PRINT_LP("Translating back");
  std::valarray<T> x(N); 
  for(i = j = 0; i < N; ++i, ++j) {
    x[i] = xp[j];
    if(!bounded[i]) {
      x[i] -= xp[++j];
    }
  }
  SH_DEBUG_PRINT_LP("relaxed x=" << x);

  SH_DEBUG_PRINT_LP("obj: " << (x * c).sum());
  SH_DEBUG_PRINT_LP("verification: " );
  for(i = 0; i < M; ++i) {
    std::valarray<T> temp(A[std::slice(i*N, N, 1)]);
    SH_DEBUG_PRINT_LP("lhs = " << (x * temp).sum() << " rhs=" << b[i]);
  }

  return x;
}

}

#endif
