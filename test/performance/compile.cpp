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
  shInit();
  shSetBackend("cc");
  ShVector3f a(10, 10, 10);
  ShVector3f b(8, 9, 10);
  ShConstPoint4f c(19, 20, 21, 22);

  for(int i = 0; i < 5000; ++i) {
    ShProgram test = SH_BEGIN_PROGRAM("gpu:stream") {
      ShInputVector3f SH_DECL(in);
      ShOutputVector3f SH_DECL(out);
      out = in * (a + b * c(2,1,0));
    } SH_END;
  }
  return 0;
}

