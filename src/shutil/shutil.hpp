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
#ifndef SHUTIL_HPP
#define SHUTIL_HPP

#include "Func.hpp"
#include "KernelLib.hpp"
#include "KernelLight.hpp"
#include "KernelSurfMap.hpp"
#include "KernelSurface.hpp"
#include "KernelPost.hpp"
#include "Mesh.hpp"
#include "ObjMesh.hpp"
#include "PngImage.hpp"
#include <string>

/** \namespace ShUtil
 * \brief The main ShUtil namespace.
 *
 * All the extra Sh utility functions and objects reside within this namespace.
 */

/** \file shutil.hpp
 * \brief The include file for extra Sh utilities 
 *
 * You can use this to include all available extra utilities.
 */
namespace ShUtil {

/// Converts "/" to "\" on Windows and the reverse on other OSes
std::string normalize_path(const std::string& path);

}

#endif
