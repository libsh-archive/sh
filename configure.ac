# Process this file with autoconf to produce a configure script.
AC_INIT(sh, 0.1, mmccool@cgl.uwaterloo.ca)
AC_CONFIG_SRCDIR([src/ShAttrib.hpp])
AM_CONFIG_HEADER(config.h)

AM_INIT_AUTOMAKE(foreign)

# Checks for programs.
AC_PROG_CXX
AC_PROG_LIBTOOL

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

# Checks for header files.
AC_CHECK_HEADER([sstream], [], [AC_MSG_ERROR([sh requires <sstream>. Please use gcc >= 3.0])])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

# Backends

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
AM_CONDITIONAL(ENABLE_SM_BACKEND, test x$ac_backend_sm = xtrue)

SH_CHECK_BACKEND([arb], [OpenGL ARB])
AM_CONDITIONAL(ENABLE_ARB_BACKEND, test x$ac_backend_arb = xtrue)

AC_CONFIG_FILES([Makefile
		 src/Makefile
		 test/Makefile
		 backends/Makefile
		 backends/sm/Makefile
		 backends/arb/Makefile
		 doc/Makefile])
AC_OUTPUT
