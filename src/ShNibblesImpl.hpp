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
#ifndef SHNIBBLESIMPL_HPP
#define SHNIBBLESIMPL_HPP

#include "ShNibbles.hpp"

namespace SH {

template<typename T>
ShProgram keep(std::string name) {
  ShProgram nibble = SH_BEGIN_PROGRAM() {
    typename T::InputType SH_NAMEDECL(attr, name); 
    typename T::OutputType SH_NAMEDECL(out, name);
    out = attr;
  } SH_END_PROGRAM;
  return nibble;
}

template<typename T>
ShProgram lose(std::string name) {
  ShProgram nibble = SH_BEGIN_PROGRAM() {
    typename T::InputType SH_NAMEDECL(attr, name);
    ShAttrib4f dummy = dummy;
  } SH_END_PROGRAM;
  return nibble;
};

template<typename T>
ShProgram access(const ShTexture1D<T> &tex, std::string name) {
  ShProgram nibble = SH_BEGIN_PROGRAM() {
    ShInputTexCoord1f SH_NAMEDECL(tc, name);
    typename T::OutputType SH_NAMEDECL(result, name) = tex(tc);
  } SH_END;
  return nibble;
}

template<typename T>
ShProgram access(const ShTexture2D<T> &tex, std::string name) {
  ShProgram nibble = SH_BEGIN_PROGRAM() {
    ShInputTexCoord2f SH_NAMEDECL(tc, name);
    typename T::OutputType SH_NAMEDECL(result, name) = tex(tc);
  } SH_END;
  return nibble;
}

template<typename T>
ShProgram access(const ShTexture3D<T> &tex, std::string name) {
  ShProgram nibble = SH_BEGIN_PROGRAM() {
    ShInputTexCoord3f SH_NAMEDECL(tc, name);
    typename T::OutputType SH_NAMEDECL(result, name) = tex(tc);
  } SH_END;
  return nibble;
}

template<typename T2, int Rows, int Cols, int Kind, typename T>
ShProgram transform(const ShMatrix<Rows, Cols, Kind, T> &m, std::string name) {
  ShProgram nibble = SH_BEGIN_PROGRAM() {
    typename T2::InputType SH_NAMEDECL(attrib, name);
    typename T2::OutputType SH_NAMEDECL(transAttrib, name) = m | attrib; 
  } SH_END;
  return nibble;
}

template<typename T, typename T2>
ShProgram cast(std::string name) {
  ShProgram nibble = SH_BEGIN_PROGRAM() {
    typename T::InputType SH_NAMEDECL(in, name);
    typename T2::OutputType SH_NAMEDECL(out, name) = cast<T2::typesize>( in );
  } SH_END;
  return nibble;
}

#define SHNIBBLE_UNARY_OP(opfunc, opcode) \
template<typename T>\
ShProgram opfunc(std::string name) {\
  ShProgram nibble = SH_BEGIN_PROGRAM() {\
    typename T::InputType SH_NAMEDECL(in, name);\
    typename T::OutputType SH_NAMEDECL(out, name) = opcode; \
  } SH_END;\
  return nibble; \
}

SHNIBBLE_UNARY_OP(abs, abs(in));
SHNIBBLE_UNARY_OP(acos, acos(in));
SHNIBBLE_UNARY_OP(asin, asin(in));
SHNIBBLE_UNARY_OP(cos, cos(in));
SHNIBBLE_UNARY_OP(frac, frac(in));
SHNIBBLE_UNARY_OP(sin, sin(in));
SHNIBBLE_UNARY_OP(sqrt, sqrt(in));
SHNIBBLE_UNARY_OP(normalize, normalize(in));
SHNIBBLE_UNARY_OP(pos, pos(in));

#define SHNIBBLE_BINARY_OP(opfunc, opcode) \
template<typename T> \
ShProgram opfunc(std::string name) { \
  ShProgram nibble = SH_BEGIN_PROGRAM() { \
    typename T::InputType SH_DECL(a); \
    typename T::InputType SH_DECL(b); \
    typename T::OutputType SH_NAMEDECL(result, name) = opcode; \
  } SH_END; \
  return nibble; \
}

SHNIBBLE_BINARY_OP(add, a + b)
SHNIBBLE_BINARY_OP(mul, a * b)
SHNIBBLE_BINARY_OP(div, a / b) 
SHNIBBLE_BINARY_OP(pow, pow(a, b))
SHNIBBLE_BINARY_OP(slt, a < b) 
SHNIBBLE_BINARY_OP(sle, a <= b) 
SHNIBBLE_BINARY_OP(sgt, a > b) 
SHNIBBLE_BINARY_OP(sge, a >= b) 
SHNIBBLE_BINARY_OP(seq, a == b) 
SHNIBBLE_BINARY_OP(sne, a != b) 
SHNIBBLE_BINARY_OP(fmod, fmod(a, b))
SHNIBBLE_BINARY_OP(min, min(a, b))
SHNIBBLE_BINARY_OP(max, max(a, b))

template<typename T> 
ShProgram dot(std::string name) { 
  ShProgram nibble = SH_BEGIN_PROGRAM() {
    typename T::InputType SH_DECL(a); 
    typename T::InputType SH_DECL(b); 
    ShOutputAttrib1f SH_NAMEDECL(result, name) = dot(a, b); 
  } SH_END; 
  return nibble;
}

template<typename T> 
ShProgram lerp(std::string name) { 
  ShProgram nibble = SH_BEGIN_PROGRAM() {
    typename T::InputType SH_DECL(a); 
    typename T::InputType SH_DECL(b); 
    typename T::InputType SH_DECL(alpha); 
    typename T::OutputType SH_NAMEDECL(result, name) = lerp(alpha, a, b); 
  } SH_END; 
  return nibble;
}

}

#endif
