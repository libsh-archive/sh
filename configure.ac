# Process this file with autoconf to produce a configure script.
AC_INIT(sh, 0.1, mmccool@cgl.uwaterloo.ca)
AC_CONFIG_SRCDIR([src/ShAttrib.hpp])

AM_INIT_AUTOMAKE(sh, 0.1)

# Checks for programs.
AC_PROG_CXX
AC_PROG_LIBTOOL

# Set up some stuff
AC_LANG(C++)
AC_SUBST(CXXFLAGS, [-Wall])

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADER([sstream], [], [AC_MSG_ERROR([sh requires <sstream>. Please use gcc >= 3.0])])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_FILES([Makefile
		 src/Makefile])
AC_OUTPUT
