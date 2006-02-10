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

#include "ShDllExport.hpp"
#include "ShVariableNode.hpp"
#include "ShMemory.hpp"
#include "ShRefCount.hpp"
#include "ShVariable.hpp"

namespace SH {

/** Texture formats.
 * An enumeration of the various ways textures can be laid out.
 */
enum ShTextureDims {
  SH_TEXTURE_1D,   // Power of two
  SH_TEXTURE_2D,   // Power of two
  SH_TEXTURE_RECT, // Non power of two
  SH_TEXTURE_3D,   // Power of two, but depth may not be
  SH_TEXTURE_CUBE // 6 "2D" memory objects, power of two
};

/** Cube map faces.
 * An enumeration of names for the various faces of a cube map.
 */
enum ShCubeDirection {
  SH_CUBE_POS_X = 0,
  SH_CUBE_NEG_X = 1,
  SH_CUBE_POS_Y = 2,
  SH_CUBE_NEG_Y = 3,
  SH_CUBE_POS_Z = 4,
  SH_CUBE_NEG_Z = 5
};

/** Texture traits.
 * An enumeration of the various wrapping modes supported
 * by textures.
 */
class 
SH_DLLEXPORT ShTextureTraits {
public:
  enum Filtering {
    SH_FILTER_NONE,
    SH_FILTER_MIPMAP_NEAREST,
    SH_FILTER_MIPMAP_LINEAR
  };
  
  enum Wrapping {
    SH_WRAP_CLAMP,
    SH_WRAP_CLAMP_TO_EDGE,
    SH_WRAP_REPEAT
  };

  ShTextureTraits(unsigned int interpolation,
                  Filtering filtering,
                  Wrapping wrapping)
    : m_interpolation(interpolation),
      m_filtering(filtering),
      m_wrapping(wrapping)
  {
  }

  bool operator==(const ShTextureTraits& other) const
  {
    return m_interpolation == other.m_interpolation
      && m_filtering == other.m_filtering
      && m_wrapping == other.m_wrapping;
  }

  bool operator!=(const ShTextureTraits& other) const { return !(*this == other); }
  
  unsigned int interpolation() const { return m_interpolation; }
  ShTextureTraits& interpolation(unsigned int interp) { m_interpolation = interp; return *this; }
  
  Filtering filtering() const { return m_filtering; }
  ShTextureTraits& filtering(Filtering filtering) { m_filtering = filtering; return *this; }
  
  Wrapping wrapping() const { return m_wrapping; }
  ShTextureTraits& wrapping(Wrapping wrapping) { m_wrapping = wrapping; return *this; }
  

private:
  unsigned int m_interpolation;
  Filtering m_filtering;
  Wrapping m_wrapping;
};

class 
SH_DLLEXPORT ShTextureNode : public ShVariableNode {
public:
  ShTextureNode(ShTextureDims dims,
                int size, // scalars per tuple 
                ShValueType valueType, // type index 
                const ShTextureTraits&,
                int width, int height = 1, int depth = 1, int max_nb_elements = -1);
  virtual ~ShTextureNode();

  ShTextureDims dims() const;

  // Memory
  ShPointer<const ShMemory> memory(int n) const;
  ShPointer<const ShMemory> memory(ShCubeDirection dir, int n) const;
  ShMemoryPtr memory(int n);
  ShMemoryPtr memory(ShCubeDirection dir, int n);
  void memory(const ShMemoryPtr& memory, int n);
  void memory(const ShMemoryPtr& memory, ShCubeDirection dir, int n);

  // Basic properties - not all may be valid for all types
  const ShTextureTraits& traits() const; // valid for all texture nodes
  ShTextureTraits& traits(); // valid for all texture nodes
  int width() const; // valid for all texture nodes
  int height() const; // 1 for SH_TEXTURE_1D
  int depth() const; // 1 unless SH_TEXTURE_3D
  int count() const; // number of elements  
  int mipmap_levels();

  void setTexSize(int w);
  void setTexSize(int w, int h);
  void setTexSize(int w, int h, int d);
  const ShVariable& texSizeVar() const;

  void count(int n);

  /// Generates the mipmaps levels if necessary (returns TRUE if it did, otherwise FALSE)
  bool build_mipmaps(ShCubeDirection dir = SH_CUBE_POS_X);
  
private:
  int m_count; // max nb of elements sent to the GPU or -1 if unknown (used by the stream backend)

  ShTextureDims m_dims;
  
  ShMemoryPtr* m_memory; // array
  int m_nb_memories; // size of the array
  int m_mipmap_levels;
  int m_mipmap_generation_timestamp[6]; // -1 if they were never generated
  ShMemory* m_mipmap_generation_basemem[6]; // NULL if never generated

  ShTextureTraits m_traits;
  int m_width, m_height, m_depth;

  ShVariable m_texSizeVar;
  ShTextureTraits::Filtering m_old_filtering;

  void initialize_memories(bool force_initialization);
  float interpolate1D(float* base_data, int scale, int x, int component);
  float interpolate2D(float* base_data, int scale, int x, int y, int component);
  float interpolate3D(float* base_data, int scale, int x, int y, int z, int component);
  ShMemoryPtr memory_error(int n) const;

  // NOT IMPLEMENTED
  ShTextureNode(const ShTextureNode& other);
  ShTextureNode& operator=(const ShTextureNode& other);
};

typedef ShPointer<ShTextureNode> ShTextureNodePtr;
typedef ShPointer<const ShTextureNode> ShTextureNodeCPtr;

}
#endif
