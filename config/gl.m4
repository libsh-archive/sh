# Various checks for GL

# GL_CHECK_GL_HEADERS(if-available, if-not-available)
#
AC_DEFUN([GL_CHECK_GL_HEADERS], [
  AC_CHECK_HEADERS([GL/gl.h])
  AC_CHECK_HEADERS([GL/glext.h], [$1],
    [$2],
    [#if HAVE_GL_GL_H
#include <GL/gl.h>
#endif])
])

# GL_CHECK_GLEXT_VERSION(version, if-avail, if-not-avail)
#
AC_DEFUN([GL_CHECK_GLEXT_VERSION], [
  AC_MSG_CHECKING([for glext.h version >= $1])
  AC_RUN_IFELSE(
    AC_LANG_PROGRAM([#if HAVE_GL_GLEXT_H
# include <GL/gl.h>
# include <GL/glext.h>
#else
# define GL_GLEXT_VERSION 0
#endif],
    [#if GL_GLEXT_VERSION >= $1
  return 0;
#else
  return 1;
#endif]),
  [AC_MSG_RESULT([yes])
   $2],
  [AC_MSG_RESULT([no])
   $3])
])

