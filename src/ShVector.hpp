#ifndef SHVECTOR_HPP
#define SHVECTOR_HPP

#include "ShAttrib.hpp"

namespace SH {

template<int N, int Kind, typename T, bool Swizzled=false>
class ShVector : public ShAttrib<N, Kind, T, Swizzled> {
public:
  ShVector();
  
  ShVector(T);
  ShVector(T, T);
  ShVector(T, T, T);
  ShVector(T, T, T, T);

  ShVector(const ShVariableN<N, T>& other);
  ShVector(const ShVector<N, Kind, T, Swizzled>& other);
  ShVector(const ShVariableNodePtr& node, const ShSwizzle& swizzle, bool neg);

  ~ShVector();


  ShVector& operator=(const ShVariableN<N, T>& other);
  ShVector& operator=(const ShVector<N, Kind, T, Swizzled>& other);

  ShVector& operator+=(const ShVariableN<N, T>& right);
  ShVector& operator-=(const ShVariableN<N, T>& right);
  template<typename T2> ShVector& operator*=(const T2& right);
  template<typename T2> ShVector& operator/=(const T2& right);

  /**@name Swizzling
   * Generate a new vector by selecting the given elements from
   * this vector.
   */
  //@{
  ShVector<N, Kind, T, true> operator()() const; ///< Identity swizzle
  ShVector<1, Kind, T, true> operator()(int) const;
  ShVector<2, Kind, T, true> operator()(int, int) const;
  ShVector<3, Kind, T, true> operator()(int, int, int) const;
  ShVector<4, Kind, T, true> operator()(int, int, int, int) const;
  ShVector<1, Kind, T, true> operator[](int) const; ///< Identical to operator()(int)
  // TODO: Arbitrary swizzles?
  //@}

  /// Negate this vector.
  ShVector<N, Kind, T, Swizzled> operator-() const;

private:
  typedef ShAttrib<N, Kind, double, Swizzled> ParentType;
};


typedef ShVector<1, SH_VAR_INPUT, double> ShInputVector1f;
typedef ShVector<1, SH_VAR_OUTPUT, double> ShOutputVector1f;
typedef ShVector<1, SH_VAR_TEMP, double> ShVector1f;

typedef ShVector<2, SH_VAR_INPUT, double> ShInputVector2f;
typedef ShVector<2, SH_VAR_OUTPUT, double> ShOutputVector2f;
typedef ShVector<2, SH_VAR_TEMP, double> ShVector2f;

typedef ShVector<3, SH_VAR_INPUT, double> ShInputVector3f;
typedef ShVector<3, SH_VAR_OUTPUT, double> ShOutputVector3f;
typedef ShVector<3, SH_VAR_TEMP, double> ShVector3f;

typedef ShVector<4, SH_VAR_INPUT, double> ShInputVector4f;
typedef ShVector<4, SH_VAR_OUTPUT, double> ShOutputVector4f;
typedef ShVector<4, SH_VAR_TEMP, double> ShVector4f;

}

#include "ShVectorImpl.hpp"

#endif
