#ifndef SHSYNTAX_HPP
#define SHSYNTAX_HPP

#include "ShShader.hpp"

/** \file ShSyntax.hpp
 * Sh syntactical definitions.
 */

namespace SH {

ShShader shBeginShader(int kind);
void shEndShader();

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

void shSwitch(bool);
void shEndSwitch(bool);
void shCase(bool);
void shDefault();

}

/// @internal
//@{
#define SH_PUSH_ARG_QUEUE ::SH::ShEnvironment::shader->tokenizer.pushArgQueue()
#define SH_PUSH_ARG ::SH::ShEnvironment::shader->tokenizer.pushArg()
#define SH_PROCESS_ARG(arg) ::SH::ShEnvironment::shader->tokenizer.processArg(arg)
//@}

/// @name Shader definitions
//@{
/** \def SH_BEGIN_SHADER(kind)
 * Begin a new shader of the given \a kind.
 * Nesting shaders is not allowed.
 * @retval ShShader A reference to the new shader.
 * @see SH_END_SHADER
 */
#define SH_BEGIN_SHADER(kind) ::SH::shBeginShader(kind);
/** \def SH_END_SHADER
 * End the current shader definition.
 * @see SH_BEGIN_SHADER
 */
#define SH_END_SHADER         ::SH::shEndShader();
//@}

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

/// @name Switch statements
//@{
/** Begin a switch statement, selecting on \a value.
 * @see SH_CASE
 * @see SH_DEFAULT
 * @see SH_ENDSWITCH
 */
#define SH_SWITCH(value)    ::SH::shSwitch(SH_PUSH_ARG_QUEUE && SH_PUSH_ARG && SH_PROCESS_ARG(value)); {{{{{{
/** Begin a particular case in a switch statement
 * @see SH_SWITCH
 * @see SH_DEFAULT
 * @see SH_ENDSWITCH
 */
#define SH_CASE(cond)      ::SH::shCase(SH_PUSH_ARG_QUEUE && SH_PUSH_ARG && SH_PROCESS_ARG(cond));
/** Indicate the default case in a switch statement.
 * @see SH_SWITCH
 * @see SH_CASE
 * @see SH_ENDSWITCH
 */
#define SH_DEFAULT      ::SH::shDefault();
/** End a switch statement.
 * @see SH_SWITCH
 * @see SH_CASE
 * @see SH_DEFAULT
 */
#define SH_ENDSWITCH ::SH::shEndSwitch();}}}}}}
//@}

#endif
