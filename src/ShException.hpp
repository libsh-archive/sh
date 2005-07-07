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
#ifndef SHEXCEPTION_HPP
#define SHEXCEPTION_HPP

#include <string>
#include "ShDllExport.hpp"

namespace SH {

/** A generic exception.
 * You should derive from this class to make more specific exceptions.
 */
class
SH_DLLEXPORT ShException : public std::exception {
public:
  /// Construct a general exception with the given message.
  ShException(const std::string& message);
  virtual ~ShException() throw (); // Make this class virtual in orer to get
                          // RTTI info in.
  
  /// Return an informative message describing the exception.
  const std::string& message() const;

  virtual const char* ShException::what() const throw();
protected:
  std::string m_message;
};

/** An exception representing a parse error.
 */
class
SH_DLLEXPORT ShParseException : public ShException {
public:
  ShParseException(const std::string& message);
};

/** An exception representing a scoping violation
 */
class
SH_DLLEXPORT ShScopeException : public ShException {
public:
  ShScopeException(const std::string& message);
};

/** An exception relating to an ShImage operation.
 */
class
SH_DLLEXPORT ShImageException : public ShException {
public:
  ShImageException(const std::string& message);
};

/** An exception relating to an ShAlgebra operation.
 */
class
SH_DLLEXPORT ShAlgebraException : public ShException {
public:
  ShAlgebraException(const std::string& message);
};

/** An exception relating to an ShOptimizer operation.
 */
class
SH_DLLEXPORT ShOptimizerException : public ShException {
public:
  ShOptimizerException(const std::string& message);
};

/** An exception relating to an ShTransformer operation.
 */
class
SH_DLLEXPORT ShTransformerException : public ShException {
public:
  ShTransformerException(const std::string& message);
};

/** An exception relating to an ShBackend operation.
 */
class
SH_DLLEXPORT ShBackendException : public ShException {
public:
  ShBackendException(const std::string& message);
};

}

#endif
