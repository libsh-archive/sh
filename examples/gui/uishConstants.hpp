#ifndef UISH_CONSTANTS_HPP
#define UISH_CONSTANTS_HPP

#include <sh/sh.hpp>

struct uishColor {
  static SH::ShConstColor4f Red; 
  static SH::ShConstColor4f Green; 
  static SH::ShConstColor4f Blue; 
  static SH::ShConstColor4f Cyan; 
  static SH::ShConstColor4f Magenta; 
  static SH::ShConstColor4f Yellow; 

  static SH::ShConstColor4f LightRed; 
  static SH::ShConstColor4f LightGreen; 
  static SH::ShConstColor4f LightBlue; 
  static SH::ShConstColor4f LightCyan; 
  static SH::ShConstColor4f LightMagenta; 
  static SH::ShConstColor4f LightYellow; 

  static SH::ShConstColor4f Black; 
  static SH::ShConstColor4f Gray25; 
  static SH::ShConstColor4f Gray50; 
  static SH::ShConstColor4f Gray75; 
  static SH::ShConstColor4f White; 

  static SH::ShConstColor4f None; 
};

#endif
