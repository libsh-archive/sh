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

#include "ShColor.hpp"
#include "ShColorImpl.hpp"
namespace SH {

template class ShColor<1, SH_INPUT, ShInterval<double> , false>;
template class ShColor<1, SH_INPUT, ShInterval<double> , true>;
template class ShColor<1, SH_INPUT, ShFracUShort, false>;
template class ShColor<1, SH_INPUT, ShFracUShort, true>;
template class ShColor<1, SH_INPUT, short, false>;
template class ShColor<1, SH_INPUT, short, true>;
template class ShColor<1, SH_INPUT, ShFracUInt, false>;
template class ShColor<1, SH_INPUT, ShFracUInt, true>;
template class ShColor<1, SH_INPUT, ShFracByte, false>;
template class ShColor<1, SH_INPUT, ShFracByte, true>;
template class ShColor<1, SH_INPUT, int, false>;
template class ShColor<1, SH_INPUT, int, true>;
template class ShColor<1, SH_INPUT, double, false>;
template class ShColor<1, SH_INPUT, double, true>;
template class ShColor<1, SH_INPUT, unsigned char, false>;
template class ShColor<1, SH_INPUT, unsigned char, true>;
template class ShColor<1, SH_INPUT, float, false>;
template class ShColor<1, SH_INPUT, float, true>;
template class ShColor<1, SH_INPUT, char, false>;
template class ShColor<1, SH_INPUT, char, true>;
template class ShColor<1, SH_INPUT, unsigned short, false>;
template class ShColor<1, SH_INPUT, unsigned short, true>;
template class ShColor<1, SH_INPUT, ShFracUByte, false>;
template class ShColor<1, SH_INPUT, ShFracUByte, true>;
template class ShColor<1, SH_INPUT, ShHalf, false>;
template class ShColor<1, SH_INPUT, ShHalf, true>;
template class ShColor<1, SH_INPUT, ShInterval<float> , false>;
template class ShColor<1, SH_INPUT, ShInterval<float> , true>;
template class ShColor<1, SH_INPUT, ShFracShort, false>;
template class ShColor<1, SH_INPUT, ShFracShort, true>;
template class ShColor<1, SH_INPUT, ShFracInt, false>;
template class ShColor<1, SH_INPUT, ShFracInt, true>;
template class ShColor<1, SH_INPUT, unsigned int, false>;
template class ShColor<1, SH_INPUT, unsigned int, true>;
template class ShColor<1, SH_OUTPUT, ShInterval<double> , false>;
template class ShColor<1, SH_OUTPUT, ShInterval<double> , true>;
template class ShColor<1, SH_OUTPUT, ShFracUShort, false>;
template class ShColor<1, SH_OUTPUT, ShFracUShort, true>;
template class ShColor<1, SH_OUTPUT, short, false>;
template class ShColor<1, SH_OUTPUT, short, true>;
template class ShColor<1, SH_OUTPUT, ShFracUInt, false>;
template class ShColor<1, SH_OUTPUT, ShFracUInt, true>;
template class ShColor<1, SH_OUTPUT, ShFracByte, false>;
template class ShColor<1, SH_OUTPUT, ShFracByte, true>;
template class ShColor<1, SH_OUTPUT, int, false>;
template class ShColor<1, SH_OUTPUT, int, true>;
template class ShColor<1, SH_OUTPUT, double, false>;
template class ShColor<1, SH_OUTPUT, double, true>;
template class ShColor<1, SH_OUTPUT, unsigned char, false>;
template class ShColor<1, SH_OUTPUT, unsigned char, true>;
template class ShColor<1, SH_OUTPUT, float, false>;
template class ShColor<1, SH_OUTPUT, float, true>;
template class ShColor<1, SH_OUTPUT, char, false>;
template class ShColor<1, SH_OUTPUT, char, true>;
template class ShColor<1, SH_OUTPUT, unsigned short, false>;
template class ShColor<1, SH_OUTPUT, unsigned short, true>;
template class ShColor<1, SH_OUTPUT, ShFracUByte, false>;
template class ShColor<1, SH_OUTPUT, ShFracUByte, true>;
template class ShColor<1, SH_OUTPUT, ShHalf, false>;
template class ShColor<1, SH_OUTPUT, ShHalf, true>;
template class ShColor<1, SH_OUTPUT, ShInterval<float> , false>;
template class ShColor<1, SH_OUTPUT, ShInterval<float> , true>;
template class ShColor<1, SH_OUTPUT, ShFracShort, false>;
template class ShColor<1, SH_OUTPUT, ShFracShort, true>;
template class ShColor<1, SH_OUTPUT, ShFracInt, false>;
template class ShColor<1, SH_OUTPUT, ShFracInt, true>;
template class ShColor<1, SH_OUTPUT, unsigned int, false>;
template class ShColor<1, SH_OUTPUT, unsigned int, true>;
template class ShColor<1, SH_INOUT, ShInterval<double> , false>;
template class ShColor<1, SH_INOUT, ShInterval<double> , true>;
template class ShColor<1, SH_INOUT, ShFracUShort, false>;
template class ShColor<1, SH_INOUT, ShFracUShort, true>;
template class ShColor<1, SH_INOUT, short, false>;
template class ShColor<1, SH_INOUT, short, true>;
template class ShColor<1, SH_INOUT, ShFracUInt, false>;
template class ShColor<1, SH_INOUT, ShFracUInt, true>;
template class ShColor<1, SH_INOUT, ShFracByte, false>;
template class ShColor<1, SH_INOUT, ShFracByte, true>;
template class ShColor<1, SH_INOUT, int, false>;
template class ShColor<1, SH_INOUT, int, true>;
template class ShColor<1, SH_INOUT, double, false>;
template class ShColor<1, SH_INOUT, double, true>;
template class ShColor<1, SH_INOUT, unsigned char, false>;
template class ShColor<1, SH_INOUT, unsigned char, true>;
template class ShColor<1, SH_INOUT, float, false>;
template class ShColor<1, SH_INOUT, float, true>;
template class ShColor<1, SH_INOUT, char, false>;
template class ShColor<1, SH_INOUT, char, true>;
template class ShColor<1, SH_INOUT, unsigned short, false>;
template class ShColor<1, SH_INOUT, unsigned short, true>;
template class ShColor<1, SH_INOUT, ShFracUByte, false>;
template class ShColor<1, SH_INOUT, ShFracUByte, true>;
template class ShColor<1, SH_INOUT, ShHalf, false>;
template class ShColor<1, SH_INOUT, ShHalf, true>;
template class ShColor<1, SH_INOUT, ShInterval<float> , false>;
template class ShColor<1, SH_INOUT, ShInterval<float> , true>;
template class ShColor<1, SH_INOUT, ShFracShort, false>;
template class ShColor<1, SH_INOUT, ShFracShort, true>;
template class ShColor<1, SH_INOUT, ShFracInt, false>;
template class ShColor<1, SH_INOUT, ShFracInt, true>;
template class ShColor<1, SH_INOUT, unsigned int, false>;
template class ShColor<1, SH_INOUT, unsigned int, true>;
template class ShColor<1, SH_TEMP, ShInterval<double> , false>;
template class ShColor<1, SH_TEMP, ShInterval<double> , true>;
template class ShColor<1, SH_TEMP, ShFracUShort, false>;
template class ShColor<1, SH_TEMP, ShFracUShort, true>;
template class ShColor<1, SH_TEMP, short, false>;
template class ShColor<1, SH_TEMP, short, true>;
template class ShColor<1, SH_TEMP, ShFracUInt, false>;
template class ShColor<1, SH_TEMP, ShFracUInt, true>;
template class ShColor<1, SH_TEMP, ShFracByte, false>;
template class ShColor<1, SH_TEMP, ShFracByte, true>;
template class ShColor<1, SH_TEMP, int, false>;
template class ShColor<1, SH_TEMP, int, true>;
template class ShColor<1, SH_TEMP, double, false>;
template class ShColor<1, SH_TEMP, double, true>;
template class ShColor<1, SH_TEMP, unsigned char, false>;
template class ShColor<1, SH_TEMP, unsigned char, true>;
template class ShColor<1, SH_TEMP, float, false>;
template class ShColor<1, SH_TEMP, float, true>;
template class ShColor<1, SH_TEMP, char, false>;
template class ShColor<1, SH_TEMP, char, true>;
template class ShColor<1, SH_TEMP, unsigned short, false>;
template class ShColor<1, SH_TEMP, unsigned short, true>;
template class ShColor<1, SH_TEMP, ShFracUByte, false>;
template class ShColor<1, SH_TEMP, ShFracUByte, true>;
template class ShColor<1, SH_TEMP, ShHalf, false>;
template class ShColor<1, SH_TEMP, ShHalf, true>;
template class ShColor<1, SH_TEMP, ShInterval<float> , false>;
template class ShColor<1, SH_TEMP, ShInterval<float> , true>;
template class ShColor<1, SH_TEMP, ShFracShort, false>;
template class ShColor<1, SH_TEMP, ShFracShort, true>;
template class ShColor<1, SH_TEMP, ShFracInt, false>;
template class ShColor<1, SH_TEMP, ShFracInt, true>;
template class ShColor<1, SH_TEMP, unsigned int, false>;
template class ShColor<1, SH_TEMP, unsigned int, true>;
template class ShColor<1, SH_CONST, ShInterval<double> , false>;
template class ShColor<1, SH_CONST, ShInterval<double> , true>;
template class ShColor<1, SH_CONST, ShFracUShort, false>;
template class ShColor<1, SH_CONST, ShFracUShort, true>;
template class ShColor<1, SH_CONST, short, false>;
template class ShColor<1, SH_CONST, short, true>;
template class ShColor<1, SH_CONST, ShFracUInt, false>;
template class ShColor<1, SH_CONST, ShFracUInt, true>;
template class ShColor<1, SH_CONST, ShFracByte, false>;
template class ShColor<1, SH_CONST, ShFracByte, true>;
template class ShColor<1, SH_CONST, int, false>;
template class ShColor<1, SH_CONST, int, true>;
template class ShColor<1, SH_CONST, double, false>;
template class ShColor<1, SH_CONST, double, true>;
template class ShColor<1, SH_CONST, unsigned char, false>;
template class ShColor<1, SH_CONST, unsigned char, true>;
template class ShColor<1, SH_CONST, float, false>;
template class ShColor<1, SH_CONST, float, true>;
template class ShColor<1, SH_CONST, char, false>;
template class ShColor<1, SH_CONST, char, true>;
template class ShColor<1, SH_CONST, unsigned short, false>;
template class ShColor<1, SH_CONST, unsigned short, true>;
template class ShColor<1, SH_CONST, ShFracUByte, false>;
template class ShColor<1, SH_CONST, ShFracUByte, true>;
template class ShColor<1, SH_CONST, ShHalf, false>;
template class ShColor<1, SH_CONST, ShHalf, true>;
template class ShColor<1, SH_CONST, ShInterval<float> , false>;
template class ShColor<1, SH_CONST, ShInterval<float> , true>;
template class ShColor<1, SH_CONST, ShFracShort, false>;
template class ShColor<1, SH_CONST, ShFracShort, true>;
template class ShColor<1, SH_CONST, ShFracInt, false>;
template class ShColor<1, SH_CONST, ShFracInt, true>;
template class ShColor<1, SH_CONST, unsigned int, false>;
template class ShColor<1, SH_CONST, unsigned int, true>;
template class ShColor<2, SH_INPUT, ShInterval<double> , false>;
template class ShColor<2, SH_INPUT, ShInterval<double> , true>;
template class ShColor<2, SH_INPUT, ShFracUShort, false>;
template class ShColor<2, SH_INPUT, ShFracUShort, true>;
template class ShColor<2, SH_INPUT, short, false>;
template class ShColor<2, SH_INPUT, short, true>;
template class ShColor<2, SH_INPUT, ShFracUInt, false>;
template class ShColor<2, SH_INPUT, ShFracUInt, true>;
template class ShColor<2, SH_INPUT, ShFracByte, false>;
template class ShColor<2, SH_INPUT, ShFracByte, true>;
template class ShColor<2, SH_INPUT, int, false>;
template class ShColor<2, SH_INPUT, int, true>;
template class ShColor<2, SH_INPUT, double, false>;
template class ShColor<2, SH_INPUT, double, true>;
template class ShColor<2, SH_INPUT, unsigned char, false>;
template class ShColor<2, SH_INPUT, unsigned char, true>;
template class ShColor<2, SH_INPUT, float, false>;
template class ShColor<2, SH_INPUT, float, true>;
template class ShColor<2, SH_INPUT, char, false>;
template class ShColor<2, SH_INPUT, char, true>;
template class ShColor<2, SH_INPUT, unsigned short, false>;
template class ShColor<2, SH_INPUT, unsigned short, true>;
template class ShColor<2, SH_INPUT, ShFracUByte, false>;
template class ShColor<2, SH_INPUT, ShFracUByte, true>;
template class ShColor<2, SH_INPUT, ShHalf, false>;
template class ShColor<2, SH_INPUT, ShHalf, true>;
template class ShColor<2, SH_INPUT, ShInterval<float> , false>;
template class ShColor<2, SH_INPUT, ShInterval<float> , true>;
template class ShColor<2, SH_INPUT, ShFracShort, false>;
template class ShColor<2, SH_INPUT, ShFracShort, true>;
template class ShColor<2, SH_INPUT, ShFracInt, false>;
template class ShColor<2, SH_INPUT, ShFracInt, true>;
template class ShColor<2, SH_INPUT, unsigned int, false>;
template class ShColor<2, SH_INPUT, unsigned int, true>;
template class ShColor<2, SH_OUTPUT, ShInterval<double> , false>;
template class ShColor<2, SH_OUTPUT, ShInterval<double> , true>;
template class ShColor<2, SH_OUTPUT, ShFracUShort, false>;
template class ShColor<2, SH_OUTPUT, ShFracUShort, true>;
template class ShColor<2, SH_OUTPUT, short, false>;
template class ShColor<2, SH_OUTPUT, short, true>;
template class ShColor<2, SH_OUTPUT, ShFracUInt, false>;
template class ShColor<2, SH_OUTPUT, ShFracUInt, true>;
template class ShColor<2, SH_OUTPUT, ShFracByte, false>;
template class ShColor<2, SH_OUTPUT, ShFracByte, true>;
template class ShColor<2, SH_OUTPUT, int, false>;
template class ShColor<2, SH_OUTPUT, int, true>;
template class ShColor<2, SH_OUTPUT, double, false>;
template class ShColor<2, SH_OUTPUT, double, true>;
template class ShColor<2, SH_OUTPUT, unsigned char, false>;
template class ShColor<2, SH_OUTPUT, unsigned char, true>;
template class ShColor<2, SH_OUTPUT, float, false>;
template class ShColor<2, SH_OUTPUT, float, true>;
template class ShColor<2, SH_OUTPUT, char, false>;
template class ShColor<2, SH_OUTPUT, char, true>;
template class ShColor<2, SH_OUTPUT, unsigned short, false>;
template class ShColor<2, SH_OUTPUT, unsigned short, true>;
template class ShColor<2, SH_OUTPUT, ShFracUByte, false>;
template class ShColor<2, SH_OUTPUT, ShFracUByte, true>;
template class ShColor<2, SH_OUTPUT, ShHalf, false>;
template class ShColor<2, SH_OUTPUT, ShHalf, true>;
template class ShColor<2, SH_OUTPUT, ShInterval<float> , false>;
template class ShColor<2, SH_OUTPUT, ShInterval<float> , true>;
template class ShColor<2, SH_OUTPUT, ShFracShort, false>;
template class ShColor<2, SH_OUTPUT, ShFracShort, true>;
template class ShColor<2, SH_OUTPUT, ShFracInt, false>;
template class ShColor<2, SH_OUTPUT, ShFracInt, true>;
template class ShColor<2, SH_OUTPUT, unsigned int, false>;
template class ShColor<2, SH_OUTPUT, unsigned int, true>;
template class ShColor<2, SH_INOUT, ShInterval<double> , false>;
template class ShColor<2, SH_INOUT, ShInterval<double> , true>;
template class ShColor<2, SH_INOUT, ShFracUShort, false>;
template class ShColor<2, SH_INOUT, ShFracUShort, true>;
template class ShColor<2, SH_INOUT, short, false>;
template class ShColor<2, SH_INOUT, short, true>;
template class ShColor<2, SH_INOUT, ShFracUInt, false>;
template class ShColor<2, SH_INOUT, ShFracUInt, true>;
template class ShColor<2, SH_INOUT, ShFracByte, false>;
template class ShColor<2, SH_INOUT, ShFracByte, true>;
template class ShColor<2, SH_INOUT, int, false>;
template class ShColor<2, SH_INOUT, int, true>;
template class ShColor<2, SH_INOUT, double, false>;
template class ShColor<2, SH_INOUT, double, true>;
template class ShColor<2, SH_INOUT, unsigned char, false>;
template class ShColor<2, SH_INOUT, unsigned char, true>;
template class ShColor<2, SH_INOUT, float, false>;
template class ShColor<2, SH_INOUT, float, true>;
template class ShColor<2, SH_INOUT, char, false>;
template class ShColor<2, SH_INOUT, char, true>;
template class ShColor<2, SH_INOUT, unsigned short, false>;
template class ShColor<2, SH_INOUT, unsigned short, true>;
template class ShColor<2, SH_INOUT, ShFracUByte, false>;
template class ShColor<2, SH_INOUT, ShFracUByte, true>;
template class ShColor<2, SH_INOUT, ShHalf, false>;
template class ShColor<2, SH_INOUT, ShHalf, true>;
template class ShColor<2, SH_INOUT, ShInterval<float> , false>;
template class ShColor<2, SH_INOUT, ShInterval<float> , true>;
template class ShColor<2, SH_INOUT, ShFracShort, false>;
template class ShColor<2, SH_INOUT, ShFracShort, true>;
template class ShColor<2, SH_INOUT, ShFracInt, false>;
template class ShColor<2, SH_INOUT, ShFracInt, true>;
template class ShColor<2, SH_INOUT, unsigned int, false>;
template class ShColor<2, SH_INOUT, unsigned int, true>;
template class ShColor<2, SH_TEMP, ShInterval<double> , false>;
template class ShColor<2, SH_TEMP, ShInterval<double> , true>;
template class ShColor<2, SH_TEMP, ShFracUShort, false>;
template class ShColor<2, SH_TEMP, ShFracUShort, true>;
template class ShColor<2, SH_TEMP, short, false>;
template class ShColor<2, SH_TEMP, short, true>;
template class ShColor<2, SH_TEMP, ShFracUInt, false>;
template class ShColor<2, SH_TEMP, ShFracUInt, true>;
template class ShColor<2, SH_TEMP, ShFracByte, false>;
template class ShColor<2, SH_TEMP, ShFracByte, true>;
template class ShColor<2, SH_TEMP, int, false>;
template class ShColor<2, SH_TEMP, int, true>;
template class ShColor<2, SH_TEMP, double, false>;
template class ShColor<2, SH_TEMP, double, true>;
template class ShColor<2, SH_TEMP, unsigned char, false>;
template class ShColor<2, SH_TEMP, unsigned char, true>;
template class ShColor<2, SH_TEMP, float, false>;
template class ShColor<2, SH_TEMP, float, true>;
template class ShColor<2, SH_TEMP, char, false>;
template class ShColor<2, SH_TEMP, char, true>;
template class ShColor<2, SH_TEMP, unsigned short, false>;
template class ShColor<2, SH_TEMP, unsigned short, true>;
template class ShColor<2, SH_TEMP, ShFracUByte, false>;
template class ShColor<2, SH_TEMP, ShFracUByte, true>;
template class ShColor<2, SH_TEMP, ShHalf, false>;
template class ShColor<2, SH_TEMP, ShHalf, true>;
template class ShColor<2, SH_TEMP, ShInterval<float> , false>;
template class ShColor<2, SH_TEMP, ShInterval<float> , true>;
template class ShColor<2, SH_TEMP, ShFracShort, false>;
template class ShColor<2, SH_TEMP, ShFracShort, true>;
template class ShColor<2, SH_TEMP, ShFracInt, false>;
template class ShColor<2, SH_TEMP, ShFracInt, true>;
template class ShColor<2, SH_TEMP, unsigned int, false>;
template class ShColor<2, SH_TEMP, unsigned int, true>;
template class ShColor<2, SH_CONST, ShInterval<double> , false>;
template class ShColor<2, SH_CONST, ShInterval<double> , true>;
template class ShColor<2, SH_CONST, ShFracUShort, false>;
template class ShColor<2, SH_CONST, ShFracUShort, true>;
template class ShColor<2, SH_CONST, short, false>;
template class ShColor<2, SH_CONST, short, true>;
template class ShColor<2, SH_CONST, ShFracUInt, false>;
template class ShColor<2, SH_CONST, ShFracUInt, true>;
template class ShColor<2, SH_CONST, ShFracByte, false>;
template class ShColor<2, SH_CONST, ShFracByte, true>;
template class ShColor<2, SH_CONST, int, false>;
template class ShColor<2, SH_CONST, int, true>;
template class ShColor<2, SH_CONST, double, false>;
template class ShColor<2, SH_CONST, double, true>;
template class ShColor<2, SH_CONST, unsigned char, false>;
template class ShColor<2, SH_CONST, unsigned char, true>;
template class ShColor<2, SH_CONST, float, false>;
template class ShColor<2, SH_CONST, float, true>;
template class ShColor<2, SH_CONST, char, false>;
template class ShColor<2, SH_CONST, char, true>;
template class ShColor<2, SH_CONST, unsigned short, false>;
template class ShColor<2, SH_CONST, unsigned short, true>;
template class ShColor<2, SH_CONST, ShFracUByte, false>;
template class ShColor<2, SH_CONST, ShFracUByte, true>;
template class ShColor<2, SH_CONST, ShHalf, false>;
template class ShColor<2, SH_CONST, ShHalf, true>;
template class ShColor<2, SH_CONST, ShInterval<float> , false>;
template class ShColor<2, SH_CONST, ShInterval<float> , true>;
template class ShColor<2, SH_CONST, ShFracShort, false>;
template class ShColor<2, SH_CONST, ShFracShort, true>;
template class ShColor<2, SH_CONST, ShFracInt, false>;
template class ShColor<2, SH_CONST, ShFracInt, true>;
template class ShColor<2, SH_CONST, unsigned int, false>;
template class ShColor<2, SH_CONST, unsigned int, true>;
template class ShColor<3, SH_INPUT, ShInterval<double> , false>;
template class ShColor<3, SH_INPUT, ShInterval<double> , true>;
template class ShColor<3, SH_INPUT, ShFracUShort, false>;
template class ShColor<3, SH_INPUT, ShFracUShort, true>;
template class ShColor<3, SH_INPUT, short, false>;
template class ShColor<3, SH_INPUT, short, true>;
template class ShColor<3, SH_INPUT, ShFracUInt, false>;
template class ShColor<3, SH_INPUT, ShFracUInt, true>;
template class ShColor<3, SH_INPUT, ShFracByte, false>;
template class ShColor<3, SH_INPUT, ShFracByte, true>;
template class ShColor<3, SH_INPUT, int, false>;
template class ShColor<3, SH_INPUT, int, true>;
template class ShColor<3, SH_INPUT, double, false>;
template class ShColor<3, SH_INPUT, double, true>;
template class ShColor<3, SH_INPUT, unsigned char, false>;
template class ShColor<3, SH_INPUT, unsigned char, true>;
template class ShColor<3, SH_INPUT, float, false>;
template class ShColor<3, SH_INPUT, float, true>;
template class ShColor<3, SH_INPUT, char, false>;
template class ShColor<3, SH_INPUT, char, true>;
template class ShColor<3, SH_INPUT, unsigned short, false>;
template class ShColor<3, SH_INPUT, unsigned short, true>;
template class ShColor<3, SH_INPUT, ShFracUByte, false>;
template class ShColor<3, SH_INPUT, ShFracUByte, true>;
template class ShColor<3, SH_INPUT, ShHalf, false>;
template class ShColor<3, SH_INPUT, ShHalf, true>;
template class ShColor<3, SH_INPUT, ShInterval<float> , false>;
template class ShColor<3, SH_INPUT, ShInterval<float> , true>;
template class ShColor<3, SH_INPUT, ShFracShort, false>;
template class ShColor<3, SH_INPUT, ShFracShort, true>;
template class ShColor<3, SH_INPUT, ShFracInt, false>;
template class ShColor<3, SH_INPUT, ShFracInt, true>;
template class ShColor<3, SH_INPUT, unsigned int, false>;
template class ShColor<3, SH_INPUT, unsigned int, true>;
template class ShColor<3, SH_OUTPUT, ShInterval<double> , false>;
template class ShColor<3, SH_OUTPUT, ShInterval<double> , true>;
template class ShColor<3, SH_OUTPUT, ShFracUShort, false>;
template class ShColor<3, SH_OUTPUT, ShFracUShort, true>;
template class ShColor<3, SH_OUTPUT, short, false>;
template class ShColor<3, SH_OUTPUT, short, true>;
template class ShColor<3, SH_OUTPUT, ShFracUInt, false>;
template class ShColor<3, SH_OUTPUT, ShFracUInt, true>;
template class ShColor<3, SH_OUTPUT, ShFracByte, false>;
template class ShColor<3, SH_OUTPUT, ShFracByte, true>;
template class ShColor<3, SH_OUTPUT, int, false>;
template class ShColor<3, SH_OUTPUT, int, true>;
template class ShColor<3, SH_OUTPUT, double, false>;
template class ShColor<3, SH_OUTPUT, double, true>;
template class ShColor<3, SH_OUTPUT, unsigned char, false>;
template class ShColor<3, SH_OUTPUT, unsigned char, true>;
template class ShColor<3, SH_OUTPUT, float, false>;
template class ShColor<3, SH_OUTPUT, float, true>;
template class ShColor<3, SH_OUTPUT, char, false>;
template class ShColor<3, SH_OUTPUT, char, true>;
template class ShColor<3, SH_OUTPUT, unsigned short, false>;
template class ShColor<3, SH_OUTPUT, unsigned short, true>;
template class ShColor<3, SH_OUTPUT, ShFracUByte, false>;
template class ShColor<3, SH_OUTPUT, ShFracUByte, true>;
template class ShColor<3, SH_OUTPUT, ShHalf, false>;
template class ShColor<3, SH_OUTPUT, ShHalf, true>;
template class ShColor<3, SH_OUTPUT, ShInterval<float> , false>;
template class ShColor<3, SH_OUTPUT, ShInterval<float> , true>;
template class ShColor<3, SH_OUTPUT, ShFracShort, false>;
template class ShColor<3, SH_OUTPUT, ShFracShort, true>;
template class ShColor<3, SH_OUTPUT, ShFracInt, false>;
template class ShColor<3, SH_OUTPUT, ShFracInt, true>;
template class ShColor<3, SH_OUTPUT, unsigned int, false>;
template class ShColor<3, SH_OUTPUT, unsigned int, true>;
template class ShColor<3, SH_INOUT, ShInterval<double> , false>;
template class ShColor<3, SH_INOUT, ShInterval<double> , true>;
template class ShColor<3, SH_INOUT, ShFracUShort, false>;
template class ShColor<3, SH_INOUT, ShFracUShort, true>;
template class ShColor<3, SH_INOUT, short, false>;
template class ShColor<3, SH_INOUT, short, true>;
template class ShColor<3, SH_INOUT, ShFracUInt, false>;
template class ShColor<3, SH_INOUT, ShFracUInt, true>;
template class ShColor<3, SH_INOUT, ShFracByte, false>;
template class ShColor<3, SH_INOUT, ShFracByte, true>;
template class ShColor<3, SH_INOUT, int, false>;
template class ShColor<3, SH_INOUT, int, true>;
template class ShColor<3, SH_INOUT, double, false>;
template class ShColor<3, SH_INOUT, double, true>;
template class ShColor<3, SH_INOUT, unsigned char, false>;
template class ShColor<3, SH_INOUT, unsigned char, true>;
template class ShColor<3, SH_INOUT, float, false>;
template class ShColor<3, SH_INOUT, float, true>;
template class ShColor<3, SH_INOUT, char, false>;
template class ShColor<3, SH_INOUT, char, true>;
template class ShColor<3, SH_INOUT, unsigned short, false>;
template class ShColor<3, SH_INOUT, unsigned short, true>;
template class ShColor<3, SH_INOUT, ShFracUByte, false>;
template class ShColor<3, SH_INOUT, ShFracUByte, true>;
template class ShColor<3, SH_INOUT, ShHalf, false>;
template class ShColor<3, SH_INOUT, ShHalf, true>;
template class ShColor<3, SH_INOUT, ShInterval<float> , false>;
template class ShColor<3, SH_INOUT, ShInterval<float> , true>;
template class ShColor<3, SH_INOUT, ShFracShort, false>;
template class ShColor<3, SH_INOUT, ShFracShort, true>;
template class ShColor<3, SH_INOUT, ShFracInt, false>;
template class ShColor<3, SH_INOUT, ShFracInt, true>;
template class ShColor<3, SH_INOUT, unsigned int, false>;
template class ShColor<3, SH_INOUT, unsigned int, true>;
template class ShColor<3, SH_TEMP, ShInterval<double> , false>;
template class ShColor<3, SH_TEMP, ShInterval<double> , true>;
template class ShColor<3, SH_TEMP, ShFracUShort, false>;
template class ShColor<3, SH_TEMP, ShFracUShort, true>;
template class ShColor<3, SH_TEMP, short, false>;
template class ShColor<3, SH_TEMP, short, true>;
template class ShColor<3, SH_TEMP, ShFracUInt, false>;
template class ShColor<3, SH_TEMP, ShFracUInt, true>;
template class ShColor<3, SH_TEMP, ShFracByte, false>;
template class ShColor<3, SH_TEMP, ShFracByte, true>;
template class ShColor<3, SH_TEMP, int, false>;
template class ShColor<3, SH_TEMP, int, true>;
template class ShColor<3, SH_TEMP, double, false>;
template class ShColor<3, SH_TEMP, double, true>;
template class ShColor<3, SH_TEMP, unsigned char, false>;
template class ShColor<3, SH_TEMP, unsigned char, true>;
template class ShColor<3, SH_TEMP, float, false>;
template class ShColor<3, SH_TEMP, float, true>;
template class ShColor<3, SH_TEMP, char, false>;
template class ShColor<3, SH_TEMP, char, true>;
template class ShColor<3, SH_TEMP, unsigned short, false>;
template class ShColor<3, SH_TEMP, unsigned short, true>;
template class ShColor<3, SH_TEMP, ShFracUByte, false>;
template class ShColor<3, SH_TEMP, ShFracUByte, true>;
template class ShColor<3, SH_TEMP, ShHalf, false>;
template class ShColor<3, SH_TEMP, ShHalf, true>;
template class ShColor<3, SH_TEMP, ShInterval<float> , false>;
template class ShColor<3, SH_TEMP, ShInterval<float> , true>;
template class ShColor<3, SH_TEMP, ShFracShort, false>;
template class ShColor<3, SH_TEMP, ShFracShort, true>;
template class ShColor<3, SH_TEMP, ShFracInt, false>;
template class ShColor<3, SH_TEMP, ShFracInt, true>;
template class ShColor<3, SH_TEMP, unsigned int, false>;
template class ShColor<3, SH_TEMP, unsigned int, true>;
template class ShColor<3, SH_CONST, ShInterval<double> , false>;
template class ShColor<3, SH_CONST, ShInterval<double> , true>;
template class ShColor<3, SH_CONST, ShFracUShort, false>;
template class ShColor<3, SH_CONST, ShFracUShort, true>;
template class ShColor<3, SH_CONST, short, false>;
template class ShColor<3, SH_CONST, short, true>;
template class ShColor<3, SH_CONST, ShFracUInt, false>;
template class ShColor<3, SH_CONST, ShFracUInt, true>;
template class ShColor<3, SH_CONST, ShFracByte, false>;
template class ShColor<3, SH_CONST, ShFracByte, true>;
template class ShColor<3, SH_CONST, int, false>;
template class ShColor<3, SH_CONST, int, true>;
template class ShColor<3, SH_CONST, double, false>;
template class ShColor<3, SH_CONST, double, true>;
template class ShColor<3, SH_CONST, unsigned char, false>;
template class ShColor<3, SH_CONST, unsigned char, true>;
template class ShColor<3, SH_CONST, float, false>;
template class ShColor<3, SH_CONST, float, true>;
template class ShColor<3, SH_CONST, char, false>;
template class ShColor<3, SH_CONST, char, true>;
template class ShColor<3, SH_CONST, unsigned short, false>;
template class ShColor<3, SH_CONST, unsigned short, true>;
template class ShColor<3, SH_CONST, ShFracUByte, false>;
template class ShColor<3, SH_CONST, ShFracUByte, true>;
template class ShColor<3, SH_CONST, ShHalf, false>;
template class ShColor<3, SH_CONST, ShHalf, true>;
template class ShColor<3, SH_CONST, ShInterval<float> , false>;
template class ShColor<3, SH_CONST, ShInterval<float> , true>;
template class ShColor<3, SH_CONST, ShFracShort, false>;
template class ShColor<3, SH_CONST, ShFracShort, true>;
template class ShColor<3, SH_CONST, ShFracInt, false>;
template class ShColor<3, SH_CONST, ShFracInt, true>;
template class ShColor<3, SH_CONST, unsigned int, false>;
template class ShColor<3, SH_CONST, unsigned int, true>;
template class ShColor<4, SH_INPUT, ShInterval<double> , false>;
template class ShColor<4, SH_INPUT, ShInterval<double> , true>;
template class ShColor<4, SH_INPUT, ShFracUShort, false>;
template class ShColor<4, SH_INPUT, ShFracUShort, true>;
template class ShColor<4, SH_INPUT, short, false>;
template class ShColor<4, SH_INPUT, short, true>;
template class ShColor<4, SH_INPUT, ShFracUInt, false>;
template class ShColor<4, SH_INPUT, ShFracUInt, true>;
template class ShColor<4, SH_INPUT, ShFracByte, false>;
template class ShColor<4, SH_INPUT, ShFracByte, true>;
template class ShColor<4, SH_INPUT, int, false>;
template class ShColor<4, SH_INPUT, int, true>;
template class ShColor<4, SH_INPUT, double, false>;
template class ShColor<4, SH_INPUT, double, true>;
template class ShColor<4, SH_INPUT, unsigned char, false>;
template class ShColor<4, SH_INPUT, unsigned char, true>;
template class ShColor<4, SH_INPUT, float, false>;
template class ShColor<4, SH_INPUT, float, true>;
template class ShColor<4, SH_INPUT, char, false>;
template class ShColor<4, SH_INPUT, char, true>;
template class ShColor<4, SH_INPUT, unsigned short, false>;
template class ShColor<4, SH_INPUT, unsigned short, true>;
template class ShColor<4, SH_INPUT, ShFracUByte, false>;
template class ShColor<4, SH_INPUT, ShFracUByte, true>;
template class ShColor<4, SH_INPUT, ShHalf, false>;
template class ShColor<4, SH_INPUT, ShHalf, true>;
template class ShColor<4, SH_INPUT, ShInterval<float> , false>;
template class ShColor<4, SH_INPUT, ShInterval<float> , true>;
template class ShColor<4, SH_INPUT, ShFracShort, false>;
template class ShColor<4, SH_INPUT, ShFracShort, true>;
template class ShColor<4, SH_INPUT, ShFracInt, false>;
template class ShColor<4, SH_INPUT, ShFracInt, true>;
template class ShColor<4, SH_INPUT, unsigned int, false>;
template class ShColor<4, SH_INPUT, unsigned int, true>;
template class ShColor<4, SH_OUTPUT, ShInterval<double> , false>;
template class ShColor<4, SH_OUTPUT, ShInterval<double> , true>;
template class ShColor<4, SH_OUTPUT, ShFracUShort, false>;
template class ShColor<4, SH_OUTPUT, ShFracUShort, true>;
template class ShColor<4, SH_OUTPUT, short, false>;
template class ShColor<4, SH_OUTPUT, short, true>;
template class ShColor<4, SH_OUTPUT, ShFracUInt, false>;
template class ShColor<4, SH_OUTPUT, ShFracUInt, true>;
template class ShColor<4, SH_OUTPUT, ShFracByte, false>;
template class ShColor<4, SH_OUTPUT, ShFracByte, true>;
template class ShColor<4, SH_OUTPUT, int, false>;
template class ShColor<4, SH_OUTPUT, int, true>;
template class ShColor<4, SH_OUTPUT, double, false>;
template class ShColor<4, SH_OUTPUT, double, true>;
template class ShColor<4, SH_OUTPUT, unsigned char, false>;
template class ShColor<4, SH_OUTPUT, unsigned char, true>;
template class ShColor<4, SH_OUTPUT, float, false>;
template class ShColor<4, SH_OUTPUT, float, true>;
template class ShColor<4, SH_OUTPUT, char, false>;
template class ShColor<4, SH_OUTPUT, char, true>;
template class ShColor<4, SH_OUTPUT, unsigned short, false>;
template class ShColor<4, SH_OUTPUT, unsigned short, true>;
template class ShColor<4, SH_OUTPUT, ShFracUByte, false>;
template class ShColor<4, SH_OUTPUT, ShFracUByte, true>;
template class ShColor<4, SH_OUTPUT, ShHalf, false>;
template class ShColor<4, SH_OUTPUT, ShHalf, true>;
template class ShColor<4, SH_OUTPUT, ShInterval<float> , false>;
template class ShColor<4, SH_OUTPUT, ShInterval<float> , true>;
template class ShColor<4, SH_OUTPUT, ShFracShort, false>;
template class ShColor<4, SH_OUTPUT, ShFracShort, true>;
template class ShColor<4, SH_OUTPUT, ShFracInt, false>;
template class ShColor<4, SH_OUTPUT, ShFracInt, true>;
template class ShColor<4, SH_OUTPUT, unsigned int, false>;
template class ShColor<4, SH_OUTPUT, unsigned int, true>;
template class ShColor<4, SH_INOUT, ShInterval<double> , false>;
template class ShColor<4, SH_INOUT, ShInterval<double> , true>;
template class ShColor<4, SH_INOUT, ShFracUShort, false>;
template class ShColor<4, SH_INOUT, ShFracUShort, true>;
template class ShColor<4, SH_INOUT, short, false>;
template class ShColor<4, SH_INOUT, short, true>;
template class ShColor<4, SH_INOUT, ShFracUInt, false>;
template class ShColor<4, SH_INOUT, ShFracUInt, true>;
template class ShColor<4, SH_INOUT, ShFracByte, false>;
template class ShColor<4, SH_INOUT, ShFracByte, true>;
template class ShColor<4, SH_INOUT, int, false>;
template class ShColor<4, SH_INOUT, int, true>;
template class ShColor<4, SH_INOUT, double, false>;
template class ShColor<4, SH_INOUT, double, true>;
template class ShColor<4, SH_INOUT, unsigned char, false>;
template class ShColor<4, SH_INOUT, unsigned char, true>;
template class ShColor<4, SH_INOUT, float, false>;
template class ShColor<4, SH_INOUT, float, true>;
template class ShColor<4, SH_INOUT, char, false>;
template class ShColor<4, SH_INOUT, char, true>;
template class ShColor<4, SH_INOUT, unsigned short, false>;
template class ShColor<4, SH_INOUT, unsigned short, true>;
template class ShColor<4, SH_INOUT, ShFracUByte, false>;
template class ShColor<4, SH_INOUT, ShFracUByte, true>;
template class ShColor<4, SH_INOUT, ShHalf, false>;
template class ShColor<4, SH_INOUT, ShHalf, true>;
template class ShColor<4, SH_INOUT, ShInterval<float> , false>;
template class ShColor<4, SH_INOUT, ShInterval<float> , true>;
template class ShColor<4, SH_INOUT, ShFracShort, false>;
template class ShColor<4, SH_INOUT, ShFracShort, true>;
template class ShColor<4, SH_INOUT, ShFracInt, false>;
template class ShColor<4, SH_INOUT, ShFracInt, true>;
template class ShColor<4, SH_INOUT, unsigned int, false>;
template class ShColor<4, SH_INOUT, unsigned int, true>;
template class ShColor<4, SH_TEMP, ShInterval<double> , false>;
template class ShColor<4, SH_TEMP, ShInterval<double> , true>;
template class ShColor<4, SH_TEMP, ShFracUShort, false>;
template class ShColor<4, SH_TEMP, ShFracUShort, true>;
template class ShColor<4, SH_TEMP, short, false>;
template class ShColor<4, SH_TEMP, short, true>;
template class ShColor<4, SH_TEMP, ShFracUInt, false>;
template class ShColor<4, SH_TEMP, ShFracUInt, true>;
template class ShColor<4, SH_TEMP, ShFracByte, false>;
template class ShColor<4, SH_TEMP, ShFracByte, true>;
template class ShColor<4, SH_TEMP, int, false>;
template class ShColor<4, SH_TEMP, int, true>;
template class ShColor<4, SH_TEMP, double, false>;
template class ShColor<4, SH_TEMP, double, true>;
template class ShColor<4, SH_TEMP, unsigned char, false>;
template class ShColor<4, SH_TEMP, unsigned char, true>;
template class ShColor<4, SH_TEMP, float, false>;
template class ShColor<4, SH_TEMP, float, true>;
template class ShColor<4, SH_TEMP, char, false>;
template class ShColor<4, SH_TEMP, char, true>;
template class ShColor<4, SH_TEMP, unsigned short, false>;
template class ShColor<4, SH_TEMP, unsigned short, true>;
template class ShColor<4, SH_TEMP, ShFracUByte, false>;
template class ShColor<4, SH_TEMP, ShFracUByte, true>;
template class ShColor<4, SH_TEMP, ShHalf, false>;
template class ShColor<4, SH_TEMP, ShHalf, true>;
template class ShColor<4, SH_TEMP, ShInterval<float> , false>;
template class ShColor<4, SH_TEMP, ShInterval<float> , true>;
template class ShColor<4, SH_TEMP, ShFracShort, false>;
template class ShColor<4, SH_TEMP, ShFracShort, true>;
template class ShColor<4, SH_TEMP, ShFracInt, false>;
template class ShColor<4, SH_TEMP, ShFracInt, true>;
template class ShColor<4, SH_TEMP, unsigned int, false>;
template class ShColor<4, SH_TEMP, unsigned int, true>;
template class ShColor<4, SH_CONST, ShInterval<double> , false>;
template class ShColor<4, SH_CONST, ShInterval<double> , true>;
template class ShColor<4, SH_CONST, ShFracUShort, false>;
template class ShColor<4, SH_CONST, ShFracUShort, true>;
template class ShColor<4, SH_CONST, short, false>;
template class ShColor<4, SH_CONST, short, true>;
template class ShColor<4, SH_CONST, ShFracUInt, false>;
template class ShColor<4, SH_CONST, ShFracUInt, true>;
template class ShColor<4, SH_CONST, ShFracByte, false>;
template class ShColor<4, SH_CONST, ShFracByte, true>;
template class ShColor<4, SH_CONST, int, false>;
template class ShColor<4, SH_CONST, int, true>;
template class ShColor<4, SH_CONST, double, false>;
template class ShColor<4, SH_CONST, double, true>;
template class ShColor<4, SH_CONST, unsigned char, false>;
template class ShColor<4, SH_CONST, unsigned char, true>;
template class ShColor<4, SH_CONST, float, false>;
template class ShColor<4, SH_CONST, float, true>;
template class ShColor<4, SH_CONST, char, false>;
template class ShColor<4, SH_CONST, char, true>;
template class ShColor<4, SH_CONST, unsigned short, false>;
template class ShColor<4, SH_CONST, unsigned short, true>;
template class ShColor<4, SH_CONST, ShFracUByte, false>;
template class ShColor<4, SH_CONST, ShFracUByte, true>;
template class ShColor<4, SH_CONST, ShHalf, false>;
template class ShColor<4, SH_CONST, ShHalf, true>;
template class ShColor<4, SH_CONST, ShInterval<float> , false>;
template class ShColor<4, SH_CONST, ShInterval<float> , true>;
template class ShColor<4, SH_CONST, ShFracShort, false>;
template class ShColor<4, SH_CONST, ShFracShort, true>;
template class ShColor<4, SH_CONST, ShFracInt, false>;
template class ShColor<4, SH_CONST, ShFracInt, true>;
template class ShColor<4, SH_CONST, unsigned int, false>;
template class ShColor<4, SH_CONST, unsigned int, true>;

} // namespace SH
