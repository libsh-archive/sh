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

