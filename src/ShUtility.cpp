#include "ShUtility.hpp"
#include <iostream>

namespace SH {

std::ostream& shPrintIndent(std::ostream& out, int indent)
{
  for (int i = 0; i < indent; i++) out << ' ';
  return out;
}

}
