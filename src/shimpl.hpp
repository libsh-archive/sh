// Sh: A GPU metaprogramming language.
//
// Copyright (c) 2003 University of Waterloo Computer Graphics Laboratory
// Project administrator: Michael D. McCool
// Authors: Zheng Qin, Stefanus Du Toit, Kevin Moule, Tiberiu S. Popa,
//          Michael D. McCool
// 
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
// 
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 
// 1. The origin of this software must not be misrepresented; you must
// not claim that you wrote the original software. If you use this
// software in a product, an acknowledgment in the product documentation
// would be appreciated but is not required.
// 
// 2. Altered source versions must be plainly marked as such, and must
// not be misrepresented as being the original software.
// 
// 3. This notice may not be removed or altered from any source
// distribution.
//////////////////////////////////////////////////////////////////////////////
#ifndef SHIMPL_HPP
#define SHIMPL_HPP

/** \file shimpl.hpp
 * \brief Sh implicit instantiation header
 *
 * If you use types which are not explicitly instantiated, you must
 * include this file. This is generally only the case if you use
 * tuples in Sh with more than 4 elements.
 */

#include "sh.hpp"
#include "ShAttribImpl.hpp"
#include "ShGenericImpl.hpp"
#include "ShVectorImpl.hpp"
#include "ShNormalImpl.hpp"
#include "ShColorImpl.hpp"
#include "ShTexCoordImpl.hpp"
#include "ShPointImpl.hpp"
#include "ShPositionImpl.hpp"



#endif
