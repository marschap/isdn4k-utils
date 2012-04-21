dnl Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])

AC_INIT(iprofd.c)
AC_PREFIX_DEFAULT(/usr)

I4LVERSION=${I4LVERSION:-"?.?"}
CONFIG_SBINDIR=${sbindir:-"/sbin"}
CONFIG_MANDIR=${mandir:-"/usr/man"}
MANDATE=`grep CHECKIN iprofd.man.in | awk '{print $4}'`

dnl Checks for programs.
AC_PROG_INSTALL
AC_PROG_CC

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(sys/ioctl.h unistd.h)

dnl Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIGNAL

dnl Define which variables to substitute & which files to generate
AC_SUBST(MANDATE)
AC_SUBST(I4LVERSION)
AC_SUBST(CONFIG_SBINDIR)
AC_SUBST(CONFIG_MANDIR)

AC_OUTPUT(Makefile iprofd.man)
