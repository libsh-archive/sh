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
#ifndef SHBASETEXTURE_HPP
#define SHBASETEXTURE_HPP

#include <string>
#include "DllExport.hpp"
#include "TextureNode.hpp"
#include "Memory.hpp"
#include "MetaForwarder.hpp"

namespace SH {

class
SH_DLLEXPORT BaseTexture : public MetaForwarder {
public:
  BaseTexture(const TextureNodePtr& node);

  void build_mipmaps() { m_node->build_mipmaps(); }

  /// Obtain access to the node that this texture refers to
  const TextureNodePtr & node() const { return m_node; }

  /// Get the count for n dimensions
  void get_count(int* count, int n) const;
  /// Get the stride for n dimensions
  void get_stride(int* stride, int n) const;
  /// Get the offset for n dimensions
  void get_offset(int* offset, int n) const;
  /// Get the repeat for n dimensions
  void get_repeat(int* repeat, int n) const;
  /// Set the count for n dimensions
  void set_count(const int* count, int n);
  /// Set the stride for n dimensions
  void set_stride(const int* count, int n);
  /// Set the offset for n dimensions
  void set_offset(const int* offset, int n);
  /// Set the repeat for n dimensions
  void set_repeat(const int* repeat, int n);
  
  /// Sync and return a pointer to memory \a n
  void* read_data(int n) const;
  /// Dirty and return a pointer to memory \a n
  void* write_data(int n);

protected:
  int m_count[3], m_stride[3], m_offset[3], m_repeat[3];
  TextureNodePtr m_node;
};

}

#endif
