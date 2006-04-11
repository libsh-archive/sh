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
#include "ArbReg.hpp"
#include <iostream>
#include <string>
#include <iomanip>
#include "Arb.hpp"

namespace shgl {

/** Information about ArbRegType members.
 */
static struct {
  char* name;
  char* estName;
} arbRegTypeInfo[] = {
  {"ATTRIB", "i"},
  {"PARAM", "u"},
  {"TEMP", "t"},
  {"HALFTEMP", "h"},
  {"ADDRESS", "a"},
  {"OUTPUT", "o"},
  {"PARAM", "c"},
  {"<texture>", "<texture>"}
};

/** Information about the ArbRegBinding members.
 */
static struct {
  ArbRegType type;
  char* name;
  bool indexed;
} arbRegBindingInfo[] = {
  {ARB_REG_PARAM, "program.local", true},
  {ARB_REG_PARAM, "program.env", true},
  {ARB_REG_PARAM, "<nil>", true},
  {ARB_REG_OUTPUT, "result.color", false}, // TODO: Special case?
  
  {ARB_REG_ATTRIB, "vertex.position", false},
  {ARB_REG_ATTRIB, "vertex.weight", true},
  {ARB_REG_ATTRIB, "vertex.normal", false},
  {ARB_REG_ATTRIB, "vertex.color", false}, // TODO: Special case?
  {ARB_REG_ATTRIB, "vertex.fogcoord", false},
  {ARB_REG_ATTRIB, "vertex.texcoord", true},
  {ARB_REG_ATTRIB, "vertex.matrixindex", true},
  {ARB_REG_ATTRIB, "vertex.attrib", true},
  {ARB_REG_OUTPUT, "result.position", false},
  {ARB_REG_OUTPUT, "result.fogcoord", false},
  {ARB_REG_OUTPUT, "result.pointsize", false},
  {ARB_REG_OUTPUT, "result.texcoord", true},

  {ARB_REG_ATTRIB, "fragment.color", false}, // TODO: Special case?
  {ARB_REG_ATTRIB, "fragment.texcoord", true},
  {ARB_REG_ATTRIB, "fragment.fogcoord", false},
  {ARB_REG_ATTRIB, "fragment.position", false},
  {ARB_REG_OUTPUT, "result.depth", false},
  {ARB_REG_OUTPUT, "result.color", true}, // Needs ATI_draw_buffers extension

  {ARB_REG_ATTRIB, "<nil>", false},
};

ArbReg::ArbReg()
  : type(ARB_REG_TEMP), index(-1), preset(false), name("")
{
    binding.type = ARB_REG_NONE;
    binding.index = -1;
    binding.count = 1;
}
  
ArbReg::ArbReg(ArbRegType type, int index, std::string name)
  : type(type), index(index), preset(false), name(name)
{
    binding.type = ARB_REG_NONE;
    binding.index = -1;
    binding.count = 1;
}


std::ostream& ArbReg::printDecl(std::ostream& out) const
{
  out << arbRegTypeInfo[type].name << " " << *this;
  if (type == ARB_REG_CONST) {
    out << " = " << "{";
    for (int i = 0; i < 4; i++) {
      if (i) out << ", ";
      std::streamsize prec = out.precision();
//      out.precision(std::numeric_limits<float>::digits10);
      out.precision(20);
      out << binding.values[i];
      out.precision(prec);
    }
    out << "}";
  } else if (binding.type != ARB_REG_NONE) {
  if (binding.type == ARB_REG_STATE)
    {
    out << " = " << binding.name;
    }
  else
    {
    if (binding.count > 1) {
      out << "[" << binding.count << "]";
    }
    out << " = ";
    if (binding.count > 1) out << "{ ";
    out << arbRegBindingInfo[binding.type].name;
    if (arbRegBindingInfo[binding.type].indexed) {
      out << "[" << binding.index;
      if (binding.count > 1) {
        out << " .. " << (binding.index + binding.count - 1);
      }
      out << "]";
    }
    if (binding.count > 1) out << " }";
    }
  }
  out << ";"; 
  if(!name.empty() && type != ARB_REG_CONST) out << " # " << name;
  return out;
}

/** Output a use of an arb register.
 */
std::ostream& operator<<(std::ostream& out, const ArbReg& reg)
{
  out << arbRegTypeInfo[reg.type].estName << reg.index;
  return out;
}

std::string ArbReg::binding_name() const
{
  return arbRegBindingInfo[binding.type].name;
}

}
