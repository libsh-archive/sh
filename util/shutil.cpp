// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
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
#include "shutil.hpp"
#include <string>

using namespace std;

namespace ShUtil {

string normalize_path(const string& path)
{
  string ret(path);
  const unsigned size = ret.size();
  for (unsigned i=0; i < size; i++) {
#ifdef _WIN32
    if ('/' == ret[i]) ret[i] = '\\';
#else
    if ('\\' == ret[i]) ret[i] = '/';
#endif
  }
  return ret;
}

}
