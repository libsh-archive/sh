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
#ifndef SHARBREG_HPP
#define SHARBREG_HPP

#include <string>
#include <iosfwd>

#include "RefCount.hpp"

namespace shgl {

/** Possible register types in the ARB spec.
 */
enum ArbRegType {
  ARB_REG_ATTRIB,
  ARB_REG_PARAM,
  ARB_REG_TEMP,
  ARB_REG_HALF_TEMP, // @todo type may want to rethink this
  ARB_REG_ADDRESS,
  ARB_REG_OUTPUT,
  ARB_REG_CONST,
  ARB_REG_TEXTURE
};

/** Possible bindings for a register (see ARB spec).
 */
enum ArbRegBinding {
  // VERTEX and FRAGMENT
  // Parameter
  ARB_REG_PROGRAMLOC,
  ARB_REG_PROGRAMENV,
  ARB_REG_STATE,
  // Output
  ARB_REG_RESULTCOL,

  // VERTEX
  // Input
  ARB_REG_VERTEXPOS,
  ARB_REG_VERTEXWGT,
  ARB_REG_VERTEXNRM,
  ARB_REG_VERTEXCOL,
  ARB_REG_VERTEXFOG,
  ARB_REG_VERTEXTEX,
  ARB_REG_VERTEXMAT,
  ARB_REG_VERTEXATR,
  // Output
  ARB_REG_RESULTPOS,
  ARB_REG_RESULTFOG,
  ARB_REG_RESULTPTS, ///< Result point size
  ARB_REG_RESULTTEX,

  // FRAGMENT
  // Input
  ARB_REG_FRAGMENTCOL,
  ARB_REG_FRAGMENTTEX,
  ARB_REG_FRAGMENTFOG,
  ARB_REG_FRAGMENTPOS,
  // Output
  ARB_REG_RESULTDPT,
  ARB_REG_RESULTCOL_ATI,

  ARB_REG_NONE
};

/** An ARB register.
 */
struct ArbReg : public SH::RefCountable {
  ArbReg();
  ArbReg(ArbRegType type, int index, std::string name = "");

  ArbRegType type;
  int index;
  bool preset;
  std::string name; //< variable name (if any) associated with the register

  struct BindingInfo
    {
    ArbRegBinding type;
    int index;
    int count;
    std::string name;
    float values[4];
    };
  BindingInfo binding;

  friend std::ostream& operator<<(std::ostream& out, const ArbReg& reg);
  
  /// Print a declaration for this register
  std::ostream& printDecl(std::ostream& out) const;

  /// Return the name of the register binding
  std::string binding_name() const;
};


}

#endif
