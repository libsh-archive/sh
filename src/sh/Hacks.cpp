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
#include "Hacks.hpp"
#include "Algebra.hpp"
#include "Record.hpp"
#include <iostream>
#include <fstream>

namespace SH {

void lock_in_uniforms(Program& p) {
  /* Build a nibble that assigns the uniform to appropriate inputs */ 
  Program binding_map = SH_BEGIN_PROGRAM() {
    for(Record::iterator V = p.uniform_inputs.begin(); V != p.uniform_inputs.end(); ++V) {
      Variable out(V->node()->clone(SH_OUTPUT, V->size(), V->valueType(), SEMANTICTYPE_END, true, false));
      shASN(out, *V);
    }
  } SH_END_PROGRAM;

  /* Unbind and connect */
  /* check for streams and complain */
  Program::BindingSpec::iterator B = p.binding_spec.begin();
  for(; B != p.binding_spec.end(); ++B) {
    if(*B == Program::STREAM) {
      error(Exception("Cannot run lock_in_uniforms on a program with bound streams."));
    }
  }
  p.binding_spec.clear();
  p.uniform_inputs.clear();
  ProgramNodePtr oldp = p.node();
  p = p << binding_map;
  static_cast<Meta*>(p.node().object())->operator=(*oldp.object()); // @todo put this in a function 
}

}
