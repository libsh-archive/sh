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
#ifndef SHDEBUG_HPP
#define SHDEBUG_HPP

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef DEBUG
#include <iostream>
#include <cstdlib>

#define SH_DEBUG_PRINT(x) { std::cerr << __FILE__ << ":" << __LINE__ << ": " << x << std::endl; }
#define SH_DEBUG_WARN(x) { SH_DEBUG_PRINT("Warning: " << x) }
#define SH_DEBUG_ERROR(x) { SH_DEBUG_PRINT("Error: " << x) }
#define SH_DEBUG_ASSERT(cond) { if (!(cond)) { SH_DEBUG_ERROR("Assertion failed: " << # cond); abort(); } }

#else

#define SH_DEBUG_PRINT(x)
#define SH_DEBUG_WARN(x)
#define SH_DEBUG_ERROR(x)
#define SH_DEBUG_ASSERT(cond)

#endif // DEBUG

#endif
