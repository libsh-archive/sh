#ifndef SHTEXCOORD_HPP
#define SHTEXCOORD_HPP

#include "ShAttrib.hpp"
#include "ShSpecialType.hpp"

namespace SH {

/** A texture coordinate with N dimensions.
 */
SH_SPECIAL_TYPE(ShTexCoord, texture coordinate);

SH_SPECIAL_TYPE_TYPEDEFS(TexCoord);

SH_SPECIAL_TYPE_IMPL(ShTexCoord);

}

#endif
