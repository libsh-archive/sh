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
#include "MultiArray.hpp"
#include "Syntax.hpp"
#include "Statement.hpp"
#include "TexCoord.hpp"
#include "Attrib.hpp"
#include "Lib.hpp"
#include "IntervalTexture.hpp"

namespace SH {

IntervalTexture::IntervalTexture(TextureNodePtr other) 
  : m_other(other), 
    m_width(other->width())
{
  SH_DEBUG_ASSERT(isRegularValueType(other->valueType()));
  for(size_t i = 0; i < 2; ++i) {
    m_node[i] = new TextureNode(other->dims(), other->size(), SH_FLOAT, other->traits(),
                             m_width * 2, other->height(), other->depth());
  }
  m_node[0]->name(other->name() + "_iat-lo");
  m_node[1]->name(other->name() + "_iat-hi");

  switch (other->dims()) {                                                              
    case SH_TEXTURE_1D:   m_dims = 1; break;                                            
                                                                                        
    case SH_TEXTURE_2D:                                                                 
    case SH_TEXTURE_RECT: m_dims = 2; break;                                            

    case SH_TEXTURE_3D:   m_dims = 3; break; 
    default:
      SH_DEBUG_PRINT("Cannot handle this texture dim");
  }

  m_other->memory()->add_dep(this);
  m_other->memory()->flush(); // update the data
}

// create a new texture memory
// save the original texture and all the mipmap levels on its right
void IntervalTexture::memory_update() 
{
  size_t i;

  size_t_array widths(m_dims + 1);
  // index of the first non-tuple element dimension.  
  // This is the dimension we will expand the texture in to fit the new "mip-map" levels of data 
  size_t FIRST = m_dims - 1; 
  size_t ELEM = m_dims; 
  if(m_dims > 2) widths[m_dims - 3] = m_other->depth();
  if(m_dims > 1) widths[m_dims - 2] = m_other->height();
  m_width = widths[m_dims - 1] = m_other->width();
  //size_t tupleSize = widths[m_dims] = m_other->size();
  //size_t count = m_other->count(); // basically all the dimensions multiplied together, except tuple size 

  // cannot handle nD rectangles for now
  for(i = 0; i < FIRST; ++i) {
    SH_DEBUG_ASSERT(widths[i] == widths[FIRST]);
  }

  int typesize = typeInfo(m_other->valueType())->datasize();
  SH_DEBUG_ASSERT(m_other->valueType() == SH_FLOAT); // @todo range - can only handle this for now

  // access the original data
  HostStoragePtr oldmem = shref_dynamic_cast<HostStorage>(m_other->memory()->findStorage("host"));
  // @todo range handle non-float
  MultiArray<float> olddata(reinterpret_cast<float*>(oldmem->data()), widths);

  // create a new memory for the new texture
  
  // We need to store the original texture + usual mip-map requirements
  // in each of lo hi (replicates the CONSTANT level so we don't have to bother
  // with an special case conditional on lookup for that level)
  size_t_array newWidths = widths;
  newWidths[FIRST] *= 2;

  // generate initialized arrays & copy over constant level both 
  MultiArray<float> newdata[2];

  size_t_array start(static_cast<size_t>(0), m_dims + 1);
  start[FIRST] = m_width;

  for(i = 0; i < 2; ++i) {
    newdata[i] = MultiArray<float>(newWidths);
    newdata[i] = 0;
    newdata[i].rangeSliceAsn(start, widths, olddata.m_data);
  }

  const size_t LO = 0;
  const size_t HI = 1;
  // Build the mip map layers
  size_t offset = m_width / 2;
  for(; offset >= 1; offset >>=1) {
    size_t_array start(static_cast<size_t>(0), m_dims + 1);
    start[FIRST] = offset;

    size_t_array length(static_cast<size_t>(offset), m_dims + 1);
    length[ELEM] = widths[ELEM]; 

    size_t_array strides(static_cast<size_t>(1), m_dims + 1);

    size_t_array large_strides(static_cast<size_t>(2), m_dims + 1);
    large_strides[ELEM] = 1;

    MultiArrayIndex S(start, length, strides);
    MultiArrayIndex L(start * static_cast<size_t>(2), length, large_strides); 

    for(;; ++S, ++L) {
      float& lo_s = newdata[LO][S.index()]; 
      float& hi_s = newdata[HI][S.index()]; 

      size_t_array large_start(L.index());
      size_t_array large_lengths(static_cast<size_t>(2), m_dims + 1); 
      large_lengths[ELEM] = 1;

      MultiArrayIndex LL(large_start, large_lengths, strides);
      for(bool first = true;; first = false, ++LL) {
        float& lo_l = newdata[LO][LL.index()]; 
        float& hi_l = newdata[HI][LL.index()]; 
        if(first) {
            lo_s = lo_l;
            hi_s = hi_l;
        } else {
          lo_s = std::min(lo_s, lo_l);
          hi_s = std::max(hi_s, hi_l);
        }
        if(LL.end()) break;
      }

      if(S.end()) break;
    }
  }

  for(i = 0; i < 2; ++i) {
    // Stick the data into textures
    HostMemoryPtr newmem = new HostMemory(product(newWidths) * typesize);
    newdata[i].copyTo(reinterpret_cast<float*>(newmem->hostStorage()->data()));

    // update the new texture parameters
    m_node[i]->memory(newmem, 0);
    switch(m_dims) {
      case 1: m_node[i]->setTexSize(newWidths[0]); break;
      case 2: m_node[i]->setTexSize(newWidths[1], newWidths[0]); break;
      case 3: m_node[i]->setTexSize(newWidths[2], newWidths[1], newWidths[0]);
    }
  }
}

VariableNodePtr IntervalTexture::makeTemp(const Variable &v, const std::string& name, int size) 
{
  if(size == 0) size = v.size();
  VariableNodePtr result = v.node()->clone(SH_TEMP, size, VALUETYPE_END, SEMANTICTYPE_END, true, false); 
  result->name(name);
  return result;
}

Record IntervalTexture::rect_lookup(const Variable &lo, const Variable &hi) const 
{
  Record result;
  SH_BEGIN_SECTION("iatex_rect_lookup") {
    size_t i, j;

    /* implementation of algorithm from Kevin's dispmap paper,
     * extended for nD, but 
     * @todo range doesn't bother trying to optimize for # of tex lookups 
     * i.e. it does 2^n always... 
     *
     * Also does not handle clamping or interpolation */

    ConstAttrib1f ZERO(0.0f);
    ConstAttrib1f HALF(0.5f);
    ConstAttrib1f ONE(1.0f);

    size_t size = lo.size();

    Variable clamplo(makeTemp(lo, "clamplo"));
    Variable clamphi(makeTemp(hi, "clamphi"));

    // clamp to [0, texsize]
    shMAX(clamplo, lo, ZERO.repeat(size));
    shMIN(clamphi, hi, m_other->texSizeVar());

    Variable width(makeTemp(lo, "width"));
    shADD(width, clamphi, -clamplo);

    Variable wu(makeTemp(lo, "wu")); // start with a guess about the level
#if 0
    // don't really understand this, so rewrote below
    shADD(wu, width, ONE.repeat(size));
    shLOG2(wu, wu);
    shCEIL(wu, wu);
#else
    // ensures wu is at least 1,
    // then the check below checks one level below
    shLOG2(wu, width);
    shCEIL(wu, wu);
    shMAX(wu, wu, ONE.repeat(size));
#endif

    Variable scale(makeTemp(lo, "scale"));
    shADD(scale, wu, -ONE.repeat(size));
    shPOW(scale, HALF.repeat(size), scale); // we have (1/2)^s where s = level - (level == 0 ? 0 : 1);

    Variable au(makeTemp(lo, "au"));
    Variable bu(makeTemp(lo, "bu"));
    shMUL(au, clamplo, scale);
    shFLR(au, au);
    shMUL(bu, clamphi, scale);
    shFLR(bu, bu);

    Variable diff(makeTemp(lo, "diff"));
    Variable lu(makeTemp(lo, "lu"));
    shADD(diff, bu, -au);
    shSLT(lu, diff, HALF.repeat(size)); // use half instead of 1 for numerical issues 
    shADD(lu, wu, -lu);

    Variable l(makeTemp(lo, "l", 1)); // max level
    if(lu.size() == 1) {
      shASN(l, lu);
    } else {
      // slow...
      shMAX(l, lu(0), lu(1));
      for(int i = 2; i < lu.size(); ++i) {
        shMAX(l, l, lu(i));
      }
    }

    Variable scaledLo(makeTemp(lo, "scaledLo"));
    Variable scaledHi(makeTemp(hi, "scaledHi"));
    Variable offset(makeTemp(scale, "offset", 1));
    ConstAttrib1f texWidth = m_width; 
    shPOW(scale, HALF.repeat(size), l); 
    shMUL(offset, texWidth, scale(0));

    shMUL(scaledLo, clamplo, scale); 
    Variable scaledLo0 = scaledLo(0);
    shADD(scaledLo0, scaledLo0, offset);

    shMUL(scaledHi, clamphi, scale);
    Variable scaledHi0 = scaledHi(0);
    shADD(scaledHi0, scaledHi0, offset);

    Variable resultLo(m_node[0]->clone(SH_TEMP, 0, VALUETYPE_END, SEMANTICTYPE_END, true, false));
    Variable resultHi(m_node[1]->clone(SH_TEMP, 0, VALUETYPE_END, SEMANTICTYPE_END, true, false));
    Variable tempLo(m_node[0]->clone(SH_TEMP, 0, VALUETYPE_END, SEMANTICTYPE_END, true, false));
    Variable tempHi(m_node[1]->clone(SH_TEMP, 0, VALUETYPE_END, SEMANTICTYPE_END, true, false));
    resultLo.name("resultLo");
    resultHi.name("resultHi");
    tempLo.name("tempLo");
    tempHi.name("tempHi");
    Variable loTex(m_node[0]);
    Variable hiTex(m_node[1]);

    // @todo range should use multi array index here, but bleah 
    for(i = 0; i < (1U << m_dims); ++i) {
      Variable texcoord(makeTemp(lo, "texcoord"));

      for(j = 0; j < m_dims; ++j) {
        Variable texcoordj = texcoord(j);
        shASN(texcoordj, (1 & (i >> j)) ? ONE : ZERO);  // really stupid
      }
      shLRP(texcoord, texcoord, scaledLo, scaledHi);

      // @todo debug - should change below back to TEXI when I figure out what's
      // wrong with this
      //shDIV(texcoord, texcoord, m_other->texSizeVar());

      Statement stmtlo(tempLo, loTex, OP_TEXI, texcoord);
      Context::current()->parsing()->tokenizer.blockList()->addStatement(stmtlo);

      Statement stmthi(tempHi, hiTex, OP_TEXI, texcoord);
      Context::current()->parsing()->tokenizer.blockList()->addStatement(stmthi);

      if(i == 0) {
        shASN(resultLo, tempLo);
        shASN(resultHi, tempHi);
      } else {
        shMIN(resultLo, tempLo, resultLo);
        shMAX(resultHi, tempHi, resultHi);
      }
    }

    // @todo debug 
    //result.append(scale);
    //result.append(resultHi);
    result.append(resultLo);
    result.append(resultHi);
  } SH_END_SECTION;

  return result;
}
    
Record IntervalTexture::lookup(const Variable& lo, const Variable& hi) const {
  Variable lookupLo(m_node[0]->clone(SH_TEMP, 0, VALUETYPE_END, SEMANTICTYPE_END, true, false));
  Variable lookupHi(m_node[1]->clone(SH_TEMP, 0, VALUETYPE_END, SEMANTICTYPE_END, true, false));
  lookupLo.name("lookupLo");
  lookupHi.name("lookupHi");

  Record result = lookupLo & lookupHi; 
  SH_BEGIN_SECTION("iatex_lookup") {
    Variable scaled_lo(makeTemp(lo, "aatex_scl_lo"));
    Variable scaled_hi(makeTemp(hi, "aatex_scl_hi"));

    // turn it into a rect lookup
    Variable texsize = m_other->texSizeVar();
    shMUL(scaled_lo, lo, texsize); 
    shMUL(scaled_hi, hi, texsize); 

    result = rect_lookup(scaled_lo, scaled_hi);
  } SH_END_SECTION;

  return result;
}


TextureNodePtr IntervalTexture::node(size_t i) {
  return m_node[i];
}

IntervalTexture::TexIATexMap IntervalTexture::tiatmap;

IntervalTexturePtr IntervalTexture::convert(TextureNodePtr other) 
{
  TexIATexMap::iterator I = tiatmap.find(other);
  if(I != tiatmap.end()) {
    return I->second;
  }
  return (tiatmap[other] = new IntervalTexture(other));
}

}
