# SM_WITH_SM_DIR
# Adds a --with-sm option to specify the SM installation directory
AC_DEFUN([SM_WITH_SM_DIR], [
  AC_ARG_WITH([sm], AC_HELP_STRING([--with-sm=DIR], [specify that SM is installed in DIR]),
    [CPPFLAGS="$CPPFLAGS -I${withval}/include"
     CXXFLAGS="$CXXFLAGS -I${withval}/include"
     LDFLAGS="$LDFLAGS -L${withval}/lib"],
    [])
])

# SM_CHECK_SM_HEADERS(TRUE, FALSE)
# Checks whether SM header files are present.
AC_DEFUN([SM_CHECK_SM_HEADERS], [
  AC_CHECK_HEADER([sm/sm.hpp], [$1],
    [$2])
])