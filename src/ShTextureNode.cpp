// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
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
#include <cassert>
#include "ShTextureNode.hpp"
#include "ShDebug.hpp"
#include "ShGeneric.hpp"
#include "ShAttrib.hpp"

namespace SH {

using namespace std;

ShTextureNode::ShTextureNode(ShTextureDims dims, int size,
                             ShValueType valueType,
                             const ShTextureTraits& traits,
                             int width, int height, int depth, int max_nb_elements)
  : ShVariableNode(SH_TEXTURE, size, valueType), m_count(max_nb_elements),
    m_dims(dims), m_nb_memories(SH_TEXTURE_CUBE == dims ? 6 : 1), m_mipmap_levels(1), m_traits(traits),
    m_width(width), m_height(height), m_depth(depth), m_old_filtering(traits.filtering())
{
  ShContext::current()->enter(0); // need m_texSizeVar to be uniform

  if (m_dims == SH_TEXTURE_1D) {
    ShAttrib1f v = ShAttrib1f(width);
    m_texSizeVar = ShVariable(v.node());
  } else if (m_dims == SH_TEXTURE_3D) {
    ShAttrib3f v = ShAttrib3f(width, height, depth);
    m_texSizeVar = ShVariable(v.node());
  } else {
    ShAttrib2f v = ShAttrib2f(width, height);
    m_texSizeVar = ShVariable(v.node());
  }

  // will be destroyed in initialize_memories()
  m_memory = new ShMemoryPtr[m_nb_memories];
  for (int i=0; i < m_nb_memories; i++) {
    m_memory[i] = NULL;
  }

  initialize_memories(true);

  ShContext::current()->exit(); // need m_texSizeVar to be uniform
}

ShTextureNode::~ShTextureNode()
{
  delete [] m_memory;
}

void ShTextureNode::initialize_memories(bool force_initialization)
{
  if ((m_traits.filtering() == m_old_filtering) && !force_initialization) {
    return; // skip initialization if the filtering trait didn't change
  }

  // make a copy of the base textures
  ShMemoryPtr* old_memory = new ShMemoryPtr[6];
  if (SH_TEXTURE_CUBE == m_dims) {
    for (int i=0; i < 6; i++) {
      old_memory[i] = m_memory[i * m_mipmap_levels];
    }
  } else {
    old_memory[0] = m_memory[0];
  }
  
  // update m_mipmap_levels and m_nb_memories
  m_mipmap_levels = 1;
  if ((m_traits.filtering() == ShTextureTraits::SH_FILTER_MIPMAP_NEAREST) ||
      (m_traits.filtering() == ShTextureTraits::SH_FILTER_MIPMAP_LINEAR)) {
    SH_DEBUG_ASSERT(m_width == m_height);
    for (int w = m_width; w > 1; w >>= 1) {
      ++m_mipmap_levels;
    }
  }
  m_nb_memories = (SH_TEXTURE_CUBE == m_dims) ? 6 * m_mipmap_levels : m_mipmap_levels;

  // reset all textures
  delete [] m_memory;
  m_memory = new ShMemoryPtr[m_nb_memories];
  for (int i=0; i < m_nb_memories; i++) {
    m_memory[i] = NULL;
  }
  for (int i=0; i < 6; i++) {
    m_mipmap_generation_timestamp[i] = -1;
    m_mipmap_generation_basemem[i] = NULL;
  }

  // restore base textures
  if (SH_TEXTURE_CUBE == m_dims) {
    for (int i=0; i < 6; i++) {
      m_memory[i * m_mipmap_levels] = old_memory[i];
    }
  } else {
    m_memory[0] = old_memory[0];
  }
  delete [] old_memory;

  m_old_filtering = m_traits.filtering();
}

int ShTextureNode::mipmap_levels()
{
  initialize_memories(false);
  return m_mipmap_levels;
}

ShTextureDims ShTextureNode::dims() const
{
  return m_dims;
}

ShMemoryPtr ShTextureNode::memory_error(int n) const
{
  stringstream s;
  s << n << " is not a valid memory number (nb_memories = " << m_nb_memories 
    << ", mipmapping = " << m_traits.filtering() <<")";
  shError(ShException(s.str()));
  return NULL;
}

ShMemoryPtr ShTextureNode::memory(int n)
{
  initialize_memories(false); // nb_memories will change if filtering was changed
  if (n < m_nb_memories) {
    return m_memory[n];
  } else {
    return memory_error(n);
  }
}

ShPointer<const ShMemory> ShTextureNode::memory(int n) const
{
  if (n < m_nb_memories) {
    return m_memory[n];
  } else {
    return memory_error(n);
  }
}

void ShTextureNode::memory(ShMemoryPtr memory, int n)
{
  initialize_memories(0 == n); // nb_memories will change if filtering was changed
  if (n < m_nb_memories) {
    m_memory[n] = memory;
  } else {
    memory_error(n);
  }
}

ShMemoryPtr ShTextureNode::memory(ShCubeDirection dir, int n)
{
  return memory(static_cast<int>(dir) * mipmap_levels() + n);
}

ShPointer<const ShMemory> ShTextureNode::memory(ShCubeDirection dir, int n) const
{
  return memory(static_cast<int>(dir) * m_mipmap_levels + n);
}

void ShTextureNode::memory(ShMemoryPtr mem, ShCubeDirection dir, int n)
{
  memory(mem, static_cast<int>(dir) * mipmap_levels() + n);
}

const ShTextureTraits& ShTextureNode::traits() const
{
  return m_traits;
}

ShTextureTraits& ShTextureNode::traits()
{
  return m_traits;
}

void ShTextureNode::setTexSize(int w)
{
  m_width = w;
  ShContext::current()->enter(0);
  ShAttrib1f s(m_texSizeVar.node(), ShSwizzle(m_texSizeVar().size()), false);
  s = static_cast<float>(w);
  initialize_memories(true);
  ShContext::current()->exit();
}

void ShTextureNode::setTexSize(int w, int h)
{
  m_width = w;
  m_height = h;
  ShContext::current()->enter(0);
  ShAttrib2f s(m_texSizeVar.node(), ShSwizzle(m_texSizeVar().size()), false);
  s = ShAttrib2f(w, h);
  initialize_memories(true);
  ShContext::current()->exit();
}

void ShTextureNode::setTexSize(int w, int h, int d)
{
  m_width = w;
  m_height = h;
  m_depth = d;
  ShContext::current()->enter(0);
  ShAttrib3f s(m_texSizeVar.node(), ShSwizzle(m_texSizeVar().size()), false);
  s = ShAttrib3f(w, h, d);
  initialize_memories(true);
  ShContext::current()->exit();
}

int ShTextureNode::width() const
{
  return m_width;
}

int ShTextureNode::height() const
{
  return m_height;
}

int ShTextureNode::depth() const
{
  return m_depth;
}

int ShTextureNode::count() const
{
  return (m_count != -1) ? m_count : m_width * m_height * m_depth;
}

void ShTextureNode::count(int n)
{
  m_count = n;
}

const ShVariable& ShTextureNode::texSizeVar() const
{
  return m_texSizeVar;
}

float ShTextureNode::interpolate1D(float* base_data, int scale,
				   int x, int component)
{
  float sum = 0;
  for (int i=0; i < scale; i++) {
    sum += base_data[m_size * (x * scale + i) + component];
  }
  return static_cast<unsigned char>(sum / scale);
}

float ShTextureNode::interpolate2D(float* base_data, int scale,
				   int x, int y, int component)
{
  float sum = 0;
  for (int j=0; j < scale; j++) {
    for (int i=0; i < scale; i++) {
      sum += base_data[m_size * ((y * scale + j) * m_width + (x * scale + i)) + component];;
    }
  }
  return sum / (scale * scale);
}

float ShTextureNode::interpolate3D(float* base_data, int scale,
				   int x, int y, int z, int component)
{
  float sum = 0;
  for (int k=0; k < scale; k++) {
    for (int j=0; j < scale; j++) {
      for (int i=0; i < scale; i++) {
	sum += base_data[m_size * ((z * scale + k) * m_height + ((y * scale + j) * m_width + (x * scale + i))) + component];;
      }
    }
  }
  return sum / (scale * scale * scale);
}

bool ShTextureNode::build_mipmaps(ShCubeDirection dir)
{
  if (m_mipmap_levels <= 1) return false; // mipmapping not enabled

  if (!memory(dir, 0)) return false; // main memory not set (probably using metadata)

  if (memory(dir, 1)) {
    // Don't overwrite user-provided mipmap levels
    if (-1 == m_mipmap_generation_timestamp[dir]) return false;

    // Don't rebuild the levels if the timestamp and memory are the same
    if ((memory(dir, 0) == m_mipmap_generation_basemem[dir]) &&
        (memory(dir, 0)->timestamp() == m_mipmap_generation_timestamp[dir]))
      return false;
  }

  int width = m_width;
  int height = m_height;
  int depth = m_depth;
  int levels = mipmap_levels();
  int direction = static_cast<int>(dir);

  // Create a float copy of the base storage
  int base_memsize = m_size * m_width * m_height * m_depth * shTypeInfo(SH_FLOAT)->datasize();
  ShHostMemoryPtr base_memory = new ShHostMemory(base_memsize, SH_FLOAT);
  float* base_data = reinterpret_cast<float*>(base_memory->hostStorage()->data());
  ShHostStoragePtr base_storage = shref_dynamic_cast<ShHostStorage>(m_memory[direction * levels]->findStorage("host"));
  if (!ShStorage::transfer(base_storage.object(), base_memory->hostStorage().object())) {
    SH_DEBUG_ASSERT(0);
    return false;
  }

  for (int i=1; i < levels; i++) {
    switch (m_dims) {
    case SH_TEXTURE_3D:
      depth >>= 1;
    case SH_TEXTURE_2D:
    case SH_TEXTURE_RECT:
    case SH_TEXTURE_CUBE:
      height >>= 1;
    case SH_TEXTURE_1D:
      width >>= 1;
    }

    int memsize = m_size * width * height * depth * shTypeInfo(SH_FLOAT)->datasize();
    ShHostMemoryPtr memory = new ShHostMemory(memsize, SH_FLOAT);
    float* new_data = reinterpret_cast<float*>(memory->hostStorage()->data());

    // Nb of texels (for each axis) in the base texture used to
    // generate one texel in this mipmap level
    //
    // e.g. given an 8x8 base texture, each texel at mipmap level 2
    // will use 4 texels from the base texture in the x axis by 4
    // texels in the y axis (total of 16 texels to look at).
    int scale = 1 << i;

    switch (m_dims) {
    case SH_TEXTURE_1D:
      for (int x=0; x < width; x++) {
	for (int c=0; c < m_size; c++) {
	  new_data[m_size * x + c] = interpolate1D(base_data, scale, x, c);
	}
      }
      break;
    case SH_TEXTURE_2D:
    case SH_TEXTURE_RECT:
    case SH_TEXTURE_CUBE:
      for (int y=0; y < height; y++) {
	for (int x=0; x < width; x++) {
	  for (int c=0; c < m_size; c++) {
	    new_data[m_size * (y * width + x) + c] = interpolate2D(base_data, scale, x, y, c);
	  }
	}
      }
      break;
    case SH_TEXTURE_3D:
      for (int z=0; z < depth; z++) {
	for (int y=0; y < height; y++) {
	  for (int x=0; x < width; x++) {
	    for (int c=0; c < m_size; c++) {
	      new_data[m_size * (z * height + (y * width + x)) + c] = interpolate3D(base_data, scale, x, y, z, c);
	    }
	  }
	}
      }
      break;
    }

    m_memory[direction * levels + i] = memory;
  }

  m_mipmap_generation_timestamp[dir] = memory(dir, 0)->timestamp();
  m_mipmap_generation_basemem[dir] = memory(dir, 0).object();
  return true;
}

}
