# Process this file with autoconf to produce a configure script.

AC_INIT(Shcript, 0.0)
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_HEADER(config.h)
AM_PROG_CC_C_O

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_LIBTOOL

# Languages
AC_LANG(C++)

# Libraries
SH_WITH_SH_DIR
SH_CHECK_SH_HEADERS

WITH_LUA_DIR

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([memory.h stddef.h stdlib.h sys/time.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions.
AC_HEADER_MAJOR
AC_FUNC_MEMCMP
AC_CHECK_FUNCS([floor memset sqrt])

# Finally output
AC_CONFIG_FILES([Makefile src/Makefile])

AC_OUTPUT