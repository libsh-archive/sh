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
#ifndef SHSECTIONUTIL_HPP
#define SHSECTIONUTIL_HPP

#include <map>
#include "ShDllExport.hpp"
#include "ShInfo.hpp"
#include "ShStructural.hpp"

/** @file ShSectionUtil.hpp
 * Utilities for generating information from nested SECTION blocks in a
 * structural analysis of a control flow graph.
 */

// @todo range
// This is poorly written...clean up if this ever needs to be merged into main

namespace SH {

typedef std::map<ShCtrlGraphNodePtr, unsigned int> NestingLevelMap;

/** Finds the section nesting level of each control flow graph node.
 * The program scope starts at level 0, and on entering each 
 * SECTION block the level increases by one.
 */
SH_DLLEXPORT NestingLevelMap sectionNestingLevel(ShStructural &s); 

typedef std::pair<int, int> IntRange;
typedef std::map<ShStructuralNodePtr, std::pair<int, int> > StructRangeMap;

/** Labels nodes in postorder sequence and returns the range of all indices
 * contained in each structural node's subtree */
SH_DLLEXPORT StructRangeMap postorderRange(ShStructural &s);

typedef std::map<ShCtrlGraphNodePtr, ShStructuralNodePtr> CfgSectionMap;

/** Finds the section structural node containing a cfg node.
 * If a cfg node is not in the map, then it is at program scope and not
 * in a section
 */
SH_DLLEXPORT CfgSectionMap gatherCfgSection(ShStructural &s);

typedef std::map<ShStructuralNodePtr, ShStructuralNodePtr> SectionParentMap;
/** Finds the parent section of given sections */
SH_DLLEXPORT SectionParentMap findParents(ShStructural &s);


}

#endif
