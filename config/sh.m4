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

# SH_CHECK_ENABLE_DEBUG
AC_DEFUN([SH_CHECK_ENABLE_DEBUG], [
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
])
