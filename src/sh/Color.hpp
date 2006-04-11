// WARNING - DO NOT EDIT THIS FILE:
// This file was automatically generated from scripts/Color.hpp.py.
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

#ifndef SHCOLOR_HPP
#define SHCOLOR_HPP

#include "Attrib.hpp"
namespace SH {

/** An n-color.
 * 
 *
 */

typedef Attrib<1, SH_INPUT, short, SH_COLOR> InputColor1s;
typedef Attrib<1, SH_OUTPUT, short, SH_COLOR> OutputColor1s;
typedef Attrib<1, SH_INOUT, short, SH_COLOR> InOutColor1s;
typedef Attrib<1, SH_TEMP, short, SH_COLOR>  Color1s;
typedef Attrib<1, SH_CONST, short, SH_COLOR> ConstColor1s;
typedef Attrib<2, SH_INPUT, short, SH_COLOR> InputColor2s;
typedef Attrib<2, SH_OUTPUT, short, SH_COLOR> OutputColor2s;
typedef Attrib<2, SH_INOUT, short, SH_COLOR> InOutColor2s;
typedef Attrib<2, SH_TEMP, short, SH_COLOR>  Color2s;
typedef Attrib<2, SH_CONST, short, SH_COLOR> ConstColor2s;
typedef Attrib<3, SH_INPUT, short, SH_COLOR> InputColor3s;
typedef Attrib<3, SH_OUTPUT, short, SH_COLOR> OutputColor3s;
typedef Attrib<3, SH_INOUT, short, SH_COLOR> InOutColor3s;
typedef Attrib<3, SH_TEMP, short, SH_COLOR>  Color3s;
typedef Attrib<3, SH_CONST, short, SH_COLOR> ConstColor3s;
typedef Attrib<4, SH_INPUT, short, SH_COLOR> InputColor4s;
typedef Attrib<4, SH_OUTPUT, short, SH_COLOR> OutputColor4s;
typedef Attrib<4, SH_INOUT, short, SH_COLOR> InOutColor4s;
typedef Attrib<4, SH_TEMP, short, SH_COLOR>  Color4s;
typedef Attrib<4, SH_CONST, short, SH_COLOR> ConstColor4s;


typedef Attrib<1, SH_INPUT, FracUByte, SH_COLOR> InputColor1fub;
typedef Attrib<1, SH_OUTPUT, FracUByte, SH_COLOR> OutputColor1fub;
typedef Attrib<1, SH_INOUT, FracUByte, SH_COLOR> InOutColor1fub;
typedef Attrib<1, SH_TEMP, FracUByte, SH_COLOR>  Color1fub;
typedef Attrib<1, SH_CONST, FracUByte, SH_COLOR> ConstColor1fub;
typedef Attrib<2, SH_INPUT, FracUByte, SH_COLOR> InputColor2fub;
typedef Attrib<2, SH_OUTPUT, FracUByte, SH_COLOR> OutputColor2fub;
typedef Attrib<2, SH_INOUT, FracUByte, SH_COLOR> InOutColor2fub;
typedef Attrib<2, SH_TEMP, FracUByte, SH_COLOR>  Color2fub;
typedef Attrib<2, SH_CONST, FracUByte, SH_COLOR> ConstColor2fub;
typedef Attrib<3, SH_INPUT, FracUByte, SH_COLOR> InputColor3fub;
typedef Attrib<3, SH_OUTPUT, FracUByte, SH_COLOR> OutputColor3fub;
typedef Attrib<3, SH_INOUT, FracUByte, SH_COLOR> InOutColor3fub;
typedef Attrib<3, SH_TEMP, FracUByte, SH_COLOR>  Color3fub;
typedef Attrib<3, SH_CONST, FracUByte, SH_COLOR> ConstColor3fub;
typedef Attrib<4, SH_INPUT, FracUByte, SH_COLOR> InputColor4fub;
typedef Attrib<4, SH_OUTPUT, FracUByte, SH_COLOR> OutputColor4fub;
typedef Attrib<4, SH_INOUT, FracUByte, SH_COLOR> InOutColor4fub;
typedef Attrib<4, SH_TEMP, FracUByte, SH_COLOR>  Color4fub;
typedef Attrib<4, SH_CONST, FracUByte, SH_COLOR> ConstColor4fub;


typedef Attrib<1, SH_INPUT, FracUShort, SH_COLOR> InputColor1fus;
typedef Attrib<1, SH_OUTPUT, FracUShort, SH_COLOR> OutputColor1fus;
typedef Attrib<1, SH_INOUT, FracUShort, SH_COLOR> InOutColor1fus;
typedef Attrib<1, SH_TEMP, FracUShort, SH_COLOR>  Color1fus;
typedef Attrib<1, SH_CONST, FracUShort, SH_COLOR> ConstColor1fus;
typedef Attrib<2, SH_INPUT, FracUShort, SH_COLOR> InputColor2fus;
typedef Attrib<2, SH_OUTPUT, FracUShort, SH_COLOR> OutputColor2fus;
typedef Attrib<2, SH_INOUT, FracUShort, SH_COLOR> InOutColor2fus;
typedef Attrib<2, SH_TEMP, FracUShort, SH_COLOR>  Color2fus;
typedef Attrib<2, SH_CONST, FracUShort, SH_COLOR> ConstColor2fus;
typedef Attrib<3, SH_INPUT, FracUShort, SH_COLOR> InputColor3fus;
typedef Attrib<3, SH_OUTPUT, FracUShort, SH_COLOR> OutputColor3fus;
typedef Attrib<3, SH_INOUT, FracUShort, SH_COLOR> InOutColor3fus;
typedef Attrib<3, SH_TEMP, FracUShort, SH_COLOR>  Color3fus;
typedef Attrib<3, SH_CONST, FracUShort, SH_COLOR> ConstColor3fus;
typedef Attrib<4, SH_INPUT, FracUShort, SH_COLOR> InputColor4fus;
typedef Attrib<4, SH_OUTPUT, FracUShort, SH_COLOR> OutputColor4fus;
typedef Attrib<4, SH_INOUT, FracUShort, SH_COLOR> InOutColor4fus;
typedef Attrib<4, SH_TEMP, FracUShort, SH_COLOR>  Color4fus;
typedef Attrib<4, SH_CONST, FracUShort, SH_COLOR> ConstColor4fus;


typedef Attrib<1, SH_INPUT, int, SH_COLOR> InputColor1i;
typedef Attrib<1, SH_OUTPUT, int, SH_COLOR> OutputColor1i;
typedef Attrib<1, SH_INOUT, int, SH_COLOR> InOutColor1i;
typedef Attrib<1, SH_TEMP, int, SH_COLOR>  Color1i;
typedef Attrib<1, SH_CONST, int, SH_COLOR> ConstColor1i;
typedef Attrib<2, SH_INPUT, int, SH_COLOR> InputColor2i;
typedef Attrib<2, SH_OUTPUT, int, SH_COLOR> OutputColor2i;
typedef Attrib<2, SH_INOUT, int, SH_COLOR> InOutColor2i;
typedef Attrib<2, SH_TEMP, int, SH_COLOR>  Color2i;
typedef Attrib<2, SH_CONST, int, SH_COLOR> ConstColor2i;
typedef Attrib<3, SH_INPUT, int, SH_COLOR> InputColor3i;
typedef Attrib<3, SH_OUTPUT, int, SH_COLOR> OutputColor3i;
typedef Attrib<3, SH_INOUT, int, SH_COLOR> InOutColor3i;
typedef Attrib<3, SH_TEMP, int, SH_COLOR>  Color3i;
typedef Attrib<3, SH_CONST, int, SH_COLOR> ConstColor3i;
typedef Attrib<4, SH_INPUT, int, SH_COLOR> InputColor4i;
typedef Attrib<4, SH_OUTPUT, int, SH_COLOR> OutputColor4i;
typedef Attrib<4, SH_INOUT, int, SH_COLOR> InOutColor4i;
typedef Attrib<4, SH_TEMP, int, SH_COLOR>  Color4i;
typedef Attrib<4, SH_CONST, int, SH_COLOR> ConstColor4i;


typedef Attrib<1, SH_INPUT, double, SH_COLOR> InputColor1d;
typedef Attrib<1, SH_OUTPUT, double, SH_COLOR> OutputColor1d;
typedef Attrib<1, SH_INOUT, double, SH_COLOR> InOutColor1d;
typedef Attrib<1, SH_TEMP, double, SH_COLOR>  Color1d;
typedef Attrib<1, SH_CONST, double, SH_COLOR> ConstColor1d;
typedef Attrib<2, SH_INPUT, double, SH_COLOR> InputColor2d;
typedef Attrib<2, SH_OUTPUT, double, SH_COLOR> OutputColor2d;
typedef Attrib<2, SH_INOUT, double, SH_COLOR> InOutColor2d;
typedef Attrib<2, SH_TEMP, double, SH_COLOR>  Color2d;
typedef Attrib<2, SH_CONST, double, SH_COLOR> ConstColor2d;
typedef Attrib<3, SH_INPUT, double, SH_COLOR> InputColor3d;
typedef Attrib<3, SH_OUTPUT, double, SH_COLOR> OutputColor3d;
typedef Attrib<3, SH_INOUT, double, SH_COLOR> InOutColor3d;
typedef Attrib<3, SH_TEMP, double, SH_COLOR>  Color3d;
typedef Attrib<3, SH_CONST, double, SH_COLOR> ConstColor3d;
typedef Attrib<4, SH_INPUT, double, SH_COLOR> InputColor4d;
typedef Attrib<4, SH_OUTPUT, double, SH_COLOR> OutputColor4d;
typedef Attrib<4, SH_INOUT, double, SH_COLOR> InOutColor4d;
typedef Attrib<4, SH_TEMP, double, SH_COLOR>  Color4d;
typedef Attrib<4, SH_CONST, double, SH_COLOR> ConstColor4d;


typedef Attrib<1, SH_INPUT, unsigned char, SH_COLOR> InputColor1ub;
typedef Attrib<1, SH_OUTPUT, unsigned char, SH_COLOR> OutputColor1ub;
typedef Attrib<1, SH_INOUT, unsigned char, SH_COLOR> InOutColor1ub;
typedef Attrib<1, SH_TEMP, unsigned char, SH_COLOR>  Color1ub;
typedef Attrib<1, SH_CONST, unsigned char, SH_COLOR> ConstColor1ub;
typedef Attrib<2, SH_INPUT, unsigned char, SH_COLOR> InputColor2ub;
typedef Attrib<2, SH_OUTPUT, unsigned char, SH_COLOR> OutputColor2ub;
typedef Attrib<2, SH_INOUT, unsigned char, SH_COLOR> InOutColor2ub;
typedef Attrib<2, SH_TEMP, unsigned char, SH_COLOR>  Color2ub;
typedef Attrib<2, SH_CONST, unsigned char, SH_COLOR> ConstColor2ub;
typedef Attrib<3, SH_INPUT, unsigned char, SH_COLOR> InputColor3ub;
typedef Attrib<3, SH_OUTPUT, unsigned char, SH_COLOR> OutputColor3ub;
typedef Attrib<3, SH_INOUT, unsigned char, SH_COLOR> InOutColor3ub;
typedef Attrib<3, SH_TEMP, unsigned char, SH_COLOR>  Color3ub;
typedef Attrib<3, SH_CONST, unsigned char, SH_COLOR> ConstColor3ub;
typedef Attrib<4, SH_INPUT, unsigned char, SH_COLOR> InputColor4ub;
typedef Attrib<4, SH_OUTPUT, unsigned char, SH_COLOR> OutputColor4ub;
typedef Attrib<4, SH_INOUT, unsigned char, SH_COLOR> InOutColor4ub;
typedef Attrib<4, SH_TEMP, unsigned char, SH_COLOR>  Color4ub;
typedef Attrib<4, SH_CONST, unsigned char, SH_COLOR> ConstColor4ub;


typedef Attrib<1, SH_INPUT, float, SH_COLOR> InputColor1f;
typedef Attrib<1, SH_OUTPUT, float, SH_COLOR> OutputColor1f;
typedef Attrib<1, SH_INOUT, float, SH_COLOR> InOutColor1f;
typedef Attrib<1, SH_TEMP, float, SH_COLOR>  Color1f;
typedef Attrib<1, SH_CONST, float, SH_COLOR> ConstColor1f;
typedef Attrib<2, SH_INPUT, float, SH_COLOR> InputColor2f;
typedef Attrib<2, SH_OUTPUT, float, SH_COLOR> OutputColor2f;
typedef Attrib<2, SH_INOUT, float, SH_COLOR> InOutColor2f;
typedef Attrib<2, SH_TEMP, float, SH_COLOR>  Color2f;
typedef Attrib<2, SH_CONST, float, SH_COLOR> ConstColor2f;
typedef Attrib<3, SH_INPUT, float, SH_COLOR> InputColor3f;
typedef Attrib<3, SH_OUTPUT, float, SH_COLOR> OutputColor3f;
typedef Attrib<3, SH_INOUT, float, SH_COLOR> InOutColor3f;
typedef Attrib<3, SH_TEMP, float, SH_COLOR>  Color3f;
typedef Attrib<3, SH_CONST, float, SH_COLOR> ConstColor3f;
typedef Attrib<4, SH_INPUT, float, SH_COLOR> InputColor4f;
typedef Attrib<4, SH_OUTPUT, float, SH_COLOR> OutputColor4f;
typedef Attrib<4, SH_INOUT, float, SH_COLOR> InOutColor4f;
typedef Attrib<4, SH_TEMP, float, SH_COLOR>  Color4f;
typedef Attrib<4, SH_CONST, float, SH_COLOR> ConstColor4f;


typedef Attrib<1, SH_INPUT, FracInt, SH_COLOR> InputColor1fi;
typedef Attrib<1, SH_OUTPUT, FracInt, SH_COLOR> OutputColor1fi;
typedef Attrib<1, SH_INOUT, FracInt, SH_COLOR> InOutColor1fi;
typedef Attrib<1, SH_TEMP, FracInt, SH_COLOR>  Color1fi;
typedef Attrib<1, SH_CONST, FracInt, SH_COLOR> ConstColor1fi;
typedef Attrib<2, SH_INPUT, FracInt, SH_COLOR> InputColor2fi;
typedef Attrib<2, SH_OUTPUT, FracInt, SH_COLOR> OutputColor2fi;
typedef Attrib<2, SH_INOUT, FracInt, SH_COLOR> InOutColor2fi;
typedef Attrib<2, SH_TEMP, FracInt, SH_COLOR>  Color2fi;
typedef Attrib<2, SH_CONST, FracInt, SH_COLOR> ConstColor2fi;
typedef Attrib<3, SH_INPUT, FracInt, SH_COLOR> InputColor3fi;
typedef Attrib<3, SH_OUTPUT, FracInt, SH_COLOR> OutputColor3fi;
typedef Attrib<3, SH_INOUT, FracInt, SH_COLOR> InOutColor3fi;
typedef Attrib<3, SH_TEMP, FracInt, SH_COLOR>  Color3fi;
typedef Attrib<3, SH_CONST, FracInt, SH_COLOR> ConstColor3fi;
typedef Attrib<4, SH_INPUT, FracInt, SH_COLOR> InputColor4fi;
typedef Attrib<4, SH_OUTPUT, FracInt, SH_COLOR> OutputColor4fi;
typedef Attrib<4, SH_INOUT, FracInt, SH_COLOR> InOutColor4fi;
typedef Attrib<4, SH_TEMP, FracInt, SH_COLOR>  Color4fi;
typedef Attrib<4, SH_CONST, FracInt, SH_COLOR> ConstColor4fi;


typedef Attrib<1, SH_INPUT, FracShort, SH_COLOR> InputColor1fs;
typedef Attrib<1, SH_OUTPUT, FracShort, SH_COLOR> OutputColor1fs;
typedef Attrib<1, SH_INOUT, FracShort, SH_COLOR> InOutColor1fs;
typedef Attrib<1, SH_TEMP, FracShort, SH_COLOR>  Color1fs;
typedef Attrib<1, SH_CONST, FracShort, SH_COLOR> ConstColor1fs;
typedef Attrib<2, SH_INPUT, FracShort, SH_COLOR> InputColor2fs;
typedef Attrib<2, SH_OUTPUT, FracShort, SH_COLOR> OutputColor2fs;
typedef Attrib<2, SH_INOUT, FracShort, SH_COLOR> InOutColor2fs;
typedef Attrib<2, SH_TEMP, FracShort, SH_COLOR>  Color2fs;
typedef Attrib<2, SH_CONST, FracShort, SH_COLOR> ConstColor2fs;
typedef Attrib<3, SH_INPUT, FracShort, SH_COLOR> InputColor3fs;
typedef Attrib<3, SH_OUTPUT, FracShort, SH_COLOR> OutputColor3fs;
typedef Attrib<3, SH_INOUT, FracShort, SH_COLOR> InOutColor3fs;
typedef Attrib<3, SH_TEMP, FracShort, SH_COLOR>  Color3fs;
typedef Attrib<3, SH_CONST, FracShort, SH_COLOR> ConstColor3fs;
typedef Attrib<4, SH_INPUT, FracShort, SH_COLOR> InputColor4fs;
typedef Attrib<4, SH_OUTPUT, FracShort, SH_COLOR> OutputColor4fs;
typedef Attrib<4, SH_INOUT, FracShort, SH_COLOR> InOutColor4fs;
typedef Attrib<4, SH_TEMP, FracShort, SH_COLOR>  Color4fs;
typedef Attrib<4, SH_CONST, FracShort, SH_COLOR> ConstColor4fs;


typedef Attrib<1, SH_INPUT, char, SH_COLOR> InputColor1b;
typedef Attrib<1, SH_OUTPUT, char, SH_COLOR> OutputColor1b;
typedef Attrib<1, SH_INOUT, char, SH_COLOR> InOutColor1b;
typedef Attrib<1, SH_TEMP, char, SH_COLOR>  Color1b;
typedef Attrib<1, SH_CONST, char, SH_COLOR> ConstColor1b;
typedef Attrib<2, SH_INPUT, char, SH_COLOR> InputColor2b;
typedef Attrib<2, SH_OUTPUT, char, SH_COLOR> OutputColor2b;
typedef Attrib<2, SH_INOUT, char, SH_COLOR> InOutColor2b;
typedef Attrib<2, SH_TEMP, char, SH_COLOR>  Color2b;
typedef Attrib<2, SH_CONST, char, SH_COLOR> ConstColor2b;
typedef Attrib<3, SH_INPUT, char, SH_COLOR> InputColor3b;
typedef Attrib<3, SH_OUTPUT, char, SH_COLOR> OutputColor3b;
typedef Attrib<3, SH_INOUT, char, SH_COLOR> InOutColor3b;
typedef Attrib<3, SH_TEMP, char, SH_COLOR>  Color3b;
typedef Attrib<3, SH_CONST, char, SH_COLOR> ConstColor3b;
typedef Attrib<4, SH_INPUT, char, SH_COLOR> InputColor4b;
typedef Attrib<4, SH_OUTPUT, char, SH_COLOR> OutputColor4b;
typedef Attrib<4, SH_INOUT, char, SH_COLOR> InOutColor4b;
typedef Attrib<4, SH_TEMP, char, SH_COLOR>  Color4b;
typedef Attrib<4, SH_CONST, char, SH_COLOR> ConstColor4b;


typedef Attrib<1, SH_INPUT, unsigned short, SH_COLOR> InputColor1us;
typedef Attrib<1, SH_OUTPUT, unsigned short, SH_COLOR> OutputColor1us;
typedef Attrib<1, SH_INOUT, unsigned short, SH_COLOR> InOutColor1us;
typedef Attrib<1, SH_TEMP, unsigned short, SH_COLOR>  Color1us;
typedef Attrib<1, SH_CONST, unsigned short, SH_COLOR> ConstColor1us;
typedef Attrib<2, SH_INPUT, unsigned short, SH_COLOR> InputColor2us;
typedef Attrib<2, SH_OUTPUT, unsigned short, SH_COLOR> OutputColor2us;
typedef Attrib<2, SH_INOUT, unsigned short, SH_COLOR> InOutColor2us;
typedef Attrib<2, SH_TEMP, unsigned short, SH_COLOR>  Color2us;
typedef Attrib<2, SH_CONST, unsigned short, SH_COLOR> ConstColor2us;
typedef Attrib<3, SH_INPUT, unsigned short, SH_COLOR> InputColor3us;
typedef Attrib<3, SH_OUTPUT, unsigned short, SH_COLOR> OutputColor3us;
typedef Attrib<3, SH_INOUT, unsigned short, SH_COLOR> InOutColor3us;
typedef Attrib<3, SH_TEMP, unsigned short, SH_COLOR>  Color3us;
typedef Attrib<3, SH_CONST, unsigned short, SH_COLOR> ConstColor3us;
typedef Attrib<4, SH_INPUT, unsigned short, SH_COLOR> InputColor4us;
typedef Attrib<4, SH_OUTPUT, unsigned short, SH_COLOR> OutputColor4us;
typedef Attrib<4, SH_INOUT, unsigned short, SH_COLOR> InOutColor4us;
typedef Attrib<4, SH_TEMP, unsigned short, SH_COLOR>  Color4us;
typedef Attrib<4, SH_CONST, unsigned short, SH_COLOR> ConstColor4us;


typedef Attrib<1, SH_INPUT, FracByte, SH_COLOR> InputColor1fb;
typedef Attrib<1, SH_OUTPUT, FracByte, SH_COLOR> OutputColor1fb;
typedef Attrib<1, SH_INOUT, FracByte, SH_COLOR> InOutColor1fb;
typedef Attrib<1, SH_TEMP, FracByte, SH_COLOR>  Color1fb;
typedef Attrib<1, SH_CONST, FracByte, SH_COLOR> ConstColor1fb;
typedef Attrib<2, SH_INPUT, FracByte, SH_COLOR> InputColor2fb;
typedef Attrib<2, SH_OUTPUT, FracByte, SH_COLOR> OutputColor2fb;
typedef Attrib<2, SH_INOUT, FracByte, SH_COLOR> InOutColor2fb;
typedef Attrib<2, SH_TEMP, FracByte, SH_COLOR>  Color2fb;
typedef Attrib<2, SH_CONST, FracByte, SH_COLOR> ConstColor2fb;
typedef Attrib<3, SH_INPUT, FracByte, SH_COLOR> InputColor3fb;
typedef Attrib<3, SH_OUTPUT, FracByte, SH_COLOR> OutputColor3fb;
typedef Attrib<3, SH_INOUT, FracByte, SH_COLOR> InOutColor3fb;
typedef Attrib<3, SH_TEMP, FracByte, SH_COLOR>  Color3fb;
typedef Attrib<3, SH_CONST, FracByte, SH_COLOR> ConstColor3fb;
typedef Attrib<4, SH_INPUT, FracByte, SH_COLOR> InputColor4fb;
typedef Attrib<4, SH_OUTPUT, FracByte, SH_COLOR> OutputColor4fb;
typedef Attrib<4, SH_INOUT, FracByte, SH_COLOR> InOutColor4fb;
typedef Attrib<4, SH_TEMP, FracByte, SH_COLOR>  Color4fb;
typedef Attrib<4, SH_CONST, FracByte, SH_COLOR> ConstColor4fb;


typedef Attrib<1, SH_INPUT, Half, SH_COLOR> InputColor1h;
typedef Attrib<1, SH_OUTPUT, Half, SH_COLOR> OutputColor1h;
typedef Attrib<1, SH_INOUT, Half, SH_COLOR> InOutColor1h;
typedef Attrib<1, SH_TEMP, Half, SH_COLOR>  Color1h;
typedef Attrib<1, SH_CONST, Half, SH_COLOR> ConstColor1h;
typedef Attrib<2, SH_INPUT, Half, SH_COLOR> InputColor2h;
typedef Attrib<2, SH_OUTPUT, Half, SH_COLOR> OutputColor2h;
typedef Attrib<2, SH_INOUT, Half, SH_COLOR> InOutColor2h;
typedef Attrib<2, SH_TEMP, Half, SH_COLOR>  Color2h;
typedef Attrib<2, SH_CONST, Half, SH_COLOR> ConstColor2h;
typedef Attrib<3, SH_INPUT, Half, SH_COLOR> InputColor3h;
typedef Attrib<3, SH_OUTPUT, Half, SH_COLOR> OutputColor3h;
typedef Attrib<3, SH_INOUT, Half, SH_COLOR> InOutColor3h;
typedef Attrib<3, SH_TEMP, Half, SH_COLOR>  Color3h;
typedef Attrib<3, SH_CONST, Half, SH_COLOR> ConstColor3h;
typedef Attrib<4, SH_INPUT, Half, SH_COLOR> InputColor4h;
typedef Attrib<4, SH_OUTPUT, Half, SH_COLOR> OutputColor4h;
typedef Attrib<4, SH_INOUT, Half, SH_COLOR> InOutColor4h;
typedef Attrib<4, SH_TEMP, Half, SH_COLOR>  Color4h;
typedef Attrib<4, SH_CONST, Half, SH_COLOR> ConstColor4h;


typedef Attrib<1, SH_INPUT, FracUInt, SH_COLOR> InputColor1fui;
typedef Attrib<1, SH_OUTPUT, FracUInt, SH_COLOR> OutputColor1fui;
typedef Attrib<1, SH_INOUT, FracUInt, SH_COLOR> InOutColor1fui;
typedef Attrib<1, SH_TEMP, FracUInt, SH_COLOR>  Color1fui;
typedef Attrib<1, SH_CONST, FracUInt, SH_COLOR> ConstColor1fui;
typedef Attrib<2, SH_INPUT, FracUInt, SH_COLOR> InputColor2fui;
typedef Attrib<2, SH_OUTPUT, FracUInt, SH_COLOR> OutputColor2fui;
typedef Attrib<2, SH_INOUT, FracUInt, SH_COLOR> InOutColor2fui;
typedef Attrib<2, SH_TEMP, FracUInt, SH_COLOR>  Color2fui;
typedef Attrib<2, SH_CONST, FracUInt, SH_COLOR> ConstColor2fui;
typedef Attrib<3, SH_INPUT, FracUInt, SH_COLOR> InputColor3fui;
typedef Attrib<3, SH_OUTPUT, FracUInt, SH_COLOR> OutputColor3fui;
typedef Attrib<3, SH_INOUT, FracUInt, SH_COLOR> InOutColor3fui;
typedef Attrib<3, SH_TEMP, FracUInt, SH_COLOR>  Color3fui;
typedef Attrib<3, SH_CONST, FracUInt, SH_COLOR> ConstColor3fui;
typedef Attrib<4, SH_INPUT, FracUInt, SH_COLOR> InputColor4fui;
typedef Attrib<4, SH_OUTPUT, FracUInt, SH_COLOR> OutputColor4fui;
typedef Attrib<4, SH_INOUT, FracUInt, SH_COLOR> InOutColor4fui;
typedef Attrib<4, SH_TEMP, FracUInt, SH_COLOR>  Color4fui;
typedef Attrib<4, SH_CONST, FracUInt, SH_COLOR> ConstColor4fui;


typedef Attrib<1, SH_INPUT, unsigned int, SH_COLOR> InputColor1ui;
typedef Attrib<1, SH_OUTPUT, unsigned int, SH_COLOR> OutputColor1ui;
typedef Attrib<1, SH_INOUT, unsigned int, SH_COLOR> InOutColor1ui;
typedef Attrib<1, SH_TEMP, unsigned int, SH_COLOR>  Color1ui;
typedef Attrib<1, SH_CONST, unsigned int, SH_COLOR> ConstColor1ui;
typedef Attrib<2, SH_INPUT, unsigned int, SH_COLOR> InputColor2ui;
typedef Attrib<2, SH_OUTPUT, unsigned int, SH_COLOR> OutputColor2ui;
typedef Attrib<2, SH_INOUT, unsigned int, SH_COLOR> InOutColor2ui;
typedef Attrib<2, SH_TEMP, unsigned int, SH_COLOR>  Color2ui;
typedef Attrib<2, SH_CONST, unsigned int, SH_COLOR> ConstColor2ui;
typedef Attrib<3, SH_INPUT, unsigned int, SH_COLOR> InputColor3ui;
typedef Attrib<3, SH_OUTPUT, unsigned int, SH_COLOR> OutputColor3ui;
typedef Attrib<3, SH_INOUT, unsigned int, SH_COLOR> InOutColor3ui;
typedef Attrib<3, SH_TEMP, unsigned int, SH_COLOR>  Color3ui;
typedef Attrib<3, SH_CONST, unsigned int, SH_COLOR> ConstColor3ui;
typedef Attrib<4, SH_INPUT, unsigned int, SH_COLOR> InputColor4ui;
typedef Attrib<4, SH_OUTPUT, unsigned int, SH_COLOR> OutputColor4ui;
typedef Attrib<4, SH_INOUT, unsigned int, SH_COLOR> InOutColor4ui;
typedef Attrib<4, SH_TEMP, unsigned int, SH_COLOR>  Color4ui;
typedef Attrib<4, SH_CONST, unsigned int, SH_COLOR> ConstColor4ui;



} // namespace SH

#endif // SHCOLOR_HPP
