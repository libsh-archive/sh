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
#include <iostream>
#include <sh.hpp>

using namespace SH;
using namespace std;

int main() 
{
  try {
  ShSwizzle a;
  ShSwizzle b(1);
  ShSwizzle c(4);
  ShSwizzle d(5);
  ShSwizzle e(1, 0);
  ShSwizzle f(2, 1, 0);
  ShSwizzle g(2, 0, 1);
  ShSwizzle h(3, 0, 2, 1);
  ShSwizzle i(4, 0, 1, 3, 2);

  std::cout << a << std::endl;
  std::cout << b << std::endl;
  std::cout << c << std::endl;
  std::cout << d << std::endl;
  std::cout << e << std::endl;
  std::cout << f << std::endl;
  std::cout << g << std::endl;
  std::cout << h << std::endl;
  std::cout << i << std::endl;

  std::cout << h * h << std::endl;
  std::cout << h << std::endl;
  h *= h;
  std::cout << h << std::endl;
  } catch(const ShException  &e) {
    std::cout << e.message() << std::endl;
  }

  return 0;
}

