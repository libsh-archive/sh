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
#include "sh/Syntax.hpp"
#include "sh/Position.hpp"
#include "sh/Manipulator.hpp"
#include "sh/Algebra.hpp"
#include "sh/Program.hpp"
#include "sh/Nibbles.hpp"
#include "sh/TexCoord.hpp"
#include "sh/Vector.hpp"
#include "sh/Point.hpp"
#include "sh/Position.hpp"
#include "sh/Normal.hpp"
#include "KernelLib.hpp"
#include "Func.hpp"

/** \file KernelLibImpl.hpp
 * This is an implementation of useful kernels and nibbles (simple kernels).
 */

namespace ShUtil {

using namespace SH;

template<int N, BindingType Binding, typename T>
Program KernelLib::vsh(const Matrix<N, N, Binding, T> &mv,
                             const Matrix<N, N, Binding, T> &mvp,
                             int numTangents, int numLights)
{
  int i;
  Program generalVsh = SH_BEGIN_VERTEX_PROGRAM {
    // INPUTS
    InputTexCoord2f SH_NAMEDECL(u, "texcoord");  
    InputNormal3f SH_NAMEDECL(nm, "normal");     
    Vector3f tgt; 
    Vector3f tgt2; 
    if(numTangents > 0) {
      InputVector3f SH_NAMEDECL(inTangent, "tangent");
      tgt = inTangent;
      if( numTangents > 1) {
        InputVector3f SH_NAMEDECL(inTangent2, "tangent2");
        tgt2 = inTangent2;
      }  else {
        tgt2 = cross(nm, tgt);
      }
    }
    InputPoint3f* lpv = new InputPoint3f[numLights];                 
    for(i = 0; i < numLights; ++i) lpv[i].name(makeName("lightPos", i));
    InputPosition4f SH_NAMEDECL(pm, "posm");     

    // OUTPUTS
    OutputTexCoord2f SH_NAMEDECL(uo, "texcoord");  
    OutputPoint3f SH_NAMEDECL(pv, "posv");         
    OutputPoint4f SH_NAMEDECL(pmo, "posm");
    
    // VCS outputs
    OutputNormal3f SH_NAMEDECL(nv, "normal");      
    OutputVector3f SH_NAMEDECL(tv, "tangent");
    OutputVector3f SH_NAMEDECL(tv2, "tangent2");
    OutputVector3f SH_NAMEDECL(vv, "viewVec");     
    OutputVector3f* hv = new OutputVector3f[numLights];    
    for(i = 0; i < numLights; ++i) hv[i].name(makeName("halfVec", i).c_str());
    OutputVector3f* lv = new OutputVector3f[numLights];    
    for(i = 0; i < numLights; ++i) lv[i].name(makeName("lightVec", i).c_str()); 

    OutputPoint3f* lpo = new OutputPoint3f[numLights];    
    for(i = 0; i < numLights; ++i) lpo[i].name(makeName("lightPos", i).c_str()); 

    // TCS outputs
    OutputNormal3f SH_NAMEDECL(nvt, "normalt");      
    OutputVector3f SH_NAMEDECL(vvt, "viewVect");     
    OutputVector3f* hvt = new OutputVector3f[numLights];    
    for(i = 0; i < numLights; ++i) hvt[i].name(makeName("halfVect", i).c_str());
    OutputVector3f* lvt = new OutputVector3f[numLights];    
    for(i = 0; i < numLights; ++i) lvt[i].name(makeName("lightVect", i).c_str()); 

    OutputPosition4f SH_NAMEDECL(pd, "posh");      

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
