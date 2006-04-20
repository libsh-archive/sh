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
#include "VariableType.hpp"

namespace SH {

const char* bindingTypeName[] = {
  "Input",
  "Output",
  "InOut",
  "",
  "Constant",
  "Texture",
  "Stream",
  "Palette",

  "Invalid Binding Type"
};

const char* semanticTypeName[] = {
  "Attrib",
  "Point",
  "Vector",
  "Normal",
  "Color",
  "TexCoord",
  "Position",
  "Plane",

  "Invalid Semantic Type"
};


} // namespace SH

