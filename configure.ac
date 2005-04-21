# Process this file with autoconf to produce a configure script.
AC_INIT(Sh, 0.7.7, libsh-devel@lists.sourceforge.net, libsh)
AC_CONFIG_SRCDIR([src/ShVariable.hpp])
AM_INIT_AUTOMAKE

AC_CANONICAL_HOST

AM_CONFIG_HEADER(config.h)

# Checks for programs.
if test "x$CXXFLAGS" = x ; then
    CXXFLAGS="-Wall -g -O2"
fi

# Additional libraries if in OSX
case ${host_os}
     in darwin*)
	 if test -f /sw/include/png.h ; then
	    CPPFLAGS="-I/sw/include $CPPFLAGS"
	    LDFLAGS="-L/sw/lib $LDFLAGS"
	 fi
esac

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

required_glext_version=21

# Mac OSX Tests
case $host_os in 
  darwin*)
    AC_MSG_NOTICE([configuring OSX build, you have $host_os])
    AC_CHECK_LIB([mx], [log2f])
    LDFLAGS="$LDFLAGS -flat_namespace"
    required_glext_version=7
    enable_pbuffer_streams=false
    ;;
  *) enable_pbuffer_streams=true ;;
esac

AM_CONDITIONAL(INCLUDE_PBUFFERSTREAMS, test x$enable_pbuffer_streams = xtrue)
# Backends

SM_WITH_SM_DIR
GL_WITH_GL_DIR

#version spec is done in the style [current[:release=0[:age=0]]]
BACKEND_LIB_VERSION=1:0:0
AC_SUBST(BACKEND_LIB_VERSION)

dnl Shared library extensions (LTDL_SHLIB_EXT gets it wrong on OS X)
AC_LIBTOOL_SYS_DYNAMIC_LINKER

# This is an abysmal hack against different versions of aclocal
if test "x$shrext" = x; then
   my_shrext=$shrext_cmds
else
   my_shrext=$shrext
fi
tst_shrext=`echo $my_shrext | sed -n 's/^\(\$.*\)/\1/p'`
if test "x$tst_shrext" = x; then
   sh_shlib_ext=$my_shrext
else
   sh_shlib_ext=`module="" eval echo $my_shrext`
fi

AC_DEFINE_UNQUOTED(SH_SHLIB_EXT, "$sh_shlib_ext", [Platform-specific shared library extension])

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
  GL_CHECK_GLEXT_VERSION([$required_glext_version], [], [
    SH_BACKEND_MESSAGE([arb], [glext.h version 18 or greater (available at http://oss.sgi.com/projects/ogl-sample/registry/) not found. Disabling ARB backend.])
    sh_backend_arb=false
  ])
fi
AM_CONDITIONAL(ENABLE_ARB_BACKEND, test x$sh_backend_arb = xtrue)

SH_CHECK_BACKEND([glsl], [OpenGL Shading Language])
if test "x$sh_backend_glsl" = xtrue ; then
  GL_CHECK_GL_HEADERS([],
    [SH_BACKEND_MESSAGE([glsl], [OpenGL headers (GL/gl.h and GL/glext.h) not found. Disabling GLSL backend.])
     sh_backend_glsl=false])
  GL_CHECK_GLEXT_EXTENSION([ARB_shading_language_100], [], [
    SH_BACKEND_MESSAGE([glsl], [Headers for ARB_shading_language_100 not found. You may need to upgrade your glext.h from http://oss.sgi.com/projects/ogl-sample/registry/. Disabling GLSL backend.])
    sh_backend_glsl=false])
fi
AM_CONDITIONAL(ENABLE_GLSL_BACKEND, test x$sh_backend_glsl = xtrue)

AM_CONDITIONAL(ENABLE_GL_BACKENDS, test x$sh_backend_arb = xtrue -o x$sh_backend_glsl = xtrue)

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
if test "x$prefix" = xNONE ; then
   AC_DEFINE_UNQUOTED(SH_INSTALL_PREFIX, "/usr/local", [Installation prefix directory])
else
   AC_DEFINE_UNQUOTED(SH_INSTALL_PREFIX, "$prefix", [Installation prefix directory])
fi


AC_CONFIG_FILES([Makefile
		 src/Makefile
		 test/Makefile
		 test/regress/Makefile
		 backends/Makefile
		 backends/cc/Makefile
		 backends/sm/Makefile
		 backends/gl/Makefile
		 util/Makefile
		 examples/Makefile
		 examples/binding/Makefile
		 examples/glut/Makefile
		 examples/particle/Makefile
		 examples/textures/Makefile
		 doc/Makefile])

AC_OUTPUT

SH_SUMMARISE_BACKENDS

