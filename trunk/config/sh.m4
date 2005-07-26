AC_DEFUN([SH_DECLARE_BACKEND], [
  define([SH_BACKENDS], ifdef([SH_BACKENDS], SH_BACKENDS [$1], [$1]))
  define([SH_BACKEND_DESC_$1], [$2])
])

AC_DEFUN([SH_BACKEND_MESSAGE], [
  if test "x${sh_backend_msg_$1}" != x ; then
    sh_backend_msg_$1="${sh_backend_msg_$1}
$2"
  else
    sh_backend_msg_$1="$2"
  fi
  AC_MSG_WARN([$2])
])

# SH_CHECK_BACKEND(BACKEND_NAME, DESCRIPTIVE_NAME, DEFAULT=true)
AC_DEFUN([SH_CHECK_BACKEND],
[SH_DECLARE_BACKEND([$1], [$2])
 AC_ARG_ENABLE([$1-backend],
               AC_HELP_STRING([--enable-$1-backend],
                              [compile $2 backend (default=yes)]),
               [case "${enableval}" in
	         yes) sh_backend_$1=true ;;
		 no)  sh_backend_$1=false ;;
                 *) AC_MSG_ERROR(bad value ${enableval} for --enable-$1-backend) ;;
              esac],[sh_backend_$1=m4_default([$3], true)])
])
AC_DEFUN([SH_CHECK_NONDEFAULT_BACKEND],
[SH_DECLARE_BACKEND([$1], [$2])
 AC_ARG_ENABLE([$1-backend],
               AC_HELP_STRING([--enable-$1-backend],
                              [compile $2 backend (default=no)]),
               [case "${enableval}" in
	         yes) sh_backend_$1=true ;;
		 no)  sh_backend_$1=false ;;
                 *) AC_MSG_ERROR(bad value ${enableval} for --enable-$1-backend) ;;
              esac],[sh_backend_$1=m4_default([$3], false)])
])

# SH_WITH_SH_DIR
# Adds a --with-sh option to specify the Sh installation directory
AC_DEFUN([SH_WITH_SH_DIR], [
  AC_ARG_WITH([sh], AC_HELP_STRING([--with-sh=DIR], [specify that Sh is installed in DIR]),
    [CPPFLAGS="$CPPFLAGS -I${withval}/include"
     CXXFLAGS="$CXXFLAGS -I${withval}/include"
     LDFLAGS="$LDFLAGS -L${withval}/lib"],
    [])
])

# SH_CHECK_SH_HEADERS(TRUE, FALSE)
# Checks whether Sh header files are present.
AC_DEFUN([SH_CHECK_SH_HEADERS], [
  AC_CHECK_HEADER([sh/sh.hpp], [$1],
    [$2])
])

AC_DEFUN([SH_SUMMARISE_BACKENDS], [
  echo
  echo "Backend Summary:"
  echo
  AC_FOREACH([BackendName], SH_BACKENDS, [
    pushdef([Description], [SH_BACKEND_DESC_]BackendName)
    echo Description [Backend: ] ${[sh_backend_]BackendName}
    pushdef([Message], [sh_backend_msg_]BackendName)
    if test "x${Message}" != x ; then
      echo ${Message}
    fi
    popdef([Message])
    popdef([Description])
  ])
])
