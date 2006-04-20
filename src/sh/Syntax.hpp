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
#ifndef SHSYNTAX_HPP
#define SHSYNTAX_HPP

#include "DllExport.hpp"
#include "Program.hpp"
#include "Utility.hpp"

/** \file Syntax.hpp
 * Sh syntactical definitions.
 */

/// @internal
//@{
#define PUSH_ARG_QUEUE ::SH::pushArgQueue()
#define PUSH_ARG ::SH::pushArg()
#define PROCESS_ARG(arg, internal_cond) ::SH::processArg(arg, internal_cond)
//@}

/// @name Shader definitions
//@{
/** \def SH_BEGIN_PROGRAM(target)
 * Begin a new program block.
 * Nesting programs is not allowed.
 * @retval Program A reference to the new program.
 * @see SH_END_PROGRAM
 */
#define SH_BEGIN_PROGRAM(target) ::SH::beginShader(target); {{{{{{
/** \def SH_END_PROGRAM
 * End the current program definition.
 *
 * If there is a backend and the program has a specified target
 * (e.g. vertex or fragment), this will also compile the program.
 *
 * @see SH_BEGIN_PROGRAM
 */
#define SH_END_PROGRAM          }}}}}} ::SH::endShader();
//@}

#define SH_END                 SH_END_PROGRAM

#define SH_BEGIN_VERTEX_PROGRAM ::SH::beginShader("gpu:vertex"); {{{{{{
#define SH_BEGIN_FRAGMENT_PROGRAM ::SH::beginShader("gpu:fragment"); {{{{{{

///@name If statements
//@{
/** \def SH_IF(cond)
 * Begin an if statement.
 * @see SH_ELSE
 * @see ENDIF
 */
#define SH_IF(cond) { \
  bool sh__internal_cond; \
  ::SH::internal_if(PUSH_ARG_QUEUE && PUSH_ARG && PROCESS_ARG(cond, &sh__internal_cond)); \
  if (::SH::Context::current()->parsing() || sh__internal_cond) {{
/** \def SH_ELSE
 * Indicate the start of the else-block of an if statement.
 * @see SH_IF
 * @see ENDIF
 */
#define SH_ELSE  \
  }} \
  ::SH::internal_else(); \
  if (::SH::Context::current()->parsing() || !sh__internal_cond) {{
/** \def ENDIF
 * Indicate the end of an if-statement.
 * @see SH_IF
 * @see SH_ELSE
 */
#define ENDIF \
  }} \
  ::SH::endIf(); \
}
//@}

/// @name While loops
//@{
/** \def SH_WHILE(cond)
 * Begin a while statement, iterating as long as \a cond is satisfied.
 * @see SH_ENDWHILE
 */
#define SH_WHILE(cond) { \
  bool sh__internal_cond; \
  ::SH::internal_while(PUSH_ARG_QUEUE && PUSH_ARG && PROCESS_ARG(cond, &sh__internal_cond)); \
  bool sh__internal_firsttime = true; \
  while ((sh__internal_firsttime && (::SH::Context::current()->parsing() || sh__internal_cond)) \
         || (!::SH::Context::current()->parsing() && ::SH::evaluateCondition(cond))) {{{
/** \def SH_ENDWHILE
 * Indicate the end of a while-statement.
 * @see SH_WHILE
 */
#define SH_ENDWHILE \
    sh__internal_firsttime = false; \
  }}} \
  ::SH::endWhile(); \
}
//@}

/// @name Do-until loops
//@{
/** \def SH_DO
 * Indicate the start of a do-until statement.
 * @see SH_UNTIL
 */
#define SH_DO       { \
  ::SH::internal_do(); \
  do {{{{
/** \def SH_UNTIL(cond)
 * End a do-until statement, iterating as long as \a cond is satisfied.
 * @see SH_DO
 */
#define SH_UNTIL(cond) \
  }}}} while (!::SH::Context::current()->parsing() && !::SH::evaluateCondition(cond)); \
  if (::SH::Context::current()->parsing()) ::SH::until(PUSH_ARG_QUEUE && PUSH_ARG && PROCESS_ARG(cond, 0)); }
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
#define SH_FOR(init,cond,update) { \
  if (::SH::Context::current()->parsing()) \
    ::SH::internal_for(PUSH_ARG_QUEUE \
             && PUSH_ARG && PROCESS_ARG(init, 0) \
             && PUSH_ARG && PROCESS_ARG(cond, 0) \
             && PUSH_ARG && PROCESS_ARG(update, 0)); \
  if (!::SH::Context::current()->parsing()) init; \
  bool sh__internal_first_time = true; \
  while (1) {{{{{ \
    if (!::SH::Context::current()->parsing()) { \
      if (!sh__internal_first_time) { update; } \
      else { sh__internal_first_time = false; } \
      if (!evaluateCondition(cond)) break; \
    }                                                                                  
/** \def SH_ENDFOR
 * Indicate the end of a for statement
 * @see SH_FOR
 */
#define SH_ENDFOR \
    if (::SH::Context::current()->parsing()) break; \
  }}}}} \
  ::SH::endFor(); \
}
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
#define SH_BREAK(cond) \
  if (!::SH::Context::current()->parsing()) { \
    if (::SH::evaluateCondition(cond)) break;\
  } else {\
    ::SH::internal_break(PUSH_ARG_QUEUE && PUSH_ARG && PROCESS_ARG(cond, 0));\
  }
/** \def SH_CONTINUE
 * Break out of a loop, continuing with the next iteration.
 * @see SH_BREAK
 * @see SH_WHILE
 * @see SH_DO
 * @see SH_FOR
 */
#define SH_CONTINUE(cond) \
  if (!::SH::Context::current()->parsing()) { \
    if (::SH::evaluateCondition(cond)) continue;\
  } else {\
    ::SH::internal_continue(PUSH_ARG_QUEUE && PUSH_ARG && PROCESS_ARG(cond, 0));\
  }
/** \def SH_RETURN
 * Terminate the fragment program without killing the fragment.
 */
#define SH_RETURN(a) ::SH::shRET(a)
//@}

/** \def SH_BEGIN_SECTION
 * Starts a block grouping - for affine analysis and organization of code for
 * visual languages.
 *
 * @param name - String name of the block (this shouldn't be required... 
 *               This is only to keep me sane when debugging 
 */
#define SH_BEGIN_SECTION(name) \
  {{{{{{ \
    if(::SH::Context::current()->parsing()) { \
      ::SH::beginSection(); \
      ::SH::comment(name); \
    }

#define SH_END_SECTION \
    if(::SH::Context::current()->parsing()) { ::SH::endSection(); } \
  }}}}}} 


/// @name Named Declaration macros 
//@{
/** \def NAME
 * Set the name of a variable to be its C++ name.
 *
 * @see DECL
 * @see NAMEDECL
 */
#define NAME(var) do { var.name( # var); } while (0)

/** \def DECL
 * Declare variable with the same name as var.
 * Usage:
 *  Sh_Some_Type DECL(var);
 * or 
 *  Sh_Some_TYPE DECL(var) = initial_value;
 *
 * @see NAMEDDECL
 */
#define DECL(var) var; var.name( # var ); ::SH::Ignore() & var

/** \def NAMEDECL
 * Declare variable with the given name 
 * Usage:
 *  Sh_Some_Type NAMEDECL(var, "name");
 * or Sh_Some_Type NAMEDECL(var, "name") = initial_value;
 *
 * @see DECL
 */
#define NAMEDECL(var, varName) var; var.name( varName ); ::SH::Ignore() & var
//@}

namespace SH {

/// \internal
SH_DLLEXPORT
bool processArg(const Variable& arg, bool* internal_cond);

/// \internal
SH_DLLEXPORT
bool pushArg();

/// \internal
SH_DLLEXPORT
bool pushArgQueue();

/// \internal
SH_DLLEXPORT
bool evaluateCondition(const Variable& arg);

/// \internal
SH_DLLEXPORT
Program beginShader(const std::string& kind = "");
/// \internal
SH_DLLEXPORT
void endShader();

/// Force (re)compilation of a program under its default target.
SH_DLLEXPORT
void compile(Program& prg);
/// Force (re)compilation of a program for a given target.
SH_DLLEXPORT
void compile(Program& prg, const std::string& target);

/// \deprecated Use compile() instead
SH_DLLEXPORT
void compileShader(Program& prg);
/// \deprecated Use compile() instead
SH_DLLEXPORT
void compileShader(Program& prg, const std::string& target);

/// Bind a program using the program's default target
SH_DLLEXPORT
void bind(Program& prg);
/// Bind a program with the given target.
SH_DLLEXPORT
void bind(const std::string& target, Program& shader);

/// \deprecated Use bind() instead.
SH_DLLEXPORT
void bindShader(Program& shader);
/// \deprecated Use bind() instead.
SH_DLLEXPORT
void bindShader(const std::string& target, Program& shader);

/// Bind a set of programs
SH_DLLEXPORT
void bind(const ProgramSet& s);

/// Unbind all currently bound programs
SH_DLLEXPORT
void unbind();

/// Unbind a program.
SH_DLLEXPORT
void unbind(Program& prg);
/// Unbind a program with the given target.
SH_DLLEXPORT
void unbind(const std::string& target, Program& shader);

/// Unbind a set of programs, if it's bound
SH_DLLEXPORT
void unbind(const ProgramSet& s);

/// Link a set of programs
SH_DLLEXPORT
void link(const ProgramSet& s);

/// Upload any textures and uniform parameters which are out-of-date
/// but required on all compilation targets that have any programs
/// bound
SH_DLLEXPORT
void update();

/// Switch to a particular backend
SH_DLLEXPORT
bool setBackend(const std::string& name);

/// Add a backend to the list of selected backends
SH_DLLEXPORT
bool useBackend(const std::string& name);

/// Checks whether the backend is available
SH_DLLEXPORT
bool haveBackend(const std::string& name);

/// Clear the list of selected backends
SH_DLLEXPORT
void clearBackends();

/// Find the name of the best backend that handles the given target
SH_DLLEXPORT
std::string findBackend(const std::string& target);

/// Register a backend
SH_DLLEXPORT
void registerBackend(const std::string& name, SH::Backend::InstantiateEntryPoint *instantiate, SH::Backend::TargetCostEntryPoint *target_cost);

/** \brief SH Initialization Function.
 *
 * The function needs to be called prior to the use of any other SH functions. Additionally,
 * in Windows, this function needs to be called after a OpenGL context/window has been created.
 */
SH_DLLEXPORT
void init();

/// \internal
SH_DLLEXPORT
void internal_if(bool);
/// \internal
SH_DLLEXPORT
void internal_else();
/// \internal
SH_DLLEXPORT
void endIf();

/// \internal
SH_DLLEXPORT
void internal_while(bool);
/// \internal
SH_DLLEXPORT
void endWhile();

/// \internal
SH_DLLEXPORT
void internal_do();
/// \internal
SH_DLLEXPORT
void until(bool);

/// \internal
SH_DLLEXPORT
void internal_for(bool);
/// \internal
SH_DLLEXPORT
void endFor();

/// \internal
SH_DLLEXPORT
void internal_break(bool);
/// \internal
SH_DLLEXPORT
void internal_continue(bool);

/// \internal
SH_DLLEXPORT
void beginSection();
/// \internal
SH_DLLEXPORT
void endSection();

/// \internal
/// Adds a comment to the immediate representation 
SH_DLLEXPORT
void comment(const std::string& comment);

}

#endif
