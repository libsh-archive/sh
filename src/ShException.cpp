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
#include "ShException.hpp"

namespace SH {

ShException::ShException(const std::string& message)
  : m_message(message)
{
}

const std::string& ShException::message() const
{
  return m_message;
}

ShParseException::ShParseException(const std::string& message)
  : ShException("Parse Error: " + message)
{
}

ShImageException::ShImageException(const std::string& message)
  : ShException("Image Error: " + message)
{
}

ShAlgebraException::ShAlgebraException(const std::string& message)
  : ShException("Algebra Error: " + message)
{
}

ShOptimizerException::ShOptimizerException(const std::string& message)
  : ShException("Optimizer Error: " + message)
{
}

ShTransformerException::ShTransformerException(const std::string& message)
  : ShException("Transformer Error: " + message)
{
}


}
