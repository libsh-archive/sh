#include "ArbReg.hpp"
#include <iostream>
#include <string>
#include "Arb.hpp"

namespace shgl {

/** Information about ArbRegType members.
 */
struct {
  char* name;
  char* estName;
} arbRegTypeInfo[] = {
  {"ATTRIB", "i"},
  {"PARAM", "u"},
  {"TEMP", "t"},
  {"ADDRESS", "a"},
  {"OUTPUT", "o"},
  {"PARAM", "c"},
  {"<texture>", "<texture>"}
};

/** Information about the ArbRegBinding members.
 */
struct {
  ArbRegType type;
  char* name;
  bool indexed;
} arbRegBindingInfo[] = {
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
  
ArbReg::ArbReg(ArbRegType type, int index, std::string name)
  : type(type), index(index), name(name), binding(SH_ARB_REG_NONE), bindingIndex(-1) 
{
}


std::ostream& ArbReg::printDecl(std::ostream& out) const
{
  out << arbRegTypeInfo[type].name << " " << *this;
  if (type == SH_ARB_REG_CONST) {
    out << " = " << "{";
    for (int i = 0; i < 4; i++) {
      if (i) out << ", ";
      out << values[i];
    }
    out << "}";
  } else if (binding != SH_ARB_REG_NONE) {
    out << " = " << arbRegBindingInfo[binding].name;
    if (arbRegBindingInfo[binding].indexed) {
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
  out << arbRegTypeInfo[reg.type].estName << reg.index;
  return out;
}

}
