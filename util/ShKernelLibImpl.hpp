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

#define ATTRIB_NAMED_DECL(Type, K, var, varName) \
  ShAttrib< Type::typesize, K, typename Type::ValueType > SH_NAMEDECL( var, varName ); \
  var.node()->specialType(Type::special_type);

#define ATTRIB_DECL(Type, K, var) \
  ATTRIB_NAMED_DECL(Type, K, var, # var)

template< int Rows, int Cols, int Kind, typename T>
ShProgram shTransform(const ShMatrix<Rows, Cols, Kind, T> &m) {
  ShProgram nibble = SH_BEGIN_PROGRAM() {
    ShAttrib<Cols, SH_VAR_INPUT, T> attrib; SH_NAMEVAR( attrib ); 
    ShAttrib<Rows, SH_VAR_OUTPUT, T> transAttrib; SH_NAMEVAR( transAttrib );
    transAttrib = m | input;
  } SH_END;
  return nibble;
}

template<typename T>
ShProgram access(const ShTexture1D<T> &tex) {
  ShProgram nibble = SH_BEGIN_PROGRAM() {
    ShInputTexCoord1f SH_DECL(tc);
    ATTRIB_DECL(T, SH_VAR_OUTPUT, result); 
    result = tex(tc);
  } SH_END;
  return nibble;
}

template<typename T>
ShProgram access(const ShTexture2D<T> &tex) {
  ShProgram nibble = SH_BEGIN_PROGRAM() {
    ShInputTexCoord2f SH_DECL(tc);
    ATTRIB_DECL(T, SH_VAR_OUTPUT, result); 
    result = tex(tc);
  } SH_END;
  return nibble;
}

template<typename T>
ShProgram access(const ShTexture3D<T> &tex) {
  ShProgram nibble = SH_BEGIN_PROGRAM() {
    ShInputTexCoord3f SH_DECL(tc);
    ATTRIB_DECL(T, SH_VAR_OUTPUT, result); 
    result = tex(tc);
  } SH_END;
  return nibble;
}

template<typename T>
ShProgram shScale() {
  ShProgram nibble = SH_BEGIN_PROGRAM() {
    ShInputAttrib1f SH_DECL(scale);
    ATTRIB_DECL(T, SH_VAR_INPUT, attrib);
    ATTRIB_DECL(T, SH_VAR_OUTPUT, result);
    result = scale * attrib; 
  } SH_END;
  return nibble;
}

template<typename T>
ShProgram shLerp() {
  ShProgram nibble = SH_BEGIN_PROGRAM() {
    ShInputAttrib1f SH_DECL(alpha);
    ATTRIB_DECL(T, SH_VAR_INPUT, a);
    ATTRIB_DECL(T, SH_VAR_INPUT, b);
    ATTRIB_DECL(T, SH_VAR_OUTPUT, result);
    result = lerp(alpha, a, b); 
  } SH_END;
  return nibble;
}

template<typename T>
ShProgram shDot() {
  ShProgram nibble = SH_BEGIN_PROGRAM() {
    ATTRIB_DECL(T, SH_VAR_INPUT, a);
    ATTRIB_DECL(T, SH_VAR_INPUT, b);
    ShAttrib<1, SH_VAR_OUTPUT, typename T::ValueType> SH_DECL(result);
    result = dot(a, b); 
  } SH_END;
  return nibble;
}

// TODO - macro for all binary operations
template<typename T>
ShProgram shAdd() {
  ShProgram nibble = SH_BEGIN_PROGRAM() {
    ATTRIB_DECL(T, SH_VAR_INPUT, a);
    ATTRIB_DECL(T, SH_VAR_INPUT, b);
    ATTRIB_DECL(T, SH_VAR_OUTPUT, result);
    result = a + b;
  } SH_END;
  return nibble;
}

template<typename T, typename T2>
ShProgram shCast() {
  ShProgram nibble = SH_BEGIN_PROGRAM() {
    ATTRIB_DECL(T, SH_VAR_INPUT, in);
    ATTRIB_DECL(T2, SH_VAR_OUTPUT, out);
    int copySize = std::min(T::typesize, T2::typesize);
    for(int i = 0; i < copySize; ++i) {
      // TODO make this a swizzled op (or let future vectorizer deal with it)
      out(i) = in(i);
    }
  } SH_END;
  return nibble;
}

template<typename T>
ShProgram ShKernelLib::shDiffuse() {
  ShProgram kernel = SH_BEGIN_FRAGMENT_PROGRAM {
    ATTRIB_DECL(T, SH_VAR_INPUT, kd);
    ShInputNormal3f SH_DECL(normal);
    ShInputVector3f SH_DECL(lightVec);
    ShInputPosition4f SH_DECL(posh);

    ShAttrib1f irrad = dot(normalize(normal), normalize(lightVec));
    irrad *= (irrad > 0.0);

    ATTRIB_DECL(T, SH_VAR_OUTPUT, result);
    result = irrad * kd; 
  } SH_END;
  return kernel;
}

template<typename T>
ShProgram ShKernelLib::shSpecular() {
  ShProgram kernel = SH_BEGIN_FRAGMENT_PROGRAM {
    ATTRIB_DECL(T, SH_VAR_INPUT, ks);
    ShInputAttrib1f SH_DECL(specExp);
    ShInputNormal3f SH_DECL(normal);
    ShInputVector3f SH_DECL(halfVec);
    ShInputVector3f SH_DECL(lightVec);
    ShInputPosition4f SH_DECL(posh);

    ShNormal3f nnv = normalize(normal);
    ShVector3f nhv = normalize(halfVec);
    ShAttrib1f irrad = dot(nnv, normalize(lightVec));
    irrad *= (irrad > 0.0);

    ATTRIB_DECL(T, SH_VAR_OUTPUT, result);
    result = irrad * ks * (dot(nhv,nnv)^specExp); 
  } SH_END;
  return kernel;
}


template<typename T>
ShProgram ShKernelLib::shPhong() {
  // TODO find a better way of expressing this - manipulating inputs
  // across vertex/fragment shader (don't want to manipulate vsh outputs)
  ShProgram permuter = keep<T>("kd") & keep<T>("ks") & keep<ShAttrib1f>("specExp") 
    & keep<ShNormal3f>("normal") & keep<ShVector3f>("halfVec") 
    & keep<ShVector3f>("lightVec") & keep<ShPosition4f>("posh"); 

  return permuter >> shRange("kd")("normal")("lightVec")("posh")("ks","posh") >>
    ( shDiffuse<T>() & shSpecular<T>() ) >> shAdd<T>(); 
}


template<int N, int Kind, typename T>
ShProgram ShKernelLib::shVsh(const ShMatrix<N, N, Kind, T> &mv, const ShMatrix<N, N, Kind, T> &mvp) {
  ShProgram generalVsh = SH_BEGIN_VERTEX_PROGRAM {
    ShInputTexCoord2f SH_NAMEDECL(u, "texcoord");  // IN(0): texture coordinate 
    ShInputNormal3f SH_NAMEDECL(nm, "normal");     // IN(1): normal vector (MCS)
    ShInputPoint3f SH_NAMEDECL(lpv, "lightPos");   // IN(2): light position (VCS)
    ShInputPosition4f SH_NAMEDECL(pm, "posm");     // IN(3): position (MCS)


    ShOutputTexCoord2f SH_NAMEDECL(uo, "texcoord");  // OUT(0): texture coordinate
    ShOutputPoint3f SH_NAMEDECL(pv, "posv");         // OUT(1): output point (VCS)
    ShOutputNormal3f SH_NAMEDECL(nv, "normal");      // OUT(2): normal vector (VCS) 
    ShOutputVector3f SH_NAMEDECL(vv, "viewVec");     // OUT(3): view vector (VCS)
    ShOutputVector3f SH_NAMEDECL(hv, "halfVec");     // OUT(4): half vector (VCS)
    ShOutputVector3f SH_NAMEDECL(lv, "lightVec");    // OUT(5): light vector (VCS)
    ShOutputPosition4f SH_NAMEDECL(pd, "posh");      // OUT(6): position (HDCS)


    ShPoint3f pvTemp = (mv | pm)(0,1,2); 
    ShNormal3f nvTemp = normalize(mv | nm); 
    ShVector3f vvTemp = normalize(-pvTemp);
    ShVector3f lvTemp = normalize(lpv - pvTemp); 
    ShVector3f hvTemp = normalize(vvTemp + lvTemp); 

    uo = u;
    pv = pvTemp;
    nv = nvTemp;
    vv = vvTemp;
    hv = hvTemp;
    lv = lvTemp;


    pd = mvp | pm;
  } SH_END;
  return generalVsh;
}

template<int N, int Kind, typename T>
ShProgram ShKernelLib::shVshTangentSpace(const ShMatrix<N, N, Kind, T> &mv, 
    const ShMatrix<N, N, Kind, T> &mvp) {
  // permute inputs to match specified inputs
  ShProgram vsh = shVsh(mv, mvp) & keep<ShVector3f>("tangent") & keep<ShVector3f>("tangent2");
  vsh = vsh << shSwizzle( "texcoord", "normal", "tangent", "tangent2", "lightPos", "posm");

  // permute outputs to match tangentVsh below
  vsh = vsh >> shRange("normal", "lightVec")("tangent")("tangent2")("texcoord")("posv")("posh");

  ShProgram tangentVsh = SH_BEGIN_VERTEX_PROGRAM {
    ShInputNormal3f SH_NAMEDECL(nv, "normal");     // IN(1): normalized normal vector (VCS)
    ShInputVector3f SH_NAMEDECL(vv, "viewVec");    // IN(2): normalized viewVec (VCS)
    ShInputVector3f SH_NAMEDECL(hv, "halfVec");    // IN(3): normalized halfVec (VCS)
    ShInputVector3f SH_NAMEDECL(lv, "lightVec");   // IN(4): normalized lightVec (VCS)
    ShInputVector3f SH_NAMEDECL(t1m, "tangent");   // IN(5): primary tangent (MCS)
    ShInputVector3f SH_NAMEDECL(t2m, "tangent2");  // IN(6): secondary tangent (MCS)

    ShOutputNormal3f SH_NAMEDECL(nt, "normal");      // OUT(0): normal vector (TCS) 
    ShOutputVector3f SH_NAMEDECL(vt, "viewVec");     // OUT(1): view vector (TCS)
    ShOutputVector3f SH_NAMEDECL(ht, "halfVec");     // OUT(2): half vector (TCS)
    ShOutputVector3f SH_NAMEDECL(lt, "lightVec");    // OUT(3): light vector (TCS)


    ShVector3f t1v = normalize(mv | t1m);
    ShVector3f t2v = normalize(mv | t2m);

    nt = ShConstant3f(1.0, 0.0, 0.0);

    vt(0) = dot(vv, nv);
    vt(1) = dot(vv, t1v);
    vt(2) = dot(vv, t2v);

    ht(0) = dot(hv, nv);
    ht(1) = dot(hv, t1v);
    ht(2) = dot(hv, t2v);

    lt(0) = dot(lv, nv);
    lt(1) = dot(lv, t1v);
    lt(2) = dot(lv, t2v);
  } SH_END;

  // permute outputs to match specified outputs
  return vsh >> tangentVsh >> shRange("texcoord")("posv")("normal", "lightVec")("posh"); 
}


}

#endif
