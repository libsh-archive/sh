#ifndef SHCONSTANT_HPP
#define SHCONSTANT_HPP

#include "ShVariable.hpp"

namespace SH {

/** A shader-compile-time constant.
 * Represent an N-tuple constant which will never actually vary.
 */
template<int N, typename T>
class ShConstant : public ShVariableN<N, T> 
{
public:
  ShConstant(T value);
  explicit ShConstant(T values[N]);

private:
  /// Will never be implemented
  ShConstant(const ShConstant<N, T>& other);
  /// Will never be implemented
  ShConstant<N, T>& operator=(const ShConstant<N, T>& other);
};

typedef ShConstant<1, double> ShConstant1f;
typedef ShConstant<2, double> ShConstant2f;
typedef ShConstant<3, double> ShConstant3f;
typedef ShConstant<4, double> ShConstant4f;

}

#include "ShConstantImpl.hpp"

#endif
