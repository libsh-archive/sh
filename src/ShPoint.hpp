#ifndef SHPOINT_HPP
#define SHPOINT_HPP

#include "ShAttrib.hpp"
#include "ShSpecialType.hpp"

namespace SH {

/** An N-Point.
 */
SH_SPECIAL_TYPE(ShPoint, point);

SH_SPECIAL_TYPE_TYPEDEFS(Point);

SH_SPECIAL_TYPE_IMPL(ShPoint, SH_VAR_POINT);

}

#endif
