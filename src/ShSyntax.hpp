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
#include "ShFramebuffer.hpp"

/** \file ShSyntax.hpp
 * Sh syntactical definitions.
 */

/// @internal
//@{
#define SH_PUSH_ARG_QUEUE ::SH::ShEnvironment::shader->tokenizer.pushArgQueue()
#define SH_PUSH_ARG ::SH::ShEnvironment::shader->tokenizer.pushArg()
#define SH_PROCESS_ARG(arg) ::SH::ShEnvironment::shader->tokenizer.processArg(arg)
//@}

/// @name Shader definitions
//@{
/** \def SH_BEGIN_PROGRAM
 * Begin a new generic program block.
 * Nesting programs is not allowed.
 * @retval ShProgram A reference to the new program.
 * @see SH_END_PROGRAM
 */
#define SH_BEGIN_PROGRAM ::SH::shBeginShader(-1);
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

#define SH_VERTEX_PROGRAM 0
#define SH_FRAGMENT_PROGRAM 1

#define SH_BEGIN_VERTEX_PROGRAM ::SH::shBeginShader(SH_VERTEX_PROGRAM);
#define SH_BEGIN_FRAGMENT_PROGRAM ::SH::shBeginShader(SH_FRAGMENT_PROGRAM);

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

namespace SH {

ShProgram shBeginShader(int kind = -1);
void shEndShader();

/// Force compilation of a shader. The shader must have a target.
void shCompileShader(ShProgram& shader);
/// Force compilation of a shader for a given target
void shCompileShader(int target, ShProgram& shader);

/// Bind a shader. The shader must have a target.
void shBindShader(ShProgram& shader);
/// Bind a shader with the given target.
void shBindShader(int target, ShProgram& shader);

/// Change the render target
void shDrawBuffer(ShFramebufferPtr fb);

void shIf(bool);
void shElse();
void shEndIf();

void shWhile(bool);
void shEndWhile();

void shDo();
void shUntil(bool);

void shFor(bool);
void shEndFor();

void shBreak();
void shContinue();

}

#endif
