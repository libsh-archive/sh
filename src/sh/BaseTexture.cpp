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
#include "BaseTexture.hpp"

namespace SH {

BaseTexture::BaseTexture(const TextureNodePtr& node)
  : MetaForwarder(node.object()),
    m_node(node)
{
  for (int i = 0; i < 3; ++i) {
    m_stride[i] = 1;
    m_offset[i] = 0;
  }
  m_count[0] = node->width();
  m_count[1] = node->height();
  m_count[2] = node->depth();
}

void BaseTexture::get_offset(int* offset, int n) const
{
  DEBUG_ASSERT(n <= 3);
  for (int i = 0; i < n; ++i)
    offset[i] = m_offset[i];
}

void BaseTexture::get_stride(int* stride, int n) const
{
  DEBUG_ASSERT(n <= 3);
  for (int i = 0; i < n; ++i)
    stride[i] = m_stride[i];
}

void BaseTexture::get_count(int* count, int n) const
{
  DEBUG_ASSERT(n <= 3);
  for (int i = 0; i < n; ++i)
    count[i] = m_count[i];
}

void BaseTexture::get_repeat(int* repeat, int n) const
{
  DEBUG_ASSERT(n <= 3);
  for (int i = 0; i < n; ++i)
    repeat[i] = m_repeat[i];
}

void BaseTexture::set_offset(const int* offset, int n)
{
  DEBUG_ASSERT(n <= 3);
  for (int i = 0; i < n; ++i)
    m_offset[i] = offset[i];
}

void BaseTexture::set_stride(const int* stride, int n)
{
  DEBUG_ASSERT(n <= 3);
  for (int i = 0; i < n; ++i)
    m_stride[i] = stride[i];
}

void BaseTexture::set_count(const int* count, int n)
{
  DEBUG_ASSERT(n <= 3);
  for (int i = 0; i < n; ++i)
    m_count[i] = count[i];
}

void BaseTexture::set_repeat(const int* repeat, int n)
{
  DEBUG_ASSERT(n <= 3);
  for (int i = 0; i < n; ++i)
    m_repeat[i] = repeat[i];
}

void* BaseTexture::read_data(int n)
{
  StoragePtr storage = m_node->memory(n)->findStorage("host");
  if (!storage) error(Exception("No host storage found"));
  HostStoragePtr host_storage = shref_dynamic_cast<HostStorage>(storage);
  DEBUG_ASSERT(host_storage);
  host_storage->sync();
  return host_storage->data();
}

void* BaseTexture::write_data(int n)
{
  StoragePtr storage = m_node->memory(n)->findStorage("host");
  if (!storage) error(Exception("No host storage found"));
  HostStoragePtr host_storage = shref_dynamic_cast<HostStorage>(storage);
  DEBUG_ASSERT(host_storage);
  host_storage->dirty();
  return host_storage->data();
}

}
