#ifndef SHATTRIB_HPP
#define SHATTRIB_HPP

#include "ShVariable.hpp"

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
 */
template<int N, int Kind, typename T, bool Swizzled=false>
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

  ShAttrib& operator+=(const ShVariableN<N, T>& right);
  ShAttrib& operator-=(const ShVariableN<N, T>& right);
  template<typename T2> ShAttrib& operator*=(const T2& right);
  template<typename T2> ShAttrib& operator/=(const T2& right);

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
  // TODO: Arbitrary swizzles?
  //@}

  /// Negate this attribute.
  ShAttrib<N, Kind, T, Swizzled> operator-() const;
};

typedef ShAttrib<1, SH_VAR_INPUT, double> ShInputAttrib1f;
typedef ShAttrib<1, SH_VAR_OUTPUT, double> ShOutputAttrib1f;
typedef ShAttrib<1, SH_VAR_TEMP, double> ShAttrib1f;

typedef ShAttrib<2, SH_VAR_INPUT, double> ShInputAttrib2f;
typedef ShAttrib<2, SH_VAR_OUTPUT, double> ShOutputAttrib2f;
typedef ShAttrib<2, SH_VAR_TEMP, double> ShAttrib2f;

typedef ShAttrib<3, SH_VAR_INPUT, double> ShInputAttrib3f;
typedef ShAttrib<3, SH_VAR_OUTPUT, double> ShOutputAttrib3f;
typedef ShAttrib<3, SH_VAR_TEMP, double> ShAttrib3f;

typedef ShAttrib<4, SH_VAR_INPUT, double> ShInputAttrib4f;
typedef ShAttrib<4, SH_VAR_OUTPUT, double> ShOutputAttrib4f;
typedef ShAttrib<4, SH_VAR_TEMP, double> ShAttrib4f;
 
} // namespace SH

#include "ShAttribImpl.hpp"

#endif
