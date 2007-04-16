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
#ifndef SHLP_HPP
#define SHLP_HPP

#include <iostream>
#include <vector>
#include <valarray>
#include "DllExport.hpp"
#include "Exception.hpp"

/* @file Lp.hpp
 * Basic badly coded linear programming, straight from CLRS with appropriate 
 * interpretations of the data structures.
 *
 * (Originally this used Sh types, but that was a touch slow..., so)
 */

namespace SH {

/* Exception thrown when a linear program has no feasible solution */
struct SH_DLLEXPORT InfeasibleLpException {};

/* Exception thrown when a linear program is unbounded */
struct SH_DLLEXPORT UnboundedLpException {};

// @todo range
// think about what types T/T, etc. should really be.
// Right now it's just general enough to do the affine texture stuff
// (i.e. T is a tuple, T are scalars)


/*
 * Solves linear programs of the form
 *    Ax <= b
 *    x >= 0
 *    maximize c | x
 *
 * (A is mxn, x, c are n-vectors, b is an m-vector)
 *
 * returns an n-vector set of values x.
 */
template<int M, int N, typename T>
std::valarray<T> lpSimplex(const std::valarray<T>& A, 
    const std::valarray<T>& b, const std::valarray<T>& c);

/* 
 * Solves a linear program with no non-negativity constraints
 *    Ax <= b
 *    x_i >= 0 only if bounded[i] = true
 *    maximize c | x
 * (This just does some mangling and calls the above function)
 */
template<int M, int N, typename T>
std::valarray<T> lpSimplexRelaxed(const std::valarray<T>& A, 
    const std::valarray<T>& b, const std::valarray<T>& c,
    const std::valarray<bool>& bounded);
}

#include "LpImpl.hpp"

#endif
