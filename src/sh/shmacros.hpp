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
#ifndef SHMACROS_HPP
#define SHMACROS_HPP

// Program definition
#define BEGIN_PROGRAM          SH_BEGIN_PROGRAM
#define BEGIN_VERTEX_PROGRAM   SH_BEGIN_VERTEX_PROGRAM
#define BEGIN_FRAGMENT_PROGRAM SH_BEGIN_FRAGMENT_PROGRAM
#define END_PROGRAM            SH_END_PROGRAM
#define END                    SH_END
#define BEGIN_SECTION          SH_BEGIN_SECTION
#define END_SECTION            SH_END_SECTION

// Control flow
#define IF       SH_IF
#define ELSE     SH_ELSE
#define ELSEIF   SH_ELSEIF
#define ENDIF    SH_ENDIF
#define FOR      SH_FOR
#define ENDFOR   SH_ENDFOR
#define DO       SH_DO
#define UNTIL    SH_UNTIL
#define WHILE    SH_WHILE
#define ENDWHILE SH_ENDWHILE
#define WHEN     SH_WHEN
#define BREAK    SH_BREAK
#define CONTINUE SH_CONTINUE
#define RETURN   SH_RETURN

// Named declaration
#define DECL     SH_DECL
#define NAME     SH_NAME
#define NAMEDECL SH_NAMEDECL

#endif /* SHMACROS_HPP */
