// WARNING - DO NOT EDIT THIS FILE:
// This file was automatically generated from scripts/Normal.hpp.py.
// If you wish to change it, edit that file instead.
//
// ---
//
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

#ifndef SHNORMAL_HPP
#define SHNORMAL_HPP

#include "Vector.hpp"
namespace SH {

/** An n-normal.
 * 
 *
 */

typedef Attrib<1, SH_INPUT, short, SH_NORMAL> InputNormal1s;
typedef Attrib<1, SH_OUTPUT, short, SH_NORMAL> OutputNormal1s;
typedef Attrib<1, SH_INOUT, short, SH_NORMAL> InOutNormal1s;
typedef Attrib<1, SH_TEMP, short, SH_NORMAL>  Normal1s;
typedef Attrib<1, SH_CONST, short, SH_NORMAL> ConstNormal1s;
typedef Attrib<2, SH_INPUT, short, SH_NORMAL> InputNormal2s;
typedef Attrib<2, SH_OUTPUT, short, SH_NORMAL> OutputNormal2s;
typedef Attrib<2, SH_INOUT, short, SH_NORMAL> InOutNormal2s;
typedef Attrib<2, SH_TEMP, short, SH_NORMAL>  Normal2s;
typedef Attrib<2, SH_CONST, short, SH_NORMAL> ConstNormal2s;
typedef Attrib<3, SH_INPUT, short, SH_NORMAL> InputNormal3s;
typedef Attrib<3, SH_OUTPUT, short, SH_NORMAL> OutputNormal3s;
typedef Attrib<3, SH_INOUT, short, SH_NORMAL> InOutNormal3s;
typedef Attrib<3, SH_TEMP, short, SH_NORMAL>  Normal3s;
typedef Attrib<3, SH_CONST, short, SH_NORMAL> ConstNormal3s;
typedef Attrib<4, SH_INPUT, short, SH_NORMAL> InputNormal4s;
typedef Attrib<4, SH_OUTPUT, short, SH_NORMAL> OutputNormal4s;
typedef Attrib<4, SH_INOUT, short, SH_NORMAL> InOutNormal4s;
typedef Attrib<4, SH_TEMP, short, SH_NORMAL>  Normal4s;
typedef Attrib<4, SH_CONST, short, SH_NORMAL> ConstNormal4s;


typedef Attrib<1, SH_INPUT, FracUByte, SH_NORMAL> InputNormal1fub;
typedef Attrib<1, SH_OUTPUT, FracUByte, SH_NORMAL> OutputNormal1fub;
typedef Attrib<1, SH_INOUT, FracUByte, SH_NORMAL> InOutNormal1fub;
typedef Attrib<1, SH_TEMP, FracUByte, SH_NORMAL>  Normal1fub;
typedef Attrib<1, SH_CONST, FracUByte, SH_NORMAL> ConstNormal1fub;
typedef Attrib<2, SH_INPUT, FracUByte, SH_NORMAL> InputNormal2fub;
typedef Attrib<2, SH_OUTPUT, FracUByte, SH_NORMAL> OutputNormal2fub;
typedef Attrib<2, SH_INOUT, FracUByte, SH_NORMAL> InOutNormal2fub;
typedef Attrib<2, SH_TEMP, FracUByte, SH_NORMAL>  Normal2fub;
typedef Attrib<2, SH_CONST, FracUByte, SH_NORMAL> ConstNormal2fub;
typedef Attrib<3, SH_INPUT, FracUByte, SH_NORMAL> InputNormal3fub;
typedef Attrib<3, SH_OUTPUT, FracUByte, SH_NORMAL> OutputNormal3fub;
typedef Attrib<3, SH_INOUT, FracUByte, SH_NORMAL> InOutNormal3fub;
typedef Attrib<3, SH_TEMP, FracUByte, SH_NORMAL>  Normal3fub;
typedef Attrib<3, SH_CONST, FracUByte, SH_NORMAL> ConstNormal3fub;
typedef Attrib<4, SH_INPUT, FracUByte, SH_NORMAL> InputNormal4fub;
typedef Attrib<4, SH_OUTPUT, FracUByte, SH_NORMAL> OutputNormal4fub;
typedef Attrib<4, SH_INOUT, FracUByte, SH_NORMAL> InOutNormal4fub;
typedef Attrib<4, SH_TEMP, FracUByte, SH_NORMAL>  Normal4fub;
typedef Attrib<4, SH_CONST, FracUByte, SH_NORMAL> ConstNormal4fub;


typedef Attrib<1, SH_INPUT, FracUShort, SH_NORMAL> InputNormal1fus;
typedef Attrib<1, SH_OUTPUT, FracUShort, SH_NORMAL> OutputNormal1fus;
typedef Attrib<1, SH_INOUT, FracUShort, SH_NORMAL> InOutNormal1fus;
typedef Attrib<1, SH_TEMP, FracUShort, SH_NORMAL>  Normal1fus;
typedef Attrib<1, SH_CONST, FracUShort, SH_NORMAL> ConstNormal1fus;
typedef Attrib<2, SH_INPUT, FracUShort, SH_NORMAL> InputNormal2fus;
typedef Attrib<2, SH_OUTPUT, FracUShort, SH_NORMAL> OutputNormal2fus;
typedef Attrib<2, SH_INOUT, FracUShort, SH_NORMAL> InOutNormal2fus;
typedef Attrib<2, SH_TEMP, FracUShort, SH_NORMAL>  Normal2fus;
typedef Attrib<2, SH_CONST, FracUShort, SH_NORMAL> ConstNormal2fus;
typedef Attrib<3, SH_INPUT, FracUShort, SH_NORMAL> InputNormal3fus;
typedef Attrib<3, SH_OUTPUT, FracUShort, SH_NORMAL> OutputNormal3fus;
typedef Attrib<3, SH_INOUT, FracUShort, SH_NORMAL> InOutNormal3fus;
typedef Attrib<3, SH_TEMP, FracUShort, SH_NORMAL>  Normal3fus;
typedef Attrib<3, SH_CONST, FracUShort, SH_NORMAL> ConstNormal3fus;
typedef Attrib<4, SH_INPUT, FracUShort, SH_NORMAL> InputNormal4fus;
typedef Attrib<4, SH_OUTPUT, FracUShort, SH_NORMAL> OutputNormal4fus;
typedef Attrib<4, SH_INOUT, FracUShort, SH_NORMAL> InOutNormal4fus;
typedef Attrib<4, SH_TEMP, FracUShort, SH_NORMAL>  Normal4fus;
typedef Attrib<4, SH_CONST, FracUShort, SH_NORMAL> ConstNormal4fus;


typedef Attrib<1, SH_INPUT, int, SH_NORMAL> InputNormal1i;
typedef Attrib<1, SH_OUTPUT, int, SH_NORMAL> OutputNormal1i;
typedef Attrib<1, SH_INOUT, int, SH_NORMAL> InOutNormal1i;
typedef Attrib<1, SH_TEMP, int, SH_NORMAL>  Normal1i;
typedef Attrib<1, SH_CONST, int, SH_NORMAL> ConstNormal1i;
typedef Attrib<2, SH_INPUT, int, SH_NORMAL> InputNormal2i;
typedef Attrib<2, SH_OUTPUT, int, SH_NORMAL> OutputNormal2i;
typedef Attrib<2, SH_INOUT, int, SH_NORMAL> InOutNormal2i;
typedef Attrib<2, SH_TEMP, int, SH_NORMAL>  Normal2i;
typedef Attrib<2, SH_CONST, int, SH_NORMAL> ConstNormal2i;
typedef Attrib<3, SH_INPUT, int, SH_NORMAL> InputNormal3i;
typedef Attrib<3, SH_OUTPUT, int, SH_NORMAL> OutputNormal3i;
typedef Attrib<3, SH_INOUT, int, SH_NORMAL> InOutNormal3i;
typedef Attrib<3, SH_TEMP, int, SH_NORMAL>  Normal3i;
typedef Attrib<3, SH_CONST, int, SH_NORMAL> ConstNormal3i;
typedef Attrib<4, SH_INPUT, int, SH_NORMAL> InputNormal4i;
typedef Attrib<4, SH_OUTPUT, int, SH_NORMAL> OutputNormal4i;
typedef Attrib<4, SH_INOUT, int, SH_NORMAL> InOutNormal4i;
typedef Attrib<4, SH_TEMP, int, SH_NORMAL>  Normal4i;
typedef Attrib<4, SH_CONST, int, SH_NORMAL> ConstNormal4i;


typedef Attrib<1, SH_INPUT, double, SH_NORMAL> InputNormal1d;
typedef Attrib<1, SH_OUTPUT, double, SH_NORMAL> OutputNormal1d;
typedef Attrib<1, SH_INOUT, double, SH_NORMAL> InOutNormal1d;
typedef Attrib<1, SH_TEMP, double, SH_NORMAL>  Normal1d;
typedef Attrib<1, SH_CONST, double, SH_NORMAL> ConstNormal1d;
typedef Attrib<2, SH_INPUT, double, SH_NORMAL> InputNormal2d;
typedef Attrib<2, SH_OUTPUT, double, SH_NORMAL> OutputNormal2d;
typedef Attrib<2, SH_INOUT, double, SH_NORMAL> InOutNormal2d;
typedef Attrib<2, SH_TEMP, double, SH_NORMAL>  Normal2d;
typedef Attrib<2, SH_CONST, double, SH_NORMAL> ConstNormal2d;
typedef Attrib<3, SH_INPUT, double, SH_NORMAL> InputNormal3d;
typedef Attrib<3, SH_OUTPUT, double, SH_NORMAL> OutputNormal3d;
typedef Attrib<3, SH_INOUT, double, SH_NORMAL> InOutNormal3d;
typedef Attrib<3, SH_TEMP, double, SH_NORMAL>  Normal3d;
typedef Attrib<3, SH_CONST, double, SH_NORMAL> ConstNormal3d;
typedef Attrib<4, SH_INPUT, double, SH_NORMAL> InputNormal4d;
typedef Attrib<4, SH_OUTPUT, double, SH_NORMAL> OutputNormal4d;
typedef Attrib<4, SH_INOUT, double, SH_NORMAL> InOutNormal4d;
typedef Attrib<4, SH_TEMP, double, SH_NORMAL>  Normal4d;
typedef Attrib<4, SH_CONST, double, SH_NORMAL> ConstNormal4d;


typedef Attrib<1, SH_INPUT, unsigned char, SH_NORMAL> InputNormal1ub;
typedef Attrib<1, SH_OUTPUT, unsigned char, SH_NORMAL> OutputNormal1ub;
typedef Attrib<1, SH_INOUT, unsigned char, SH_NORMAL> InOutNormal1ub;
typedef Attrib<1, SH_TEMP, unsigned char, SH_NORMAL>  Normal1ub;
typedef Attrib<1, SH_CONST, unsigned char, SH_NORMAL> ConstNormal1ub;
typedef Attrib<2, SH_INPUT, unsigned char, SH_NORMAL> InputNormal2ub;
typedef Attrib<2, SH_OUTPUT, unsigned char, SH_NORMAL> OutputNormal2ub;
typedef Attrib<2, SH_INOUT, unsigned char, SH_NORMAL> InOutNormal2ub;
typedef Attrib<2, SH_TEMP, unsigned char, SH_NORMAL>  Normal2ub;
typedef Attrib<2, SH_CONST, unsigned char, SH_NORMAL> ConstNormal2ub;
typedef Attrib<3, SH_INPUT, unsigned char, SH_NORMAL> InputNormal3ub;
typedef Attrib<3, SH_OUTPUT, unsigned char, SH_NORMAL> OutputNormal3ub;
typedef Attrib<3, SH_INOUT, unsigned char, SH_NORMAL> InOutNormal3ub;
typedef Attrib<3, SH_TEMP, unsigned char, SH_NORMAL>  Normal3ub;
typedef Attrib<3, SH_CONST, unsigned char, SH_NORMAL> ConstNormal3ub;
typedef Attrib<4, SH_INPUT, unsigned char, SH_NORMAL> InputNormal4ub;
typedef Attrib<4, SH_OUTPUT, unsigned char, SH_NORMAL> OutputNormal4ub;
typedef Attrib<4, SH_INOUT, unsigned char, SH_NORMAL> InOutNormal4ub;
typedef Attrib<4, SH_TEMP, unsigned char, SH_NORMAL>  Normal4ub;
typedef Attrib<4, SH_CONST, unsigned char, SH_NORMAL> ConstNormal4ub;


typedef Attrib<1, SH_INPUT, float, SH_NORMAL> InputNormal1f;
typedef Attrib<1, SH_OUTPUT, float, SH_NORMAL> OutputNormal1f;
typedef Attrib<1, SH_INOUT, float, SH_NORMAL> InOutNormal1f;
typedef Attrib<1, SH_TEMP, float, SH_NORMAL>  Normal1f;
typedef Attrib<1, SH_CONST, float, SH_NORMAL> ConstNormal1f;
typedef Attrib<2, SH_INPUT, float, SH_NORMAL> InputNormal2f;
typedef Attrib<2, SH_OUTPUT, float, SH_NORMAL> OutputNormal2f;
typedef Attrib<2, SH_INOUT, float, SH_NORMAL> InOutNormal2f;
typedef Attrib<2, SH_TEMP, float, SH_NORMAL>  Normal2f;
typedef Attrib<2, SH_CONST, float, SH_NORMAL> ConstNormal2f;
typedef Attrib<3, SH_INPUT, float, SH_NORMAL> InputNormal3f;
typedef Attrib<3, SH_OUTPUT, float, SH_NORMAL> OutputNormal3f;
typedef Attrib<3, SH_INOUT, float, SH_NORMAL> InOutNormal3f;
typedef Attrib<3, SH_TEMP, float, SH_NORMAL>  Normal3f;
typedef Attrib<3, SH_CONST, float, SH_NORMAL> ConstNormal3f;
typedef Attrib<4, SH_INPUT, float, SH_NORMAL> InputNormal4f;
typedef Attrib<4, SH_OUTPUT, float, SH_NORMAL> OutputNormal4f;
typedef Attrib<4, SH_INOUT, float, SH_NORMAL> InOutNormal4f;
typedef Attrib<4, SH_TEMP, float, SH_NORMAL>  Normal4f;
typedef Attrib<4, SH_CONST, float, SH_NORMAL> ConstNormal4f;


typedef Attrib<1, SH_INPUT, FracInt, SH_NORMAL> InputNormal1fi;
typedef Attrib<1, SH_OUTPUT, FracInt, SH_NORMAL> OutputNormal1fi;
typedef Attrib<1, SH_INOUT, FracInt, SH_NORMAL> InOutNormal1fi;
typedef Attrib<1, SH_TEMP, FracInt, SH_NORMAL>  Normal1fi;
typedef Attrib<1, SH_CONST, FracInt, SH_NORMAL> ConstNormal1fi;
typedef Attrib<2, SH_INPUT, FracInt, SH_NORMAL> InputNormal2fi;
typedef Attrib<2, SH_OUTPUT, FracInt, SH_NORMAL> OutputNormal2fi;
typedef Attrib<2, SH_INOUT, FracInt, SH_NORMAL> InOutNormal2fi;
typedef Attrib<2, SH_TEMP, FracInt, SH_NORMAL>  Normal2fi;
typedef Attrib<2, SH_CONST, FracInt, SH_NORMAL> ConstNormal2fi;
typedef Attrib<3, SH_INPUT, FracInt, SH_NORMAL> InputNormal3fi;
typedef Attrib<3, SH_OUTPUT, FracInt, SH_NORMAL> OutputNormal3fi;
typedef Attrib<3, SH_INOUT, FracInt, SH_NORMAL> InOutNormal3fi;
typedef Attrib<3, SH_TEMP, FracInt, SH_NORMAL>  Normal3fi;
typedef Attrib<3, SH_CONST, FracInt, SH_NORMAL> ConstNormal3fi;
typedef Attrib<4, SH_INPUT, FracInt, SH_NORMAL> InputNormal4fi;
typedef Attrib<4, SH_OUTPUT, FracInt, SH_NORMAL> OutputNormal4fi;
typedef Attrib<4, SH_INOUT, FracInt, SH_NORMAL> InOutNormal4fi;
typedef Attrib<4, SH_TEMP, FracInt, SH_NORMAL>  Normal4fi;
typedef Attrib<4, SH_CONST, FracInt, SH_NORMAL> ConstNormal4fi;


typedef Attrib<1, SH_INPUT, FracShort, SH_NORMAL> InputNormal1fs;
typedef Attrib<1, SH_OUTPUT, FracShort, SH_NORMAL> OutputNormal1fs;
typedef Attrib<1, SH_INOUT, FracShort, SH_NORMAL> InOutNormal1fs;
typedef Attrib<1, SH_TEMP, FracShort, SH_NORMAL>  Normal1fs;
typedef Attrib<1, SH_CONST, FracShort, SH_NORMAL> ConstNormal1fs;
typedef Attrib<2, SH_INPUT, FracShort, SH_NORMAL> InputNormal2fs;
typedef Attrib<2, SH_OUTPUT, FracShort, SH_NORMAL> OutputNormal2fs;
typedef Attrib<2, SH_INOUT, FracShort, SH_NORMAL> InOutNormal2fs;
typedef Attrib<2, SH_TEMP, FracShort, SH_NORMAL>  Normal2fs;
typedef Attrib<2, SH_CONST, FracShort, SH_NORMAL> ConstNormal2fs;
typedef Attrib<3, SH_INPUT, FracShort, SH_NORMAL> InputNormal3fs;
typedef Attrib<3, SH_OUTPUT, FracShort, SH_NORMAL> OutputNormal3fs;
typedef Attrib<3, SH_INOUT, FracShort, SH_NORMAL> InOutNormal3fs;
typedef Attrib<3, SH_TEMP, FracShort, SH_NORMAL>  Normal3fs;
typedef Attrib<3, SH_CONST, FracShort, SH_NORMAL> ConstNormal3fs;
typedef Attrib<4, SH_INPUT, FracShort, SH_NORMAL> InputNormal4fs;
typedef Attrib<4, SH_OUTPUT, FracShort, SH_NORMAL> OutputNormal4fs;
typedef Attrib<4, SH_INOUT, FracShort, SH_NORMAL> InOutNormal4fs;
typedef Attrib<4, SH_TEMP, FracShort, SH_NORMAL>  Normal4fs;
typedef Attrib<4, SH_CONST, FracShort, SH_NORMAL> ConstNormal4fs;


typedef Attrib<1, SH_INPUT, char, SH_NORMAL> InputNormal1b;
typedef Attrib<1, SH_OUTPUT, char, SH_NORMAL> OutputNormal1b;
typedef Attrib<1, SH_INOUT, char, SH_NORMAL> InOutNormal1b;
typedef Attrib<1, SH_TEMP, char, SH_NORMAL>  Normal1b;
typedef Attrib<1, SH_CONST, char, SH_NORMAL> ConstNormal1b;
typedef Attrib<2, SH_INPUT, char, SH_NORMAL> InputNormal2b;
typedef Attrib<2, SH_OUTPUT, char, SH_NORMAL> OutputNormal2b;
typedef Attrib<2, SH_INOUT, char, SH_NORMAL> InOutNormal2b;
typedef Attrib<2, SH_TEMP, char, SH_NORMAL>  Normal2b;
typedef Attrib<2, SH_CONST, char, SH_NORMAL> ConstNormal2b;
typedef Attrib<3, SH_INPUT, char, SH_NORMAL> InputNormal3b;
typedef Attrib<3, SH_OUTPUT, char, SH_NORMAL> OutputNormal3b;
typedef Attrib<3, SH_INOUT, char, SH_NORMAL> InOutNormal3b;
typedef Attrib<3, SH_TEMP, char, SH_NORMAL>  Normal3b;
typedef Attrib<3, SH_CONST, char, SH_NORMAL> ConstNormal3b;
typedef Attrib<4, SH_INPUT, char, SH_NORMAL> InputNormal4b;
typedef Attrib<4, SH_OUTPUT, char, SH_NORMAL> OutputNormal4b;
typedef Attrib<4, SH_INOUT, char, SH_NORMAL> InOutNormal4b;
typedef Attrib<4, SH_TEMP, char, SH_NORMAL>  Normal4b;
typedef Attrib<4, SH_CONST, char, SH_NORMAL> ConstNormal4b;


typedef Attrib<1, SH_INPUT, unsigned short, SH_NORMAL> InputNormal1us;
typedef Attrib<1, SH_OUTPUT, unsigned short, SH_NORMAL> OutputNormal1us;
typedef Attrib<1, SH_INOUT, unsigned short, SH_NORMAL> InOutNormal1us;
typedef Attrib<1, SH_TEMP, unsigned short, SH_NORMAL>  Normal1us;
typedef Attrib<1, SH_CONST, unsigned short, SH_NORMAL> ConstNormal1us;
typedef Attrib<2, SH_INPUT, unsigned short, SH_NORMAL> InputNormal2us;
typedef Attrib<2, SH_OUTPUT, unsigned short, SH_NORMAL> OutputNormal2us;
typedef Attrib<2, SH_INOUT, unsigned short, SH_NORMAL> InOutNormal2us;
typedef Attrib<2, SH_TEMP, unsigned short, SH_NORMAL>  Normal2us;
typedef Attrib<2, SH_CONST, unsigned short, SH_NORMAL> ConstNormal2us;
typedef Attrib<3, SH_INPUT, unsigned short, SH_NORMAL> InputNormal3us;
typedef Attrib<3, SH_OUTPUT, unsigned short, SH_NORMAL> OutputNormal3us;
typedef Attrib<3, SH_INOUT, unsigned short, SH_NORMAL> InOutNormal3us;
typedef Attrib<3, SH_TEMP, unsigned short, SH_NORMAL>  Normal3us;
typedef Attrib<3, SH_CONST, unsigned short, SH_NORMAL> ConstNormal3us;
typedef Attrib<4, SH_INPUT, unsigned short, SH_NORMAL> InputNormal4us;
typedef Attrib<4, SH_OUTPUT, unsigned short, SH_NORMAL> OutputNormal4us;
typedef Attrib<4, SH_INOUT, unsigned short, SH_NORMAL> InOutNormal4us;
typedef Attrib<4, SH_TEMP, unsigned short, SH_NORMAL>  Normal4us;
typedef Attrib<4, SH_CONST, unsigned short, SH_NORMAL> ConstNormal4us;


typedef Attrib<1, SH_INPUT, FracByte, SH_NORMAL> InputNormal1fb;
typedef Attrib<1, SH_OUTPUT, FracByte, SH_NORMAL> OutputNormal1fb;
typedef Attrib<1, SH_INOUT, FracByte, SH_NORMAL> InOutNormal1fb;
typedef Attrib<1, SH_TEMP, FracByte, SH_NORMAL>  Normal1fb;
typedef Attrib<1, SH_CONST, FracByte, SH_NORMAL> ConstNormal1fb;
typedef Attrib<2, SH_INPUT, FracByte, SH_NORMAL> InputNormal2fb;
typedef Attrib<2, SH_OUTPUT, FracByte, SH_NORMAL> OutputNormal2fb;
typedef Attrib<2, SH_INOUT, FracByte, SH_NORMAL> InOutNormal2fb;
typedef Attrib<2, SH_TEMP, FracByte, SH_NORMAL>  Normal2fb;
typedef Attrib<2, SH_CONST, FracByte, SH_NORMAL> ConstNormal2fb;
typedef Attrib<3, SH_INPUT, FracByte, SH_NORMAL> InputNormal3fb;
typedef Attrib<3, SH_OUTPUT, FracByte, SH_NORMAL> OutputNormal3fb;
typedef Attrib<3, SH_INOUT, FracByte, SH_NORMAL> InOutNormal3fb;
typedef Attrib<3, SH_TEMP, FracByte, SH_NORMAL>  Normal3fb;
typedef Attrib<3, SH_CONST, FracByte, SH_NORMAL> ConstNormal3fb;
typedef Attrib<4, SH_INPUT, FracByte, SH_NORMAL> InputNormal4fb;
typedef Attrib<4, SH_OUTPUT, FracByte, SH_NORMAL> OutputNormal4fb;
typedef Attrib<4, SH_INOUT, FracByte, SH_NORMAL> InOutNormal4fb;
typedef Attrib<4, SH_TEMP, FracByte, SH_NORMAL>  Normal4fb;
typedef Attrib<4, SH_CONST, FracByte, SH_NORMAL> ConstNormal4fb;


typedef Attrib<1, SH_INPUT, Half, SH_NORMAL> InputNormal1h;
typedef Attrib<1, SH_OUTPUT, Half, SH_NORMAL> OutputNormal1h;
typedef Attrib<1, SH_INOUT, Half, SH_NORMAL> InOutNormal1h;
typedef Attrib<1, SH_TEMP, Half, SH_NORMAL>  Normal1h;
typedef Attrib<1, SH_CONST, Half, SH_NORMAL> ConstNormal1h;
typedef Attrib<2, SH_INPUT, Half, SH_NORMAL> InputNormal2h;
typedef Attrib<2, SH_OUTPUT, Half, SH_NORMAL> OutputNormal2h;
typedef Attrib<2, SH_INOUT, Half, SH_NORMAL> InOutNormal2h;
typedef Attrib<2, SH_TEMP, Half, SH_NORMAL>  Normal2h;
typedef Attrib<2, SH_CONST, Half, SH_NORMAL> ConstNormal2h;
typedef Attrib<3, SH_INPUT, Half, SH_NORMAL> InputNormal3h;
typedef Attrib<3, SH_OUTPUT, Half, SH_NORMAL> OutputNormal3h;
typedef Attrib<3, SH_INOUT, Half, SH_NORMAL> InOutNormal3h;
typedef Attrib<3, SH_TEMP, Half, SH_NORMAL>  Normal3h;
typedef Attrib<3, SH_CONST, Half, SH_NORMAL> ConstNormal3h;
typedef Attrib<4, SH_INPUT, Half, SH_NORMAL> InputNormal4h;
typedef Attrib<4, SH_OUTPUT, Half, SH_NORMAL> OutputNormal4h;
typedef Attrib<4, SH_INOUT, Half, SH_NORMAL> InOutNormal4h;
typedef Attrib<4, SH_TEMP, Half, SH_NORMAL>  Normal4h;
typedef Attrib<4, SH_CONST, Half, SH_NORMAL> ConstNormal4h;


typedef Attrib<1, SH_INPUT, FracUInt, SH_NORMAL> InputNormal1fui;
typedef Attrib<1, SH_OUTPUT, FracUInt, SH_NORMAL> OutputNormal1fui;
typedef Attrib<1, SH_INOUT, FracUInt, SH_NORMAL> InOutNormal1fui;
typedef Attrib<1, SH_TEMP, FracUInt, SH_NORMAL>  Normal1fui;
typedef Attrib<1, SH_CONST, FracUInt, SH_NORMAL> ConstNormal1fui;
typedef Attrib<2, SH_INPUT, FracUInt, SH_NORMAL> InputNormal2fui;
typedef Attrib<2, SH_OUTPUT, FracUInt, SH_NORMAL> OutputNormal2fui;
typedef Attrib<2, SH_INOUT, FracUInt, SH_NORMAL> InOutNormal2fui;
typedef Attrib<2, SH_TEMP, FracUInt, SH_NORMAL>  Normal2fui;
typedef Attrib<2, SH_CONST, FracUInt, SH_NORMAL> ConstNormal2fui;
typedef Attrib<3, SH_INPUT, FracUInt, SH_NORMAL> InputNormal3fui;
typedef Attrib<3, SH_OUTPUT, FracUInt, SH_NORMAL> OutputNormal3fui;
typedef Attrib<3, SH_INOUT, FracUInt, SH_NORMAL> InOutNormal3fui;
typedef Attrib<3, SH_TEMP, FracUInt, SH_NORMAL>  Normal3fui;
typedef Attrib<3, SH_CONST, FracUInt, SH_NORMAL> ConstNormal3fui;
typedef Attrib<4, SH_INPUT, FracUInt, SH_NORMAL> InputNormal4fui;
typedef Attrib<4, SH_OUTPUT, FracUInt, SH_NORMAL> OutputNormal4fui;
typedef Attrib<4, SH_INOUT, FracUInt, SH_NORMAL> InOutNormal4fui;
typedef Attrib<4, SH_TEMP, FracUInt, SH_NORMAL>  Normal4fui;
typedef Attrib<4, SH_CONST, FracUInt, SH_NORMAL> ConstNormal4fui;


typedef Attrib<1, SH_INPUT, unsigned int, SH_NORMAL> InputNormal1ui;
typedef Attrib<1, SH_OUTPUT, unsigned int, SH_NORMAL> OutputNormal1ui;
typedef Attrib<1, SH_INOUT, unsigned int, SH_NORMAL> InOutNormal1ui;
typedef Attrib<1, SH_TEMP, unsigned int, SH_NORMAL>  Normal1ui;
typedef Attrib<1, SH_CONST, unsigned int, SH_NORMAL> ConstNormal1ui;
typedef Attrib<2, SH_INPUT, unsigned int, SH_NORMAL> InputNormal2ui;
typedef Attrib<2, SH_OUTPUT, unsigned int, SH_NORMAL> OutputNormal2ui;
typedef Attrib<2, SH_INOUT, unsigned int, SH_NORMAL> InOutNormal2ui;
typedef Attrib<2, SH_TEMP, unsigned int, SH_NORMAL>  Normal2ui;
typedef Attrib<2, SH_CONST, unsigned int, SH_NORMAL> ConstNormal2ui;
typedef Attrib<3, SH_INPUT, unsigned int, SH_NORMAL> InputNormal3ui;
typedef Attrib<3, SH_OUTPUT, unsigned int, SH_NORMAL> OutputNormal3ui;
typedef Attrib<3, SH_INOUT, unsigned int, SH_NORMAL> InOutNormal3ui;
typedef Attrib<3, SH_TEMP, unsigned int, SH_NORMAL>  Normal3ui;
typedef Attrib<3, SH_CONST, unsigned int, SH_NORMAL> ConstNormal3ui;
typedef Attrib<4, SH_INPUT, unsigned int, SH_NORMAL> InputNormal4ui;
typedef Attrib<4, SH_OUTPUT, unsigned int, SH_NORMAL> OutputNormal4ui;
typedef Attrib<4, SH_INOUT, unsigned int, SH_NORMAL> InOutNormal4ui;
typedef Attrib<4, SH_TEMP, unsigned int, SH_NORMAL>  Normal4ui;
typedef Attrib<4, SH_CONST, unsigned int, SH_NORMAL> ConstNormal4ui;



} // namespace SH

#endif // SHNORMAL_HPP