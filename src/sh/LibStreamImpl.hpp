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
#ifndef SHLIBSTREAMIMPL_HPP
#define SHLIBSTREAMIMPL_HPP

#include "LibStream.hpp"

namespace SH {

/*
Record reduce(const Program& program, const Stream& stream)
{
  Stream dest;
  for (Stream::const_iterator I = stream.begin(); I != stream.end(); ++I) {
    std::size_t dest_size = (*I)->count() + 1 / 2;
    HostMemoryPtr dest_mem = new HostMemory(dest_size, (*I)->valueType());
    ChannelNodePtr dest_chan = new ChannelNode
  }
}
*/

template <typename T>
Array1D<T> offset(const Array1D<T>& array, int val)
{
  Array1D<T> result = array;
  int offset, stride;
  result.get_offset(&offset, 1);
  result.get_stride(&stride, 1);
  offset += val * stride;
  result.set_offset(&offset, 1);
  return result;
}
  
template <typename T>
Array2D<T> offset(const Array2D<T>& array, int x, int y)
{
  Array2D<T> result = array;
  int offset[2], stride[2];
  result.get_offset(offset, 2);
  result.get_stride(stride, 2);
  offset[0] += x * stride[0];
  offset[1] += y * stride[1];
  result.set_offset(offset, 2);
  return result;
}

template <typename T>
Array3D<T> offset(const Array3D<T>& array, int x, int y, int z)
{
  Array3D<T> result = array;
  int offset[3], stride[2];
  result.get_offset(offset, 3);
  result.get_stride(stride, 3);
  offset[0] += x * stride[0];
  offset[1] += y * stride[1];
  offset[2] += z * stride[2];
  result.set_offset(offset, 3);
  return result;
}

template <typename T>
Array1D<T> stride(const Array1D<T>& array, int val)
{
  Array1D<T> result = array;
  int stride, count;
  result.get_stride(&stride, 1);
  result.get_count(&count, 1);
  stride *= val;
  count  /= val;
  result.set_stride(&stride, 1);
  result.set_count(&count, 1);
  return result;
}
  
template <typename T>
Array2D<T> stride(const Array2D<T>& array, int x, int y)
{
  Array2D<T> result = array;
  int stride[2], count[2];
  result.get_stride(stride, 2);
  result.get_count(count, 2);
  stride[0] *= x;
  stride[1] *= y;
  count[0]  /= x;
  count[1]  /= y;
  result.set_stride(stride, 2);
  result.set_count(count, 2);
  return result;
}

template <typename T>
Array3D<T> stride(const Array3D<T>& array, int x, int y, int z)
{
  Array3D<T> result = array;
  int stride[3], count[3];
  result.get_stride(stride, 3);
  result.get_count(count, 3);
  stride[0] *= x;
  stride[1] *= y;
  stride[2] *= z;
  count[0]  /= x;
  count[1]  /= y;
  count[2]  /= z;
  result.set_stride(stride, 3);
  result.set_count(count, 3);
  return result;
}

template <typename T>
Array1D<T> count(const Array1D<T>& array, int val)
{
  Array1D<T> result = array;
  result.set_count(&val, 1);
  return result;
}
  
template <typename T>
Array2D<T> count(const Array2D<T>& array, int x, int y)
{
  Array2D<T> result = array;
  int count[2];
  count[0] = x;
  count[1] = y;
  result.set_count(count, 2);
  return result;
}

template <typename T>
Array3D<T> count(const Array3D<T>& array, int x, int y, int z)
{
  Array3D<T> result = array;
  int count[3];
  count[0] = x;
  count[1] = y;
  count[2] = z;
  result.set_count(count, 3);
  return result;
}


template <typename T>
Array1D<T> repeat(const Array1D<T>& array, int val)
{
  Array1D<T> result = array;
  int repeat;
  result.get_repeat(&repeat, 1);
  repeat *= val;
  result.set_repeat(&repeat, 1);
  return result;
}
  
template <typename T>
Array2D<T> repeat(const Array2D<T>& array, int x, int y)
{
  Array2D<T> result = array;
  int repeat[2];
  result.get_repeat(repeat, 2);
  repeat[0] *= x;
  repeat[1] *= y;
  result.set_repeat(repeat, 2);
  return result;
}

template <typename T>
Array3D<T> repeat(const Array3D<T>& array, int x, int y, int z)
{
  Array3D<T> result = array;
  int repeat[3];
  result.get_repeat(repeat, 3);
  repeat[0] *= x;
  repeat[1] *= y;
  repeat[2] *= z;
  result.set_repeat(repeat, 3);
  return result;
}

template <typename T>
Array1D<T> slice(const Array1D<T>& array, int start, int count)
{
  Array1D<T> result = array;
  int offset, stride;
  result.get_offset(&offset, 1);
  result.get_stride(&stride, 1);
  offset += start * stride;
  result.set_offset(&offset, 1);
  result.set_count(&count, 1);
  return result;
}
  
template <typename T>
Array2D<T> slice(const Array2D<T>& array, int x, int y, int nx, int ny)
{
  Array2D<T> result = array;
  int offset[2], stride[2], count[2];
  result.get_offset(offset, 2);
  result.get_stride(stride, 2);
  offset[0] += x * stride[0];
  offset[1] += y * stride[1];
  count[0]   = nx;
  count[1]   = ny;
  result.set_offset(offset, 2);
  result.set_count(count, 2);
  return result;
}

template <typename T>
Array3D<T> slice(const Array3D<T>& array, 
                 int x, int y, int z, int nx, int ny, int nz)
{
  Array3D<T> result = array;
  int offset[3], stride[2], count[3];
  result.get_offset(offset, 3);
  result.get_stride(stride, 3);
  offset[0] += x * stride[0];
  offset[1] += y * stride[1];
  offset[2] += z * stride[2];
  count[0]   = nx;
  count[1]   = ny;
  count[2]   = nz;
  result.set_offset(offset, 3);
  result.set_count(count, 3);
  return result;
}

template <typename T, typename T2>
Array1D<T> gather(const Array1D<T>& src, const Array1D<T2>& index, std::string target)
{
  if (T2::typesize != 1) {
    error(Exception("gather index must be a 1 tuple"));
    return BaseTexture(0);
  }
  BackendPtr backend = Backend::get_backend(target);
  SH_DEBUG_ASSERT(backend);
  return backend->gather(src, index);
}

}

#endif
