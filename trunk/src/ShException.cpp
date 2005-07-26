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
#include "ShException.hpp"

namespace SH {

ShException::ShException(const std::string& message)
  : m_message(message)
{
}

ShException::~ShException() throw()
{
}

const std::string& ShException::message() const
{
  return m_message;
}

const char* ShException::what() const throw()
{
  return m_message.c_str();
}

ShParseException::ShParseException(const std::string& message)
  : ShException("Parse Error: " + message)
{
}

ShScopeException::ShScopeException(const std::string& message)
  : ShException("Scoping Violation: " + message)
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

ShBackendException::ShBackendException(const std::string& message)
  : ShException("Backend Error: " + message)
{
}


}
