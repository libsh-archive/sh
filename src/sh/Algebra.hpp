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
#ifndef SHALGEBRA_HPP
#define SHALGEBRA_HPP

#include <string>
#include "DllExport.hpp"
#include "Program.hpp"
#include "Syntax.hpp"
#include "Attrib.hpp"

namespace SH {

/** Replace inputs of b with outputs of a.
 * Functional composition.
 * Let A = a.outputs.size(), B = b.inputs.size().
 * If A > B, extra outputs are kept at end
 * If A < B, extra inputs are kept at end
 */
DLLEXPORT
Program connect(Program a, Program b);

/** Combine a and b.
 * Use all inputs from a and b and all outputs from a and b,
 * concatenated in order,
 * and perform all operations from both programs.
 */
DLLEXPORT
Program combine(Program a, Program b);

/** Combine a and b.
 * Use all inputs from a and b and all outputs from a and b,
 * combined by name,
 * and perform all operations from both programs.
 *
 * This combine detects pairs of inputs with matching names and types.
 * If this occurs, the later input is discarded and replaced with
 * a copy of the earlier one.   Unnamed inputs are all considered to
 * be unique.
 *
 * For instance, if a has inputs x, y, k, x, z and b has inputs w, y, x, v
 * then the result has inputs x, y, k, z, w, v
 */
DLLEXPORT
Program namedCombine(Program a, Program b);

/** Replace inputs of b with outputs of a.
 * Functional composition.
 * The outputs of a and inputs of b must all be named.
 *
 * For each output of a in positional order, this connects the output with an 
 * input of b of the same name/type that is not already connected with
 * another output of a.
 * Extra inputs remain at the end.  Extra outputs remain iff keepExtra = true 
 */
DLLEXPORT
Program namedConnect(Program a, Program b, bool keepExtra = false );

/** Renames all inputs named oldName to newName.
 */
DLLEXPORT
Program renameInput(Program a, const std::string& oldName, const std::string& newName);

/** Renames all outputs named oldName to newName.
 */
DLLEXPORT
Program renameOutput(Program a, const std::string& oldName, const std::string& newName);

/** Swizzles named outputs of a to match named inputs of b.
 * This only works on programs with inputs/outputs that all have unique names. 
 * Also, the inputs of b must be a subset of the outputs of a.
 */
DLLEXPORT
Program namedAlign(Program a, Program b);

/** Replaces variable with input attribute.
 * Replaces a variable by appending an
 * input attribute to the end of the list of inputs.
 */
DLLEXPORT
Program replaceVariable(Program a, const Variable &var); 

/** Equivalent to combine(a,b).
 */
DLLEXPORT
Program operator&(Program a, Program b);

/** Equivalent to connect(b,a).
 */
DLLEXPORT
Program operator<<(Program a, Program b);

/** Equivalent to replaceVariable(p,var).
 */
DLLEXPORT
Program operator>>(Program p, const Variable &var); 

/** Application operator.
 * The operator used for combine can also be used to apply a program
 * to a variable.   The implementation supports currying with delayed
 * read, which is equivalent to replacing an input with a parameter.
 */
DLLEXPORT
Program operator<<(Program a, const Variable& var); 

}

#endif
