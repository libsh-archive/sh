// Sh: A GPU metaprogramming language.
//
// Copyright (c) 2003 University of Waterloo Computer Graphics Laboratory
// Project administrator: Michael D. McCool
// Authors: Zheng Qin, Stefanus Du Toit, Kevin Moule, Tiberiu S. Popa,
//          Michael D. McCool
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
#include "PSReg.hpp"
#include <iostream>
#include <string>
#include "PS.hpp"

namespace shdx {

/** Information about PSRegType members.
 */
struct {
  char* name;
  char* estName;
} psRegTypeInfo[] = {
  {"INPUT", "v"},
  {"VSOUTPUT", "o"},
  {"PSOUTPUT", "oC"},
  {"ADDRESS", "a"},
  {"TEMP", "r"},
  {"PARAM", "c"},
  {"CONST", "c"},
  {"TEXTURE", "t"},
  {"COLOR", "v"},
};

/** Information about the PSRegBinding members.
 */
struct {
  PSRegType type;
  char* name;
  bool indexed;
} psRegBindingInfo[] = {

  // VS Input
  {SH_PS_REG_INPUT, "dcl_position", false},
  {SH_PS_REG_INPUT, "dcl_normal", false},
  {SH_PS_REG_INPUT, "dcl_color0", false},
  {SH_PS_REG_INPUT, "dcl_color1", false},
  {SH_PS_REG_INPUT, "dcl_texcoord", true},
  
  // VS Output
  {SH_PS_REG_VSOUTPUT, "dcl_color0", false},
  {SH_PS_REG_VSOUTPUT, "dcl_color1", false},
  {SH_PS_REG_VSOUTPUT, "dcl_fog", false},
  {SH_PS_REG_VSOUTPUT, "dcl_position", false},
  {SH_PS_REG_VSOUTPUT, "dcl_psize", false},
  {SH_PS_REG_VSOUTPUT, "dcl_texcoord", true},

  // PS Input
  {SH_PS_REG_INPUT, "dcl vPos.xy", false},
  {SH_PS_REG_INPUT, "dcl_normal", false},
  {SH_PS_REG_INPUT, "dcl_color0", false},
  {SH_PS_REG_INPUT, "dcl_color1", false},
  {SH_PS_REG_INPUT, "dcl_texcoord", true},

  // PS Output
  {SH_PS_REG_PSOUTPUT, "oC", true},

  {SH_PS_REG_INPUT, "<nil>", false},
};


PSReg::PSReg()
  : type(SH_PS_REG_TEMP), index(-1), name(""), binding(SH_PS_REG_NONE), bindingIndex(-1)
{
}
  
PSReg::PSReg(PSRegType type, int index, std::string name)
  : type(type), index(index), name(name), binding(SH_PS_REG_NONE), bindingIndex(-1) 
{
}


std::ostream& PSReg::printDecl(std::ostream& out) const
{
	// In general, we don't have to declare things under ps & vs

	if (type == SH_PS_REG_INPUT || type == SH_PS_REG_VSOUTPUT)
	{
		out << psRegBindingInfo[binding].name;
		if (psRegBindingInfo[binding].indexed)
			out << bindingIndex;
		if (binding != SH_PS_REG_FRAGMENTPOS)
			out << " " << *this;
	}
	else if (type == SH_PS_REG_CONST)
	{
		out << "def " << *this;
		for (int i = 0; i < 4; i++)
		{
			out << ", " << values[i];
		}
	}
	else if (type == SH_PS_REG_PARAM || type == SH_PS_REG_PSOUTPUT) // Just put a comment
	{
		out << "// " << *this;
		if (!name.empty())
			out << " = " << name;
		return out;
	}

	if(!name.empty() && type != SH_PS_REG_CONST) out << " // " << name;

	return out;
}

/** Output a use of a ps register.
 */
std::ostream& operator<<(std::ostream& out, const PSReg& reg)
{
	out << psRegTypeInfo[reg.type].estName << reg.index;
	return out;
}

}
