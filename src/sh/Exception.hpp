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
#ifndef SHEXCEPTION_HPP
#define SHEXCEPTION_HPP

#include <string>
#include "DllExport.hpp"

namespace SH {

/** A generic exception.
 * You should derive from this class to make more specific exceptions.
 */
class
DLLEXPORT Exception : public std::exception {
public:
  /// Construct a general exception with the given message.
  Exception(const std::string& message);
  virtual ~Exception() throw (); // Make this class virtual in orer to get
                          // RTTI info in.
  
  /// Return an informative message describing the exception.
  const std::string& message() const;

  virtual const char* what() const throw();
protected:
  std::string m_message;
};

/** An exception representing a parse error.
 */
class
DLLEXPORT ParseException : public Exception {
public:
  ParseException(const std::string& message);
};

/** An exception representing a scoping violation
 */
class
DLLEXPORT ScopeException : public Exception {
public:
  ScopeException(const std::string& message);
};

/** An exception relating to an Image operation.
 */
class
DLLEXPORT ImageException : public Exception {
public:
  ImageException(const std::string& message);
};

/** An exception relating to an Algebra operation.
 */
class
DLLEXPORT AlgebraException : public Exception {
public:
  AlgebraException(const std::string& message);
};

/** An exception relating to an Optimizer operation.
 */
class
DLLEXPORT OptimizerException : public Exception {
public:
  OptimizerException(const std::string& message);
};

/** An exception relating to an Transformer operation.
 */
class
DLLEXPORT TransformerException : public Exception {
public:
  TransformerException(const std::string& message);
};

/** An exception relating to an Backend operation.
 */
class
DLLEXPORT BackendException : public Exception {
public:
  BackendException(const std::string& message);
};

}

#endif
