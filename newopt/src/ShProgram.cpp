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
#include "ShProgram.hpp"
#include "ShBackend.hpp"

namespace SH {

ShProgram::ShProgram()
  : ShMetaForwarder(0),
    m_node(0)
{
}

ShProgram::ShProgram(const std::string& target)
  : ShMetaForwarder(0),
    m_node(new ShProgramNode(target))
{
  real_meta(m_node.object());
}

ShProgram::ShProgram(const ShProgram& other)
  : ShMetaForwarder(other.m_node.object()),
    m_node(other.m_node)
{
}

ShProgram::ShProgram(const ShProgramNodePtr& node)
  : ShMetaForwarder(node.object()),
    m_node(node)
{
}

ShProgram& ShProgram::operator=(const ShProgram& other)
{
  m_node = other.m_node;
  real_meta(m_node.object());
  return *this;
}

}
