dnl Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])

AC_INIT(ipppstats.c)

I4LVERSION=${I4LVERSION:-"?.?"}
CONFIG_SBINDIR=${sbindir:-"/sbin"}
CONFIG_MANDIR=${mandir:-"/usr/man"}

dnl Checks for programs.
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL

dnl Checks for libraries.

dnl Checks for header files.
AC_CHECK_HEADERS(fcntl.h sys/ioctl.h)

dnl Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIGNAL

dnl Checks for library functions.
AC_HEADER_STDC
AC_CHECK_FUNCS(socket)

dnl Define which variables to substitute & which files to generate
AC_SUBST(I4LVERSION)
AC_SUBST(CONFIG_SBINDIR)
AC_SUBST(CONFIG_MANDIR)

AC_OUTPUT(Makefile ipppstats.man)
