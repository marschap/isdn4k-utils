dnl Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])

AC_INIT(icnctrl.c)
AC_PREFIX_DEFAULT([/usr])

I4LVERSION=${I4LVERSION:-"?.?"}
CONFIG_DATADIR=${CONFIG_FIRMWAREDIR:-"/usr/lib/isdn"}
CONFIG_SBINDIR=${CONFIG_CARD_SBINDIR:-"/sbin"}
CONFIG_MANDIR=${mandir:-"/usr/man"}
CONFIG_ICNCTRL_DEBUG=${CONFIG_ICNCTRL_DEBUG:-"n"}
MANDATE=`grep CHECKIN icnctrl.man.in | awk '{print $4}'`

dnl Checks for programs.
AC_PROG_INSTALL
AC_PROG_CC

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(fcntl.h sys/ioctl.h unistd.h)

dnl Checks for library functions.
AC_FUNC_MMAP

dnl Optional firmware directory
AC_ARG_WITH(firmware,
	[  --with-firmware=DIR     Set dir where firmware istalled. [/usr/lib/isdn]],
	AC_DEFINE(CONFIG_DATADIR, "${withval}", [icn firmware directory]),
)

dnl Check, if user wants dump option for debugging
AC_ARG_ENABLE(dump,
	[  --enable-dump           Enable dump feature for debugging [no]],
	CONFIG_ICNCTRL_DEBUG="y"
)

dnl Define which variables to substitute & which files to generate
AC_SUBST(MANDATE)
AC_SUBST(CONFIG_ICNCTRL_DEBUG)
AC_SUBST(CONFIG_DATADIR)
AC_SUBST(CONFIG_SBINDIR)
AC_SUBST(CONFIG_MANDIR)
AC_SUBST(I4LVERSION)

AC_OUTPUT(Makefile icnctrl.man)
