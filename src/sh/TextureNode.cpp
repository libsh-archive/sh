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
#include <cassert>
#include "TextureNode.hpp"
#include "Debug.hpp"
#include "Generic.hpp"
#include "Attrib.hpp"

namespace SH {

using namespace std;

TextureNode::TextureNode(TextureDims dims, int size,
                             ValueType valueType,
                             const TextureTraits& traits,
                             int width, int height, int depth)
  : VariableNode(SH_TEXTURE, size, valueType), m_dims(dims),
    m_nb_memories(SH_TEXTURE_CUBE == dims ? 6 : 1), m_mipmap_levels(1), m_traits(traits),
    m_width(width), m_height(height), m_depth(depth), m_old_filtering(traits.filtering())
{
  Context::current()->enter(0); // need m_texSizeVar to be uniform

  if (m_dims == SH_TEXTURE_1D) {
    Attrib1f v = Attrib1f(width);
    m_texSizeVar = Variable(v.node());
  } else if (m_dims == SH_TEXTURE_3D) {
    Attrib3f v = Attrib3f(width, height, depth);
    m_texSizeVar = Variable(v.node());
  } else {
    Attrib2f v = Attrib2f(width, height);
    m_texSizeVar = Variable(v.node());
  }

  // will be destroyed in initialize_memories()
  m_memory = new MemoryPtr[m_nb_memories];
  for (int i=0; i < m_nb_memories; i++) {
    m_memory[i] = NULL;
  }

  initialize_memories(true);

  Context::current()->exit(); // need m_texSizeVar to be uniform
}

TextureNode::~TextureNode()
{
  delete [] m_memory;
}

void TextureNode::initialize_memories(bool force_initialization)
{
  if ((m_traits.filtering() == m_old_filtering) && !force_initialization) {
    return; // skip initialization if the filtering trait didn't change
  }

  // make a copy of the base textures
  MemoryPtr* old_memory = new MemoryPtr[6];
  if (SH_TEXTURE_CUBE == m_dims) {
    for (int i=0; i < 6; i++) {
      old_memory[i] = m_memory[i * m_mipmap_levels];
    }
  } else {
    old_memory[0] = m_memory[0];
  }
  
  // update m_mipmap_levels and m_nb_memories
  m_mipmap_levels = 1;
  if ((m_traits.filtering() == TextureTraits::FILTER_MIPMAP_NEAREST) ||
      (m_traits.filtering() == TextureTraits::FILTER_MIPMAP_LINEAR)) {
    for (int w = m_width; w > 1; w >>= 1) {
      ++m_mipmap_levels;
    }
  }
  m_nb_memories = (SH_TEXTURE_CUBE == m_dims) ? 6 * m_mipmap_levels : m_mipmap_levels;

  // reset all textures
  delete [] m_memory;
  m_memory = new MemoryPtr[m_nb_memories];
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

int TextureNode::mipmap_levels()
{
  initialize_memories(false);
  return m_mipmap_levels;
}

int TextureNode::num_memories() const
{
  return m_nb_memories;
}

TextureDims TextureNode::dims() const
{
  return m_dims;
}

MemoryPtr TextureNode::memory_error(int n) const
{
  stringstream s;
  s << n << " is not a valid memory number (nb_memories = " << m_nb_memories 
    << ", mipmapping = " << m_traits.filtering() <<")";
  error(Exception(s.str()));
  return NULL;
}

MemoryPtr TextureNode::memory(int n)
{
  initialize_memories(false); // nb_memories will change if filtering was changed
  if (n < m_nb_memories) {
    return m_memory[n];
  } else {
    return memory_error(n);
  }
}

Pointer<const Memory> TextureNode::memory(int n) const
{
  if (n < m_nb_memories) {
    return m_memory[n];
  } else {
    return memory_error(n);
  }
}

void TextureNode::memory(const MemoryPtr& memory, int n)
{
  initialize_memories(0 == n); // nb_memories will change if filtering was changed
  if (n < m_nb_memories) {
    m_memory[n] = memory;
  } else {
    memory_error(n);
  }
}

MemoryPtr TextureNode::memory(CubeDirection dir, int n)
{
  return memory(static_cast<int>(dir) * mipmap_levels() + n);
}

Pointer<const Memory> TextureNode::memory(CubeDirection dir, int n) const
{
  return memory(static_cast<int>(dir) * m_mipmap_levels + n);
}

void TextureNode::memory(const MemoryPtr& mem, CubeDirection dir, int n)
{
  memory(mem, static_cast<int>(dir) * mipmap_levels() + n);
}

const TextureTraits& TextureNode::traits() const
{
  return m_traits;
}

TextureTraits& TextureNode::traits()
{
  return m_traits;
}

void TextureNode::setTexSize(int w)
{
  m_width = w;
  Context::current()->enter(0);
  Attrib1f s(m_texSizeVar.node(), Swizzle(m_texSizeVar().size()), false);
  s = static_cast<float>(w);
  initialize_memories(true);
  Context::current()->exit();
}

void TextureNode::setTexSize(int w, int h)
{
  m_width = w;
  m_height = h;
  Context::current()->enter(0);
  Attrib2f s(m_texSizeVar.node(), Swizzle(m_texSizeVar().size()), false);
  s = Attrib2f(w, h);
  initialize_memories(true);
  Context::current()->exit();
}

void TextureNode::setTexSize(int w, int h, int d)
{
  m_width = w;
  m_height = h;
  m_depth = d;
  Context::current()->enter(0);
  Attrib3f s(m_texSizeVar.node(), Swizzle(m_texSizeVar().size()), false);
  s = Attrib3f(w, h, d);
  initialize_memories(true);
  Context::current()->exit();
}

int TextureNode::width() const
{
  return m_width;
}

int TextureNode::height() const
{
  return m_height;
}

int TextureNode::depth() const
{
  return m_depth;
}

const Variable& TextureNode::texSizeVar() const
{
  return m_texSizeVar;
}

float TextureNode::interpolate1D(float* base_data, int scale,
				   int x, int component)
{
  float sum = 0;
  for (int i=0; i < scale; i++) {
    sum += base_data[m_size * (x * scale + i) + component];
  }
  return static_cast<unsigned char>(sum / scale);
}

float TextureNode::interpolate2D(float* base_data, int scale,
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

float TextureNode::interpolate3D(float* base_data, int scale,
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

bool TextureNode::build_mipmaps(CubeDirection dir)
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

  // Check for supported texture sizes
  switch (m_dims) {
  case SH_TEXTURE_RECT:
    error(Exception("Rect textures cannot be mipmapped."));
    break;
  case SH_TEXTURE_1D:
    break; // no check necessary
  case SH_TEXTURE_3D: {
    if (depth != height || width != height || width != depth) {
      error(Exception("The width, height and depth of mipmapped 3D textures must all be the same size."));
    }
    break;
  }
  case SH_TEXTURE_2D:
  case SH_TEXTURE_CUBE: {
    if (width != height) {
      error(Exception("The width and height of mipmapped 2D textures must be of the same size."));
    }
    break;
  }
  }

  // Create a float copy of the base storage
  int base_memsize = m_size * m_width * m_height * m_depth * typeInfo(SH_FLOAT)->datasize();
  HostMemoryPtr base_memory = new HostMemory(base_memsize, SH_FLOAT);
  float* base_data = reinterpret_cast<float*>(base_memory->hostStorage()->data());
  HostStoragePtr base_storage = shref_dynamic_cast<HostStorage>(m_memory[direction * levels]->findStorage("host"));
  if (!Storage::transfer(base_storage.object(), base_memory->hostStorage().object())) {
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

    int memsize = m_size * width * height * depth * typeInfo(SH_FLOAT)->datasize();
    HostMemoryPtr memory = new HostMemory(memsize, SH_FLOAT);
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
