# Process this file with autoconf to produce a configure script.
AC_INIT(sh, 0.0.20031119, libsh-devel@lists.sourceforge.net, libsh)
AM_INIT_AUTOMAKE

AC_CONFIG_SRCDIR([src/ShAttrib.hpp])
AM_CONFIG_HEADER(config.h)

# Checks for programs.
AC_PROG_CXX
AC_LIBLTDL_CONVENIENCE
AC_LIBTOOL_DLOPEN
AC_PROG_LIBTOOL
AC_LIB_LTDL
AC_SUBST(LIBLTDL)
AC_SUBST(LTDLINCL)
AC_CONFIG_SUBDIRS(libltdl)
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

# Set up some stuff
AC_LANG(C++)

SH_CHECK_ENABLE_DEBUG

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

SM_WITH_SM_DIR

SH_CHECK_BACKEND([sm], [SM])
if test "x$ac_backend_sm" = xtrue ; then
  SM_CHECK_SM_HEADERS([],
    [AC_MSG_WARN([sm.hpp not found. Disabling SM backend.])
     ac_backend_sm=false ])
fi
AM_CONDITIONAL(ENABLE_SM_BACKEND, test x$ac_backend_sm = xtrue)

SH_CHECK_BACKEND([arb], [OpenGL ARB])
if test "x$ac_backend_arb" = xtrue ; then
  GL_CHECK_GL_HEADERS([],
    [AC_MSG_WARN([OpenGL headers (GL/gl.h and GL/glext.h) not found. Disabling ARB backend.])
     ac_backend_arb=false])
  GL_CHECK_GLEXT_VERSION([18], [], [
    AC_MSG_WARN([glext.h version 18 or greater (available at http://oss.sgi.com/projects/ogl-sample/registry/) not found. Disabling ARB backend.])
    ac_backend_arb=false
  ])
fi
AM_CONDITIONAL(ENABLE_ARB_BACKEND, test x$ac_backend_arb = xtrue)

SH_CHECK_BACKEND([ati], [OpenGL ATI])
if test "x$ac_backend_ati" = xtrue ; then
  GL_CHECK_GL_HEADERS([],
    [AC_MSG_WARN([OpenGL headers (GL/gl.h and GL/glext.h) not found. Disabling ATI backend.])
     ac_backend_ati=false])
  GL_CHECK_GLEXT_VERSION([18], [], [
    AC_MSG_WARN([glext.h version 18 or greater (available at http://oss.sgi.com/projects/ogl-sample/registry/) not found. Disabling ATI backend.])
    ac_backend_ati=false
  ])
fi
AM_CONDITIONAL(ENABLE_ATI_BACKEND, test x$ac_backend_ati = xtrue)

# Store the prefix as a variable
AC_DEFINE_UNQUOTED(SH_INSTALL_PREFIX, "$prefix", [Installation prefix directory])


AC_CONFIG_FILES([Makefile
		 src/Makefile
		 test/Makefile
		 backends/Makefile
		 backends/sm/Makefile
		 backends/arb/Makefile
		 backends/ati/Makefile
		 util/Makefile
		 shla/Makefile
		 doc/Makefile])
AC_OUTPUT
