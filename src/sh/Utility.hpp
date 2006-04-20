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
#ifndef SHUTILITY_HPP
#define SHUTILITY_HPP

/** @file Utility.hh
 * Various utility functions, mostly for internal use.
 */

#include <iosfwd>
#include <map>
#include "DllExport.hpp"

namespace SH {

/// Print "indent" spaces to out.
SH_DLLEXPORT
std::ostream& printIndent(std::ostream& out, int indent);

/** Check a boolean condition at compile time.
 * This follows Alexandrescu's excellent book "Modern C++ Design"
 */
template<bool B> struct CompileTimeChecker
{
  CompileTimeChecker(...);
};
template<> struct CompileTimeChecker<false> {
};

#define STATIC_CHECK(expr, msg) \
{ \
  class ERROR_##msg {} y; \
  (void)sizeof(CompileTimeChecker<(expr)>(y));\
}

struct 
SH_DLLEXPORT Ignore {
  template<typename T>
  T& operator&(T& other) { return other; }
};

/// SelectType::type == B ? T1 : T2 
template<bool B, typename T1, typename T2>
struct SelectType;

template<typename T1, typename T2>
struct SelectType<true, T1, T2> {
  typedef T1 type;
};

template<typename T1, typename T2>
struct SelectType<false, T1, T2> {
  typedef T2 type;
};

/// MatchType::matches = (T1 == T2)
template<typename T1, typename T2>
struct MatchType {
  static const bool matches = false; 
};

template<typename T>
struct MatchType<T, T> {
  static const bool matches = true; 
};

template<typename T, typename T1, typename T2>
struct MatchEitherType {
    static const bool matches = MatchType<T1, T>::matches ||
                                    MatchType<T2, T>::matches;
};

/** Returns true if T matches a given templated type.
 * For example, MatchTemplateType<int, Interval>::matches == false
 * but MatchTemplateType<Interval<int>, Interval>::matches == true
 *
 * You can subclass this like this:
 * template<typename T> struct MatchMyType: public MatchTemplateType<T, MyType> {}; 
 * to match your own complex types.
 *
 * The basic class here is standards compliant and works in VC .NET 2003.
 */
template<typename T, template<typename A> class B>
struct MatchTemplateType {
    static const bool matches = false; 
};

template<typename T, template<typename A> class B>
struct MatchTemplateType<B<T>, B> {
    static const bool matches = true; 
};

/** Takes a templated type and returns its template parameter. */
template<typename T, template<typename A> class B>
struct TemplateParameterType; 

template<typename T, template<typename A> class B>
struct TemplateParameterType<B<T>, B> {
    typedef T type;
};

}

#endif
