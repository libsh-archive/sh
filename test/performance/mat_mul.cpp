// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
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
#include <cmath>
#include <iostream>
#include <sh.hpp>

using namespace SH;
using namespace std;

int main() 
{
  shInit();
  ShMatrix4x4f foo;
  ShMatrix4x4f a;

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

