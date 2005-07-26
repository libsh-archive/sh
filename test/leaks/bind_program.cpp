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

