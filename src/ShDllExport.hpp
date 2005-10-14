// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
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
#ifndef SHDLLEXPORT_HPP
#define SHDLLEXPORT_HPP

#if defined(WIN32)
# define SH_DLLEXPORT __declspec(dllimport)
# define SH_DLLLOCAL
#elif defined(HAVE_GCCVISIBILITYPATCH)
# define SH_DLLEXPORT __attribute__ ((visibility("default")))
# define SH_DLLLOCAL __attribute__ ((visibility("hidden")))
#else
# define SH_DLLEXPORT
# define SH_DLLLOCAL
#endif

#endif
