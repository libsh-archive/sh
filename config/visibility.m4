AC_DEFUN([GCC_VISIBILITY_CHECK], [
  AC_MSG_CHECKING([GCC major version (for visibility support)])
  GCC_VERSION="`$CXX --version| grep "GCC" | cut -f3 -d" " | cut -f1 -d.`"
  AC_MSG_RESULT([$GCC_VERSION])
  if test "x$GCC_VERSION" != x ; then
    if test "$GCC_VERSION" -ge 4 ; then
      CXXFLAGS="$CXXFLAGS -fvisibility-inlines-hidden -DHAVE_GCCVISIBILITYPATCH"
    fi
  fi
])
