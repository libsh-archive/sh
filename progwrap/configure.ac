# Process this file with autoconf to produce a configure script.
AC_INIT(sh, 0.7.1, libsh-devel@lists.sourceforge.net, libsh)
AC_CONFIG_SRCDIR([src/ShAttrib.hpp])
AM_INIT_AUTOMAKE

AM_CONFIG_HEADER(config.h)

# Checks for programs.
if test "x$CXXFLAGS" = x ; then
    CXXFLAGS="-Wall -g -O2"
fi

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
GL_WITH_GL_DIR


# SH_CHECK_BACKEND([sm], [SM])
# if test "x$sh_backend_sm" = xtrue ; then
#   SM_CHECK_SM_HEADERS([],
#     [AC_MSG_WARN([sm.hpp not found. Disabling SM backend.])
#      sh_backend_sm=false ])
# fi
sh_backend_sm=false
SH_DECLARE_BACKEND([sm], [SM])
SH_BACKEND_MESSAGE([sm], [The SM backend is disabled for the moment.])

AM_CONDITIONAL(ENABLE_SM_BACKEND, test x$sh_backend_sm = xtrue)

SH_CHECK_BACKEND([arb], [OpenGL ARB])
if test "x$sh_backend_arb" = xtrue ; then
  GL_CHECK_GL_HEADERS([],
    [SH_BACKEND_MESSAGE([arb], [OpenGL headers (GL/gl.h and GL/glext.h) not found. Disabling ARB backend.])
     sh_backend_arb=false])
  GL_CHECK_GLEXT_VERSION([18], [], [
    SH_BACKEND_MESSAGE([arb], [glext.h version 18 or greater (available at http://oss.sgi.com/projects/ogl-sample/registry/) not found. Disabling ARB backend.])
    sh_backend_arb=false
  ])
fi
AM_CONDITIONAL(ENABLE_ARB_BACKEND, test x$sh_backend_arb = xtrue)

SH_CHECK_NONDEFAULT_BACKEND([ati], [ATI])
if test "x$sh_backend_ati" = xtrue ; then
  GL_CHECK_GL_HEADERS([],
    [SH_BACKEND_MESSAGE([ati], [OpenGL headers (GL/gl.h and GL/glext.h) not found. Disabling ATI backend.])
     sh_backend_ati=false])
  GL_CHECK_GLEXT_VERSION([18], [], [
    SH_BACKEND_MESSAGE([ati], [glext.h version 18 or greater (available at http://oss.sgi.com/projects/ogl-sample/regi\
stry/) not found. Disabling ATI backend.])
    sh_backend_ati=false
  ])
fi
AM_CONDITIONAL(ENABLE_ATI_BACKEND, test x$sh_backend_ati = xtrue)

AM_CONDITIONAL(ENABLE_GL_BACKENDS, test x$sh_backend_ati = xtrue -o x$sh_backend_arb = xtrue)

AC_ARG_ENABLE([docs],
	      AC_HELP_STRING([--enable-docs],
                             [generate doxygen documentation (default=yes)]),
	      [case ${enableval} in
	         yes) sh_enable_docs=true ;;
		 no)  sh_enable_docs=false ;;
                 *) AC_MSG_ERROR(bad value ${enableval} for --enable-docs) ;;
              esac],[sh_enable_docs=true])

if test "x$sh_enable_docs" = xtrue ; then
  DOXYGEN_FIND_DOXYGEN
  if test "x$DOXYGEN" = xno ; then
    sh_enable_docs=false
  fi
fi

AM_CONDITIONAL(ENABLE_DOXYGEN_DOCS, test x$sh_enable_docs = xtrue)

# Store the prefix as a variable
AC_DEFINE_UNQUOTED(SH_INSTALL_PREFIX, "$prefix", [Installation prefix directory])


AC_CONFIG_FILES([Makefile
		 src/Makefile
		 test/Makefile
		 test/regress/Makefile
		 backends/Makefile
		 backends/cpu/Makefile
		 backends/sm/Makefile
		 backends/gl/Makefile
		 util/Makefile
		 doc/Makefile])
AC_OUTPUT

SH_SUMMARISE_BACKENDS
