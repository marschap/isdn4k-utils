dnl Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])

AC_INIT(cdb.c)
AC_PREFIX_DEFAULT([/usr])

CONFIG_MANDIR=${mandir:-"/usr/man"}

dnl Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(unistd.h)

dnl Only look for uint32
AC_CHECK_SIZEOF(long,4)
AC_CHECK_SIZEOF(int,4)
AC_C_BIGENDIAN

dnl Define which variables to substitute & which files to generate
AC_SUBST(CC)
AC_SUBST(INSTALL)
AC_SUBST(CONFIG_MANDIR)
AC_SUBST(WORDS_BIGENDIAN)
AC_CONFIG_HEADER(config.h)

AC_OUTPUT(Makefile)
