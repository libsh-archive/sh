#include "ShArb.hpp"
#include <iostream>

namespace ShArb {

/** Information about ShArbRegType members.
 */
struct {
  char* name;
  char* estName;
} shArbRegTypeInfo[] = {
  {"ATTRIB", "i"},
  {"PARAM", "u"},
  {"TEMP", "t"},
  {"ADDRESS", "a"},
  {"OUTPUT", "o"},
  {"PARAM", "c"},
  {"<texture>", "<texture>"}
};

/** Information about the ShArbRegBinding members.
 */
struct {
  ShArbRegType type;
  char* name;
  bool indexed;
} shArbRegBindingInfo[] = {
  {SH_ARB_REG_PARAM, "program.local", true},
  {SH_ARB_REG_PARAM, "program.env", true},
  {SH_ARB_REG_OUTPUT, "result.color", false}, // TODO: Special case?
  
  {SH_ARB_REG_ATTRIB, "vertex.position", false},
  {SH_ARB_REG_ATTRIB, "vertex.weight", true},
  {SH_ARB_REG_ATTRIB, "vertex.normal", false},
  {SH_ARB_REG_ATTRIB, "vertex.color", false}, // TODO: Special case?
  {SH_ARB_REG_ATTRIB, "vertex.fogcoord", false},
  {SH_ARB_REG_ATTRIB, "vertex.texcoord", true},
  {SH_ARB_REG_ATTRIB, "vertex.matrixindex", true},
  {SH_ARB_REG_ATTRIB, "vertex.attrib", true},
  {SH_ARB_REG_OUTPUT, "result.position", false},
  {SH_ARB_REG_OUTPUT, "result.fogcoord", false},
  {SH_ARB_REG_OUTPUT, "result.pointsize", false},
  {SH_ARB_REG_OUTPUT, "result.texcoord", true},

  {SH_ARB_REG_ATTRIB, "fragment.color", false}, // TODO: Special case?
  {SH_ARB_REG_ATTRIB, "fragment.texcoord", true},
  {SH_ARB_REG_ATTRIB, "fragment.fogcoord", false},
  {SH_ARB_REG_ATTRIB, "fragment.position", false},
  {SH_ARB_REG_OUTPUT, "result.depth", false},

  {SH_ARB_REG_ATTRIB, "<nil>", false},
};

ArbReg::ArbReg()
  : type(SH_ARB_REG_TEMP), index(-1), name(""), binding(SH_ARB_REG_NONE), bindingIndex(-1)
{
}
  
ArbReg::ArbReg(ShArbRegType type, int index, std::string name)
  : type(type), index(index), name(name), binding(SH_ARB_REG_NONE), bindingIndex(-1) 
{
}


std::ostream& ArbReg::printDecl(std::ostream& out) const
{
  out << shArbRegTypeInfo[type].name << " " << *this;
  if (type == SH_ARB_REG_CONST) {
    out << " = " << "{";
    for (int i = 0; i < 4; i++) {
      if (i) out << ", ";
      out << values[i];
    }
    out << "}";
  } else if (binding != SH_ARB_REG_NONE) {
    out << " = " << shArbRegBindingInfo[binding].name;
    if (shArbRegBindingInfo[binding].indexed) {
      out << "[" << bindingIndex << "]";
    }
  }
  out << ";"; 
  if(!name.empty() && type != SH_ARB_REG_CONST) out << " # " << name;
  return out;
}

/** Output a use of an arb register.
 */
std::ostream& operator<<(std::ostream& out, const ArbReg& reg)
{
  out << shArbRegTypeInfo[reg.type].estName << reg.index;
  return out;
}

}
