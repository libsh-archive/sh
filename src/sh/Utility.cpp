// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2006 Serious Hack Inc.
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
// MA  02110-1301, USA
//////////////////////////////////////////////////////////////////////////////
#include "Utility.hpp"
#include <iostream>
#include <fstream>

namespace SH {

std::ostream& printIndent(std::ostream& out, int indent)
{
  for (int i = 0; i < indent; i++) out << ' ';
  return out;
}

void dotGen(const std::string& dot_string, const std::string& filename)
{
  // @todo range does not work on most platforms
  // use tmpnam to generate a real temp file
  std::string dot_filename = filename + ".dot"; 
  std::ofstream fout(dot_filename.c_str());
  fout << dot_string;
  fout.close();
  std::string cmd;
  //std::string cmd = std::string("dot -Tps < ") + dot_filename + " > " + filename + ".ps"; 
  //system(cmd.c_str());
  cmd = std::string("dot -Tpng < ") + dot_filename + " > " + filename + ".png";
  system(cmd.c_str());
//  cmd = std::string("rm -f ") + dot_filename; 
//  system(cmd.c_str());
}

}
