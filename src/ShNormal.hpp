#ifndef SHNORMAL_HPP
#define SHNORMAL_HPP

#include "ShAttrib.hpp"
#include "ShSpecialType.hpp"

namespace SH {

/** A surface normal with N dimensions.
 */
SH_SPECIAL_TYPE(ShNormal, surface normal);

SH_SPECIAL_TYPE_TYPEDEFS(Normal);

SH_SPECIAL_TYPE_IMPL(ShNormal);

}

#endif
