dnl Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])

AC_INIT(hisaxctrl.c)
AC_PREFIX_DEFAULT([/usr])

I4LVERSION=${I4LVERSION:-"?.?"}
CONFIG_SBINDIR=${sbindir:-"/sbin"}
CONFIG_DATADIR=${CONFIG_FIRMWAREDIR:-"/usr/lib/isdn"}
CONFIG_MANDIR=${mandir:-"/usr/man"}
CONFIG_HISAXCTRL=${CONFIG_HISAXCTRL:-"n"}

dnl Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL

dnl Checks for libraries.

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(fcntl.h sys/ioctl.h unistd.h)

dnl Find current kernel source

dnl Checks for typedefs, structures, and compiler characteristics.

dnl Checks for library functions.
AC_CHECK_FUNCS(strtol)

dnl Build hisaxctrl
AC_ARG_WITH(hisax,
	[  --with-hisax=y          Build hisaxctrl. [no]],
	CONFIG_HISAXCTRL="${withval}"
	AC_DEFINE(CONFIG_HISAXCTRL,"${withval}", [Build hisaxctrl]),
)

dnl Define which variables to substitute & which files to generate
AC_SUBST(I4LVERSION)
AC_SUBST(CONFIG_SBINDIR)
AC_SUBST(CONFIG_DATADIR)
AC_SUBST(CONFIG_MANDIR)
AC_SUBST(CONFIG_HISAXCTRL)

AC_OUTPUT(Makefile hisaxctrl.man)
