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
#ifndef SHNIBBLESIMPL_HPP
#define SHNIBBLESIMPL_HPP

#include "Nibbles.hpp"
#include "TexCoord.hpp"
#include "LibNoise.hpp"

namespace SH {

template<typename T>
Program keep(const std::string & name) 
{
  Program nibble = SH_BEGIN_PROGRAM() {
    typename T::InOutType NAMEDECL(attr, name); 
  } SH_END_PROGRAM;
  nibble.name("keep");
  return nibble;
}

template<typename T>
Program dup(const std::string & name) 
{
  Program nibble = SH_BEGIN_PROGRAM() {
    typename T::InputType NAMEDECL(attr, name); 
    typename T::OutputType NAMEDECL(attr1, name + "_1") = attr; 
    typename T::OutputType NAMEDECL(attr2, name + "_2") = attr; 
  } SH_END_PROGRAM;
  nibble.name("dup");
  return nibble;
}

template<typename T>
Program lose(const std::string & name) 
{
  Program nibble = SH_BEGIN_PROGRAM() {
    typename T::InputType NAMEDECL(attr, name);
  } SH_END_PROGRAM;
  nibble.name("lose");
  return nibble;
};

template<typename T>
Program access(const BaseTexture1D<T> &tex, const std::string &tcname, const std::string & name) 
{
  Program nibble = SH_BEGIN_PROGRAM() {
    InputTexCoord1f NAMEDECL(tc, tcname);
    typename T::OutputType NAMEDECL(result, name) = tex(tc);
  } SH_END;
  nibble.name("access");
  return nibble;
}

template<typename T>
Program access(const BaseTexture2D<T> &tex, const std::string & tcname, const std::string & name) 
{
  Program nibble = SH_BEGIN_PROGRAM() {
    InputTexCoord2f NAMEDECL(tc, tcname);
    typename T::OutputType NAMEDECL(result, name) = tex(tc);
  } SH_END;
  nibble.name("access");
  return nibble;
}

template<typename T>
Program access(const BaseTextureRect<T> &tex, const std::string & tcname, const std::string & name) 
{
  Program nibble = SH_BEGIN_PROGRAM() {
    InputTexCoord2f NAMEDECL(tc, tcname);
    typename T::OutputType NAMEDECL(result, name) = tex(tc);
  } SH_END;
  nibble.name("access");
  return nibble;
}

template<typename T>
Program access(const BaseTexture3D<T> &tex, const std::string & tcname, const std::string & name) 
{
  Program nibble = SH_BEGIN_PROGRAM() {
    InputTexCoord3f NAMEDECL(tc, tcname);
    typename T::OutputType NAMEDECL(result, name) = tex(tc);
  } SH_END;
  nibble.name("access");
  return nibble;
}

template<typename T>
Program access(const BaseTextureCube<T> &tex, const std::string & tcname, const std::string & name) 
{
  Program nibble = SH_BEGIN_PROGRAM() {
    InputTexCoord3f NAMEDECL(tc, tcname);
    typename T::OutputType NAMEDECL(result, name) = tex(tc);
  } SH_END;
  nibble.name("access");
  return nibble;
}

template<typename T, int Rows, int Cols, BindingType Binding, typename T2>
Program transform(const Matrix<Rows, Cols, Binding, T2> &m, const std::string & name) 
{
  Program nibble = SH_BEGIN_PROGRAM() {
    typename T::InOutType NAMEDECL(attrib, name) = m | attrib;
  } SH_END;
  nibble.name("transform");
  return nibble;
}

template<typename T, typename T2>
Program cast(const std::string & name)
{
  Program nibble = SH_BEGIN_PROGRAM() {
    typename T::InputType NAMEDECL(in, name);
    typename T2::OutputType NAMEDECL(out, name) = cast<T2::typesize>( in );
  } SH_END;
  nibble.name("cast");
  return nibble;
}

template<typename T, typename T2>
Program fillcast(const std::string & name)
{
  Program nibble = SH_BEGIN_PROGRAM() {
    typename T::InputType NAMEDECL(in, name);
    typename T2::OutputType NAMEDECL(out, name) = fillcast<T2::typesize>( in );
  } SH_END;
  nibble.name("fillcast");
  return nibble;
}

#define NIBBLE_UNARY_OP(opfunc, opcode) \
template<typename T>\
Program opfunc(const std::string & name) \
{\
  Program nibble = SH_BEGIN_PROGRAM() {\
    typename T::InOutType NAMEDECL(x, name) = opcode;\
  } SH_END;\
  nibble.name(# opfunc); \
  return nibble; \
}

NIBBLE_UNARY_OP(abs, abs(x))
NIBBLE_UNARY_OP(acos, acos(x))
NIBBLE_UNARY_OP(acosh, acosh(x))
NIBBLE_UNARY_OP(asin, asin(x))
NIBBLE_UNARY_OP(asinh, asinh(x))
NIBBLE_UNARY_OP(atan, atan(x))
NIBBLE_UNARY_OP(atanh, atanh(x))
NIBBLE_UNARY_OP(cbrt, cbrt(x))
NIBBLE_UNARY_OP(ceil, ceil(x))
NIBBLE_UNARY_OP(cos, cos(x))
NIBBLE_UNARY_OP(cosh, cosh(x))
NIBBLE_UNARY_OP(dx, dx(x))
NIBBLE_UNARY_OP(dy, dy(x))
NIBBLE_UNARY_OP(floor, floor(x))
NIBBLE_UNARY_OP(frac, frac(x))
NIBBLE_UNARY_OP(exp, exp(x))
NIBBLE_UNARY_OP(expm1, expm1(x))
NIBBLE_UNARY_OP(exp2, exp2(x))
NIBBLE_UNARY_OP(exp10, exp10(x))
NIBBLE_UNARY_OP(fwidth, fwidth(x))
NIBBLE_UNARY_OP(log, log(x))
NIBBLE_UNARY_OP(logp1, logp1(x))
NIBBLE_UNARY_OP(log2, log2(x))
NIBBLE_UNARY_OP(log10, log10(x))
NIBBLE_UNARY_OP(normalize, normalize(x))
NIBBLE_UNARY_OP(logical_not, !x)
NIBBLE_UNARY_OP(pos, pos(x))
NIBBLE_UNARY_OP(rcp, rcp(x))
NIBBLE_UNARY_OP(round, round(x))
NIBBLE_UNARY_OP(rsqrt, rsqrt(x))
NIBBLE_UNARY_OP(sat, sat(x))
NIBBLE_UNARY_OP(sign, sign(x))
NIBBLE_UNARY_OP(sin, sin(x))
NIBBLE_UNARY_OP(sinh, sinh(x))
NIBBLE_UNARY_OP(sort, sort(x))
NIBBLE_UNARY_OP(sqrt, sqrt(x))
NIBBLE_UNARY_OP(tan, tan(x))
NIBBLE_UNARY_OP(tanh, tanh(x))

#define NIBBLE_BINARY_OP(opfunc, opcode) \
template<typename T1, typename T2> \
Program opfunc(const std::string & output_name, \
    const std::string & input_name0, const std::string & input_name1) \
{ \
  Program nibble = SH_BEGIN_PROGRAM() { \
    typename T1::InputType NAMEDECL(a, input_name0); \
    typename T2::InputType NAMEDECL(b, input_name1); \
    typename SelectType<(T1::typesize > T2::typesize), typename T1::OutputType, typename T2::OutputType>::type\
      NAMEDECL(result, output_name) = opcode; \
  } SH_END; \
  nibble.name(# opfunc); \
  return nibble; \
} \
\
template<typename T1> \
Program opfunc(const std::string & output_name,\
    const std::string & input_name0, const std::string & input_name1 ) \
{ \
  return opfunc<T1, T1>(output_name, input_name0, input_name1); \
}

NIBBLE_BINARY_OP(add, a + b)
NIBBLE_BINARY_OP(logical_and, a && b)
NIBBLE_BINARY_OP(atan2, atan2(a, b)) 
NIBBLE_BINARY_OP(div, a / b)
NIBBLE_BINARY_OP(faceforward, faceforward(a, b))
NIBBLE_BINARY_OP(max, max(a, b))
NIBBLE_BINARY_OP(min, min(a, b))
NIBBLE_BINARY_OP(mod, mod(a, b))
NIBBLE_BINARY_OP(mul, a * b)
NIBBLE_BINARY_OP(logical_or, a || b)
NIBBLE_BINARY_OP(pow, pow(a, b))
NIBBLE_BINARY_OP(reflect, reflect(a, b)) 
NIBBLE_BINARY_OP(seq, a == b) 
NIBBLE_BINARY_OP(sle, a <= b) 
NIBBLE_BINARY_OP(slt, a < b) 
NIBBLE_BINARY_OP(sge, a >= b) 
NIBBLE_BINARY_OP(sgt, a > b) 
NIBBLE_BINARY_OP(sne, a != b) 
NIBBLE_BINARY_OP(sub, a - b)

#define NIBBLE_TERNARY_OP(opfunc, opcode) \
template<typename T1, typename T2, typename T3> \
Program opfunc(const std::string & output_name, \
    const std::string & input_name0, const std::string & input_name1, \
    const std::string & input_name2) \
{ \
  Program nibble = SH_BEGIN_PROGRAM() { \
    typename T1::InputType NAMEDECL(a, input_name0); \
    typename T2::InputType NAMEDECL(b, input_name1); \
    typename T3::InputType NAMEDECL(c, input_name2); \
    typedef typename SelectType<(T1::typesize > T2::typesize), typename T1::OutputType, \
                                typename T2::OutputType>::type T1T2; \
    typename SelectType<(T1T2::typesize > T3::typesize), typename T1T2::OutputType, \
                        typename T3::OutputType>::type NAMEDECL(result, output_name) = opcode; \
  } SH_END; \
  nibble.name(# opfunc); \
  return nibble; \
} \
\
template<typename T1, typename T2> \
Program opfunc(const std::string & output_name,\
    const std::string & input_name0, const std::string & input_name1, \
    const std::string & input_name2) \
{ \
  return opfunc<T1, T2, T2>(output_name, input_name0, input_name1, input_name2); \
} \
\
template<typename T1> \
Program opfunc(const std::string & output_name,\
    const std::string & input_name0, const std::string & input_name1, \
    const std::string & input_name2) \
{ \
  return opfunc<T1, T1, T1>(output_name, input_name0, input_name1, input_name2); \
}

NIBBLE_TERNARY_OP(clamp, clamp(a, b, c))
NIBBLE_TERNARY_OP(cond, cond(a, b, c))
NIBBLE_TERNARY_OP(mad, mad(a, b, c))
NIBBLE_TERNARY_OP(smoothstep, smoothstep(a, b, c))

#define NIBBLE_HASH_FUNC(opfunc, opcode) \
template<typename T1, typename T2> \
Program opfunc(const std::string & output_name, const std::string & input_name0) \
{ \
  Program nibble = SH_BEGIN_PROGRAM() { \
    typename T1::InputType NAMEDECL(a, input_name0); \
    typename T2::OutputType NAMEDECL(result, output_name) = opcode<T2::typesize>(a); \
  } SH_END; \
  nibble.name(# opfunc); \
  return nibble; \
}
  
NIBBLE_HASH_FUNC(cellnoise, cellnoise)
NIBBLE_HASH_FUNC(scellnoise, scellnoise)
NIBBLE_HASH_FUNC(hash, SH::hash)
NIBBLE_HASH_FUNC(texhash, texhash)

#define NIBBLE_NOISE_FUNC(opfunc, opcode) \
template<typename T1, typename T2> \
Program opfunc(const std::string & output_name, const std::string & input_name0) \
{ \
  Program nibble = SH_BEGIN_PROGRAM() { \
    typename T1::InputType NAMEDECL(a, input_name0); \
    typename T2::OutputType NAMEDECL(result, output_name) = opcode<T2::typesize>(a); \
  } SH_END; \
  nibble.name(# opfunc); \
  return nibble; \
}\
template<typename T1, typename T2, typename T3> \
Program opfunc(const std::string & output_name, const std::string & input_name0, \
                 const std::string & input_name1) \
{ \
  Program nibble = SH_BEGIN_PROGRAM() { \
    typename T1::InputType NAMEDECL(a, input_name0); \
    typename T2::InputType NAMEDECL(b, input_name1); \
    typename T3::OutputType NAMEDECL(result, output_name) = opcode<T2::typesize>(a, b); \
  } SH_END; \
  nibble.name(# opfunc); \
  return nibble; \
}

NIBBLE_NOISE_FUNC(linnoise, linnoise)
NIBBLE_NOISE_FUNC(noise, noise)
NIBBLE_NOISE_FUNC(perlin, perlin)
NIBBLE_NOISE_FUNC(turbulence, turbulence)
NIBBLE_NOISE_FUNC(slinnoise, slinnoise)
NIBBLE_NOISE_FUNC(snoise, snoise)
NIBBLE_NOISE_FUNC(sperlin, sperlin)
NIBBLE_NOISE_FUNC(sturbulence, sturbulence)

#define NIBBLE_DISTANCE_FUNC(opfunc, opcode) \
template<typename T1, typename T2> \
Program opfunc(const std::string & output_name, \
                 const std::string & input_name0, \
                 const std::string & input_name1) \
{ \
  Program nibble = SH_BEGIN_PROGRAM() { \
    typename T1::InputType NAMEDECL(a, input_name0); \
    typename T2::InputType NAMEDECL(b, input_name1); \
    OutputAttrib1f NAMEDECL(result, output_name) = opcode; \
  } SH_END; \
  nibble.name(# opfunc); \
  return nibble; \
} \
template<typename T> \
Program opfunc(const std::string & output_name, \
                 const std::string & input_name0, \
                 const std::string & input_name1) \
{ \
  return opfunc<T, T>(output_name, input_name0, input_name1);\
}

NIBBLE_DISTANCE_FUNC(distance, distance(a, b))
NIBBLE_DISTANCE_FUNC(distance_1, distance_1(a, b))
NIBBLE_DISTANCE_FUNC(distance_inf, distance_inf(a, b))

#define NIBBLE_UNARY_TUPLEOP(opfunc, opcode) \
template<typename T> \
Program opfunc(const std::string & output_name, \
                 const std::string & input_name0) \
{ \
  Program nibble = SH_BEGIN_PROGRAM() { \
    typename T::InputType NAMEDECL(a, input_name0); \
    OutputAttrib1f NAMEDECL(result, output_name) = opcode; \
  } SH_END; \
  nibble.name(# opfunc); \
  return nibble; \
}

NIBBLE_UNARY_TUPLEOP(all, all(a));
NIBBLE_UNARY_TUPLEOP(any, any(a));
NIBBLE_UNARY_TUPLEOP(length, length(a))
NIBBLE_UNARY_TUPLEOP(length_1, length(a))
NIBBLE_UNARY_TUPLEOP(length_inf, length(a))
NIBBLE_UNARY_TUPLEOP(max1, max(a));
NIBBLE_UNARY_TUPLEOP(min1, min(a));
NIBBLE_UNARY_TUPLEOP(prod, prod(a));
NIBBLE_UNARY_TUPLEOP(sum, sum(a));

template<int N, typename T>
Program bernstein(const std::string& name, const std::string& input_name0)
{
  Program nibble = SH_BEGIN_PROGRAM() {
    typename T::InputType NAMEDECL(a, input_name0);
    Attrib<N, OUTPUT, float> NAMEDECL(result, name) = bernstein<N>(a);
  } SH_END;
  nibble.name("bernstein");
  return nibble;
}

template<typename T1, typename T2>
Program bezier(const std::string& name, const std::string& input_name0,
                   const std::string& input_name1)
{
  Program nibble = SH_BEGIN_PROGRAM() {
    typename T1::InputType NAMEDECL(a, input_name0);
    typename T2::InputType NAMEDECL(b, input_name1);
    typename T2::OutputType NAMEDECL(result, name) = bezier(a, b);
  } SH_END;
  nibble.name("bezier");
  return nibble;
}

template<typename T> 
Program cross(const std::string& name, const std::string& input_name0,
                  const std::string& input_name1)
{ 
  Program nibble = SH_BEGIN_PROGRAM() {
    typename T::InputType NAMEDECL(a, input_name0); 
    typename T::InputType NAMEDECL(b, input_name1); 
    typename T::OutputType NAMEDECL(result, name) = cross(a, b); 
  } SH_END; 
  nibble.name("cross");
  return nibble;
}

template<typename T> 
Program discard(const std::string & name, const std::string& input_name0)
{ 
  Program nibble = SH_BEGIN_PROGRAM() {
    typename T::InputType NAMEDECL(a, input_name0);
    discard(a);
  } SH_END;
  nibble.name("discard");
  return nibble;
}

template<typename T> 
Program dot(const std::string & name, const std::string& input_name0,
                const std::string& input_name1)
{ 
  Program nibble = SH_BEGIN_PROGRAM() {
    typename T::InputType NAMEDECL(a, input_name0);
    typename T::InputType NAMEDECL(b, input_name1);
    OutputAttrib1f NAMEDECL(result, name) = dot(a, b);
  } SH_END;
  nibble.name("dot");
  return nibble;
}

template<typename T> 
Program gradient(const std::string& name, const std::string& input_name0)
{ 
  Program nibble = SH_BEGIN_PROGRAM() {
    typename T::InputType NAMEDECL(a, input_name0);
    OutputAttrib2f NAMEDECL(result, name) = gradient(a); 
  } SH_END; 
  nibble.name("gradient");
  return nibble;
}

template<int S, typename T>
Program groupsort(const std::string & name, const std::string& input_name0)
{ 
  Program nibble = SH_BEGIN_PROGRAM() {
    typename T::InputType NAMEDECL(a, input_name0);
    groupsort<S>(a);
  } SH_END;
  nibble.name("groupsort");
  return nibble;
}

template<typename T1, typename T2>
Program hermite(const std::string& name, const std::string& input_name0,
                    const std::string& input_name1, const std::string& input_name2,
                    const std::string& input_name3, const std::string& input_name4)
{
  Program nibble = SH_BEGIN_PROGRAM() {
    typename T1::InputType NAMEDECL(a, input_name0);
    typename T2::InputType NAMEDECL(b, input_name1);
    typename T2::InputType NAMEDECL(c, input_name2);
    typename T2::InputType NAMEDECL(d, input_name3);
    typename T2::InputType NAMEDECL(e, input_name4);
    typename T2::OutputType NAMEDECL(result, name) = hermite(a, b, c, d, e);
  } SH_END;
  nibble.name("hermite");
  return nibble;
}

template<typename T1, typename T2> 
Program join(const std::string & name,
                 const std::string & input_name0, 
                 const std::string & input_name1)
{
  Program nibble = SH_BEGIN_PROGRAM() {
    typename T1::InputType NAMEDECL(a, input_name0);
    typename T2::InputType NAMEDECL(b, input_name1);
    Attrib<T1::typesize + T2::typesize, OUTPUT, float> NAMEDECL(result, name) = join(a, b); 
  } SH_END; 
  nibble.name("join");
  return nibble;
}

template<typename T1, typename T2>
Program lerp(const std::string & name, const std::string& input_name0,
                 const std::string& input_name1, const std::string& input_name2)
{ 
  Program nibble = SH_BEGIN_PROGRAM() {
    typename T1::InputType NAMEDECL(a, input_name0);
    typename T1::InputType NAMEDECL(b, input_name1); 
    typename T2::InputType NAMEDECL(alpha, input_name2);
    typename T1::OutputType NAMEDECL(result, name) = lerp(alpha, a, b); 
  } SH_END; 
  nibble.name("lerp");
  return nibble;
}

template<typename T1>
Program lerp(const std::string & name, const std::string& input_name0,
                 const std::string& input_name1, const std::string& input_name2)
{ 
  return lerp<T1, T1>(name, input_name0, input_name1, input_name2);
}

template<typename T> 
Program lit(const std::string& name,
                const std::string& input_name0, 
                const std::string& input_name1,
                const std::string& input_name2)
{ 
  Program nibble = SH_BEGIN_PROGRAM() {
    typename T::InputType NAMEDECL(a, input_name0);
    typename T::InputType NAMEDECL(b, input_name1);
    typename T::InputType NAMEDECL(c, input_name2);
    OutputAttrib4f NAMEDECL(result, name) = lit(a, b, c); 
  } SH_END; 
  nibble.name("lit");
  return nibble;
}

template<typename T1, typename T2>
Program poly(const std::string & name,
                 const std::string & input_name0, 
                 const std::string & input_name1)
{
  Program nibble = SH_BEGIN_PROGRAM() {
    typename T1::InputType NAMEDECL(a, input_name0);
    typename T2::InputType NAMEDECL(b, input_name1);
    typename T1::OutputType NAMEDECL(result, name) = poly(a, b);
  } SH_END;
  nibble.name("poly");
  return nibble;
}

template<typename T1>
Program poly(const std::string & name,
                 const std::string & input_name0, 
                 const std::string & input_name1)
{
  return poly<T1, T1>(name, input_name0, input_name1);
}

template<typename T1, typename T2>
Program refract(const std::string& name,
                    const std::string& input_name0, 
                    const std::string& input_name1,
                    const std::string& input_name2)
{
  Program nibble = SH_BEGIN_PROGRAM() {
    typename T1::InputType NAMEDECL(a, input_name0);
    typename T1::InputType NAMEDECL(b, input_name1);
    typename T2::InputType NAMEDECL(c, input_name2);
    typename T1::OutputType NAMEDECL(result, name) = refract(a, b, c);
  } SH_END;
  nibble.name("refract");
  return nibble;
}

template<typename T>
Program refract(const std::string& name,
                    const std::string& input_name0, 
                    const std::string& input_name1,
                    const std::string& input_name2)
{
  return refract<T, T>(name, input_name0, input_name1, input_name2);
}

template<typename T>
Program smoothpulse(const std::string& name,
                        const std::string& input_name0, 
                        const std::string& input_name1,
                        const std::string& input_name2,
                        const std::string& input_name3)
{
  Program nibble = SH_BEGIN_PROGRAM() {
    typename T::InputType NAMEDECL(a, input_name0);
    typename T::InputType NAMEDECL(b, input_name1);
    typename T::InputType NAMEDECL(c, input_name2);
    typename T::InputType NAMEDECL(d, input_name3);
    typename T::OutputType NAMEDECL(result, name) = smoothpulse(a, b, c, d);
  } SH_END;
  nibble.name("smoothpulse");
  return nibble;
}

} // namespace

#endif // SHNIBBLESIMPL_HPP
