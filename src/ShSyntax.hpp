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
#ifndef SHSYNTAX_HPP
#define SHSYNTAX_HPP

#include "ShProgram.hpp"
#include "ShUtility.hpp"

/** \file ShSyntax.hpp
 * Sh syntactical definitions.
 */

/// @internal
//@{
#define SH_PUSH_ARG_QUEUE ::SH::ShContext::current()->parsing()->tokenizer.pushArgQueue()
#define SH_PUSH_ARG ::SH::ShContext::current()->parsing()->tokenizer.pushArg()
#define SH_PROCESS_ARG(arg) ::SH::ShContext::current()->parsing()->tokenizer.processArg(arg)
//@}

/// @name Shader definitions
//@{
/** \def SH_BEGIN_PROGRAM(target)
 * Begin a new program block.
 * Nesting programs is not allowed.
 * @retval ShProgram A reference to the new program.
 * @see SH_END_PROGRAM
 */
#define SH_BEGIN_PROGRAM(target) ::SH::shBeginShader(target);
/** \def SH_END_PROGRAM
 * End the current program definition.
 *
 * If there is a backend and the program has a specified target
 * (e.g. vertex or fragment), this will also compile the program.
 *
 * @see SH_BEGIN_PROGRAM
 */
#define SH_END_PROGRAM         ::SH::shEndShader();
//@}

#define SH_END                 SH_END_PROGRAM

#define SH_BEGIN_VERTEX_PROGRAM ::SH::shBeginShader("gpu:vertex");
#define SH_BEGIN_FRAGMENT_PROGRAM ::SH::shBeginShader("gpu:fragment");

///@name If statements
//@{
/** \def SH_IF(cond)
 * Begin an if statement.
 * @see SH_ELSE
 * @see SH_ENDIF
 */
#define SH_IF(cond) ::SH::shIf(SH_PUSH_ARG_QUEUE && SH_PUSH_ARG && SH_PROCESS_ARG(cond)); {{
/** \def SH_ELSE
 * Indicate the start of the else-block of an if statement.
 * @see SH_IF
 * @see SH_ENDIF
 */
#define SH_ELSE  }} ::SH::shElse(); {{
/** \def SH_ENDIF
 * Indicate the end of an if-statement.
 * @see SH_IF
 * @see SH_ELSE
 */
#define SH_ENDIF ::SH::shEndIf();}}
//@}

/// @name While loops
//@{
/** \def SH_WHILE(cond)
 * Begin a while statement, iterating as long as \a cond is satisfied.
 * @see SH_ENDWHILE
 */
#define SH_WHILE(cond) ::SH::shWhile(SH_PUSH_ARG_QUEUE && SH_PUSH_ARG && SH_PROCESS_ARG(cond)); {{{
/** \def SH_ENDWHILE
 * Indicate the end of a while-statement.
 * @see SH_WHILE
 */
#define SH_ENDWHILE ::SH::shEndWhile();}}}
//@}

/// @name Do-until loops
//@{
/** \def SH_DO
 * Indicate the start of a do-until statement.
 * @see SH_UNTIL
 */
#define SH_DO       ::SH::shDo(); {{{{
/** \def SH_UNTIL(cond)
 * End a do-until statement, iterating as long as \a cond is satisfied.
 * @see SH_DO
 */
#define SH_UNTIL(cond) ::SH::shUntil(SH_PUSH_ARG_QUEUE && SH_PUSH_ARG && SH_PROCESS_ARG(cond));}}}}
//@}

/// @name For loops
//@{
/** \def SH_FOR(init, cond, update)
 * Begin a for statement.
 * The semantics are mostly the same as for C++ for statements, but
 * unlike in C++ declaring variables in \a init part is not allowed.
 * @see SH_ENDFOR
 */
// TODO: It is possible to make declaring variables in init work. do so.
#define SH_FOR(init,cond,update) ::SH::shFor(SH_PUSH_ARG_QUEUE \
                                 && SH_PUSH_ARG && SH_PROCESS_ARG(init) \
                                 && SH_PUSH_ARG && SH_PROCESS_ARG(cond) \
                                 && SH_PUSH_ARG && SH_PROCESS_ARG(update)); {{{{{
/** \def SH_ENDFOR
 * Indicate the end of a for statement
 * @see SH_FOR
 */
#define SH_ENDFOR     ::SH::shEndFor();}}}}}
//@}

/// @name Loop flow control
//@{
/** \def SH_BREAK
 * Break out of a loop, terminating the loop.
 * @see SH_CONTINUE
 * @see SH_WHILE
 * @see SH_DO
 * @see SH_FOR
 */
#define SH_BREAK    ::SH::shBreak();
/** \def SH_CONTINUE
 * Break out of a loop, continuing with the next iteration.
 * @see SH_BREAK
 * @see SH_WHILE
 * @see SH_DO
 * @see SH_FOR
 */
#define SH_CONTINUE ::SH::shBreak();
//@}

/// @name Named Declaration macros 
//@{
/** \def SH_NAME
 * Set the name of a variable to be its C++ name.
 *
 * @see SH_DECL
 * @see SH_NAMEDECL
 */
#define SH_NAME(var) do { var.name( # var); } while (0)

/** \def SH_DECL
 * Declare variable with the same name as var.
 * Usage:
 *  Sh_Some_Type SH_DECL(var);
 * or 
 *  Sh_Some_TYPE SH_DECL(var) = initial_value;
 *
 * @see SH_NAMEDDECL
 */
#define SH_DECL(var) var; var.name( # var ); ::SH::ShIgnore() & var

/** \def SH_NAMEDECL
 * Declare variable with the given name 
 * Usage:
 *  Sh_Some_Type SH_NAMEDECL(var, "name");
 * or Sh_Some_Type SH_NAMEDECL(var, "name") = initial_value;
 *
 * @see SH_DECL
 */
#define SH_NAMEDECL(var, varName) var; var.name( varName ); ::SH::ShIgnore() & var
//@}

namespace SH {

/// \internal
ShProgram shBeginShader(const std::string& kind = "");
/// \internal
void shEndShader();

/// Force (re)compilation of a program under its default target.
void shCompile(ShProgram& prg);
/// Force (re)compilation of a program for a given target.
void shCompile(ShProgram& prg, const std::string& target);

/// \deprecated Use shCompile() instead
void shCompileShader(ShProgram& prg);
/// \deprecated Use shCompile() instead
void shCompileShader(ShProgram& prg, const std::string& target);

/// Bind a program using the program's default target
void shBind(ShProgram& prg);
/// Bind a program with the given target.
void shBind(const std::string& target, ShProgram& shader);

/// \deprecated Use shBind() instead.
void shBindShader(ShProgram& shader);
/// \deprecated Use shBind() instead.
void shBindShader(const std::string& target, ShProgram& shader);

/// Switch to a particular backend
bool shSetBackend(const std::string& name);


/** \brief SH Initialization Function.
 *
 * The function needs to be called prior to the use of any other SH functions. Additionally,
 * in Windows, this function needs to be called after a OpenGL context/window has been created.
 */
void shInit();

/// \internal
void shIf(bool);
/// \internal
void shElse();
/// \internal
void shEndIf();

/// \internal
void shWhile(bool);
/// \internal
void shEndWhile();

/// \internal
void shDo();
/// \internal
void shUntil(bool);

/// \internal
void shFor(bool);
/// \internal
void shEndFor();

/// \internal
void shBreak();
/// \internal
void shContinue();

}

#endif
