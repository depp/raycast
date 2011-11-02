AC_PREREQ(2.61)
AC_INIT([raycast], [0.1], [depp@zdome.net])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_FILES([Makefile src/Makefile])
AC_CONFIG_HEADERS([src/config.h])

AC_PROG_CC_C99

AM_PATH_SDL(1.2.0, , AC_MSG_ERROR([SDL 1.2 is required.]))

ENABLE_WARNINGS

AC_OUTPUT
