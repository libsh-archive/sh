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
#include "ShSyntax.hpp"
#include "ShStatement.hpp"
#include "ShTexCoord.hpp"
#include "ShAttrib.hpp"
#include "ShLib.hpp"
#include "ShIntervalTexture.hpp"

namespace {
  struct Min4 {
    static float doit(float a, float b, float c, float d) {
      return std::min(std::min(a, b), std::min(c, d));
    }
  };

  struct Max4 {
    static float doit(float a, float b, float c, float d) {
      return std::max(std::max(a, b), std::max(c, d));
    }
  };
}

namespace SH {

ShIntervalTexture::ShIntervalTexture(ShTextureNodePtr other) 
  : m_other(other), 
    m_node(new ShTextureNode(SH_TEXTURE_RECT, other->count(), other->valueType(), other->traits(),
                             other->width() * 2, other->height(), other->depth()))
{
    m_other->memory()->add_dep(this);
    m_other->memory()->flush(); // update the data
    SH_DEBUG_ASSERT(!shIsInterval(other->valueType()));
}

// create a new texture memory
// save the original texture and all the mipmap levels on its right
void ShIntervalTexture::memory_update() {
  int i, j, k;
  int width = m_other->width(); 
  int height = m_other->height();
  //int depth = m_other->depth();
  int count = m_other->count(); 
  //int typesize = shTypeInfo(m_other->valueType())->datasize();

  // generate memory for lo/hi if they don't have them yet or they need to be
  // resized

  // access the original data
  ShHostStoragePtr oldmem = shref_dynamic_cast<ShHostStorage>(m_other->memory()->findStorage("host"));
  // @todo range - if not float, convert it to float
  ArrayAdaptor olddata((float*)oldmem->data(), width, height, count);

  // create a new memory for the new texture
  m_width = 2 * width;
  ShHostMemoryPtr newmem = new ShHostMemory(m_width * height * count * sizeof(float));
  ArrayAdaptor newdata((float*)newmem->hostStorage()->data(), m_width, height, count);

  // update the new texture parameters
  m_node->memory(newmem, 0);
  m_node->setTexSize(m_width, height);

  // copy over the original data into new memory 
  for(j = 0; j < height; ++j) for(i = 0; i < width; ++i) for(k = 0; k < count; ++k) {
    newdata(i + width, j, k) = olddata(i, j, k); 
  }

  // create the lower resolution lo/hi levels
  int offy = 0; // offset of the hi level (starts at 0 for the first level, goes to height/2 afterwards)
  for(; width > 0 && height > 0; width /= 2, height /= 2) {
    generate_level<Min4>(newdata, width, 0, width, height, count, width/2, 0); 
    generate_level<Max4>(newdata, width, offy, width, height, count, width/2, height / 2); 
    offy = height / 2;
  }

  for(j = 0; j < height; ++j) {
    for(i = 0; i < width; ++i) {
      for(k = 0; k < count; ++k) {
        std::cout << newdata(i, j, k) << ",";
      }
      std::cout << " ";
    }
    std::cout << "\n";
  }
}

void ShIntervalTexture::add_lookup(const ShVariable &tc, const ShVariable &result) const
{
  ShVariable texVar(m_node);
  ShStatement stmt(result, texVar, SH_OP_TEXI, tc); 
  ShContext::current()->parsing()->tokenizer.blockList()->addStatement(stmt);
}

void ShIntervalTexture::rect_lookup(ShVariable &result, ShVariable &tcvar) const 
{
  SH_DEBUG_ASSERT(tcvar.size() == 2); // @todo range make this class more general 
  ShTexCoord2i_f SH_DECL(tc);
  shASN(tc, tcvar);
  ShAttrib2f SH_DECL(width) = range_width(tc) + 1; // width of the range; 
  ShAttrib1f SH_DECL(level) = min(1, ceil(log2(max(width)))); // mip-map level
  ShAttrib1f SH_DECL(scale) = pow(0.5,level - 1); // reduction factor
  ShAttrib2f SH_DECL(texsize);
  shASN(texsize, m_node->texSizeVar());

  ShVariable temp(result.node()->clone(SH_TEMP, false, false));
  ShVariable lo_temp(new ShVariableNode(SH_TEMP, m_node->count(),
        m_node->valueType())); 
  ShVariable hi_temp(new ShVariableNode(SH_TEMP, m_node->count(),
        m_node->valueType())); 

  // figure out how much to offset for the lo/hi lookups
  ShConstAttrib2f loOffset, hiOffset;
  loOffset(0) = scale;
  loOffset(1) = 0;
  hiOffset(0) = scale;
  hiOffset(1) = cond(level < 1.5, 0, 0.5); 

  // generate texcoords for two corners
  tc *= scale;
  tc = floor(tc);

  ShTexCoord2f u[2]; 
  u[0] = range_lo(tc);
  u[1] = range_hi(tc);
  
  if(this->m_node->traits().wrapping() == ShTextureTraits::SH_WRAP_REPEAT) {
    u[0] = frac(u[0]/texsize)*texsize;
    u[1] = frac(u[1]/texsize)*texsize;
  } 

  ShTexCoord2f lou[2], hiu[2];
  for(int i = 0; i < 2; ++i) {
    lou[i] = u[i] + texsize * loOffset;  
    hiu[i] = u[i] + texsize * hiOffset;  
  }

  // merge lookup results 
  // @todo range - don't always need all four samples
  // but conditional code may very well be slower than always
  // taking four samples since if we sample the same spot twice
  // it should be in the texture cache.
  for(int i = 0; i < 2; ++i) for(int j = 0; j < 2; ++j) {
    ShTexCoord2f lotc, hitc;
    lotc(0) = lou[i](0);
    lotc(1) = lou[j](1);
    hitc(0) = hiu[i](0);
    hitc(1) = hiu[j](1);

    add_lookup(lo_temp, lotc);     
    add_lookup(hi_temp, hitc); 
    if(i == 0 && j == 0) {
      shIVAL(result, lo_temp, hi_temp);
    } else {
      shIVAL(temp, lo_temp, hi_temp);
      shUNION(result, result, temp);
    }
  }
}
    
void ShIntervalTexture::lookup(ShVariable &result, ShVariable &tcvar) const 
{
  ShTexCoord2f SH_DECL(tc);
  shMUL(tc, tcvar, m_node->texSizeVar());
  rect_lookup(result, tc);
}

ShVariable ShIntervalTexture::texVar() {
  return ShVariable(m_node);
}

}
