# Process this file with autoconf to produce a configure script.
AC_INIT(sh, 0.1, mmccool@cgl.uwaterloo.ca)
AC_CONFIG_SRCDIR([src/ShAttrib.hpp])
AM_CONFIG_HEADER(config.h)

AM_INIT_AUTOMAKE(sh, 0.1)

# Checks for programs.
AC_PROG_CXX
AC_PROG_LIBTOOL
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

# Set up some stuff
AC_LANG(C++)
AC_ARG_ENABLE([debug], AC_HELP_STRING([--enable-debug], [enable debugging information (default=yes)]),
  [AC_MSG_CHECKING([for debugging information])
   msg=disabled
   if test ".$enableval" = ".yes" ; then
       AC_DEFINE(SH_DEBUG, 1, [Enable debugging information])
       msg=enabled
   fi
   AC_MSG_RESULT($msg)
  ],
  [AC_MSG_CHECKING([for debugging information])
   AC_DEFINE(SH_DEBUG, 1, [Enable debugging information])
   AC_MSG_RESULT(default (yes))
   ])
CXXFLAGS="-g -Wall -O0"

# Checks for libraries.
AC_CHECK_LIB([png], [png_create_read_struct],
             [],
             [AC_MSG_ERROR([sh requires libpng.])])

# Checks for header files.
AC_CHECK_HEADER([png.h], [], [AC_MSG_ERROR([sh requires the libpng header files.])])
AC_CHECK_HEADER([sstream], [], [AC_MSG_ERROR([sh requires <sstream>. Please use gcc >= 3.0])])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_CHECK_FUNCS([pow sqrt])

# Backends

AC_ARG_WITH([sm], AC_HELP_STRING([--with-sm=DIR], [specify that SM is installed in DIR]),
  [CPPFLAGS="$CPPFLAGS -I${withval}/include"
   CXXFLAGS="$CXXFLAGS -I${withval}/include"
   LDFLAGS="$LDFLAGS -L${withval}/lib"],
  [])

# SH_CHECK_BACKEND(BACKEND_NAME, DESCRIPTIVE_NAME, DEFAULT=true)
AC_DEFUN([SH_CHECK_BACKEND],
[AC_ARG_ENABLE([$1-backend],
               AC_HELP_STRING([--enable-$1-backend],
                              [compile $2 backend (default=yes)]),
               [case "${enableval}" in
	         yes) ac_backend_$1=true ;;
		 no)  ac_backend_$1=false ;;
                 *) AC_MSG_ERROR(bad value ${enableval} for --enable-$1-backend) ;;
              esac],[ac_backend_$1=m4_default([$3], true)])
])

SH_CHECK_BACKEND([sm], [SM])
if test "x$ac_backend_sm" = xtrue ; then
  AC_CHECK_HEADER([sm/sm.hpp], [],
    [AC_MSG_WARN([sm.hpp not found. Disabling SM backend.])
     ac_backend_sm=false ])
fi
AM_CONDITIONAL(ENABLE_SM_BACKEND, test x$ac_backend_sm = xtrue)

SH_CHECK_BACKEND([arb], [OpenGL ARB])
if test "x$ac_backend_arb" = xtrue ; then
  AC_CHECK_HEADERS([GL/gl.h])
  AC_CHECK_HEADERS([GL/glext.h], [],
    [AC_MSG_WARN([OpenGL headers (GL/gl.h and GL/glext.h) not found. Disabling ARB backend.])
     ac_backend_arb=false ],
    [#if HAVE_GL_GL_H
#include <GL/gl.h>
#endif])
  AC_RUN_IFELSE(
    AC_LANG_PROGRAM([#if HAVE_GL_GLEXT_H
# include <GL/gl.h>
# include <GL/glext.h>
#else
# define GL_GLEXT_VERSION 0
#endif],
    [#if GL_GLEXT_VERSION >= 18
  return 0;
#else
  return 1;
#endif]), [],
    [AC_MSG_WARN([glext.h version 18 or greater (available at http://oss.sgi.com/projects/ogl-sample/registry/) not found. Disabling ARB backend.])])
fi
AM_CONDITIONAL(ENABLE_ARB_BACKEND, test x$ac_backend_arb = xtrue)

AC_CONFIG_FILES([Makefile
		 src/Makefile
		 test/Makefile
		 backends/Makefile
		 backends/sm/Makefile
		 backends/arb/Makefile
		 doc/Makefile])
AC_OUTPUT
