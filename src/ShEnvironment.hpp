#ifndef SHENVIRONMENT_HPP
#define SHENVIRONMENT_HPP

#include "ShShader.hpp"

namespace SH {

/** Global settings for the current program.
 * This class with only static members contains global settings such
 * as the currently active shader or whether or not the program is
 * inside a shader definition right now.
 */
struct ShEnvironment 
{
  static ShShader shader;
  
  static bool insideShader;
};

}

#endif
