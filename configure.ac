# Process this file with autoconf to produce a configure script.
AC_INIT(sh, 0.1, mmccool@cgl.uwaterloo.ca)
AC_CONFIG_SRCDIR([src/ShAttrib.hpp])
AM_CONFIG_HEADER(config.h)

AM_INIT_AUTOMAKE(sh, 0.1)

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

AC_CONFIG_FILES([Makefile
		 src/Makefile
		 test/Makefile
		 backends/Makefile
		 backends/sm/Makefile])
AC_OUTPUT
