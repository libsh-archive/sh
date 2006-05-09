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
#ifndef SHLIBSTREAM_HPP
#define SHLIBSTREAM_HPP

#include "Stream.hpp"

namespace SH {

/*
Record reduce(const Program& program, const Stream& stream);

template <typename T>
T reduce(const Program& program, const Channel<T>& channel);
*/

template <typename T>
Array1D<T> offset(const Array1D<T>& array, int val);

template <typename T>
Array2D<T> offset(const Array2D<T>& array, int x, int y);

template <typename T>
Array3D<T> offset(const Array3D<T>& array, int x, int y, int z);


template <typename T>
Array1D<T> stride(const Array1D<T>& array, int val);

template <typename T>
Array2D<T> stride(const Array2D<T>& array, int x, int y);

template <typename T>
Array3D<T> stride(const Array3D<T>& array, int x, int y, int z);


template <typename T>
Array1D<T> count(const Array1D<T>& array, int val);

template <typename T>
Array2D<T> count(const Array2D<T>& array, int x, int y);

template <typename T>
Array3D<T> count(const Array3D<T>& array, int x, int y, int z);


template <typename T>
Array1D<T> repeat(const Array1D<T>& array, int val);

template <typename T>
Array2D<T> repeat(const Array2D<T>& array, int x, int y);

template <typename T>
Array3D<T> repeat(const Array3D<T>& array, int x, int y, int z);


template <typename T>
Array1D<T> slice(const Array1D<T>& array, int start, int count);

template <typename T>
Array2D<T> slice(const Array2D<T>& array, int x, int y, int nx, int ny);

template <typename T>
Array3D<T> slice(const Array3D<T>& array, 
                 int x, int y, int z, int nx, int ny, int nz);


template <typename T, typename T2>
void gather(const Array1D<T>& dest,
            const Array1D<T>& src, 
            const Array1D<T2>& index);

}

#include "LibStreamImpl.hpp"

#endif
