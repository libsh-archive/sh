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
#ifndef SHHACKS_HPP
#define SHHACKS_HPP

/** @file Hacks.hpp 
 * Various hacks to get things working for now
 */
#include "Program.hpp"

namespace SH {

/* Removes any bound uniform inputs and sticks them into the program.
 * This is needed right now to run programs with bound uniforms as shaders,
 * including all the kernels in shutil.
 *
 * (Support for bound uniforms only existed for stream kernels)   
 *
 * It also currently does not work if there are any bound streams */
SH_DLLEXPORT
void lock_in_uniforms(Program& p); 
}

#endif
