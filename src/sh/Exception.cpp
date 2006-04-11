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
#include "Exception.hpp"

namespace SH {

Exception::Exception(const std::string& message)
  : m_message(message)
{
}

Exception::~Exception() throw()
{
}

const std::string& Exception::message() const
{
  return m_message;
}

const char* Exception::what() const throw()
{
  return m_message.c_str();
}

ParseException::ParseException(const std::string& message)
  : Exception("Parse Error: " + message)
{
}

ScopeException::ScopeException(const std::string& message)
  : Exception("Scoping Violation: " + message)
{
}

ImageException::ImageException(const std::string& message)
  : Exception("Image Error: " + message)
{
}

AlgebraException::AlgebraException(const std::string& message)
  : Exception("Algebra Error: " + message)
{
}

OptimizerException::OptimizerException(const std::string& message)
  : Exception("Optimizer Error: " + message)
{
}

TransformerException::TransformerException(const std::string& message)
  : Exception("Transformer Error: " + message)
{
}

BackendException::BackendException(const std::string& message)
  : Exception("Backend Error: " + message)
{
}


}
