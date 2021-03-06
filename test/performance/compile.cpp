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
#include <iostream>
#include <sh.hpp>

using namespace SH;
using namespace std;

int main() 
{
  init();
  setBackend("cc");
  Vector3f a(10, 10, 10);
  Vector3f b(8, 9, 10);
  ConstPoint4f c(19, 20, 21, 22);

  for(int i = 0; i < 5000; ++i) {
    Program test = SH_BEGIN_PROGRAM("gpu:stream") {
      InputVector3f SH_DECL(in);
      OutputVector3f SH_DECL(out);
      out = in * (a + b * c(2,1,0));
    } SH_END;
  }
  return 0;
}

