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
#ifndef SHUTIL_KERNELLIBIMPL_HPP 
#define SHUTIL_KERNELLIBIMPL_HPP 

#include "ShSyntax.hpp"
#include "ShPosition.hpp"
#include "ShManipulator.hpp"
#include "ShAlgebra.hpp"
#include "ShKernelLib.hpp"

/** \file ShKernelLibImpl.hpp
 * This is an implementation of useful kernels and nibbles (simple kernels).
 */

namespace ShUtil {

using namespace SH;

#define ATTRIB_DECL(ATTRIB_TYPE, Kind, var) \
  ShAttrib< ATTRIB_TYPE::typesize, Kind, typename ATTRIB_TYPE::ValueType > var; \
  var.node()->specialType(ATTRIB_TYPE::special_type);

template< int Rows, int Cols, int Kind, typename T>
ShProgram ShKernelLib::transformNibble(const ShMatrix<Rows, Cols, Kind, T> &m) {
  ShProgram nibble = SH_BEGIN_PROGRAM() {
    ShAttrib<Cols, SH_VAR_INPUT, T> input;
    ShAttrib<Rows, SH_VAR_OUTPUT, T> output = m | input;
  } SH_END_PROGRAM;
  return nibble;
}

template<typename T>
ShProgram ShKernelLib::textureNibble(const ShTexture2D<T> &tex) {
  ShProgram nibble = SH_BEGIN_PROGRAM() {
    ShInputTexCoord2f tc;
    ATTRIB_DECL(T, SH_VAR_OUTPUT, output);
    output = tex(tc);
  } SH_END_PROGRAM;
  return nibble;
}

template<typename T>
ShProgram ShKernelLib::lerpNibble() {
  ShProgram nibble = SH_BEGIN_PROGRAM() {
    ShInputAttrib1f alpha;
    ATTRIB_DECL(T, SH_VAR_INPUT, a);
    ATTRIB_DECL(T, SH_VAR_INPUT, b);
    ATTRIB_DECL(T, SH_VAR_OUTPUT, result);
    result = lerp(alpha, a, b); 
  } SH_END_PROGRAM;
  return nibble;
}

template<typename T>
ShProgram ShKernelLib::dotNibble() {
  ShProgram nibble = SH_BEGIN_PROGRAM() {
    ATTRIB_DECL(T, SH_VAR_INPUT, a);
    ATTRIB_DECL(T, SH_VAR_INPUT, b);
    ShAttrib<1, SH_VAR_OUTPUT, typename T::ValueType> result = dot(a, b); 
  } SH_END_PROGRAM;
  return nibble;
}

// TODO - macro for all binary operations
template<typename T>
ShProgram ShKernelLib::addNibble() {
  ShProgram nibble = SH_BEGIN_PROGRAM() {
    ATTRIB_DECL(T, SH_VAR_INPUT, a);
    ATTRIB_DECL(T, SH_VAR_INPUT, b);
    ATTRIB_DECL(T, SH_VAR_OUTPUT, result);
    result = a + b;
  } SH_END_PROGRAM;
  return nibble;
}

template<typename T, typename T2>
ShProgram ShKernelLib::castNibble() {
  ShProgram nibble = SH_BEGIN_PROGRAM() {
    ATTRIB_DECL(T, SH_VAR_INPUT, in);
    ATTRIB_DECL(T2, SH_VAR_OUTPUT, out);
    int copySize = std::min(T::typesize, T2::typesize);
    for(int i = 0; i < copySize; ++i) {
      // TODO make this a swizzled op (or let future vectorizer deal with it)
      out(i) = in(i);
    }
  } SH_END_PROGRAM;
  return nibble;
}

template<typename T>
ShProgram ShKernelLib::diffuse() {
  ShProgram kernel = SH_BEGIN_FRAGMENT_PROGRAM {
    ATTRIB_DECL(T, SH_VAR_INPUT, kd);
    ShInputNormal3f n;
    ShInputVector3f lv;
    ShInputPosition4f pd;

    ShAttrib1f irrad = dot(normalize(n), normalize(lv));
    irrad *= (irrad > 0.0);

    ATTRIB_DECL(T, SH_VAR_OUTPUT, outColour);
    outColour = irrad * kd; 
  } SH_END_PROGRAM;
  return kernel;
}

template<typename T>
ShProgram ShKernelLib::specular() {
  ShProgram kernel = SH_BEGIN_FRAGMENT_PROGRAM {
    ATTRIB_DECL(T, SH_VAR_INPUT, ks);
    ShInputAttrib1f exp;
    ShInputNormal3f n;
    ShInputVector3f vv;
    ShInputVector3f hv;
    ShInputVector3f lv;
    ShInputPosition4f pd;

    ShNormal3f nnv = normalize(n);
    ShVector3f nhv = normalize(hv);
    ShAttrib1f irrad = dot(nnv, normalize(lv));
    irrad *= (irrad > 0.0);

    ATTRIB_DECL(T, SH_VAR_OUTPUT, outColour);
    outColour = irrad * ks * (dot(nhv,nnv)^exp); 
  } SH_END_PROGRAM;
  return kernel;
}


template<typename T>
ShProgram ShKernelLib::phong() {
  // TODO find a better way of expressing this - manipulating inputs
  // across vertex/fragment shader (don't want to manipulate vsh outputs)
  ShProgram permuter = keep<T>() &  keep<T>() & keep<ShAttrib1f>() & keep<ShNormal3f>() 
    & keep<ShVector3f>() & keep<ShVector3f>() & keep<ShVector3f>() & keep<ShPosition4f>(); 

  ShManipulator inManip = shRange(0)(3)(6,7)(1,7); // manipulate for diffuse & specular inputs
  ShProgram diffuseSpecular = diffuse<T>() & specular<T>();

  return permuter >> inManip >> diffuseSpecular >> addNibble<T>();
}


template<int N, int Kind, typename T>
ShProgram ShKernelLib::vsh(const ShMatrix<N, N, Kind, T> &mv, const ShMatrix<N, N, Kind, T> &mvp) {
  ShProgram generalVsh = SH_BEGIN_VERTEX_PROGRAM {
    ShInputTexCoord2f u;    // IN(0): texture coordinate 
    ShInputNormal3f nm;     // IN(1): normal vector (MCS)
    ShInputVector3f t1m;    // IN(2): tangent (MCS) 
    ShInputVector3f t2m;    // IN(3): secondary tangent (MCS)
    ShInputPoint3f lpv;      // IN(4): light position (VCS)
    ShInputPosition4f pm;   // IN(5): position (MCS)


    ShOutputTexCoord2f uo;    // OUT(0): texture coordinate
    ShOutputPoint3f  pv;      // OUT(1): output point (VCS)
    ShOutputNormal3f nv;      // OUT(2): normal vector (VCS) 
    ShOutputVector3f vv;      // OUT(3): view vector (VCS)
    ShOutputVector3f hv;      // OUT(4): half vector (VCS)
    ShOutputVector3f lv;      // OUT(5): light vector (VCS)
    ShOutputVector3f vt;      // OUT(6): view vector (tangent coordinate space)
    ShOutputVector3f ht;      // OUT(7): half vector (tangent coordinate space)
    ShOutputVector3f lt;      // OUT(8): light vector (tangent coordinate space)
    ShOutputPosition4f pd;    // OUT(9): position (HDCS)

    ShPoint3f pvTemp = (mv | pm)(0,1,2); 
    ShNormal3f nvTemp = normalize(mv | nm); 
    ShVector3f t1v = normalize(mv | t1m);
    ShVector3f t2v = normalize(mv | t2m);
    ShVector3f vvTemp = normalize(-pvTemp);
    ShVector3f lvTemp = normalize(lpv - pvTemp); 
    ShVector3f hvTemp = normalize(vvTemp + lvTemp); 

    uo = u;
    pv = pvTemp;
    nv = nvTemp;
    vv = vvTemp;
    hv = hvTemp;
    lv = lvTemp;

    vt(0) = dot(vv, nv);
    vt(1) = dot(vv, t1v);
    vt(2) = dot(vv, t2v);

    ht(0) = dot(hv, nv);
    ht(1) = dot(hv, t1v);
    ht(2) = dot(hv, t2v);

    lt(0) = dot(lv, nv);
    lt(1) = dot(lv, t1v);
    lt(2) = dot(lv, t2v);

    pd = mvp | pm;
  } SH_END_PROGRAM;
  return generalVsh;
}

}

#endif
