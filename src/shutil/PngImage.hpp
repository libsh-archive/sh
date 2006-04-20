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
#ifndef SHPNGIMAGE_HPP
#define SHPNGIMAGE_HPP

#include "sh/Image.hpp"

namespace ShUtil {

/** Libpng wrapper functions
 * Allows Image objects to load their data from or save to a PNG file.
 * @{
 */

/// Load the PNG file into this image
template<typename T>
void load_PNG(SH::TypedImage<T>& image, const std::string& filename);

/// Save the PNG image into this file
template<typename T>
void save_PNG(const SH::TypedImage<T>& image, const std::string& filename, int inverse_alpha=0);

/// Save the PNG image into this file
template<typename T>
void save_PNG16(const SH::TypedImage<T>& image, const std::string& filename, int inverse_alpha=0);

// @}

} // namespace

#include "PngImageImpl.hpp"

#endif // SHPNGIMAGE_HPP
