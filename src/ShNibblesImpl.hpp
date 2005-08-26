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
#ifndef SHNIBBLESIMPL_HPP
#define SHNIBBLESIMPL_HPP

#include "ShNibbles.hpp"
#include "ShTexCoord.hpp"
#include "ShLibNoise.hpp"

namespace SH {

template<typename T>
ShProgram keep(const std::string & name) 
{
  ShProgram nibble = SH_BEGIN_PROGRAM() {
    typename T::InOutType SH_NAMEDECL(attr, name); 
  } SH_END_PROGRAM;
  nibble.name("keep");
  return nibble;
}

template<typename T>
ShProgram dup(const std::string & name) 
{
  ShProgram nibble = SH_BEGIN_PROGRAM() {
    typename T::InputType SH_NAMEDECL(attr, name); 
    typename T::OutputType SH_NAMEDECL(attr1, name + "_1") = attr; 
    typename T::OutputType SH_NAMEDECL(attr2, name + "_2") = attr; 
  } SH_END_PROGRAM;
  nibble.name("dup");
  return nibble;
}

template<typename T>
ShProgram lose(const std::string & name) 
{
  ShProgram nibble = SH_BEGIN_PROGRAM() {
    typename T::InputType SH_NAMEDECL(attr, name);
  } SH_END_PROGRAM;
  nibble.name("lose");
  return nibble;
};

template<typename T>
ShProgram shAccess(const ShBaseTexture1D<T> &tex, const std::string &tcname, const std::string & name) 
{
  ShProgram nibble = SH_BEGIN_PROGRAM() {
    ShInputTexCoord1f SH_NAMEDECL(tc, tcname);
    typename T::OutputType SH_NAMEDECL(result, name) = tex(tc);
  } SH_END;
  nibble.name("shAccess");
  return nibble;
}

template<typename T>
ShProgram shAccess(const ShBaseTexture2D<T> &tex, const std::string & tcname, const std::string & name) 
{
  ShProgram nibble = SH_BEGIN_PROGRAM() {
    ShInputTexCoord2f SH_NAMEDECL(tc, tcname);
    typename T::OutputType SH_NAMEDECL(result, name) = tex(tc);
  } SH_END;
  nibble.name("shAccess");
  return nibble;
}

template<typename T>
ShProgram shAccess(const ShBaseTextureRect<T> &tex, const std::string & tcname, const std::string & name) 
{
  ShProgram nibble = SH_BEGIN_PROGRAM() {
    ShInputTexCoord2f SH_NAMEDECL(tc, tcname);
    typename T::OutputType SH_NAMEDECL(result, name) = tex(tc);
  } SH_END;
  nibble.name("shAccess");
  return nibble;
}

template<typename T>
ShProgram shAccess(const ShBaseTexture3D<T> &tex, const std::string & tcname, const std::string & name) 
{
  ShProgram nibble = SH_BEGIN_PROGRAM() {
    ShInputTexCoord3f SH_NAMEDECL(tc, tcname);
    typename T::OutputType SH_NAMEDECL(result, name) = tex(tc);
  } SH_END;
  nibble.name("shAccess");
  return nibble;
}

template<typename T>
ShProgram shAccess(const ShBaseTextureCube<T> &tex, const std::string & tcname, const std::string & name) 
{
  ShProgram nibble = SH_BEGIN_PROGRAM() {
    ShInputTexCoord3f SH_NAMEDECL(tc, tcname);
    typename T::OutputType SH_NAMEDECL(result, name) = tex(tc);
  } SH_END;
  nibble.name("shAccess");
  return nibble;
}

template<typename T, int Rows, int Cols, ShBindingType Binding, typename T2>
ShProgram shTransform(const ShMatrix<Rows, Cols, Binding, T2> &m, const std::string & name) 
{
  ShProgram nibble = SH_BEGIN_PROGRAM() {
    typename T::InOutType SH_NAMEDECL(attrib, name) = m | attrib;
  } SH_END;
  nibble.name("shTransform");
  return nibble;
}

template<typename T, typename T2>
ShProgram shCast(const std::string & name)
{
  ShProgram nibble = SH_BEGIN_PROGRAM() {
    typename T::InputType SH_NAMEDECL(in, name);
    typename T2::OutputType SH_NAMEDECL(out, name) = cast<T2::typesize>( in );
  } SH_END;
  nibble.name("shCast");
  return nibble;
}

template<typename T, typename T2>
ShProgram shFillcast(const std::string & name)
{
  ShProgram nibble = SH_BEGIN_PROGRAM() {
    typename T::InputType SH_NAMEDECL(in, name);
    typename T2::OutputType SH_NAMEDECL(out, name) = fillcast<T2::typesize>( in );
  } SH_END;
  nibble.name("shFillcast");
  return nibble;
}

#define SHNIBBLE_UNARY_OP(opfunc, opcode) \
template<typename T>\
ShProgram opfunc(const std::string & name) \
{\
  ShProgram nibble = SH_BEGIN_PROGRAM() {\
    typename T::InOutType SH_NAMEDECL(x, name) = opcode;\
  } SH_END;\
  nibble.name(# opfunc); \
  return nibble; \
}

SHNIBBLE_UNARY_OP(shAbs, abs(x))
SHNIBBLE_UNARY_OP(shAcos, acos(x))
SHNIBBLE_UNARY_OP(shAcosh, acosh(x))
SHNIBBLE_UNARY_OP(shAsin, asin(x))
SHNIBBLE_UNARY_OP(shAsinh, asinh(x))
SHNIBBLE_UNARY_OP(shAtan, atan(x))
SHNIBBLE_UNARY_OP(shAtanh, atanh(x))
SHNIBBLE_UNARY_OP(shCbrt, cbrt(x))
SHNIBBLE_UNARY_OP(shCeil, ceil(x))
SHNIBBLE_UNARY_OP(shCos, cos(x))
SHNIBBLE_UNARY_OP(shCosh, cosh(x))
SHNIBBLE_UNARY_OP(shDx, dx(x))
SHNIBBLE_UNARY_OP(shDy, dy(x))
SHNIBBLE_UNARY_OP(shFloor, floor(x))
SHNIBBLE_UNARY_OP(shFrac, frac(x))
SHNIBBLE_UNARY_OP(shExp, exp(x))
SHNIBBLE_UNARY_OP(shExpm1, expm1(x))
SHNIBBLE_UNARY_OP(shExp2, exp2(x))
SHNIBBLE_UNARY_OP(shExp10, exp10(x))
SHNIBBLE_UNARY_OP(shFwidth, fwidth(x))
SHNIBBLE_UNARY_OP(shLog, log(x))
SHNIBBLE_UNARY_OP(shLogp1, logp1(x))
SHNIBBLE_UNARY_OP(shLog2, log2(x))
SHNIBBLE_UNARY_OP(shLog10, log10(x))
SHNIBBLE_UNARY_OP(shNormalize, normalize(x))
SHNIBBLE_UNARY_OP(shNot, !x)
SHNIBBLE_UNARY_OP(shPos, pos(x))
SHNIBBLE_UNARY_OP(shRcp, rcp(x))
SHNIBBLE_UNARY_OP(shRound, round(x))
SHNIBBLE_UNARY_OP(shRsqrt, rsqrt(x))
SHNIBBLE_UNARY_OP(shSat, sat(x))
SHNIBBLE_UNARY_OP(shSign, sign(x))
SHNIBBLE_UNARY_OP(shSin, sin(x))
SHNIBBLE_UNARY_OP(shSinh, sinh(x))
SHNIBBLE_UNARY_OP(shSort, sort(x))
SHNIBBLE_UNARY_OP(shSqrt, sqrt(x))
SHNIBBLE_UNARY_OP(shTan, tan(x))
SHNIBBLE_UNARY_OP(shTanh, tanh(x))

#define SHNIBBLE_BINARY_OP(opfunc, opcode) \
template<typename T1, typename T2> \
ShProgram opfunc(const std::string & output_name, \
    const std::string & input_name0, const std::string & input_name1) \
{ \
  ShProgram nibble = SH_BEGIN_PROGRAM() { \
    typename T1::InputType SH_NAMEDECL(a, input_name0); \
    typename T2::InputType SH_NAMEDECL(b, input_name1); \
    typename SelectType<(T1::typesize > T2::typesize), typename T1::OutputType, typename T2::OutputType>::type\
      SH_NAMEDECL(result, output_name) = opcode; \
  } SH_END; \
  nibble.name(# opfunc); \
  return nibble; \
} \
\
template<typename T1> \
ShProgram opfunc(const std::string & output_name,\
    const std::string & input_name0, const std::string & input_name1 ) \
{ \
  return opfunc<T1, T1>(output_name, input_name0, input_name1); \
}

SHNIBBLE_BINARY_OP(shAdd, a + b)
SHNIBBLE_BINARY_OP(shAnd, a && b)
SHNIBBLE_BINARY_OP(shAtan2, atan2(a, b)) 
SHNIBBLE_BINARY_OP(shDiv, a / b)
SHNIBBLE_BINARY_OP(shFaceforward, faceforward(a, b))
SHNIBBLE_BINARY_OP(shMax, max(a, b))
SHNIBBLE_BINARY_OP(shMin, min(a, b))
SHNIBBLE_BINARY_OP(shMod, mod(a, b))
SHNIBBLE_BINARY_OP(shMul, a * b)
SHNIBBLE_BINARY_OP(shOr, a || b)
SHNIBBLE_BINARY_OP(shPow, pow(a, b))
SHNIBBLE_BINARY_OP(shReflect, reflect(a, b)) 
SHNIBBLE_BINARY_OP(shSeq, a == b) 
SHNIBBLE_BINARY_OP(shSle, a <= b) 
SHNIBBLE_BINARY_OP(shSlt, a < b) 
SHNIBBLE_BINARY_OP(shSge, a >= b) 
SHNIBBLE_BINARY_OP(shSgt, a > b) 
SHNIBBLE_BINARY_OP(shSne, a != b) 
SHNIBBLE_BINARY_OP(shSub, a - b)

#define SHNIBBLE_TERNARY_OP(opfunc, opcode) \
template<typename T1, typename T2, typename T3> \
ShProgram opfunc(const std::string & output_name, \
    const std::string & input_name0, const std::string & input_name1, \
    const std::string & input_name2) \
{ \
  ShProgram nibble = SH_BEGIN_PROGRAM() { \
    typename T1::InputType SH_NAMEDECL(a, input_name0); \
    typename T2::InputType SH_NAMEDECL(b, input_name1); \
    typename T3::InputType SH_NAMEDECL(c, input_name2); \
    typedef typename SelectType<(T1::typesize > T2::typesize), typename T1::OutputType, \
                                typename T2::OutputType>::type T1T2; \
    typename SelectType<(T1T2::typesize > T3::typesize), typename T1T2::OutputType, \
                        typename T3::OutputType>::type SH_NAMEDECL(result, output_name) = opcode; \
  } SH_END; \
  nibble.name(# opfunc); \
  return nibble; \
} \
\
template<typename T1, typename T2> \
ShProgram opfunc(const std::string & output_name,\
    const std::string & input_name0, const std::string & input_name1, \
    const std::string & input_name2) \
{ \
  return opfunc<T1, T2, T2>(output_name, input_name0, input_name1, input_name2); \
} \
\
template<typename T1> \
ShProgram opfunc(const std::string & output_name,\
    const std::string & input_name0, const std::string & input_name1, \
    const std::string & input_name2) \
{ \
  return opfunc<T1, T1, T1>(output_name, input_name0, input_name1, input_name2); \
}

SHNIBBLE_TERNARY_OP(shClamp, clamp(a, b, c))
SHNIBBLE_TERNARY_OP(shCond, cond(a, b, c))
SHNIBBLE_TERNARY_OP(shMad, mad(a, b, c))
SHNIBBLE_TERNARY_OP(shSmoothstep, smoothstep(a, b, c))

#define SHNIBBLE_HASH_FUNC(opfunc, opcode) \
template<typename T1, typename T2> \
ShProgram opfunc(const std::string & output_name, const std::string & input_name0) \
{ \
  ShProgram nibble = SH_BEGIN_PROGRAM() { \
    typename T1::InputType SH_NAMEDECL(a, input_name0); \
    typename T2::OutputType SH_NAMEDECL(result, output_name) = opcode<T2::typesize>(a); \
  } SH_END; \
  nibble.name(# opfunc); \
  return nibble; \
}
  
SHNIBBLE_HASH_FUNC(shCellnoise, cellnoise)
SHNIBBLE_HASH_FUNC(shScellnoise, scellnoise)
SHNIBBLE_HASH_FUNC(shHash, SH::hash)
SHNIBBLE_HASH_FUNC(shTexhash, texhash)

#define SHNIBBLE_NOISE_FUNC(opfunc, opcode) \
template<typename T1, typename T2> \
ShProgram opfunc(const std::string & output_name, const std::string & input_name0) \
{ \
  ShProgram nibble = SH_BEGIN_PROGRAM() { \
    typename T1::InputType SH_NAMEDECL(a, input_name0); \
    typename T2::OutputType SH_NAMEDECL(result, output_name) = opcode<T2::typesize>(a); \
  } SH_END; \
  nibble.name(# opfunc); \
  return nibble; \
}\
template<typename T1, typename T2, typename T3> \
ShProgram opfunc(const std::string & output_name, const std::string & input_name0, \
                 const std::string & input_name1) \
{ \
  ShProgram nibble = SH_BEGIN_PROGRAM() { \
    typename T1::InputType SH_NAMEDECL(a, input_name0); \
    typename T2::InputType SH_NAMEDECL(b, input_name1); \
    typename T3::OutputType SH_NAMEDECL(result, output_name) = opcode<T2::typesize>(a, b); \
  } SH_END; \
  nibble.name(# opfunc); \
  return nibble; \
}

SHNIBBLE_NOISE_FUNC(shLinnoise, linnoise)
SHNIBBLE_NOISE_FUNC(shNoise, noise)
SHNIBBLE_NOISE_FUNC(shPerlin, perlin)
SHNIBBLE_NOISE_FUNC(shTurbulence, turbulence)
SHNIBBLE_NOISE_FUNC(shSlinnoise, slinnoise)
SHNIBBLE_NOISE_FUNC(shSnoise, snoise)
SHNIBBLE_NOISE_FUNC(shSperlin, sperlin)
SHNIBBLE_NOISE_FUNC(shSturbulence, sturbulence)

#define SHNIBBLE_DISTANCE_FUNC(opfunc, opcode) \
template<typename T1, typename T2> \
ShProgram opfunc(const std::string & output_name, \
                 const std::string & input_name0, \
                 const std::string & input_name1) \
{ \
  ShProgram nibble = SH_BEGIN_PROGRAM() { \
    typename T1::InputType SH_NAMEDECL(a, input_name0); \
    typename T2::InputType SH_NAMEDECL(b, input_name1); \
    ShOutputAttrib1f SH_NAMEDECL(result, output_name) = opcode; \
  } SH_END; \
  nibble.name(# opfunc); \
  return nibble; \
} \
template<typename T> \
ShProgram opfunc(const std::string & output_name, \
                 const std::string & input_name0, \
                 const std::string & input_name1) \
{ \
  return opfunc<T, T>(output_name, input_name0, input_name1);\
}

SHNIBBLE_DISTANCE_FUNC(shDistance, distance(a, b))
SHNIBBLE_DISTANCE_FUNC(shDistance_1, distance_1(a, b))
SHNIBBLE_DISTANCE_FUNC(shDistance_inf, distance_inf(a, b))

#define SHNIBBLE_UNARY_TUPLEOP(opfunc, opcode) \
template<typename T> \
ShProgram opfunc(const std::string & output_name, \
                 const std::string & input_name0) \
{ \
  ShProgram nibble = SH_BEGIN_PROGRAM() { \
    typename T::InputType SH_NAMEDECL(a, input_name0); \
    ShOutputAttrib1f SH_NAMEDECL(result, output_name) = opcode; \
  } SH_END; \
  nibble.name(# opfunc); \
  return nibble; \
}

SHNIBBLE_UNARY_TUPLEOP(shAll, all(a));
SHNIBBLE_UNARY_TUPLEOP(shAny, any(a));
SHNIBBLE_UNARY_TUPLEOP(shLength, length(a))
SHNIBBLE_UNARY_TUPLEOP(shLength_1, length(a))
SHNIBBLE_UNARY_TUPLEOP(shLength_inf, length(a))
SHNIBBLE_UNARY_TUPLEOP(shMax1, max(a));
SHNIBBLE_UNARY_TUPLEOP(shMin1, min(a));
SHNIBBLE_UNARY_TUPLEOP(shProd, prod(a));
SHNIBBLE_UNARY_TUPLEOP(shSum, sum(a));

template<typename T> 
ShProgram shCross(const std::string& name, const std::string& input_name0,
                  const std::string& input_name1)
{ 
  ShProgram nibble = SH_BEGIN_PROGRAM() {
    typename T::InputType SH_NAMEDECL(a, input_name0); 
    typename T::InputType SH_NAMEDECL(b, input_name1); 
    typename T::OutputType SH_NAMEDECL(result, name) = cross(a, b); 
  } SH_END; 
  nibble.name("shCross");
  return nibble;
}

template<typename T> 
ShProgram shDot(const std::string & name, const std::string& input_name0,
                const std::string& input_name1)
{ 
  ShProgram nibble = SH_BEGIN_PROGRAM() {
    typename T::InputType SH_NAMEDECL(a, input_name0);
    typename T::InputType SH_NAMEDECL(b, input_name1);
    ShOutputAttrib1f SH_NAMEDECL(result, name) = dot(a, b);
  } SH_END;
  nibble.name("shDot");
  return nibble;
}

template<typename T> 
ShProgram shGradient(const std::string& name, const std::string& input_name0)
{ 
  ShProgram nibble = SH_BEGIN_PROGRAM() {
    typename T::InputType SH_NAMEDECL(a, input_name0);
    ShOutputAttrib2f SH_NAMEDECL(result, name) = gradient(a); 
  } SH_END; 
  nibble.name("shGradient");
  return nibble;
}

template<typename T1, typename T2> 
ShProgram shJoin(const std::string & name = "result",
                 const std::string & input_name0 = "x", 
                 const std::string & input_name1 = "y")
{
  ShProgram nibble = SH_BEGIN_PROGRAM() {
    typename T1::InputType SH_NAMEDECL(a, input_name0);
    typename T2::InputType SH_NAMEDECL(b, input_name1);
    ShAttrib<T1::typesize + T2::typesize, SH_OUTPUT,  float> SH_NAMEDECL(result, name) = join(a, b); 
  } SH_END; 
  nibble.name("shJoin");
  return nibble;
}

template<typename T1, typename T2>
ShProgram shLerp(const std::string & name, const std::string& input_name0,
                 const std::string& input_name1, const std::string& input_name2)
{ 
  ShProgram nibble = SH_BEGIN_PROGRAM() {
    typename T1::InputType SH_NAMEDECL(a, input_name0);
    typename T1::InputType SH_NAMEDECL(b, input_name1); 
    typename T2::InputType SH_NAMEDECL(alpha, input_name2);
    typename T1::OutputType SH_NAMEDECL(result, name) = lerp(alpha, a, b); 
  } SH_END; 
  nibble.name("shLerp");
  return nibble;
}

template<typename T1>
ShProgram shLerp(const std::string & name, const std::string& input_name0,
                 const std::string& input_name1, const std::string& input_name2)
{ 
  return shLerp<T1, T1>(name, input_name0, input_name1, input_name2);
}

template<typename T> 
ShProgram shLit(const std::string& name = "result",
                const std::string& input_name0 = "x", 
                const std::string& input_name1 = "y",
                const std::string& input_name2 = "z")
{ 
  ShProgram nibble = SH_BEGIN_PROGRAM() {
    typename T::InputType SH_NAMEDECL(a, input_name0);
    typename T::InputType SH_NAMEDECL(b, input_name1);
    typename T::InputType SH_NAMEDECL(c, input_name2);
    ShOutputAttrib4f SH_NAMEDECL(result, name) = lit(a, b, c); 
  } SH_END; 
  nibble.name("shLit");
  return nibble;
}

template<typename T1, typename T2>
ShProgram shPoly(const std::string & name,
                 const std::string & input_name0, 
                 const std::string & input_name1)
{
  ShProgram nibble = SH_BEGIN_PROGRAM() {
    typename T1::InputType SH_NAMEDECL(a, input_name0);
    typename T2::InputType SH_NAMEDECL(b, input_name1);
    typename T1::OutputType SH_NAMEDECL(result, name) = poly(a, b);
  } SH_END;
  nibble.name("shPoly");
  return nibble;
}

template<typename T1>
ShProgram shPoly(const std::string & name,
                 const std::string & input_name0, 
                 const std::string & input_name1)
{
  return shPoly<T1, T1>(name, input_name0, input_name1);
}

template<typename T1, typename T2>
ShProgram shRefract(const std::string& name = "result",
                    const std::string& input_name0 = "x", 
                    const std::string& input_name1 = "y",
                    const std::string& input_name2 = "z")
{
  ShProgram nibble = SH_BEGIN_PROGRAM() {
    typename T1::InputType SH_NAMEDECL(a, input_name0);
    typename T1::InputType SH_NAMEDECL(b, input_name1);
    typename T2::InputType SH_NAMEDECL(c, input_name2);
    typename T1::OutputType SH_NAMEDECL(result, name) = refract(a, b, c);
  } SH_END;
  nibble.name("shRefract");
  return nibble;
}

template<typename T>
ShProgram shRefract(const std::string& name = "result",
                    const std::string& input_name0 = "x", 
                    const std::string& input_name1 = "y",
                    const std::string& input_name2 = "z")
{
  return shRefract<T, T>(name, input_name0, input_name1, input_name2);
}

template<typename T>
ShProgram shSmoothpulse(const std::string& name = "result",
                        const std::string& input_name0 = "x", 
                        const std::string& input_name1 = "y",
                        const std::string& input_name2 = "z",
                        const std::string& input_name3 = "w")
{
  ShProgram nibble = SH_BEGIN_PROGRAM() {
    typename T::InputType SH_NAMEDECL(a, input_name0);
    typename T::InputType SH_NAMEDECL(b, input_name1);
    typename T::InputType SH_NAMEDECL(c, input_name2);
    typename T::InputType SH_NAMEDECL(d, input_name3);
    typename T::OutputType SH_NAMEDECL(result, name) = smoothpulse(a, b, c, d);
  } SH_END;
  nibble.name("shSmoothpulse");
  return nibble;
}

} // namespace

#endif // SHNIBBLESIMPL_HPP
