#ifndef SHENVIRONMENT_HPP
#define SHENVIRONMENT_HPP

#include "ShShader.hpp"
#include "ShBackend.hpp"

namespace SH {

/** Global settings for the current program.
 * This class with only static members contains global settings such
 * as the currently active shader or whether or not the program is
 * inside a shader definition right now.
 */
struct ShEnvironment 
{
  /// The shader currently being defined
  static ShShader shader;

  /// Whether we are in the process of defining a shader
  static bool insideShader;

  /// The shader bound under the current backend
  static ShShader boundShader;

  /// The currently active backend
  static ShBackendPtr backend;
};

}

#endif
