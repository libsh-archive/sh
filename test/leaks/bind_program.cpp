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
#include <string>
#include <iostream>

using namespace SH;
using namespace std;

void create_vertex_shader()
{
  ShProgram vsh = SH_BEGIN_VERTEX_PROGRAM {
    ShInOutPosition4f pos;
    ShInOutNormal3f normal;
    ShOutputVector3f lightv;

    ShPoint3f posv = pos(0,1,2);
    pos(0,1,2) = pos(0,3,2) + posv;
    normal = normal + ShAttrib3f(1, 1, 1);
    lightv = normal;
  } SH_END;
  shBind(vsh);
  shUnbind(vsh);
}

void create_fragment_shader()
{
  ShProgram fsh = SH_BEGIN_FRAGMENT_PROGRAM {
    ShInputPosition4f pos;
    ShInputNormal3f normal;
    ShInputVector3f lightv;

    ShOutputColor3f color(0, 4, 5);

    normal = normalize(normal);
    lightv = normalize(lightv);
    
    color = (normal | lightv) * color;
  } SH_END;
}

int main() 
{
  create_vertex_shader();
  create_fragment_shader();
  cout << "Press ENTER to start..." << endl;
  string s;
  getline(cin, s);

  for (int i=0; i < 100000; i++) {
    create_vertex_shader();
    create_fragment_shader();
  }

  cout << "Press ENTER to exit this program." << endl;
  getline(cin, s);
}

