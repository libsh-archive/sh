// WARNING - DO NOT EDIT THIS FILE:
// This file was automatically generated from scripts/Plane.hpp.py.
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

#ifndef SHPLANE_HPP
#define SHPLANE_HPP

#include "Attrib.hpp"
namespace SH {

/** An n-plane.
 * 
 *
 */

typedef Attrib<1, SH_INPUT, short, SH_PLANE> InputPlane1s;
typedef Attrib<1, SH_OUTPUT, short, SH_PLANE> OutputPlane1s;
typedef Attrib<1, SH_INOUT, short, SH_PLANE> InOutPlane1s;
typedef Attrib<1, SH_TEMP, short, SH_PLANE>  Plane1s;
typedef Attrib<1, SH_CONST, short, SH_PLANE> ConstPlane1s;
typedef Attrib<2, SH_INPUT, short, SH_PLANE> InputPlane2s;
typedef Attrib<2, SH_OUTPUT, short, SH_PLANE> OutputPlane2s;
typedef Attrib<2, SH_INOUT, short, SH_PLANE> InOutPlane2s;
typedef Attrib<2, SH_TEMP, short, SH_PLANE>  Plane2s;
typedef Attrib<2, SH_CONST, short, SH_PLANE> ConstPlane2s;
typedef Attrib<3, SH_INPUT, short, SH_PLANE> InputPlane3s;
typedef Attrib<3, SH_OUTPUT, short, SH_PLANE> OutputPlane3s;
typedef Attrib<3, SH_INOUT, short, SH_PLANE> InOutPlane3s;
typedef Attrib<3, SH_TEMP, short, SH_PLANE>  Plane3s;
typedef Attrib<3, SH_CONST, short, SH_PLANE> ConstPlane3s;
typedef Attrib<4, SH_INPUT, short, SH_PLANE> InputPlane4s;
typedef Attrib<4, SH_OUTPUT, short, SH_PLANE> OutputPlane4s;
typedef Attrib<4, SH_INOUT, short, SH_PLANE> InOutPlane4s;
typedef Attrib<4, SH_TEMP, short, SH_PLANE>  Plane4s;
typedef Attrib<4, SH_CONST, short, SH_PLANE> ConstPlane4s;


typedef Attrib<1, SH_INPUT, FracUByte, SH_PLANE> InputPlane1fub;
typedef Attrib<1, SH_OUTPUT, FracUByte, SH_PLANE> OutputPlane1fub;
typedef Attrib<1, SH_INOUT, FracUByte, SH_PLANE> InOutPlane1fub;
typedef Attrib<1, SH_TEMP, FracUByte, SH_PLANE>  Plane1fub;
typedef Attrib<1, SH_CONST, FracUByte, SH_PLANE> ConstPlane1fub;
typedef Attrib<2, SH_INPUT, FracUByte, SH_PLANE> InputPlane2fub;
typedef Attrib<2, SH_OUTPUT, FracUByte, SH_PLANE> OutputPlane2fub;
typedef Attrib<2, SH_INOUT, FracUByte, SH_PLANE> InOutPlane2fub;
typedef Attrib<2, SH_TEMP, FracUByte, SH_PLANE>  Plane2fub;
typedef Attrib<2, SH_CONST, FracUByte, SH_PLANE> ConstPlane2fub;
typedef Attrib<3, SH_INPUT, FracUByte, SH_PLANE> InputPlane3fub;
typedef Attrib<3, SH_OUTPUT, FracUByte, SH_PLANE> OutputPlane3fub;
typedef Attrib<3, SH_INOUT, FracUByte, SH_PLANE> InOutPlane3fub;
typedef Attrib<3, SH_TEMP, FracUByte, SH_PLANE>  Plane3fub;
typedef Attrib<3, SH_CONST, FracUByte, SH_PLANE> ConstPlane3fub;
typedef Attrib<4, SH_INPUT, FracUByte, SH_PLANE> InputPlane4fub;
typedef Attrib<4, SH_OUTPUT, FracUByte, SH_PLANE> OutputPlane4fub;
typedef Attrib<4, SH_INOUT, FracUByte, SH_PLANE> InOutPlane4fub;
typedef Attrib<4, SH_TEMP, FracUByte, SH_PLANE>  Plane4fub;
typedef Attrib<4, SH_CONST, FracUByte, SH_PLANE> ConstPlane4fub;


typedef Attrib<1, SH_INPUT, FracUShort, SH_PLANE> InputPlane1fus;
typedef Attrib<1, SH_OUTPUT, FracUShort, SH_PLANE> OutputPlane1fus;
typedef Attrib<1, SH_INOUT, FracUShort, SH_PLANE> InOutPlane1fus;
typedef Attrib<1, SH_TEMP, FracUShort, SH_PLANE>  Plane1fus;
typedef Attrib<1, SH_CONST, FracUShort, SH_PLANE> ConstPlane1fus;
typedef Attrib<2, SH_INPUT, FracUShort, SH_PLANE> InputPlane2fus;
typedef Attrib<2, SH_OUTPUT, FracUShort, SH_PLANE> OutputPlane2fus;
typedef Attrib<2, SH_INOUT, FracUShort, SH_PLANE> InOutPlane2fus;
typedef Attrib<2, SH_TEMP, FracUShort, SH_PLANE>  Plane2fus;
typedef Attrib<2, SH_CONST, FracUShort, SH_PLANE> ConstPlane2fus;
typedef Attrib<3, SH_INPUT, FracUShort, SH_PLANE> InputPlane3fus;
typedef Attrib<3, SH_OUTPUT, FracUShort, SH_PLANE> OutputPlane3fus;
typedef Attrib<3, SH_INOUT, FracUShort, SH_PLANE> InOutPlane3fus;
typedef Attrib<3, SH_TEMP, FracUShort, SH_PLANE>  Plane3fus;
typedef Attrib<3, SH_CONST, FracUShort, SH_PLANE> ConstPlane3fus;
typedef Attrib<4, SH_INPUT, FracUShort, SH_PLANE> InputPlane4fus;
typedef Attrib<4, SH_OUTPUT, FracUShort, SH_PLANE> OutputPlane4fus;
typedef Attrib<4, SH_INOUT, FracUShort, SH_PLANE> InOutPlane4fus;
typedef Attrib<4, SH_TEMP, FracUShort, SH_PLANE>  Plane4fus;
typedef Attrib<4, SH_CONST, FracUShort, SH_PLANE> ConstPlane4fus;


typedef Attrib<1, SH_INPUT, int, SH_PLANE> InputPlane1i;
typedef Attrib<1, SH_OUTPUT, int, SH_PLANE> OutputPlane1i;
typedef Attrib<1, SH_INOUT, int, SH_PLANE> InOutPlane1i;
typedef Attrib<1, SH_TEMP, int, SH_PLANE>  Plane1i;
typedef Attrib<1, SH_CONST, int, SH_PLANE> ConstPlane1i;
typedef Attrib<2, SH_INPUT, int, SH_PLANE> InputPlane2i;
typedef Attrib<2, SH_OUTPUT, int, SH_PLANE> OutputPlane2i;
typedef Attrib<2, SH_INOUT, int, SH_PLANE> InOutPlane2i;
typedef Attrib<2, SH_TEMP, int, SH_PLANE>  Plane2i;
typedef Attrib<2, SH_CONST, int, SH_PLANE> ConstPlane2i;
typedef Attrib<3, SH_INPUT, int, SH_PLANE> InputPlane3i;
typedef Attrib<3, SH_OUTPUT, int, SH_PLANE> OutputPlane3i;
typedef Attrib<3, SH_INOUT, int, SH_PLANE> InOutPlane3i;
typedef Attrib<3, SH_TEMP, int, SH_PLANE>  Plane3i;
typedef Attrib<3, SH_CONST, int, SH_PLANE> ConstPlane3i;
typedef Attrib<4, SH_INPUT, int, SH_PLANE> InputPlane4i;
typedef Attrib<4, SH_OUTPUT, int, SH_PLANE> OutputPlane4i;
typedef Attrib<4, SH_INOUT, int, SH_PLANE> InOutPlane4i;
typedef Attrib<4, SH_TEMP, int, SH_PLANE>  Plane4i;
typedef Attrib<4, SH_CONST, int, SH_PLANE> ConstPlane4i;


typedef Attrib<1, SH_INPUT, double, SH_PLANE> InputPlane1d;
typedef Attrib<1, SH_OUTPUT, double, SH_PLANE> OutputPlane1d;
typedef Attrib<1, SH_INOUT, double, SH_PLANE> InOutPlane1d;
typedef Attrib<1, SH_TEMP, double, SH_PLANE>  Plane1d;
typedef Attrib<1, SH_CONST, double, SH_PLANE> ConstPlane1d;
typedef Attrib<2, SH_INPUT, double, SH_PLANE> InputPlane2d;
typedef Attrib<2, SH_OUTPUT, double, SH_PLANE> OutputPlane2d;
typedef Attrib<2, SH_INOUT, double, SH_PLANE> InOutPlane2d;
typedef Attrib<2, SH_TEMP, double, SH_PLANE>  Plane2d;
typedef Attrib<2, SH_CONST, double, SH_PLANE> ConstPlane2d;
typedef Attrib<3, SH_INPUT, double, SH_PLANE> InputPlane3d;
typedef Attrib<3, SH_OUTPUT, double, SH_PLANE> OutputPlane3d;
typedef Attrib<3, SH_INOUT, double, SH_PLANE> InOutPlane3d;
typedef Attrib<3, SH_TEMP, double, SH_PLANE>  Plane3d;
typedef Attrib<3, SH_CONST, double, SH_PLANE> ConstPlane3d;
typedef Attrib<4, SH_INPUT, double, SH_PLANE> InputPlane4d;
typedef Attrib<4, SH_OUTPUT, double, SH_PLANE> OutputPlane4d;
typedef Attrib<4, SH_INOUT, double, SH_PLANE> InOutPlane4d;
typedef Attrib<4, SH_TEMP, double, SH_PLANE>  Plane4d;
typedef Attrib<4, SH_CONST, double, SH_PLANE> ConstPlane4d;


typedef Attrib<1, SH_INPUT, unsigned char, SH_PLANE> InputPlane1ub;
typedef Attrib<1, SH_OUTPUT, unsigned char, SH_PLANE> OutputPlane1ub;
typedef Attrib<1, SH_INOUT, unsigned char, SH_PLANE> InOutPlane1ub;
typedef Attrib<1, SH_TEMP, unsigned char, SH_PLANE>  Plane1ub;
typedef Attrib<1, SH_CONST, unsigned char, SH_PLANE> ConstPlane1ub;
typedef Attrib<2, SH_INPUT, unsigned char, SH_PLANE> InputPlane2ub;
typedef Attrib<2, SH_OUTPUT, unsigned char, SH_PLANE> OutputPlane2ub;
typedef Attrib<2, SH_INOUT, unsigned char, SH_PLANE> InOutPlane2ub;
typedef Attrib<2, SH_TEMP, unsigned char, SH_PLANE>  Plane2ub;
typedef Attrib<2, SH_CONST, unsigned char, SH_PLANE> ConstPlane2ub;
typedef Attrib<3, SH_INPUT, unsigned char, SH_PLANE> InputPlane3ub;
typedef Attrib<3, SH_OUTPUT, unsigned char, SH_PLANE> OutputPlane3ub;
typedef Attrib<3, SH_INOUT, unsigned char, SH_PLANE> InOutPlane3ub;
typedef Attrib<3, SH_TEMP, unsigned char, SH_PLANE>  Plane3ub;
typedef Attrib<3, SH_CONST, unsigned char, SH_PLANE> ConstPlane3ub;
typedef Attrib<4, SH_INPUT, unsigned char, SH_PLANE> InputPlane4ub;
typedef Attrib<4, SH_OUTPUT, unsigned char, SH_PLANE> OutputPlane4ub;
typedef Attrib<4, SH_INOUT, unsigned char, SH_PLANE> InOutPlane4ub;
typedef Attrib<4, SH_TEMP, unsigned char, SH_PLANE>  Plane4ub;
typedef Attrib<4, SH_CONST, unsigned char, SH_PLANE> ConstPlane4ub;


typedef Attrib<1, SH_INPUT, float, SH_PLANE> InputPlane1f;
typedef Attrib<1, SH_OUTPUT, float, SH_PLANE> OutputPlane1f;
typedef Attrib<1, SH_INOUT, float, SH_PLANE> InOutPlane1f;
typedef Attrib<1, SH_TEMP, float, SH_PLANE>  Plane1f;
typedef Attrib<1, SH_CONST, float, SH_PLANE> ConstPlane1f;
typedef Attrib<2, SH_INPUT, float, SH_PLANE> InputPlane2f;
typedef Attrib<2, SH_OUTPUT, float, SH_PLANE> OutputPlane2f;
typedef Attrib<2, SH_INOUT, float, SH_PLANE> InOutPlane2f;
typedef Attrib<2, SH_TEMP, float, SH_PLANE>  Plane2f;
typedef Attrib<2, SH_CONST, float, SH_PLANE> ConstPlane2f;
typedef Attrib<3, SH_INPUT, float, SH_PLANE> InputPlane3f;
typedef Attrib<3, SH_OUTPUT, float, SH_PLANE> OutputPlane3f;
typedef Attrib<3, SH_INOUT, float, SH_PLANE> InOutPlane3f;
typedef Attrib<3, SH_TEMP, float, SH_PLANE>  Plane3f;
typedef Attrib<3, SH_CONST, float, SH_PLANE> ConstPlane3f;
typedef Attrib<4, SH_INPUT, float, SH_PLANE> InputPlane4f;
typedef Attrib<4, SH_OUTPUT, float, SH_PLANE> OutputPlane4f;
typedef Attrib<4, SH_INOUT, float, SH_PLANE> InOutPlane4f;
typedef Attrib<4, SH_TEMP, float, SH_PLANE>  Plane4f;
typedef Attrib<4, SH_CONST, float, SH_PLANE> ConstPlane4f;


typedef Attrib<1, SH_INPUT, FracInt, SH_PLANE> InputPlane1fi;
typedef Attrib<1, SH_OUTPUT, FracInt, SH_PLANE> OutputPlane1fi;
typedef Attrib<1, SH_INOUT, FracInt, SH_PLANE> InOutPlane1fi;
typedef Attrib<1, SH_TEMP, FracInt, SH_PLANE>  Plane1fi;
typedef Attrib<1, SH_CONST, FracInt, SH_PLANE> ConstPlane1fi;
typedef Attrib<2, SH_INPUT, FracInt, SH_PLANE> InputPlane2fi;
typedef Attrib<2, SH_OUTPUT, FracInt, SH_PLANE> OutputPlane2fi;
typedef Attrib<2, SH_INOUT, FracInt, SH_PLANE> InOutPlane2fi;
typedef Attrib<2, SH_TEMP, FracInt, SH_PLANE>  Plane2fi;
typedef Attrib<2, SH_CONST, FracInt, SH_PLANE> ConstPlane2fi;
typedef Attrib<3, SH_INPUT, FracInt, SH_PLANE> InputPlane3fi;
typedef Attrib<3, SH_OUTPUT, FracInt, SH_PLANE> OutputPlane3fi;
typedef Attrib<3, SH_INOUT, FracInt, SH_PLANE> InOutPlane3fi;
typedef Attrib<3, SH_TEMP, FracInt, SH_PLANE>  Plane3fi;
typedef Attrib<3, SH_CONST, FracInt, SH_PLANE> ConstPlane3fi;
typedef Attrib<4, SH_INPUT, FracInt, SH_PLANE> InputPlane4fi;
typedef Attrib<4, SH_OUTPUT, FracInt, SH_PLANE> OutputPlane4fi;
typedef Attrib<4, SH_INOUT, FracInt, SH_PLANE> InOutPlane4fi;
typedef Attrib<4, SH_TEMP, FracInt, SH_PLANE>  Plane4fi;
typedef Attrib<4, SH_CONST, FracInt, SH_PLANE> ConstPlane4fi;


typedef Attrib<1, SH_INPUT, FracShort, SH_PLANE> InputPlane1fs;
typedef Attrib<1, SH_OUTPUT, FracShort, SH_PLANE> OutputPlane1fs;
typedef Attrib<1, SH_INOUT, FracShort, SH_PLANE> InOutPlane1fs;
typedef Attrib<1, SH_TEMP, FracShort, SH_PLANE>  Plane1fs;
typedef Attrib<1, SH_CONST, FracShort, SH_PLANE> ConstPlane1fs;
typedef Attrib<2, SH_INPUT, FracShort, SH_PLANE> InputPlane2fs;
typedef Attrib<2, SH_OUTPUT, FracShort, SH_PLANE> OutputPlane2fs;
typedef Attrib<2, SH_INOUT, FracShort, SH_PLANE> InOutPlane2fs;
typedef Attrib<2, SH_TEMP, FracShort, SH_PLANE>  Plane2fs;
typedef Attrib<2, SH_CONST, FracShort, SH_PLANE> ConstPlane2fs;
typedef Attrib<3, SH_INPUT, FracShort, SH_PLANE> InputPlane3fs;
typedef Attrib<3, SH_OUTPUT, FracShort, SH_PLANE> OutputPlane3fs;
typedef Attrib<3, SH_INOUT, FracShort, SH_PLANE> InOutPlane3fs;
typedef Attrib<3, SH_TEMP, FracShort, SH_PLANE>  Plane3fs;
typedef Attrib<3, SH_CONST, FracShort, SH_PLANE> ConstPlane3fs;
typedef Attrib<4, SH_INPUT, FracShort, SH_PLANE> InputPlane4fs;
typedef Attrib<4, SH_OUTPUT, FracShort, SH_PLANE> OutputPlane4fs;
typedef Attrib<4, SH_INOUT, FracShort, SH_PLANE> InOutPlane4fs;
typedef Attrib<4, SH_TEMP, FracShort, SH_PLANE>  Plane4fs;
typedef Attrib<4, SH_CONST, FracShort, SH_PLANE> ConstPlane4fs;


typedef Attrib<1, SH_INPUT, char, SH_PLANE> InputPlane1b;
typedef Attrib<1, SH_OUTPUT, char, SH_PLANE> OutputPlane1b;
typedef Attrib<1, SH_INOUT, char, SH_PLANE> InOutPlane1b;
typedef Attrib<1, SH_TEMP, char, SH_PLANE>  Plane1b;
typedef Attrib<1, SH_CONST, char, SH_PLANE> ConstPlane1b;
typedef Attrib<2, SH_INPUT, char, SH_PLANE> InputPlane2b;
typedef Attrib<2, SH_OUTPUT, char, SH_PLANE> OutputPlane2b;
typedef Attrib<2, SH_INOUT, char, SH_PLANE> InOutPlane2b;
typedef Attrib<2, SH_TEMP, char, SH_PLANE>  Plane2b;
typedef Attrib<2, SH_CONST, char, SH_PLANE> ConstPlane2b;
typedef Attrib<3, SH_INPUT, char, SH_PLANE> InputPlane3b;
typedef Attrib<3, SH_OUTPUT, char, SH_PLANE> OutputPlane3b;
typedef Attrib<3, SH_INOUT, char, SH_PLANE> InOutPlane3b;
typedef Attrib<3, SH_TEMP, char, SH_PLANE>  Plane3b;
typedef Attrib<3, SH_CONST, char, SH_PLANE> ConstPlane3b;
typedef Attrib<4, SH_INPUT, char, SH_PLANE> InputPlane4b;
typedef Attrib<4, SH_OUTPUT, char, SH_PLANE> OutputPlane4b;
typedef Attrib<4, SH_INOUT, char, SH_PLANE> InOutPlane4b;
typedef Attrib<4, SH_TEMP, char, SH_PLANE>  Plane4b;
typedef Attrib<4, SH_CONST, char, SH_PLANE> ConstPlane4b;


typedef Attrib<1, SH_INPUT, unsigned short, SH_PLANE> InputPlane1us;
typedef Attrib<1, SH_OUTPUT, unsigned short, SH_PLANE> OutputPlane1us;
typedef Attrib<1, SH_INOUT, unsigned short, SH_PLANE> InOutPlane1us;
typedef Attrib<1, SH_TEMP, unsigned short, SH_PLANE>  Plane1us;
typedef Attrib<1, SH_CONST, unsigned short, SH_PLANE> ConstPlane1us;
typedef Attrib<2, SH_INPUT, unsigned short, SH_PLANE> InputPlane2us;
typedef Attrib<2, SH_OUTPUT, unsigned short, SH_PLANE> OutputPlane2us;
typedef Attrib<2, SH_INOUT, unsigned short, SH_PLANE> InOutPlane2us;
typedef Attrib<2, SH_TEMP, unsigned short, SH_PLANE>  Plane2us;
typedef Attrib<2, SH_CONST, unsigned short, SH_PLANE> ConstPlane2us;
typedef Attrib<3, SH_INPUT, unsigned short, SH_PLANE> InputPlane3us;
typedef Attrib<3, SH_OUTPUT, unsigned short, SH_PLANE> OutputPlane3us;
typedef Attrib<3, SH_INOUT, unsigned short, SH_PLANE> InOutPlane3us;
typedef Attrib<3, SH_TEMP, unsigned short, SH_PLANE>  Plane3us;
typedef Attrib<3, SH_CONST, unsigned short, SH_PLANE> ConstPlane3us;
typedef Attrib<4, SH_INPUT, unsigned short, SH_PLANE> InputPlane4us;
typedef Attrib<4, SH_OUTPUT, unsigned short, SH_PLANE> OutputPlane4us;
typedef Attrib<4, SH_INOUT, unsigned short, SH_PLANE> InOutPlane4us;
typedef Attrib<4, SH_TEMP, unsigned short, SH_PLANE>  Plane4us;
typedef Attrib<4, SH_CONST, unsigned short, SH_PLANE> ConstPlane4us;


typedef Attrib<1, SH_INPUT, FracByte, SH_PLANE> InputPlane1fb;
typedef Attrib<1, SH_OUTPUT, FracByte, SH_PLANE> OutputPlane1fb;
typedef Attrib<1, SH_INOUT, FracByte, SH_PLANE> InOutPlane1fb;
typedef Attrib<1, SH_TEMP, FracByte, SH_PLANE>  Plane1fb;
typedef Attrib<1, SH_CONST, FracByte, SH_PLANE> ConstPlane1fb;
typedef Attrib<2, SH_INPUT, FracByte, SH_PLANE> InputPlane2fb;
typedef Attrib<2, SH_OUTPUT, FracByte, SH_PLANE> OutputPlane2fb;
typedef Attrib<2, SH_INOUT, FracByte, SH_PLANE> InOutPlane2fb;
typedef Attrib<2, SH_TEMP, FracByte, SH_PLANE>  Plane2fb;
typedef Attrib<2, SH_CONST, FracByte, SH_PLANE> ConstPlane2fb;
typedef Attrib<3, SH_INPUT, FracByte, SH_PLANE> InputPlane3fb;
typedef Attrib<3, SH_OUTPUT, FracByte, SH_PLANE> OutputPlane3fb;
typedef Attrib<3, SH_INOUT, FracByte, SH_PLANE> InOutPlane3fb;
typedef Attrib<3, SH_TEMP, FracByte, SH_PLANE>  Plane3fb;
typedef Attrib<3, SH_CONST, FracByte, SH_PLANE> ConstPlane3fb;
typedef Attrib<4, SH_INPUT, FracByte, SH_PLANE> InputPlane4fb;
typedef Attrib<4, SH_OUTPUT, FracByte, SH_PLANE> OutputPlane4fb;
typedef Attrib<4, SH_INOUT, FracByte, SH_PLANE> InOutPlane4fb;
typedef Attrib<4, SH_TEMP, FracByte, SH_PLANE>  Plane4fb;
typedef Attrib<4, SH_CONST, FracByte, SH_PLANE> ConstPlane4fb;


typedef Attrib<1, SH_INPUT, Half, SH_PLANE> InputPlane1h;
typedef Attrib<1, SH_OUTPUT, Half, SH_PLANE> OutputPlane1h;
typedef Attrib<1, SH_INOUT, Half, SH_PLANE> InOutPlane1h;
typedef Attrib<1, SH_TEMP, Half, SH_PLANE>  Plane1h;
typedef Attrib<1, SH_CONST, Half, SH_PLANE> ConstPlane1h;
typedef Attrib<2, SH_INPUT, Half, SH_PLANE> InputPlane2h;
typedef Attrib<2, SH_OUTPUT, Half, SH_PLANE> OutputPlane2h;
typedef Attrib<2, SH_INOUT, Half, SH_PLANE> InOutPlane2h;
typedef Attrib<2, SH_TEMP, Half, SH_PLANE>  Plane2h;
typedef Attrib<2, SH_CONST, Half, SH_PLANE> ConstPlane2h;
typedef Attrib<3, SH_INPUT, Half, SH_PLANE> InputPlane3h;
typedef Attrib<3, SH_OUTPUT, Half, SH_PLANE> OutputPlane3h;
typedef Attrib<3, SH_INOUT, Half, SH_PLANE> InOutPlane3h;
typedef Attrib<3, SH_TEMP, Half, SH_PLANE>  Plane3h;
typedef Attrib<3, SH_CONST, Half, SH_PLANE> ConstPlane3h;
typedef Attrib<4, SH_INPUT, Half, SH_PLANE> InputPlane4h;
typedef Attrib<4, SH_OUTPUT, Half, SH_PLANE> OutputPlane4h;
typedef Attrib<4, SH_INOUT, Half, SH_PLANE> InOutPlane4h;
typedef Attrib<4, SH_TEMP, Half, SH_PLANE>  Plane4h;
typedef Attrib<4, SH_CONST, Half, SH_PLANE> ConstPlane4h;


typedef Attrib<1, SH_INPUT, FracUInt, SH_PLANE> InputPlane1fui;
typedef Attrib<1, SH_OUTPUT, FracUInt, SH_PLANE> OutputPlane1fui;
typedef Attrib<1, SH_INOUT, FracUInt, SH_PLANE> InOutPlane1fui;
typedef Attrib<1, SH_TEMP, FracUInt, SH_PLANE>  Plane1fui;
typedef Attrib<1, SH_CONST, FracUInt, SH_PLANE> ConstPlane1fui;
typedef Attrib<2, SH_INPUT, FracUInt, SH_PLANE> InputPlane2fui;
typedef Attrib<2, SH_OUTPUT, FracUInt, SH_PLANE> OutputPlane2fui;
typedef Attrib<2, SH_INOUT, FracUInt, SH_PLANE> InOutPlane2fui;
typedef Attrib<2, SH_TEMP, FracUInt, SH_PLANE>  Plane2fui;
typedef Attrib<2, SH_CONST, FracUInt, SH_PLANE> ConstPlane2fui;
typedef Attrib<3, SH_INPUT, FracUInt, SH_PLANE> InputPlane3fui;
typedef Attrib<3, SH_OUTPUT, FracUInt, SH_PLANE> OutputPlane3fui;
typedef Attrib<3, SH_INOUT, FracUInt, SH_PLANE> InOutPlane3fui;
typedef Attrib<3, SH_TEMP, FracUInt, SH_PLANE>  Plane3fui;
typedef Attrib<3, SH_CONST, FracUInt, SH_PLANE> ConstPlane3fui;
typedef Attrib<4, SH_INPUT, FracUInt, SH_PLANE> InputPlane4fui;
typedef Attrib<4, SH_OUTPUT, FracUInt, SH_PLANE> OutputPlane4fui;
typedef Attrib<4, SH_INOUT, FracUInt, SH_PLANE> InOutPlane4fui;
typedef Attrib<4, SH_TEMP, FracUInt, SH_PLANE>  Plane4fui;
typedef Attrib<4, SH_CONST, FracUInt, SH_PLANE> ConstPlane4fui;


typedef Attrib<1, SH_INPUT, unsigned int, SH_PLANE> InputPlane1ui;
typedef Attrib<1, SH_OUTPUT, unsigned int, SH_PLANE> OutputPlane1ui;
typedef Attrib<1, SH_INOUT, unsigned int, SH_PLANE> InOutPlane1ui;
typedef Attrib<1, SH_TEMP, unsigned int, SH_PLANE>  Plane1ui;
typedef Attrib<1, SH_CONST, unsigned int, SH_PLANE> ConstPlane1ui;
typedef Attrib<2, SH_INPUT, unsigned int, SH_PLANE> InputPlane2ui;
typedef Attrib<2, SH_OUTPUT, unsigned int, SH_PLANE> OutputPlane2ui;
typedef Attrib<2, SH_INOUT, unsigned int, SH_PLANE> InOutPlane2ui;
typedef Attrib<2, SH_TEMP, unsigned int, SH_PLANE>  Plane2ui;
typedef Attrib<2, SH_CONST, unsigned int, SH_PLANE> ConstPlane2ui;
typedef Attrib<3, SH_INPUT, unsigned int, SH_PLANE> InputPlane3ui;
typedef Attrib<3, SH_OUTPUT, unsigned int, SH_PLANE> OutputPlane3ui;
typedef Attrib<3, SH_INOUT, unsigned int, SH_PLANE> InOutPlane3ui;
typedef Attrib<3, SH_TEMP, unsigned int, SH_PLANE>  Plane3ui;
typedef Attrib<3, SH_CONST, unsigned int, SH_PLANE> ConstPlane3ui;
typedef Attrib<4, SH_INPUT, unsigned int, SH_PLANE> InputPlane4ui;
typedef Attrib<4, SH_OUTPUT, unsigned int, SH_PLANE> OutputPlane4ui;
typedef Attrib<4, SH_INOUT, unsigned int, SH_PLANE> InOutPlane4ui;
typedef Attrib<4, SH_TEMP, unsigned int, SH_PLANE>  Plane4ui;
typedef Attrib<4, SH_CONST, unsigned int, SH_PLANE> ConstPlane4ui;



} // namespace SH

#endif // SHPLANE_HPP
