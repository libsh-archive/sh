// Sh: A GPU metaprogramming language.
//
// Copyright (c) 2003 University of Waterloo Computer Graphics Laboratory
// Project administrator: Michael D. McCool
// Authors: Zheng Qin, Stefanus Du Toit, Kevin Moule, Tiberiu S. Popa,
//          Bryan Chan, Michael D. McCool
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
#include "ShProgram.hpp"
#include "ShNibbles.hpp"
#include "ShKernelLib.hpp"

/** \file ShKernelLibImpl.hpp
 * This is an implementation of useful kernels and nibbles (simple kernels).
 */

namespace ShUtil {

using namespace SH;

template<typename T>
ShProgram ShKernelLib::shDiffuse() {
  ShProgram kernel = SH_BEGIN_FRAGMENT_PROGRAM {
    typename T::InputType SH_DECL(kd);
    ShInputNormal3f SH_DECL(normal);
    ShInputVector3f SH_DECL(lightVec);
    ShInputPosition4f SH_DECL(posh);

    ShAttrib1f irrad = pos(dot(normalize(normal), normalize(lightVec)));
    typename T::OutputType SH_DECL(result);
    result = irrad * kd; 
  } SH_END;
  return kernel;
}

template<typename T>
ShProgram ShKernelLib::shSpecular() {
  ShProgram kernel = SH_BEGIN_FRAGMENT_PROGRAM {
    typename T::InputType SH_DECL(ks);
    ShInputAttrib1f SH_DECL(specExp);
    ShInputNormal3f SH_DECL(normal);
    ShInputVector3f SH_DECL(halfVec);
    ShInputVector3f SH_DECL(lightVec);
    ShInputPosition4f SH_DECL(posh);

    normal = normalize(normal);
    halfVec = normalize(halfVec);
    lightVec = normalize(lightVec);
    ShAttrib1f irrad = pos(normal | lightVec);

    typename T::OutputType SH_DECL(result);
    result = irrad * ks * ((normal | halfVec)^specExp); 
  } SH_END;
  return kernel;
}


template<typename T>
ShProgram ShKernelLib::shPhong() {
  ShProgram kernel = SH_BEGIN_PROGRAM("gpu:fragment") {
    typename T::InputType SH_DECL(kd);
    typename T::InputType SH_DECL(ks);
    ShInputAttrib1f SH_DECL(specExp);
    ShInputNormal3f SH_DECL(normal);
    ShInputVector3f SH_DECL(halfVec);
    ShInputVector3f SH_DECL(lightVec);
    ShInputPosition4f SH_DECL(posh);

    typename T::OutputType SH_DECL(result);

    normal = normalize(normal);
    halfVec = normalize(halfVec);
    lightVec = normalize(lightVec);
    ShAttrib1f irrad = pos(normal | lightVec);
    result = kd * irrad + ks * pow(normal | halfVec, specExp); 
  } SH_END;
  return kernel;
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
  ShProgram vsh = shVsh(mv, mvp) & keep<ShVector3f>("tangent") & keep<ShVector3f>("tangent2");

  // convert view, half, and light to orthonormal bases {normal, tangent, tangent2}
  ShProgram ConvertToTCS = ShKernelLib::shConvertBasis("viewVec", "normal", "tangent", "tangent2");
  ConvertToTCS = namedCombine(ConvertToTCS, 
      ShKernelLib::shConvertBasis("halfVec", "normal", "tangent", "tangent2"));
  ConvertToTCS = namedCombine(ConvertToTCS, 
      ShKernelLib::shConvertBasis("lightVec", "normal", "tangent", "tangent2"));

  // normal becomes (1.0,0.0,0.0) in TCS
  vsh = namedConnect(vsh, ConvertToTCS) & 
    (keep<ShNormal3f>("normal") << ShConstant3f(1.0, 0.0, 0.0));

  // swizzle inputs/outputs to match specs
  return  shSwizzle("texcoord", "posv", "normal", "viewVec", "halfVec", "lightVec", "posh")
    << vsh 
    << shSwizzle("texcoord", "normal", "tangent", "tangent2", "lightPos", "posm");
}


}

#endif
