dnl Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])

AC_INIT(divertctrl.c)
AC_PREFIX_DEFAULT([/usr])

I4LVERSION=${I4LVERSION:-"?.?"}
CONFIG_SBINDIR=${sbindir:-"/usr/sbin"}
CONFIG_MANDIR=${mandir:-"/usr/man"}

dnl Checks for programs.
AC_PROG_INSTALL
AC_PROG_CC

dnl Checks for libraries.

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(fcntl.h sys/ioctl.h unistd.h linux/isdn.h)

dnl Checks for typedefs, structures, and compiler characteristics.

dnl Checks for library functions.
AC_FUNC_MMAP

dnl Check, if user wants dump option for debugging
AC_ARG_ENABLE(dump,
	[  --enable-dump        Enable dump feature for debugging],
	DEBUGVAR=-D__DEBUGVAR__)

AC_SUBST(DEBUGVAR)
AC_SUBST(I4LVERSION)
AC_SUBST(CONFIG_SBINDIR)
AC_SUBST(CONFIG_MANDIR)
AC_OUTPUT(Makefile divertctrl.man)

