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
ShProgram keep(std::string name = "") {
  ShProgram prog = SH_BEGIN_PROGRAM() {
    typename T::InputType SH_NAMEDECL( attr, name ); 
    typename T::OutputType SH_NAMEDECL( out, name );
    out = attr;
  } SH_END_PROGRAM;
  return prog;
}

template<typename T>
ShProgram lose(std::string name = "") {
  ShProgram prog = SH_BEGIN_PROGRAM() {
    typename T::InputType SH_NAMEDECL( attr, name );
    ShAttrib4f dummy = dummy;
  } SH_END_PROGRAM;
  return prog;
};

template<typename T>
ShProgram access(const ShTexture1D<T> &tex) {
  ShProgram nibble = SH_BEGIN_PROGRAM() {
    ShInputTexCoord1f SH_DECL(tc);
    typename T::OutputType SH_DECL(result) = tex(tc);
  } SH_END;
  return nibble;
}

template<typename T>
ShProgram access(const ShTexture2D<T> &tex) {
  ShProgram nibble = SH_BEGIN_PROGRAM() {
    ShInputTexCoord2f SH_DECL(tc);
    typename T::OutputType SH_DECL(result) = tex(tc);
  } SH_END;
  return nibble;
}

template<typename T>
ShProgram access(const ShTexture3D<T> &tex) {
  ShProgram nibble = SH_BEGIN_PROGRAM() {
    ShInputTexCoord3f SH_DECL(tc);
    typename T::OutputType SH_DECL(result) = tex(tc);
  } SH_END;
  return nibble;
}

template<typename T2, int Rows, int Cols, int Kind, typename T>
ShProgram transform(const ShMatrix<Rows, Cols, Kind, T> &m) {
  ShProgram nibble = SH_BEGIN_PROGRAM() {
    typename T2::InputType SH_DECL(attrib);
    typename T2::OutputType SH_DECL(transAttrib) = m | attrib; 
  } SH_END;
  return nibble;
}

template<typename T, typename T2>
ShProgram cast() {
  ShProgram nibble = SH_BEGIN_PROGRAM() {
    typename T::InputType SH_DECL(in);
    typename T2::OutputType SH_DECL(out); 
    int size1 = T::typesize;
    int size2 = T2::typesize;
    int copySize = std::min(size1, size2);

    int indices[copySize];
    for(int i = 0; i < copySize; ++i) swiz[i] = i;
    if( T::typesize < T2::typesize ) {
      out<size2>(indices) = in;
    } else {
      out = in<size1>(indices);
    }
  } SH_END;
  return nibble;
}
template<typename T, typename T2>
ShProgram rcast() {
  ShProgram nibble = SH_BEGIN_PROGRAM() {
    typename T::InputType SH_DECL(in);
    typename T2::OutputType SH_DECL(out); 
    int size1 = T::typesize;
    int size2 = T2::typesize;
    int copySize = std::min(size1, size2);

    int indices[copySize];
    for(int i = 0; i < copySize; ++i) swiz[i] = i + size1 - copySize;
    if( T::typesize < T2::typesize ) {
      out<size2>(indices) = in;
    } else {
      out = in<size1>(indices);
    }
  } SH_END;
  return nibble;
}

#define SHNIBBLE_UNARY_OP( opfunc, opcode ) \
template<typename T>\
ShProgram opfunc() {\
  ShProgram nibble = SH_BEGIN_PROGRAM() {\
    typename T::InputType SH_DECL( in );\
    typename T::OutputType SH_DECL( out ) = opcode; \
  } SH_END;\
  return nibble; \
}

SHNIBBLE_UNARY_OP( abs, abs(in) );
SHNIBBLE_UNARY_OP( acos, acos(in) );
SHNIBBLE_UNARY_OP( asin, asin(in) );
SHNIBBLE_UNARY_OP( cos, cos(in) );
SHNIBBLE_UNARY_OP( frac, frac(in) );
SHNIBBLE_UNARY_OP( sin, sin(in) );
SHNIBBLE_UNARY_OP( sqrt, sqrt(in) );
SHNIBBLE_UNARY_OP( normalize, normalize(in) );
SHNIBBLE_UNARY_OP( pos, pos(in) );

#define SHNIBBLE_BINARY_OP( opfunc, opcode ) \
template<typename T> \
ShProgram opfunc() { \
  ShProgram nibble = SH_BEGIN_PROGRAM() { \
    typename T::InputType SH_DECL(a); \
    typename T::InputType SH_DECL(b); \
    typename T::OutputType SH_DECL( result ) = opcode; \
  } SH_END; \
  return nibble; \
}

SHNIBBLE_BINARY_OP( add, add( a, b ) )
SHNIBBLE_BINARY_OP( mul, mul( a, b ) )
SHNIBBLE_BINARY_OP( div, div( a, b ) )
SHNIBBLE_BINARY_OP( pow, pow( a, b ) )
SHNIBBLE_BINARY_OP( slt, slt( a, b ) )
SHNIBBLE_BINARY_OP( sle, sle( a, b ) )
SHNIBBLE_BINARY_OP( sgt, sgt( a, b ) )
SHNIBBLE_BINARY_OP( sge, sge( a, b ) )
SHNIBBLE_BINARY_OP( seq, seq( a, b ) )
SHNIBBLE_BINARY_OP( sne, sne( a, b ) )
SHNIBBLE_BINARY_OP( fmod, fmod( a, b ) )
SHNIBBLE_BINARY_OP( min, min( a, b ) )
SHNIBBLE_BINARY_OP( max, max( a, b ) )

template<typename T> 
ShProgram dot() { 
  ShProgram nibble = SH_BEGIN_PROGRAM() {
    typename T::InputType SH_DECL(a); 
    typename T::InputType SH_DECL(b); 
    ShAttrib1f ( result ) = dot( a, b ); 
  } SH_END; 
}

}

#endif
