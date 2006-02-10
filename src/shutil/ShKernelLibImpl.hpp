// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2006 Serious Hack Inc.
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
// MA  02110-1301, USA
//////////////////////////////////////////////////////////////////////////////
#ifndef SHUTIL_KERNELLIBIMPL_HPP 
#define SHUTIL_KERNELLIBIMPL_HPP 

#include <sstream>
#include "sh/ShSyntax.hpp"
#include "sh/ShPosition.hpp"
#include "sh/ShManipulator.hpp"
#include "sh/ShAlgebra.hpp"
#include "sh/ShProgram.hpp"
#include "sh/ShNibbles.hpp"
#include "sh/ShTexCoord.hpp"
#include "sh/ShVector.hpp"
#include "sh/ShPoint.hpp"
#include "sh/ShPosition.hpp"
#include "sh/ShNormal.hpp"
#include "ShKernelLib.hpp"
#include "ShFunc.hpp"

/** \file ShKernelLibImpl.hpp
 * This is an implementation of useful kernels and nibbles (simple kernels).
 */

namespace ShUtil {

using namespace SH;

template<int N, ShBindingType Binding, typename T>
ShProgram ShKernelLib::shVsh(const ShMatrix<N, N, Binding, T> &mv,
                             const ShMatrix<N, N, Binding, T> &mvp,
                             int numTangents, int numLights)
{
  int i;
  ShProgram generalVsh = SH_BEGIN_VERTEX_PROGRAM {
    // INPUTS
    ShInputTexCoord2f SH_NAMEDECL(u, "texcoord");  
    ShInputNormal3f SH_NAMEDECL(nm, "normal");     
    ShVector3f tgt; 
    ShVector3f tgt2; 
    if(numTangents > 0) {
      ShInputVector3f SH_NAMEDECL(inTangent, "tangent");
      tgt = inTangent;
      if( numTangents > 1) {
        ShInputVector3f SH_NAMEDECL(inTangent2, "tangent2");
        tgt2 = inTangent2;
      }  else {
        tgt2 = cross(nm, tgt);
      }
    }
    ShInputPoint3f* lpv = new ShInputPoint3f[numLights];                 
    for(i = 0; i < numLights; ++i) lpv[i].name(makeName("lightPos", i));
    ShInputPosition4f SH_NAMEDECL(pm, "posm");     

    // OUTPUTS
    ShOutputTexCoord2f SH_NAMEDECL(uo, "texcoord");  
    ShOutputPoint3f SH_NAMEDECL(pv, "posv");         
    ShOutputPoint4f SH_NAMEDECL(pmo, "posm");
    
    // VCS outputs
    ShOutputNormal3f SH_NAMEDECL(nv, "normal");      
    ShOutputVector3f SH_NAMEDECL(tv, "tangent");
    ShOutputVector3f SH_NAMEDECL(tv2, "tangent2");
    ShOutputVector3f SH_NAMEDECL(vv, "viewVec");     
    ShOutputVector3f* hv = new ShOutputVector3f[numLights];    
    for(i = 0; i < numLights; ++i) hv[i].name(makeName("halfVec", i).c_str());
    ShOutputVector3f* lv = new ShOutputVector3f[numLights];    
    for(i = 0; i < numLights; ++i) lv[i].name(makeName("lightVec", i).c_str()); 

    ShOutputPoint3f* lpo = new ShOutputPoint3f[numLights];    
    for(i = 0; i < numLights; ++i) lpo[i].name(makeName("lightPos", i).c_str()); 

    // TCS outputs
    ShOutputNormal3f SH_NAMEDECL(nvt, "normalt");      
    ShOutputVector3f SH_NAMEDECL(vvt, "viewVect");     
    ShOutputVector3f* hvt = new ShOutputVector3f[numLights];    
    for(i = 0; i < numLights; ++i) hvt[i].name(makeName("halfVect", i).c_str());
    ShOutputVector3f* lvt = new ShOutputVector3f[numLights];    
    for(i = 0; i < numLights; ++i) lvt[i].name(makeName("lightVect", i).c_str()); 

    ShOutputPosition4f SH_NAMEDECL(pd, "posh");      

    uo = u;
    pv = (mv | pm)(0,1,2); 
    pmo = pm;

    // VCS outputs
    nv = normalize(mv | nm); 
    vv = normalize(-pv);
    for(i = 0; i < numLights; ++i) {
      lv[i] = normalize(lpv[i] - pv); 
      hv[i] = normalize(vv + lv[i]); 
      lpo[i] = lpv[i];
    }

    // TCS outputs
    tv = mv | tgt;
    tv2 = mv | tgt2;
    nvt = normalize(changeBasis(nv, tv, tv2, nv));
    vvt = normalize(changeBasis(nv, tv, tv2, vv));
    for(i = 0; i < numLights; ++i) {
      hvt[i] = normalize(changeBasis(nv, tv, tv2, hv[i]));
      lvt[i] = normalize(changeBasis(nv, tv, tv2, lv[i])); 
    }

    pd = mvp | pm;

    delete [] lvt;
    delete [] hvt;
    delete [] lpo;
    delete [] lv;
    delete [] hv;
    delete [] lpv;
  } SH_END;
  return generalVsh;
}

}

#endif
