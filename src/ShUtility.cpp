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
#include "ShUtility.hpp"
#include <iostream>
#include <fstream>

namespace SH {

std::ostream& shPrintIndent(std::ostream& out, int indent)
{
  for (int i = 0; i < indent; i++) out << ' ';
  return out;
}

void shDotToPs(const std::string& dot_string, const std::string& ps_filename)
{
  // @todo range does not work on most platforms
  // use tmpnam to generate a real temp file
  std::string dot_filename = ps_filename + ".dot"; 
  std::ofstream fout(dot_filename.c_str());
  fout << dot_string;
  fout.close();
  std::string cmd = std::string("dot -Tps < ") + dot_filename + " > " + ps_filename; 
  system(cmd.c_str());
//  cmd = std::string("rm -f ") + dot_filename; 
//  system(cmd.c_str());
}

}
