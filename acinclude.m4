# -*- autoconf -*-

AC_DEFUN([ENABLE_WARNINGS],[
  AC_ARG_ENABLE(warnings,
    [  --enable-warnings       enable warnings for GCC ],
    [enable_warnings=$enableval], [warnings=no])
  if test "x$enable_warnings" != xno ; then
    [WARNING_CFLAGS="-Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes"]
    if test "x$enable_warnings" != xyes ; then
      [WARNING_CFLAGS="$warning_cppflags $enable_warnings"]
    fi
  fi
  AC_SUBST([WARNING_CFLAGS])
])
