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
#ifndef SHTEXTURENODE_HPP
#define SHTEXTURENODE_HPP

#include "DllExport.hpp"
#include "VariableNode.hpp"
#include "Memory.hpp"
#include "RefCount.hpp"
#include "Variable.hpp"

namespace SH {

/** Texture formats.
 * An enumeration of the various ways textures can be laid out.
 */
enum TextureDims {
  SH_TEXTURE_1D,   // Power of two
  SH_TEXTURE_2D,   // Power of two
  SH_TEXTURE_RECT, // Non power of two
  SH_TEXTURE_3D,   // Power of two, but depth may not be
  SH_TEXTURE_CUBE // 6 "2D" memory objects, power of two
};

/** Cube map faces.
 * An enumeration of names for the various faces of a cube map.
 */
enum CubeDirection {
  CUBE_POS_X = 0,
  CUBE_NEG_X = 1,
  CUBE_POS_Y = 2,
  CUBE_NEG_Y = 3,
  CUBE_POS_Z = 4,
  CUBE_NEG_Z = 5
};

/** Texture traits.
 * An enumeration of the various wrapping modes supported
 * by textures.
 */
class 
SH_DLLEXPORT TextureTraits {
public:
  enum Filtering {
    FILTER_NONE,
    FILTER_MIPMAP_NEAREST,
    FILTER_MIPMAP_LINEAR
  };
  
  enum Wrapping {
    WRAP_CLAMP,
    WRAP_CLAMP_TO_EDGE,
    WRAP_REPEAT
  };

  TextureTraits(unsigned int interpolation,
                  Filtering filtering,
                  Wrapping wrapping)
    : m_interpolation(interpolation),
      m_filtering(filtering),
      m_wrapping(wrapping)
  {
  }

  bool operator==(const TextureTraits& other) const
  {
    return m_interpolation == other.m_interpolation
      && m_filtering == other.m_filtering
      && m_wrapping == other.m_wrapping;
  }

  bool operator!=(const TextureTraits& other) const { return !(*this == other); }
  
  unsigned int interpolation() const { return m_interpolation; }
  TextureTraits& interpolation(unsigned int interp) { m_interpolation = interp; return *this; }
  
  Filtering filtering() const { return m_filtering; }
  TextureTraits& filtering(Filtering filtering) { m_filtering = filtering; return *this; }
  
  Wrapping wrapping() const { return m_wrapping; }
  TextureTraits& wrapping(Wrapping wrapping) { m_wrapping = wrapping; return *this; }
  

private:
  unsigned int m_interpolation;
  Filtering m_filtering;
  Wrapping m_wrapping;
};

class 
SH_DLLEXPORT TextureNode : public VariableNode {
public:
  TextureNode(TextureDims dims,
                int size, // scalars per tuple 
                ValueType valueType, // type index 
                const TextureTraits&,
                int width, int height = 1, int depth = 1, int max_nb_elements = -1);
  virtual ~TextureNode();

  TextureDims dims() const;

  // Memory
  Pointer<const Memory> memory(int n) const;
  Pointer<const Memory> memory(CubeDirection dir, int n) const;
  MemoryPtr memory(int n);
  MemoryPtr memory(CubeDirection dir, int n);
  void memory(const MemoryPtr& memory, int n);
  void memory(const MemoryPtr& memory, CubeDirection dir, int n);

  // Basic properties - not all may be valid for all types
  const TextureTraits& traits() const; // valid for all texture nodes
  TextureTraits& traits(); // valid for all texture nodes
  int width() const; // valid for all texture nodes
  int height() const; // 1 for SH_TEXTURE_1D
  int depth() const; // 1 unless SH_TEXTURE_3D
  int count() const; // number of elements  
  int mipmap_levels();
  int num_memories() const;

  void setTexSize(int w);
  void setTexSize(int w, int h);
  void setTexSize(int w, int h, int d);
  const Variable& texSizeVar() const;

  void count(int n);

  /// Generates the mipmaps levels if necessary (returns TRUE if it did, otherwise FALSE)
  bool build_mipmaps(CubeDirection dir = CUBE_POS_X);
  
private:
  int m_count; // max nb of elements sent to the GPU or -1 if unknown (used by the stream backend)

  TextureDims m_dims;
  
  MemoryPtr* m_memory; // array
  int m_nb_memories; // size of the array
  int m_mipmap_levels;
  int m_mipmap_generation_timestamp[6]; // -1 if they were never generated
  Memory* m_mipmap_generation_basemem[6]; // NULL if never generated

  TextureTraits m_traits;
  int m_width, m_height, m_depth;

  Variable m_texSizeVar;
  TextureTraits::Filtering m_old_filtering;

  void initialize_memories(bool force_initialization);
  float interpolate1D(float* base_data, int scale, int x, int component);
  float interpolate2D(float* base_data, int scale, int x, int y, int component);
  float interpolate3D(float* base_data, int scale, int x, int y, int z, int component);
  MemoryPtr memory_error(int n) const;

  // NOT IMPLEMENTED
  TextureNode(const TextureNode& other);
  TextureNode& operator=(const TextureNode& other);
};

typedef Pointer<TextureNode> TextureNodePtr;
typedef Pointer<const TextureNode> TextureNodeCPtr;

}
#endif
