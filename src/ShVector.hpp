#ifndef SHVECTOR_HPP
#define SHVECTOR_HPP

#include "ShAttrib.hpp"
#include "ShSpecialType.hpp"

namespace SH {

/** An N-Vector.
 */
SH_SPECIAL_TYPE(ShVector, vector);

SH_SPECIAL_TYPE_TYPEDEFS(Vector);

SH_SPECIAL_TYPE_IMPL(ShVector);

}

#endif
