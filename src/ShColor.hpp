// WARNING - DO NOT EDIT THIS FILE:
// This file was automatically generated from scripts/ShColor.hpp.py.
// If you wish to change it, edit that file instead.
//
// ---
//
// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
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

#ifndef SH_SHCOLOR_HPP
#define SH_SHCOLOR_HPP

#include "ShAttrib.hpp"
namespace SH {

/** An n-color.
 * 
 *
 */

typedef ShAttrib<1, SH_INPUT, ShFracUShort, SH_COLOR> ShInputColor1fus;
typedef ShAttrib<1, SH_OUTPUT, ShFracUShort, SH_COLOR> ShOutputColor1fus;
typedef ShAttrib<1, SH_INOUT, ShFracUShort, SH_COLOR> ShInOutColor1fus;
typedef ShAttrib<1, SH_TEMP, ShFracUShort, SH_COLOR> ShColor1fus;
typedef ShAttrib<1, SH_CONST, ShFracUShort, SH_COLOR> ShConstColor1fus;
typedef ShAttrib<2, SH_INPUT, ShFracUShort, SH_COLOR> ShInputColor2fus;
typedef ShAttrib<2, SH_OUTPUT, ShFracUShort, SH_COLOR> ShOutputColor2fus;
typedef ShAttrib<2, SH_INOUT, ShFracUShort, SH_COLOR> ShInOutColor2fus;
typedef ShAttrib<2, SH_TEMP, ShFracUShort, SH_COLOR> ShColor2fus;
typedef ShAttrib<2, SH_CONST, ShFracUShort, SH_COLOR> ShConstColor2fus;
typedef ShAttrib<3, SH_INPUT, ShFracUShort, SH_COLOR> ShInputColor3fus;
typedef ShAttrib<3, SH_OUTPUT, ShFracUShort, SH_COLOR> ShOutputColor3fus;
typedef ShAttrib<3, SH_INOUT, ShFracUShort, SH_COLOR> ShInOutColor3fus;
typedef ShAttrib<3, SH_TEMP, ShFracUShort, SH_COLOR> ShColor3fus;
typedef ShAttrib<3, SH_CONST, ShFracUShort, SH_COLOR> ShConstColor3fus;
typedef ShAttrib<4, SH_INPUT, ShFracUShort, SH_COLOR> ShInputColor4fus;
typedef ShAttrib<4, SH_OUTPUT, ShFracUShort, SH_COLOR> ShOutputColor4fus;
typedef ShAttrib<4, SH_INOUT, ShFracUShort, SH_COLOR> ShInOutColor4fus;
typedef ShAttrib<4, SH_TEMP, ShFracUShort, SH_COLOR> ShColor4fus;
typedef ShAttrib<4, SH_CONST, ShFracUShort, SH_COLOR> ShConstColor4fus;


typedef ShAttrib<1, SH_INPUT, short, SH_COLOR> ShInputColor1s;
typedef ShAttrib<1, SH_OUTPUT, short, SH_COLOR> ShOutputColor1s;
typedef ShAttrib<1, SH_INOUT, short, SH_COLOR> ShInOutColor1s;
typedef ShAttrib<1, SH_TEMP, short, SH_COLOR> ShColor1s;
typedef ShAttrib<1, SH_CONST, short, SH_COLOR> ShConstColor1s;
typedef ShAttrib<2, SH_INPUT, short, SH_COLOR> ShInputColor2s;
typedef ShAttrib<2, SH_OUTPUT, short, SH_COLOR> ShOutputColor2s;
typedef ShAttrib<2, SH_INOUT, short, SH_COLOR> ShInOutColor2s;
typedef ShAttrib<2, SH_TEMP, short, SH_COLOR> ShColor2s;
typedef ShAttrib<2, SH_CONST, short, SH_COLOR> ShConstColor2s;
typedef ShAttrib<3, SH_INPUT, short, SH_COLOR> ShInputColor3s;
typedef ShAttrib<3, SH_OUTPUT, short, SH_COLOR> ShOutputColor3s;
typedef ShAttrib<3, SH_INOUT, short, SH_COLOR> ShInOutColor3s;
typedef ShAttrib<3, SH_TEMP, short, SH_COLOR> ShColor3s;
typedef ShAttrib<3, SH_CONST, short, SH_COLOR> ShConstColor3s;
typedef ShAttrib<4, SH_INPUT, short, SH_COLOR> ShInputColor4s;
typedef ShAttrib<4, SH_OUTPUT, short, SH_COLOR> ShOutputColor4s;
typedef ShAttrib<4, SH_INOUT, short, SH_COLOR> ShInOutColor4s;
typedef ShAttrib<4, SH_TEMP, short, SH_COLOR> ShColor4s;
typedef ShAttrib<4, SH_CONST, short, SH_COLOR> ShConstColor4s;


typedef ShAttrib<1, SH_INPUT, ShFracUInt, SH_COLOR> ShInputColor1fui;
typedef ShAttrib<1, SH_OUTPUT, ShFracUInt, SH_COLOR> ShOutputColor1fui;
typedef ShAttrib<1, SH_INOUT, ShFracUInt, SH_COLOR> ShInOutColor1fui;
typedef ShAttrib<1, SH_TEMP, ShFracUInt, SH_COLOR> ShColor1fui;
typedef ShAttrib<1, SH_CONST, ShFracUInt, SH_COLOR> ShConstColor1fui;
typedef ShAttrib<2, SH_INPUT, ShFracUInt, SH_COLOR> ShInputColor2fui;
typedef ShAttrib<2, SH_OUTPUT, ShFracUInt, SH_COLOR> ShOutputColor2fui;
typedef ShAttrib<2, SH_INOUT, ShFracUInt, SH_COLOR> ShInOutColor2fui;
typedef ShAttrib<2, SH_TEMP, ShFracUInt, SH_COLOR> ShColor2fui;
typedef ShAttrib<2, SH_CONST, ShFracUInt, SH_COLOR> ShConstColor2fui;
typedef ShAttrib<3, SH_INPUT, ShFracUInt, SH_COLOR> ShInputColor3fui;
typedef ShAttrib<3, SH_OUTPUT, ShFracUInt, SH_COLOR> ShOutputColor3fui;
typedef ShAttrib<3, SH_INOUT, ShFracUInt, SH_COLOR> ShInOutColor3fui;
typedef ShAttrib<3, SH_TEMP, ShFracUInt, SH_COLOR> ShColor3fui;
typedef ShAttrib<3, SH_CONST, ShFracUInt, SH_COLOR> ShConstColor3fui;
typedef ShAttrib<4, SH_INPUT, ShFracUInt, SH_COLOR> ShInputColor4fui;
typedef ShAttrib<4, SH_OUTPUT, ShFracUInt, SH_COLOR> ShOutputColor4fui;
typedef ShAttrib<4, SH_INOUT, ShFracUInt, SH_COLOR> ShInOutColor4fui;
typedef ShAttrib<4, SH_TEMP, ShFracUInt, SH_COLOR> ShColor4fui;
typedef ShAttrib<4, SH_CONST, ShFracUInt, SH_COLOR> ShConstColor4fui;


typedef ShAttrib<1, SH_INPUT, ShFracByte, SH_COLOR> ShInputColor1fb;
typedef ShAttrib<1, SH_OUTPUT, ShFracByte, SH_COLOR> ShOutputColor1fb;
typedef ShAttrib<1, SH_INOUT, ShFracByte, SH_COLOR> ShInOutColor1fb;
typedef ShAttrib<1, SH_TEMP, ShFracByte, SH_COLOR> ShColor1fb;
typedef ShAttrib<1, SH_CONST, ShFracByte, SH_COLOR> ShConstColor1fb;
typedef ShAttrib<2, SH_INPUT, ShFracByte, SH_COLOR> ShInputColor2fb;
typedef ShAttrib<2, SH_OUTPUT, ShFracByte, SH_COLOR> ShOutputColor2fb;
typedef ShAttrib<2, SH_INOUT, ShFracByte, SH_COLOR> ShInOutColor2fb;
typedef ShAttrib<2, SH_TEMP, ShFracByte, SH_COLOR> ShColor2fb;
typedef ShAttrib<2, SH_CONST, ShFracByte, SH_COLOR> ShConstColor2fb;
typedef ShAttrib<3, SH_INPUT, ShFracByte, SH_COLOR> ShInputColor3fb;
typedef ShAttrib<3, SH_OUTPUT, ShFracByte, SH_COLOR> ShOutputColor3fb;
typedef ShAttrib<3, SH_INOUT, ShFracByte, SH_COLOR> ShInOutColor3fb;
typedef ShAttrib<3, SH_TEMP, ShFracByte, SH_COLOR> ShColor3fb;
typedef ShAttrib<3, SH_CONST, ShFracByte, SH_COLOR> ShConstColor3fb;
typedef ShAttrib<4, SH_INPUT, ShFracByte, SH_COLOR> ShInputColor4fb;
typedef ShAttrib<4, SH_OUTPUT, ShFracByte, SH_COLOR> ShOutputColor4fb;
typedef ShAttrib<4, SH_INOUT, ShFracByte, SH_COLOR> ShInOutColor4fb;
typedef ShAttrib<4, SH_TEMP, ShFracByte, SH_COLOR> ShColor4fb;
typedef ShAttrib<4, SH_CONST, ShFracByte, SH_COLOR> ShConstColor4fb;


typedef ShAttrib<1, SH_INPUT, int, SH_COLOR> ShInputColor1i;
typedef ShAttrib<1, SH_OUTPUT, int, SH_COLOR> ShOutputColor1i;
typedef ShAttrib<1, SH_INOUT, int, SH_COLOR> ShInOutColor1i;
typedef ShAttrib<1, SH_TEMP, int, SH_COLOR> ShColor1i;
typedef ShAttrib<1, SH_CONST, int, SH_COLOR> ShConstColor1i;
typedef ShAttrib<2, SH_INPUT, int, SH_COLOR> ShInputColor2i;
typedef ShAttrib<2, SH_OUTPUT, int, SH_COLOR> ShOutputColor2i;
typedef ShAttrib<2, SH_INOUT, int, SH_COLOR> ShInOutColor2i;
typedef ShAttrib<2, SH_TEMP, int, SH_COLOR> ShColor2i;
typedef ShAttrib<2, SH_CONST, int, SH_COLOR> ShConstColor2i;
typedef ShAttrib<3, SH_INPUT, int, SH_COLOR> ShInputColor3i;
typedef ShAttrib<3, SH_OUTPUT, int, SH_COLOR> ShOutputColor3i;
typedef ShAttrib<3, SH_INOUT, int, SH_COLOR> ShInOutColor3i;
typedef ShAttrib<3, SH_TEMP, int, SH_COLOR> ShColor3i;
typedef ShAttrib<3, SH_CONST, int, SH_COLOR> ShConstColor3i;
typedef ShAttrib<4, SH_INPUT, int, SH_COLOR> ShInputColor4i;
typedef ShAttrib<4, SH_OUTPUT, int, SH_COLOR> ShOutputColor4i;
typedef ShAttrib<4, SH_INOUT, int, SH_COLOR> ShInOutColor4i;
typedef ShAttrib<4, SH_TEMP, int, SH_COLOR> ShColor4i;
typedef ShAttrib<4, SH_CONST, int, SH_COLOR> ShConstColor4i;


typedef ShAttrib<1, SH_INPUT, double, SH_COLOR> ShInputColor1d;
typedef ShAttrib<1, SH_OUTPUT, double, SH_COLOR> ShOutputColor1d;
typedef ShAttrib<1, SH_INOUT, double, SH_COLOR> ShInOutColor1d;
typedef ShAttrib<1, SH_TEMP, double, SH_COLOR> ShColor1d;
typedef ShAttrib<1, SH_CONST, double, SH_COLOR> ShConstColor1d;
typedef ShAttrib<2, SH_INPUT, double, SH_COLOR> ShInputColor2d;
typedef ShAttrib<2, SH_OUTPUT, double, SH_COLOR> ShOutputColor2d;
typedef ShAttrib<2, SH_INOUT, double, SH_COLOR> ShInOutColor2d;
typedef ShAttrib<2, SH_TEMP, double, SH_COLOR> ShColor2d;
typedef ShAttrib<2, SH_CONST, double, SH_COLOR> ShConstColor2d;
typedef ShAttrib<3, SH_INPUT, double, SH_COLOR> ShInputColor3d;
typedef ShAttrib<3, SH_OUTPUT, double, SH_COLOR> ShOutputColor3d;
typedef ShAttrib<3, SH_INOUT, double, SH_COLOR> ShInOutColor3d;
typedef ShAttrib<3, SH_TEMP, double, SH_COLOR> ShColor3d;
typedef ShAttrib<3, SH_CONST, double, SH_COLOR> ShConstColor3d;
typedef ShAttrib<4, SH_INPUT, double, SH_COLOR> ShInputColor4d;
typedef ShAttrib<4, SH_OUTPUT, double, SH_COLOR> ShOutputColor4d;
typedef ShAttrib<4, SH_INOUT, double, SH_COLOR> ShInOutColor4d;
typedef ShAttrib<4, SH_TEMP, double, SH_COLOR> ShColor4d;
typedef ShAttrib<4, SH_CONST, double, SH_COLOR> ShConstColor4d;


typedef ShAttrib<1, SH_INPUT, unsigned char, SH_COLOR> ShInputColor1ub;
typedef ShAttrib<1, SH_OUTPUT, unsigned char, SH_COLOR> ShOutputColor1ub;
typedef ShAttrib<1, SH_INOUT, unsigned char, SH_COLOR> ShInOutColor1ub;
typedef ShAttrib<1, SH_TEMP, unsigned char, SH_COLOR> ShColor1ub;
typedef ShAttrib<1, SH_CONST, unsigned char, SH_COLOR> ShConstColor1ub;
typedef ShAttrib<2, SH_INPUT, unsigned char, SH_COLOR> ShInputColor2ub;
typedef ShAttrib<2, SH_OUTPUT, unsigned char, SH_COLOR> ShOutputColor2ub;
typedef ShAttrib<2, SH_INOUT, unsigned char, SH_COLOR> ShInOutColor2ub;
typedef ShAttrib<2, SH_TEMP, unsigned char, SH_COLOR> ShColor2ub;
typedef ShAttrib<2, SH_CONST, unsigned char, SH_COLOR> ShConstColor2ub;
typedef ShAttrib<3, SH_INPUT, unsigned char, SH_COLOR> ShInputColor3ub;
typedef ShAttrib<3, SH_OUTPUT, unsigned char, SH_COLOR> ShOutputColor3ub;
typedef ShAttrib<3, SH_INOUT, unsigned char, SH_COLOR> ShInOutColor3ub;
typedef ShAttrib<3, SH_TEMP, unsigned char, SH_COLOR> ShColor3ub;
typedef ShAttrib<3, SH_CONST, unsigned char, SH_COLOR> ShConstColor3ub;
typedef ShAttrib<4, SH_INPUT, unsigned char, SH_COLOR> ShInputColor4ub;
typedef ShAttrib<4, SH_OUTPUT, unsigned char, SH_COLOR> ShOutputColor4ub;
typedef ShAttrib<4, SH_INOUT, unsigned char, SH_COLOR> ShInOutColor4ub;
typedef ShAttrib<4, SH_TEMP, unsigned char, SH_COLOR> ShColor4ub;
typedef ShAttrib<4, SH_CONST, unsigned char, SH_COLOR> ShConstColor4ub;


typedef ShAttrib<1, SH_INPUT, float, SH_COLOR> ShInputColor1f;
typedef ShAttrib<1, SH_OUTPUT, float, SH_COLOR> ShOutputColor1f;
typedef ShAttrib<1, SH_INOUT, float, SH_COLOR> ShInOutColor1f;
typedef ShAttrib<1, SH_TEMP, float, SH_COLOR> ShColor1f;
typedef ShAttrib<1, SH_CONST, float, SH_COLOR> ShConstColor1f;
typedef ShAttrib<2, SH_INPUT, float, SH_COLOR> ShInputColor2f;
typedef ShAttrib<2, SH_OUTPUT, float, SH_COLOR> ShOutputColor2f;
typedef ShAttrib<2, SH_INOUT, float, SH_COLOR> ShInOutColor2f;
typedef ShAttrib<2, SH_TEMP, float, SH_COLOR> ShColor2f;
typedef ShAttrib<2, SH_CONST, float, SH_COLOR> ShConstColor2f;
typedef ShAttrib<3, SH_INPUT, float, SH_COLOR> ShInputColor3f;
typedef ShAttrib<3, SH_OUTPUT, float, SH_COLOR> ShOutputColor3f;
typedef ShAttrib<3, SH_INOUT, float, SH_COLOR> ShInOutColor3f;
typedef ShAttrib<3, SH_TEMP, float, SH_COLOR> ShColor3f;
typedef ShAttrib<3, SH_CONST, float, SH_COLOR> ShConstColor3f;
typedef ShAttrib<4, SH_INPUT, float, SH_COLOR> ShInputColor4f;
typedef ShAttrib<4, SH_OUTPUT, float, SH_COLOR> ShOutputColor4f;
typedef ShAttrib<4, SH_INOUT, float, SH_COLOR> ShInOutColor4f;
typedef ShAttrib<4, SH_TEMP, float, SH_COLOR> ShColor4f;
typedef ShAttrib<4, SH_CONST, float, SH_COLOR> ShConstColor4f;


typedef ShAttrib<1, SH_INPUT, char, SH_COLOR> ShInputColor1b;
typedef ShAttrib<1, SH_OUTPUT, char, SH_COLOR> ShOutputColor1b;
typedef ShAttrib<1, SH_INOUT, char, SH_COLOR> ShInOutColor1b;
typedef ShAttrib<1, SH_TEMP, char, SH_COLOR> ShColor1b;
typedef ShAttrib<1, SH_CONST, char, SH_COLOR> ShConstColor1b;
typedef ShAttrib<2, SH_INPUT, char, SH_COLOR> ShInputColor2b;
typedef ShAttrib<2, SH_OUTPUT, char, SH_COLOR> ShOutputColor2b;
typedef ShAttrib<2, SH_INOUT, char, SH_COLOR> ShInOutColor2b;
typedef ShAttrib<2, SH_TEMP, char, SH_COLOR> ShColor2b;
typedef ShAttrib<2, SH_CONST, char, SH_COLOR> ShConstColor2b;
typedef ShAttrib<3, SH_INPUT, char, SH_COLOR> ShInputColor3b;
typedef ShAttrib<3, SH_OUTPUT, char, SH_COLOR> ShOutputColor3b;
typedef ShAttrib<3, SH_INOUT, char, SH_COLOR> ShInOutColor3b;
typedef ShAttrib<3, SH_TEMP, char, SH_COLOR> ShColor3b;
typedef ShAttrib<3, SH_CONST, char, SH_COLOR> ShConstColor3b;
typedef ShAttrib<4, SH_INPUT, char, SH_COLOR> ShInputColor4b;
typedef ShAttrib<4, SH_OUTPUT, char, SH_COLOR> ShOutputColor4b;
typedef ShAttrib<4, SH_INOUT, char, SH_COLOR> ShInOutColor4b;
typedef ShAttrib<4, SH_TEMP, char, SH_COLOR> ShColor4b;
typedef ShAttrib<4, SH_CONST, char, SH_COLOR> ShConstColor4b;


typedef ShAttrib<1, SH_INPUT, unsigned short, SH_COLOR> ShInputColor1us;
typedef ShAttrib<1, SH_OUTPUT, unsigned short, SH_COLOR> ShOutputColor1us;
typedef ShAttrib<1, SH_INOUT, unsigned short, SH_COLOR> ShInOutColor1us;
typedef ShAttrib<1, SH_TEMP, unsigned short, SH_COLOR> ShColor1us;
typedef ShAttrib<1, SH_CONST, unsigned short, SH_COLOR> ShConstColor1us;
typedef ShAttrib<2, SH_INPUT, unsigned short, SH_COLOR> ShInputColor2us;
typedef ShAttrib<2, SH_OUTPUT, unsigned short, SH_COLOR> ShOutputColor2us;
typedef ShAttrib<2, SH_INOUT, unsigned short, SH_COLOR> ShInOutColor2us;
typedef ShAttrib<2, SH_TEMP, unsigned short, SH_COLOR> ShColor2us;
typedef ShAttrib<2, SH_CONST, unsigned short, SH_COLOR> ShConstColor2us;
typedef ShAttrib<3, SH_INPUT, unsigned short, SH_COLOR> ShInputColor3us;
typedef ShAttrib<3, SH_OUTPUT, unsigned short, SH_COLOR> ShOutputColor3us;
typedef ShAttrib<3, SH_INOUT, unsigned short, SH_COLOR> ShInOutColor3us;
typedef ShAttrib<3, SH_TEMP, unsigned short, SH_COLOR> ShColor3us;
typedef ShAttrib<3, SH_CONST, unsigned short, SH_COLOR> ShConstColor3us;
typedef ShAttrib<4, SH_INPUT, unsigned short, SH_COLOR> ShInputColor4us;
typedef ShAttrib<4, SH_OUTPUT, unsigned short, SH_COLOR> ShOutputColor4us;
typedef ShAttrib<4, SH_INOUT, unsigned short, SH_COLOR> ShInOutColor4us;
typedef ShAttrib<4, SH_TEMP, unsigned short, SH_COLOR> ShColor4us;
typedef ShAttrib<4, SH_CONST, unsigned short, SH_COLOR> ShConstColor4us;


typedef ShAttrib<1, SH_INPUT, ShFracUByte, SH_COLOR> ShInputColor1fub;
typedef ShAttrib<1, SH_OUTPUT, ShFracUByte, SH_COLOR> ShOutputColor1fub;
typedef ShAttrib<1, SH_INOUT, ShFracUByte, SH_COLOR> ShInOutColor1fub;
typedef ShAttrib<1, SH_TEMP, ShFracUByte, SH_COLOR> ShColor1fub;
typedef ShAttrib<1, SH_CONST, ShFracUByte, SH_COLOR> ShConstColor1fub;
typedef ShAttrib<2, SH_INPUT, ShFracUByte, SH_COLOR> ShInputColor2fub;
typedef ShAttrib<2, SH_OUTPUT, ShFracUByte, SH_COLOR> ShOutputColor2fub;
typedef ShAttrib<2, SH_INOUT, ShFracUByte, SH_COLOR> ShInOutColor2fub;
typedef ShAttrib<2, SH_TEMP, ShFracUByte, SH_COLOR> ShColor2fub;
typedef ShAttrib<2, SH_CONST, ShFracUByte, SH_COLOR> ShConstColor2fub;
typedef ShAttrib<3, SH_INPUT, ShFracUByte, SH_COLOR> ShInputColor3fub;
typedef ShAttrib<3, SH_OUTPUT, ShFracUByte, SH_COLOR> ShOutputColor3fub;
typedef ShAttrib<3, SH_INOUT, ShFracUByte, SH_COLOR> ShInOutColor3fub;
typedef ShAttrib<3, SH_TEMP, ShFracUByte, SH_COLOR> ShColor3fub;
typedef ShAttrib<3, SH_CONST, ShFracUByte, SH_COLOR> ShConstColor3fub;
typedef ShAttrib<4, SH_INPUT, ShFracUByte, SH_COLOR> ShInputColor4fub;
typedef ShAttrib<4, SH_OUTPUT, ShFracUByte, SH_COLOR> ShOutputColor4fub;
typedef ShAttrib<4, SH_INOUT, ShFracUByte, SH_COLOR> ShInOutColor4fub;
typedef ShAttrib<4, SH_TEMP, ShFracUByte, SH_COLOR> ShColor4fub;
typedef ShAttrib<4, SH_CONST, ShFracUByte, SH_COLOR> ShConstColor4fub;


typedef ShAttrib<1, SH_INPUT, ShHalf, SH_COLOR> ShInputColor1h;
typedef ShAttrib<1, SH_OUTPUT, ShHalf, SH_COLOR> ShOutputColor1h;
typedef ShAttrib<1, SH_INOUT, ShHalf, SH_COLOR> ShInOutColor1h;
typedef ShAttrib<1, SH_TEMP, ShHalf, SH_COLOR> ShColor1h;
typedef ShAttrib<1, SH_CONST, ShHalf, SH_COLOR> ShConstColor1h;
typedef ShAttrib<2, SH_INPUT, ShHalf, SH_COLOR> ShInputColor2h;
typedef ShAttrib<2, SH_OUTPUT, ShHalf, SH_COLOR> ShOutputColor2h;
typedef ShAttrib<2, SH_INOUT, ShHalf, SH_COLOR> ShInOutColor2h;
typedef ShAttrib<2, SH_TEMP, ShHalf, SH_COLOR> ShColor2h;
typedef ShAttrib<2, SH_CONST, ShHalf, SH_COLOR> ShConstColor2h;
typedef ShAttrib<3, SH_INPUT, ShHalf, SH_COLOR> ShInputColor3h;
typedef ShAttrib<3, SH_OUTPUT, ShHalf, SH_COLOR> ShOutputColor3h;
typedef ShAttrib<3, SH_INOUT, ShHalf, SH_COLOR> ShInOutColor3h;
typedef ShAttrib<3, SH_TEMP, ShHalf, SH_COLOR> ShColor3h;
typedef ShAttrib<3, SH_CONST, ShHalf, SH_COLOR> ShConstColor3h;
typedef ShAttrib<4, SH_INPUT, ShHalf, SH_COLOR> ShInputColor4h;
typedef ShAttrib<4, SH_OUTPUT, ShHalf, SH_COLOR> ShOutputColor4h;
typedef ShAttrib<4, SH_INOUT, ShHalf, SH_COLOR> ShInOutColor4h;
typedef ShAttrib<4, SH_TEMP, ShHalf, SH_COLOR> ShColor4h;
typedef ShAttrib<4, SH_CONST, ShHalf, SH_COLOR> ShConstColor4h;


typedef ShAttrib<1, SH_INPUT, ShFracShort, SH_COLOR> ShInputColor1fs;
typedef ShAttrib<1, SH_OUTPUT, ShFracShort, SH_COLOR> ShOutputColor1fs;
typedef ShAttrib<1, SH_INOUT, ShFracShort, SH_COLOR> ShInOutColor1fs;
typedef ShAttrib<1, SH_TEMP, ShFracShort, SH_COLOR> ShColor1fs;
typedef ShAttrib<1, SH_CONST, ShFracShort, SH_COLOR> ShConstColor1fs;
typedef ShAttrib<2, SH_INPUT, ShFracShort, SH_COLOR> ShInputColor2fs;
typedef ShAttrib<2, SH_OUTPUT, ShFracShort, SH_COLOR> ShOutputColor2fs;
typedef ShAttrib<2, SH_INOUT, ShFracShort, SH_COLOR> ShInOutColor2fs;
typedef ShAttrib<2, SH_TEMP, ShFracShort, SH_COLOR> ShColor2fs;
typedef ShAttrib<2, SH_CONST, ShFracShort, SH_COLOR> ShConstColor2fs;
typedef ShAttrib<3, SH_INPUT, ShFracShort, SH_COLOR> ShInputColor3fs;
typedef ShAttrib<3, SH_OUTPUT, ShFracShort, SH_COLOR> ShOutputColor3fs;
typedef ShAttrib<3, SH_INOUT, ShFracShort, SH_COLOR> ShInOutColor3fs;
typedef ShAttrib<3, SH_TEMP, ShFracShort, SH_COLOR> ShColor3fs;
typedef ShAttrib<3, SH_CONST, ShFracShort, SH_COLOR> ShConstColor3fs;
typedef ShAttrib<4, SH_INPUT, ShFracShort, SH_COLOR> ShInputColor4fs;
typedef ShAttrib<4, SH_OUTPUT, ShFracShort, SH_COLOR> ShOutputColor4fs;
typedef ShAttrib<4, SH_INOUT, ShFracShort, SH_COLOR> ShInOutColor4fs;
typedef ShAttrib<4, SH_TEMP, ShFracShort, SH_COLOR> ShColor4fs;
typedef ShAttrib<4, SH_CONST, ShFracShort, SH_COLOR> ShConstColor4fs;


typedef ShAttrib<1, SH_INPUT, ShFracInt, SH_COLOR> ShInputColor1fi;
typedef ShAttrib<1, SH_OUTPUT, ShFracInt, SH_COLOR> ShOutputColor1fi;
typedef ShAttrib<1, SH_INOUT, ShFracInt, SH_COLOR> ShInOutColor1fi;
typedef ShAttrib<1, SH_TEMP, ShFracInt, SH_COLOR> ShColor1fi;
typedef ShAttrib<1, SH_CONST, ShFracInt, SH_COLOR> ShConstColor1fi;
typedef ShAttrib<2, SH_INPUT, ShFracInt, SH_COLOR> ShInputColor2fi;
typedef ShAttrib<2, SH_OUTPUT, ShFracInt, SH_COLOR> ShOutputColor2fi;
typedef ShAttrib<2, SH_INOUT, ShFracInt, SH_COLOR> ShInOutColor2fi;
typedef ShAttrib<2, SH_TEMP, ShFracInt, SH_COLOR> ShColor2fi;
typedef ShAttrib<2, SH_CONST, ShFracInt, SH_COLOR> ShConstColor2fi;
typedef ShAttrib<3, SH_INPUT, ShFracInt, SH_COLOR> ShInputColor3fi;
typedef ShAttrib<3, SH_OUTPUT, ShFracInt, SH_COLOR> ShOutputColor3fi;
typedef ShAttrib<3, SH_INOUT, ShFracInt, SH_COLOR> ShInOutColor3fi;
typedef ShAttrib<3, SH_TEMP, ShFracInt, SH_COLOR> ShColor3fi;
typedef ShAttrib<3, SH_CONST, ShFracInt, SH_COLOR> ShConstColor3fi;
typedef ShAttrib<4, SH_INPUT, ShFracInt, SH_COLOR> ShInputColor4fi;
typedef ShAttrib<4, SH_OUTPUT, ShFracInt, SH_COLOR> ShOutputColor4fi;
typedef ShAttrib<4, SH_INOUT, ShFracInt, SH_COLOR> ShInOutColor4fi;
typedef ShAttrib<4, SH_TEMP, ShFracInt, SH_COLOR> ShColor4fi;
typedef ShAttrib<4, SH_CONST, ShFracInt, SH_COLOR> ShConstColor4fi;


typedef ShAttrib<1, SH_INPUT, unsigned int, SH_COLOR> ShInputColor1ui;
typedef ShAttrib<1, SH_OUTPUT, unsigned int, SH_COLOR> ShOutputColor1ui;
typedef ShAttrib<1, SH_INOUT, unsigned int, SH_COLOR> ShInOutColor1ui;
typedef ShAttrib<1, SH_TEMP, unsigned int, SH_COLOR> ShColor1ui;
typedef ShAttrib<1, SH_CONST, unsigned int, SH_COLOR> ShConstColor1ui;
typedef ShAttrib<2, SH_INPUT, unsigned int, SH_COLOR> ShInputColor2ui;
typedef ShAttrib<2, SH_OUTPUT, unsigned int, SH_COLOR> ShOutputColor2ui;
typedef ShAttrib<2, SH_INOUT, unsigned int, SH_COLOR> ShInOutColor2ui;
typedef ShAttrib<2, SH_TEMP, unsigned int, SH_COLOR> ShColor2ui;
typedef ShAttrib<2, SH_CONST, unsigned int, SH_COLOR> ShConstColor2ui;
typedef ShAttrib<3, SH_INPUT, unsigned int, SH_COLOR> ShInputColor3ui;
typedef ShAttrib<3, SH_OUTPUT, unsigned int, SH_COLOR> ShOutputColor3ui;
typedef ShAttrib<3, SH_INOUT, unsigned int, SH_COLOR> ShInOutColor3ui;
typedef ShAttrib<3, SH_TEMP, unsigned int, SH_COLOR> ShColor3ui;
typedef ShAttrib<3, SH_CONST, unsigned int, SH_COLOR> ShConstColor3ui;
typedef ShAttrib<4, SH_INPUT, unsigned int, SH_COLOR> ShInputColor4ui;
typedef ShAttrib<4, SH_OUTPUT, unsigned int, SH_COLOR> ShOutputColor4ui;
typedef ShAttrib<4, SH_INOUT, unsigned int, SH_COLOR> ShInOutColor4ui;
typedef ShAttrib<4, SH_TEMP, unsigned int, SH_COLOR> ShColor4ui;
typedef ShAttrib<4, SH_CONST, unsigned int, SH_COLOR> ShConstColor4ui;



} // namespace SH

#endif // SH_SHCOLOR_HPP
