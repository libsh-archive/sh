#ifndef SHUTILITY_HPP
#define SHUTILITY_HPP

/** @file Utility.hh
 * Various utility functions, mostly for internal use.
 */

#include <iosfwd>

namespace SH {

/// Print "indent" spaces to out.
std::ostream& shPrintIndent(std::ostream& out, int indent);

}

#endif
