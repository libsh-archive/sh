AC_DEFUN([DOXYGEN_FIND_DOXYGEN], [
  AC_PATH_PROG(DOXYGEN, doxygen, no)
  if [[ x"${DOXYGEN}" = "xno" ]] ; then
    AC_MSG_WARN([doxygen not found. Disabling generated API documentation.])
  fi
])
