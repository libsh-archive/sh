
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
