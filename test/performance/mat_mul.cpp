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
#include <cmath>
#include <iostream>
#include <sh.hpp>

using namespace SH;
using namespace std;

int main() 
{
  init();
  Matrix4x4f foo;
  Matrix4x4f a;

  for(int i = 0; i < 4; ++i) for(int j = 0; j < 4; ++j) {
    foo[i][j] = i * M_PI / j;
    a[i][j] = sin(i + j * 2.14);
  }

  for(int i = 0; i < 30000; ++i) {
    foo = a * foo; 
  }
  std::cout  << foo << std::endl;
  return 0;
}

