// WARNING - DO NOT EDIT THIS FILE:
// This file was automatically generated from scripts/ShPlane.hpp.py.
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

#ifndef SH_SHPLANE_HPP
#define SH_SHPLANE_HPP

#include "ShAttrib.hpp"
namespace SH {

/** An n-plane.
 * 
 *
 */

typedef ShAttrib<1, SH_INPUT, ShFracUShort, SH_PLANE> ShInputPlane1fus;
typedef ShAttrib<1, SH_OUTPUT, ShFracUShort, SH_PLANE> ShOutputPlane1fus;
typedef ShAttrib<1, SH_INOUT, ShFracUShort, SH_PLANE> ShInOutPlane1fus;
typedef ShAttrib<1, SH_TEMP, ShFracUShort, SH_PLANE> ShPlane1fus;
typedef ShAttrib<1, SH_CONST, ShFracUShort, SH_PLANE> ShConstPlane1fus;
typedef ShAttrib<2, SH_INPUT, ShFracUShort, SH_PLANE> ShInputPlane2fus;
typedef ShAttrib<2, SH_OUTPUT, ShFracUShort, SH_PLANE> ShOutputPlane2fus;
typedef ShAttrib<2, SH_INOUT, ShFracUShort, SH_PLANE> ShInOutPlane2fus;
typedef ShAttrib<2, SH_TEMP, ShFracUShort, SH_PLANE> ShPlane2fus;
typedef ShAttrib<2, SH_CONST, ShFracUShort, SH_PLANE> ShConstPlane2fus;
typedef ShAttrib<3, SH_INPUT, ShFracUShort, SH_PLANE> ShInputPlane3fus;
typedef ShAttrib<3, SH_OUTPUT, ShFracUShort, SH_PLANE> ShOutputPlane3fus;
typedef ShAttrib<3, SH_INOUT, ShFracUShort, SH_PLANE> ShInOutPlane3fus;
typedef ShAttrib<3, SH_TEMP, ShFracUShort, SH_PLANE> ShPlane3fus;
typedef ShAttrib<3, SH_CONST, ShFracUShort, SH_PLANE> ShConstPlane3fus;
typedef ShAttrib<4, SH_INPUT, ShFracUShort, SH_PLANE> ShInputPlane4fus;
typedef ShAttrib<4, SH_OUTPUT, ShFracUShort, SH_PLANE> ShOutputPlane4fus;
typedef ShAttrib<4, SH_INOUT, ShFracUShort, SH_PLANE> ShInOutPlane4fus;
typedef ShAttrib<4, SH_TEMP, ShFracUShort, SH_PLANE> ShPlane4fus;
typedef ShAttrib<4, SH_CONST, ShFracUShort, SH_PLANE> ShConstPlane4fus;


typedef ShAttrib<1, SH_INPUT, short, SH_PLANE> ShInputPlane1s;
typedef ShAttrib<1, SH_OUTPUT, short, SH_PLANE> ShOutputPlane1s;
typedef ShAttrib<1, SH_INOUT, short, SH_PLANE> ShInOutPlane1s;
typedef ShAttrib<1, SH_TEMP, short, SH_PLANE> ShPlane1s;
typedef ShAttrib<1, SH_CONST, short, SH_PLANE> ShConstPlane1s;
typedef ShAttrib<2, SH_INPUT, short, SH_PLANE> ShInputPlane2s;
typedef ShAttrib<2, SH_OUTPUT, short, SH_PLANE> ShOutputPlane2s;
typedef ShAttrib<2, SH_INOUT, short, SH_PLANE> ShInOutPlane2s;
typedef ShAttrib<2, SH_TEMP, short, SH_PLANE> ShPlane2s;
typedef ShAttrib<2, SH_CONST, short, SH_PLANE> ShConstPlane2s;
typedef ShAttrib<3, SH_INPUT, short, SH_PLANE> ShInputPlane3s;
typedef ShAttrib<3, SH_OUTPUT, short, SH_PLANE> ShOutputPlane3s;
typedef ShAttrib<3, SH_INOUT, short, SH_PLANE> ShInOutPlane3s;
typedef ShAttrib<3, SH_TEMP, short, SH_PLANE> ShPlane3s;
typedef ShAttrib<3, SH_CONST, short, SH_PLANE> ShConstPlane3s;
typedef ShAttrib<4, SH_INPUT, short, SH_PLANE> ShInputPlane4s;
typedef ShAttrib<4, SH_OUTPUT, short, SH_PLANE> ShOutputPlane4s;
typedef ShAttrib<4, SH_INOUT, short, SH_PLANE> ShInOutPlane4s;
typedef ShAttrib<4, SH_TEMP, short, SH_PLANE> ShPlane4s;
typedef ShAttrib<4, SH_CONST, short, SH_PLANE> ShConstPlane4s;


typedef ShAttrib<1, SH_INPUT, ShFracUInt, SH_PLANE> ShInputPlane1fui;
typedef ShAttrib<1, SH_OUTPUT, ShFracUInt, SH_PLANE> ShOutputPlane1fui;
typedef ShAttrib<1, SH_INOUT, ShFracUInt, SH_PLANE> ShInOutPlane1fui;
typedef ShAttrib<1, SH_TEMP, ShFracUInt, SH_PLANE> ShPlane1fui;
typedef ShAttrib<1, SH_CONST, ShFracUInt, SH_PLANE> ShConstPlane1fui;
typedef ShAttrib<2, SH_INPUT, ShFracUInt, SH_PLANE> ShInputPlane2fui;
typedef ShAttrib<2, SH_OUTPUT, ShFracUInt, SH_PLANE> ShOutputPlane2fui;
typedef ShAttrib<2, SH_INOUT, ShFracUInt, SH_PLANE> ShInOutPlane2fui;
typedef ShAttrib<2, SH_TEMP, ShFracUInt, SH_PLANE> ShPlane2fui;
typedef ShAttrib<2, SH_CONST, ShFracUInt, SH_PLANE> ShConstPlane2fui;
typedef ShAttrib<3, SH_INPUT, ShFracUInt, SH_PLANE> ShInputPlane3fui;
typedef ShAttrib<3, SH_OUTPUT, ShFracUInt, SH_PLANE> ShOutputPlane3fui;
typedef ShAttrib<3, SH_INOUT, ShFracUInt, SH_PLANE> ShInOutPlane3fui;
typedef ShAttrib<3, SH_TEMP, ShFracUInt, SH_PLANE> ShPlane3fui;
typedef ShAttrib<3, SH_CONST, ShFracUInt, SH_PLANE> ShConstPlane3fui;
typedef ShAttrib<4, SH_INPUT, ShFracUInt, SH_PLANE> ShInputPlane4fui;
typedef ShAttrib<4, SH_OUTPUT, ShFracUInt, SH_PLANE> ShOutputPlane4fui;
typedef ShAttrib<4, SH_INOUT, ShFracUInt, SH_PLANE> ShInOutPlane4fui;
typedef ShAttrib<4, SH_TEMP, ShFracUInt, SH_PLANE> ShPlane4fui;
typedef ShAttrib<4, SH_CONST, ShFracUInt, SH_PLANE> ShConstPlane4fui;


typedef ShAttrib<1, SH_INPUT, ShFracByte, SH_PLANE> ShInputPlane1fb;
typedef ShAttrib<1, SH_OUTPUT, ShFracByte, SH_PLANE> ShOutputPlane1fb;
typedef ShAttrib<1, SH_INOUT, ShFracByte, SH_PLANE> ShInOutPlane1fb;
typedef ShAttrib<1, SH_TEMP, ShFracByte, SH_PLANE> ShPlane1fb;
typedef ShAttrib<1, SH_CONST, ShFracByte, SH_PLANE> ShConstPlane1fb;
typedef ShAttrib<2, SH_INPUT, ShFracByte, SH_PLANE> ShInputPlane2fb;
typedef ShAttrib<2, SH_OUTPUT, ShFracByte, SH_PLANE> ShOutputPlane2fb;
typedef ShAttrib<2, SH_INOUT, ShFracByte, SH_PLANE> ShInOutPlane2fb;
typedef ShAttrib<2, SH_TEMP, ShFracByte, SH_PLANE> ShPlane2fb;
typedef ShAttrib<2, SH_CONST, ShFracByte, SH_PLANE> ShConstPlane2fb;
typedef ShAttrib<3, SH_INPUT, ShFracByte, SH_PLANE> ShInputPlane3fb;
typedef ShAttrib<3, SH_OUTPUT, ShFracByte, SH_PLANE> ShOutputPlane3fb;
typedef ShAttrib<3, SH_INOUT, ShFracByte, SH_PLANE> ShInOutPlane3fb;
typedef ShAttrib<3, SH_TEMP, ShFracByte, SH_PLANE> ShPlane3fb;
typedef ShAttrib<3, SH_CONST, ShFracByte, SH_PLANE> ShConstPlane3fb;
typedef ShAttrib<4, SH_INPUT, ShFracByte, SH_PLANE> ShInputPlane4fb;
typedef ShAttrib<4, SH_OUTPUT, ShFracByte, SH_PLANE> ShOutputPlane4fb;
typedef ShAttrib<4, SH_INOUT, ShFracByte, SH_PLANE> ShInOutPlane4fb;
typedef ShAttrib<4, SH_TEMP, ShFracByte, SH_PLANE> ShPlane4fb;
typedef ShAttrib<4, SH_CONST, ShFracByte, SH_PLANE> ShConstPlane4fb;


typedef ShAttrib<1, SH_INPUT, int, SH_PLANE> ShInputPlane1i;
typedef ShAttrib<1, SH_OUTPUT, int, SH_PLANE> ShOutputPlane1i;
typedef ShAttrib<1, SH_INOUT, int, SH_PLANE> ShInOutPlane1i;
typedef ShAttrib<1, SH_TEMP, int, SH_PLANE> ShPlane1i;
typedef ShAttrib<1, SH_CONST, int, SH_PLANE> ShConstPlane1i;
typedef ShAttrib<2, SH_INPUT, int, SH_PLANE> ShInputPlane2i;
typedef ShAttrib<2, SH_OUTPUT, int, SH_PLANE> ShOutputPlane2i;
typedef ShAttrib<2, SH_INOUT, int, SH_PLANE> ShInOutPlane2i;
typedef ShAttrib<2, SH_TEMP, int, SH_PLANE> ShPlane2i;
typedef ShAttrib<2, SH_CONST, int, SH_PLANE> ShConstPlane2i;
typedef ShAttrib<3, SH_INPUT, int, SH_PLANE> ShInputPlane3i;
typedef ShAttrib<3, SH_OUTPUT, int, SH_PLANE> ShOutputPlane3i;
typedef ShAttrib<3, SH_INOUT, int, SH_PLANE> ShInOutPlane3i;
typedef ShAttrib<3, SH_TEMP, int, SH_PLANE> ShPlane3i;
typedef ShAttrib<3, SH_CONST, int, SH_PLANE> ShConstPlane3i;
typedef ShAttrib<4, SH_INPUT, int, SH_PLANE> ShInputPlane4i;
typedef ShAttrib<4, SH_OUTPUT, int, SH_PLANE> ShOutputPlane4i;
typedef ShAttrib<4, SH_INOUT, int, SH_PLANE> ShInOutPlane4i;
typedef ShAttrib<4, SH_TEMP, int, SH_PLANE> ShPlane4i;
typedef ShAttrib<4, SH_CONST, int, SH_PLANE> ShConstPlane4i;


typedef ShAttrib<1, SH_INPUT, double, SH_PLANE> ShInputPlane1d;
typedef ShAttrib<1, SH_OUTPUT, double, SH_PLANE> ShOutputPlane1d;
typedef ShAttrib<1, SH_INOUT, double, SH_PLANE> ShInOutPlane1d;
typedef ShAttrib<1, SH_TEMP, double, SH_PLANE> ShPlane1d;
typedef ShAttrib<1, SH_CONST, double, SH_PLANE> ShConstPlane1d;
typedef ShAttrib<2, SH_INPUT, double, SH_PLANE> ShInputPlane2d;
typedef ShAttrib<2, SH_OUTPUT, double, SH_PLANE> ShOutputPlane2d;
typedef ShAttrib<2, SH_INOUT, double, SH_PLANE> ShInOutPlane2d;
typedef ShAttrib<2, SH_TEMP, double, SH_PLANE> ShPlane2d;
typedef ShAttrib<2, SH_CONST, double, SH_PLANE> ShConstPlane2d;
typedef ShAttrib<3, SH_INPUT, double, SH_PLANE> ShInputPlane3d;
typedef ShAttrib<3, SH_OUTPUT, double, SH_PLANE> ShOutputPlane3d;
typedef ShAttrib<3, SH_INOUT, double, SH_PLANE> ShInOutPlane3d;
typedef ShAttrib<3, SH_TEMP, double, SH_PLANE> ShPlane3d;
typedef ShAttrib<3, SH_CONST, double, SH_PLANE> ShConstPlane3d;
typedef ShAttrib<4, SH_INPUT, double, SH_PLANE> ShInputPlane4d;
typedef ShAttrib<4, SH_OUTPUT, double, SH_PLANE> ShOutputPlane4d;
typedef ShAttrib<4, SH_INOUT, double, SH_PLANE> ShInOutPlane4d;
typedef ShAttrib<4, SH_TEMP, double, SH_PLANE> ShPlane4d;
typedef ShAttrib<4, SH_CONST, double, SH_PLANE> ShConstPlane4d;


typedef ShAttrib<1, SH_INPUT, unsigned char, SH_PLANE> ShInputPlane1ub;
typedef ShAttrib<1, SH_OUTPUT, unsigned char, SH_PLANE> ShOutputPlane1ub;
typedef ShAttrib<1, SH_INOUT, unsigned char, SH_PLANE> ShInOutPlane1ub;
typedef ShAttrib<1, SH_TEMP, unsigned char, SH_PLANE> ShPlane1ub;
typedef ShAttrib<1, SH_CONST, unsigned char, SH_PLANE> ShConstPlane1ub;
typedef ShAttrib<2, SH_INPUT, unsigned char, SH_PLANE> ShInputPlane2ub;
typedef ShAttrib<2, SH_OUTPUT, unsigned char, SH_PLANE> ShOutputPlane2ub;
typedef ShAttrib<2, SH_INOUT, unsigned char, SH_PLANE> ShInOutPlane2ub;
typedef ShAttrib<2, SH_TEMP, unsigned char, SH_PLANE> ShPlane2ub;
typedef ShAttrib<2, SH_CONST, unsigned char, SH_PLANE> ShConstPlane2ub;
typedef ShAttrib<3, SH_INPUT, unsigned char, SH_PLANE> ShInputPlane3ub;
typedef ShAttrib<3, SH_OUTPUT, unsigned char, SH_PLANE> ShOutputPlane3ub;
typedef ShAttrib<3, SH_INOUT, unsigned char, SH_PLANE> ShInOutPlane3ub;
typedef ShAttrib<3, SH_TEMP, unsigned char, SH_PLANE> ShPlane3ub;
typedef ShAttrib<3, SH_CONST, unsigned char, SH_PLANE> ShConstPlane3ub;
typedef ShAttrib<4, SH_INPUT, unsigned char, SH_PLANE> ShInputPlane4ub;
typedef ShAttrib<4, SH_OUTPUT, unsigned char, SH_PLANE> ShOutputPlane4ub;
typedef ShAttrib<4, SH_INOUT, unsigned char, SH_PLANE> ShInOutPlane4ub;
typedef ShAttrib<4, SH_TEMP, unsigned char, SH_PLANE> ShPlane4ub;
typedef ShAttrib<4, SH_CONST, unsigned char, SH_PLANE> ShConstPlane4ub;


typedef ShAttrib<1, SH_INPUT, float, SH_PLANE> ShInputPlane1f;
typedef ShAttrib<1, SH_OUTPUT, float, SH_PLANE> ShOutputPlane1f;
typedef ShAttrib<1, SH_INOUT, float, SH_PLANE> ShInOutPlane1f;
typedef ShAttrib<1, SH_TEMP, float, SH_PLANE> ShPlane1f;
typedef ShAttrib<1, SH_CONST, float, SH_PLANE> ShConstPlane1f;
typedef ShAttrib<2, SH_INPUT, float, SH_PLANE> ShInputPlane2f;
typedef ShAttrib<2, SH_OUTPUT, float, SH_PLANE> ShOutputPlane2f;
typedef ShAttrib<2, SH_INOUT, float, SH_PLANE> ShInOutPlane2f;
typedef ShAttrib<2, SH_TEMP, float, SH_PLANE> ShPlane2f;
typedef ShAttrib<2, SH_CONST, float, SH_PLANE> ShConstPlane2f;
typedef ShAttrib<3, SH_INPUT, float, SH_PLANE> ShInputPlane3f;
typedef ShAttrib<3, SH_OUTPUT, float, SH_PLANE> ShOutputPlane3f;
typedef ShAttrib<3, SH_INOUT, float, SH_PLANE> ShInOutPlane3f;
typedef ShAttrib<3, SH_TEMP, float, SH_PLANE> ShPlane3f;
typedef ShAttrib<3, SH_CONST, float, SH_PLANE> ShConstPlane3f;
typedef ShAttrib<4, SH_INPUT, float, SH_PLANE> ShInputPlane4f;
typedef ShAttrib<4, SH_OUTPUT, float, SH_PLANE> ShOutputPlane4f;
typedef ShAttrib<4, SH_INOUT, float, SH_PLANE> ShInOutPlane4f;
typedef ShAttrib<4, SH_TEMP, float, SH_PLANE> ShPlane4f;
typedef ShAttrib<4, SH_CONST, float, SH_PLANE> ShConstPlane4f;


typedef ShAttrib<1, SH_INPUT, char, SH_PLANE> ShInputPlane1b;
typedef ShAttrib<1, SH_OUTPUT, char, SH_PLANE> ShOutputPlane1b;
typedef ShAttrib<1, SH_INOUT, char, SH_PLANE> ShInOutPlane1b;
typedef ShAttrib<1, SH_TEMP, char, SH_PLANE> ShPlane1b;
typedef ShAttrib<1, SH_CONST, char, SH_PLANE> ShConstPlane1b;
typedef ShAttrib<2, SH_INPUT, char, SH_PLANE> ShInputPlane2b;
typedef ShAttrib<2, SH_OUTPUT, char, SH_PLANE> ShOutputPlane2b;
typedef ShAttrib<2, SH_INOUT, char, SH_PLANE> ShInOutPlane2b;
typedef ShAttrib<2, SH_TEMP, char, SH_PLANE> ShPlane2b;
typedef ShAttrib<2, SH_CONST, char, SH_PLANE> ShConstPlane2b;
typedef ShAttrib<3, SH_INPUT, char, SH_PLANE> ShInputPlane3b;
typedef ShAttrib<3, SH_OUTPUT, char, SH_PLANE> ShOutputPlane3b;
typedef ShAttrib<3, SH_INOUT, char, SH_PLANE> ShInOutPlane3b;
typedef ShAttrib<3, SH_TEMP, char, SH_PLANE> ShPlane3b;
typedef ShAttrib<3, SH_CONST, char, SH_PLANE> ShConstPlane3b;
typedef ShAttrib<4, SH_INPUT, char, SH_PLANE> ShInputPlane4b;
typedef ShAttrib<4, SH_OUTPUT, char, SH_PLANE> ShOutputPlane4b;
typedef ShAttrib<4, SH_INOUT, char, SH_PLANE> ShInOutPlane4b;
typedef ShAttrib<4, SH_TEMP, char, SH_PLANE> ShPlane4b;
typedef ShAttrib<4, SH_CONST, char, SH_PLANE> ShConstPlane4b;


typedef ShAttrib<1, SH_INPUT, unsigned short, SH_PLANE> ShInputPlane1us;
typedef ShAttrib<1, SH_OUTPUT, unsigned short, SH_PLANE> ShOutputPlane1us;
typedef ShAttrib<1, SH_INOUT, unsigned short, SH_PLANE> ShInOutPlane1us;
typedef ShAttrib<1, SH_TEMP, unsigned short, SH_PLANE> ShPlane1us;
typedef ShAttrib<1, SH_CONST, unsigned short, SH_PLANE> ShConstPlane1us;
typedef ShAttrib<2, SH_INPUT, unsigned short, SH_PLANE> ShInputPlane2us;
typedef ShAttrib<2, SH_OUTPUT, unsigned short, SH_PLANE> ShOutputPlane2us;
typedef ShAttrib<2, SH_INOUT, unsigned short, SH_PLANE> ShInOutPlane2us;
typedef ShAttrib<2, SH_TEMP, unsigned short, SH_PLANE> ShPlane2us;
typedef ShAttrib<2, SH_CONST, unsigned short, SH_PLANE> ShConstPlane2us;
typedef ShAttrib<3, SH_INPUT, unsigned short, SH_PLANE> ShInputPlane3us;
typedef ShAttrib<3, SH_OUTPUT, unsigned short, SH_PLANE> ShOutputPlane3us;
typedef ShAttrib<3, SH_INOUT, unsigned short, SH_PLANE> ShInOutPlane3us;
typedef ShAttrib<3, SH_TEMP, unsigned short, SH_PLANE> ShPlane3us;
typedef ShAttrib<3, SH_CONST, unsigned short, SH_PLANE> ShConstPlane3us;
typedef ShAttrib<4, SH_INPUT, unsigned short, SH_PLANE> ShInputPlane4us;
typedef ShAttrib<4, SH_OUTPUT, unsigned short, SH_PLANE> ShOutputPlane4us;
typedef ShAttrib<4, SH_INOUT, unsigned short, SH_PLANE> ShInOutPlane4us;
typedef ShAttrib<4, SH_TEMP, unsigned short, SH_PLANE> ShPlane4us;
typedef ShAttrib<4, SH_CONST, unsigned short, SH_PLANE> ShConstPlane4us;


typedef ShAttrib<1, SH_INPUT, ShFracUByte, SH_PLANE> ShInputPlane1fub;
typedef ShAttrib<1, SH_OUTPUT, ShFracUByte, SH_PLANE> ShOutputPlane1fub;
typedef ShAttrib<1, SH_INOUT, ShFracUByte, SH_PLANE> ShInOutPlane1fub;
typedef ShAttrib<1, SH_TEMP, ShFracUByte, SH_PLANE> ShPlane1fub;
typedef ShAttrib<1, SH_CONST, ShFracUByte, SH_PLANE> ShConstPlane1fub;
typedef ShAttrib<2, SH_INPUT, ShFracUByte, SH_PLANE> ShInputPlane2fub;
typedef ShAttrib<2, SH_OUTPUT, ShFracUByte, SH_PLANE> ShOutputPlane2fub;
typedef ShAttrib<2, SH_INOUT, ShFracUByte, SH_PLANE> ShInOutPlane2fub;
typedef ShAttrib<2, SH_TEMP, ShFracUByte, SH_PLANE> ShPlane2fub;
typedef ShAttrib<2, SH_CONST, ShFracUByte, SH_PLANE> ShConstPlane2fub;
typedef ShAttrib<3, SH_INPUT, ShFracUByte, SH_PLANE> ShInputPlane3fub;
typedef ShAttrib<3, SH_OUTPUT, ShFracUByte, SH_PLANE> ShOutputPlane3fub;
typedef ShAttrib<3, SH_INOUT, ShFracUByte, SH_PLANE> ShInOutPlane3fub;
typedef ShAttrib<3, SH_TEMP, ShFracUByte, SH_PLANE> ShPlane3fub;
typedef ShAttrib<3, SH_CONST, ShFracUByte, SH_PLANE> ShConstPlane3fub;
typedef ShAttrib<4, SH_INPUT, ShFracUByte, SH_PLANE> ShInputPlane4fub;
typedef ShAttrib<4, SH_OUTPUT, ShFracUByte, SH_PLANE> ShOutputPlane4fub;
typedef ShAttrib<4, SH_INOUT, ShFracUByte, SH_PLANE> ShInOutPlane4fub;
typedef ShAttrib<4, SH_TEMP, ShFracUByte, SH_PLANE> ShPlane4fub;
typedef ShAttrib<4, SH_CONST, ShFracUByte, SH_PLANE> ShConstPlane4fub;


typedef ShAttrib<1, SH_INPUT, ShHalf, SH_PLANE> ShInputPlane1h;
typedef ShAttrib<1, SH_OUTPUT, ShHalf, SH_PLANE> ShOutputPlane1h;
typedef ShAttrib<1, SH_INOUT, ShHalf, SH_PLANE> ShInOutPlane1h;
typedef ShAttrib<1, SH_TEMP, ShHalf, SH_PLANE> ShPlane1h;
typedef ShAttrib<1, SH_CONST, ShHalf, SH_PLANE> ShConstPlane1h;
typedef ShAttrib<2, SH_INPUT, ShHalf, SH_PLANE> ShInputPlane2h;
typedef ShAttrib<2, SH_OUTPUT, ShHalf, SH_PLANE> ShOutputPlane2h;
typedef ShAttrib<2, SH_INOUT, ShHalf, SH_PLANE> ShInOutPlane2h;
typedef ShAttrib<2, SH_TEMP, ShHalf, SH_PLANE> ShPlane2h;
typedef ShAttrib<2, SH_CONST, ShHalf, SH_PLANE> ShConstPlane2h;
typedef ShAttrib<3, SH_INPUT, ShHalf, SH_PLANE> ShInputPlane3h;
typedef ShAttrib<3, SH_OUTPUT, ShHalf, SH_PLANE> ShOutputPlane3h;
typedef ShAttrib<3, SH_INOUT, ShHalf, SH_PLANE> ShInOutPlane3h;
typedef ShAttrib<3, SH_TEMP, ShHalf, SH_PLANE> ShPlane3h;
typedef ShAttrib<3, SH_CONST, ShHalf, SH_PLANE> ShConstPlane3h;
typedef ShAttrib<4, SH_INPUT, ShHalf, SH_PLANE> ShInputPlane4h;
typedef ShAttrib<4, SH_OUTPUT, ShHalf, SH_PLANE> ShOutputPlane4h;
typedef ShAttrib<4, SH_INOUT, ShHalf, SH_PLANE> ShInOutPlane4h;
typedef ShAttrib<4, SH_TEMP, ShHalf, SH_PLANE> ShPlane4h;
typedef ShAttrib<4, SH_CONST, ShHalf, SH_PLANE> ShConstPlane4h;


typedef ShAttrib<1, SH_INPUT, ShFracShort, SH_PLANE> ShInputPlane1fs;
typedef ShAttrib<1, SH_OUTPUT, ShFracShort, SH_PLANE> ShOutputPlane1fs;
typedef ShAttrib<1, SH_INOUT, ShFracShort, SH_PLANE> ShInOutPlane1fs;
typedef ShAttrib<1, SH_TEMP, ShFracShort, SH_PLANE> ShPlane1fs;
typedef ShAttrib<1, SH_CONST, ShFracShort, SH_PLANE> ShConstPlane1fs;
typedef ShAttrib<2, SH_INPUT, ShFracShort, SH_PLANE> ShInputPlane2fs;
typedef ShAttrib<2, SH_OUTPUT, ShFracShort, SH_PLANE> ShOutputPlane2fs;
typedef ShAttrib<2, SH_INOUT, ShFracShort, SH_PLANE> ShInOutPlane2fs;
typedef ShAttrib<2, SH_TEMP, ShFracShort, SH_PLANE> ShPlane2fs;
typedef ShAttrib<2, SH_CONST, ShFracShort, SH_PLANE> ShConstPlane2fs;
typedef ShAttrib<3, SH_INPUT, ShFracShort, SH_PLANE> ShInputPlane3fs;
typedef ShAttrib<3, SH_OUTPUT, ShFracShort, SH_PLANE> ShOutputPlane3fs;
typedef ShAttrib<3, SH_INOUT, ShFracShort, SH_PLANE> ShInOutPlane3fs;
typedef ShAttrib<3, SH_TEMP, ShFracShort, SH_PLANE> ShPlane3fs;
typedef ShAttrib<3, SH_CONST, ShFracShort, SH_PLANE> ShConstPlane3fs;
typedef ShAttrib<4, SH_INPUT, ShFracShort, SH_PLANE> ShInputPlane4fs;
typedef ShAttrib<4, SH_OUTPUT, ShFracShort, SH_PLANE> ShOutputPlane4fs;
typedef ShAttrib<4, SH_INOUT, ShFracShort, SH_PLANE> ShInOutPlane4fs;
typedef ShAttrib<4, SH_TEMP, ShFracShort, SH_PLANE> ShPlane4fs;
typedef ShAttrib<4, SH_CONST, ShFracShort, SH_PLANE> ShConstPlane4fs;


typedef ShAttrib<1, SH_INPUT, ShFracInt, SH_PLANE> ShInputPlane1fi;
typedef ShAttrib<1, SH_OUTPUT, ShFracInt, SH_PLANE> ShOutputPlane1fi;
typedef ShAttrib<1, SH_INOUT, ShFracInt, SH_PLANE> ShInOutPlane1fi;
typedef ShAttrib<1, SH_TEMP, ShFracInt, SH_PLANE> ShPlane1fi;
typedef ShAttrib<1, SH_CONST, ShFracInt, SH_PLANE> ShConstPlane1fi;
typedef ShAttrib<2, SH_INPUT, ShFracInt, SH_PLANE> ShInputPlane2fi;
typedef ShAttrib<2, SH_OUTPUT, ShFracInt, SH_PLANE> ShOutputPlane2fi;
typedef ShAttrib<2, SH_INOUT, ShFracInt, SH_PLANE> ShInOutPlane2fi;
typedef ShAttrib<2, SH_TEMP, ShFracInt, SH_PLANE> ShPlane2fi;
typedef ShAttrib<2, SH_CONST, ShFracInt, SH_PLANE> ShConstPlane2fi;
typedef ShAttrib<3, SH_INPUT, ShFracInt, SH_PLANE> ShInputPlane3fi;
typedef ShAttrib<3, SH_OUTPUT, ShFracInt, SH_PLANE> ShOutputPlane3fi;
typedef ShAttrib<3, SH_INOUT, ShFracInt, SH_PLANE> ShInOutPlane3fi;
typedef ShAttrib<3, SH_TEMP, ShFracInt, SH_PLANE> ShPlane3fi;
typedef ShAttrib<3, SH_CONST, ShFracInt, SH_PLANE> ShConstPlane3fi;
typedef ShAttrib<4, SH_INPUT, ShFracInt, SH_PLANE> ShInputPlane4fi;
typedef ShAttrib<4, SH_OUTPUT, ShFracInt, SH_PLANE> ShOutputPlane4fi;
typedef ShAttrib<4, SH_INOUT, ShFracInt, SH_PLANE> ShInOutPlane4fi;
typedef ShAttrib<4, SH_TEMP, ShFracInt, SH_PLANE> ShPlane4fi;
typedef ShAttrib<4, SH_CONST, ShFracInt, SH_PLANE> ShConstPlane4fi;


typedef ShAttrib<1, SH_INPUT, unsigned int, SH_PLANE> ShInputPlane1ui;
typedef ShAttrib<1, SH_OUTPUT, unsigned int, SH_PLANE> ShOutputPlane1ui;
typedef ShAttrib<1, SH_INOUT, unsigned int, SH_PLANE> ShInOutPlane1ui;
typedef ShAttrib<1, SH_TEMP, unsigned int, SH_PLANE> ShPlane1ui;
typedef ShAttrib<1, SH_CONST, unsigned int, SH_PLANE> ShConstPlane1ui;
typedef ShAttrib<2, SH_INPUT, unsigned int, SH_PLANE> ShInputPlane2ui;
typedef ShAttrib<2, SH_OUTPUT, unsigned int, SH_PLANE> ShOutputPlane2ui;
typedef ShAttrib<2, SH_INOUT, unsigned int, SH_PLANE> ShInOutPlane2ui;
typedef ShAttrib<2, SH_TEMP, unsigned int, SH_PLANE> ShPlane2ui;
typedef ShAttrib<2, SH_CONST, unsigned int, SH_PLANE> ShConstPlane2ui;
typedef ShAttrib<3, SH_INPUT, unsigned int, SH_PLANE> ShInputPlane3ui;
typedef ShAttrib<3, SH_OUTPUT, unsigned int, SH_PLANE> ShOutputPlane3ui;
typedef ShAttrib<3, SH_INOUT, unsigned int, SH_PLANE> ShInOutPlane3ui;
typedef ShAttrib<3, SH_TEMP, unsigned int, SH_PLANE> ShPlane3ui;
typedef ShAttrib<3, SH_CONST, unsigned int, SH_PLANE> ShConstPlane3ui;
typedef ShAttrib<4, SH_INPUT, unsigned int, SH_PLANE> ShInputPlane4ui;
typedef ShAttrib<4, SH_OUTPUT, unsigned int, SH_PLANE> ShOutputPlane4ui;
typedef ShAttrib<4, SH_INOUT, unsigned int, SH_PLANE> ShInOutPlane4ui;
typedef ShAttrib<4, SH_TEMP, unsigned int, SH_PLANE> ShPlane4ui;
typedef ShAttrib<4, SH_CONST, unsigned int, SH_PLANE> ShConstPlane4ui;



} // namespace SH

#endif // SH_SHPLANE_HPP