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
#ifndef SHATTRIB_HPP
#define SHATTRIB_HPP

#include "ShVariable.hpp"
#include "ShConstant.hpp"

namespace SH {

/** A generic attribute (or parameter) holding N values.
 *
 * The reason we have the Swizzle template argument is so that
 * swizzled variables, which need to be copied (i.e. have an ASN
 * statement generated) when another variable is initialized through
 * them, may otherwise be simply placed into new variables (through
 * copy constructor eliding, as per paragraph 12.8.15 of the C++
 * standard), causing variables to share ShVariableNodes when they
 * shouldn't. By making swizzled variables separate types we enforce
 * calling a conversion constructor instead, which cannot be elided.
 * If this paragraph confuses you, and you're not modifying Sh
 * internals, you may safely ignore it.
 *
 * The default 
 */
template<int N, int Kind, typename T=double, bool Swizzled=false>
class ShAttrib : public ShVariableN<N, T> {
public:
  ShAttrib();
  
  ShAttrib(T);
  ShAttrib(T, T);
  ShAttrib(T, T, T);
  ShAttrib(T, T, T, T);

  ShAttrib(const ShVariableN<N, T>& other);
  ShAttrib(const ShAttrib<N, Kind, T, Swizzled>& other);
  ShAttrib(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg);
  
  ~ShAttrib();

  ShAttrib& operator=(const ShVariableN<N, T>& other);
  ShAttrib& operator=(const ShAttrib<N, Kind, T, Swizzled>& other);
  ShAttrib& operator=(T other);

  ShAttrib& operator+=(const ShVariableN<N, T>& right);
  ShAttrib& operator-=(const ShVariableN<N, T>& right);
  template<typename T2> ShAttrib& operator*=(T2 right);
  template<typename T2> ShAttrib& operator/=(T2 right);

  ShAttrib& operator+=(const ShConstant<N, T>& right);
  ShAttrib& operator-=(const ShConstant<N, T>& right);

  /**@name Swizzling
   * Generate a new attribute by selecting the given elements from
   * this attribute.
   */
  //@{
  ShAttrib<N, Kind, T, true> operator()() const; ///< Identity swizzle
  ShAttrib<1, Kind, T, true> operator()(int) const;
  ShAttrib<2, Kind, T, true> operator()(int, int) const;
  ShAttrib<3, Kind, T, true> operator()(int, int, int) const;
  ShAttrib<4, Kind, T, true> operator()(int, int, int, int) const;
  ShAttrib<1, Kind, T, true> operator[](int) const; ///< Identical to operator()(int)

  // Arbitrary Swizzle
  template<int N2>
  ShAttrib<N2, Kind, T, true> swiz(int indices[]) const;

  //@}

  /// Negate this attribute.
  ShAttrib<N, Kind, T, Swizzled> operator-() const;

  static const int typesize = N;
  static const int typekind = Kind;
  static const ShVariableSpecialType special_type = SH_VAR_ATTRIB;
  typedef T ValueType;

  typedef ShAttrib<N, SH_VAR_INPUT, T> InputType;
  typedef ShAttrib<N, SH_VAR_OUTPUT, T> OutputType;
  typedef ShAttrib<N, SH_VAR_TEMP, T> TempType;
};

typedef ShAttrib<1, SH_VAR_INPUT, float> ShInputAttrib1f;
typedef ShAttrib<1, SH_VAR_OUTPUT, float> ShOutputAttrib1f;
typedef ShAttrib<1, SH_VAR_TEMP, float> ShAttrib1f;

typedef ShAttrib<2, SH_VAR_INPUT, float> ShInputAttrib2f;
typedef ShAttrib<2, SH_VAR_OUTPUT, float> ShOutputAttrib2f;
typedef ShAttrib<2, SH_VAR_TEMP, float> ShAttrib2f;

typedef ShAttrib<3, SH_VAR_INPUT, float> ShInputAttrib3f;
typedef ShAttrib<3, SH_VAR_OUTPUT, float> ShOutputAttrib3f;
typedef ShAttrib<3, SH_VAR_TEMP, float> ShAttrib3f;

typedef ShAttrib<4, SH_VAR_INPUT, float> ShInputAttrib4f;
typedef ShAttrib<4, SH_VAR_OUTPUT, float> ShOutputAttrib4f;
typedef ShAttrib<4, SH_VAR_TEMP, float> ShAttrib4f;


} // namespace SH

#include "ShAttribImpl.hpp"

#endif












