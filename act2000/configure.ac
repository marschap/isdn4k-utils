dnl Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])

AC_INIT(actctrl.c)
AC_PREFIX_DEFAULT(/usr)

I4LCONFDIR=${I4LCONFDIR:-"/etc/isdn"}
I4LVERSION=${I4LVERSION:-"?.?"}
CONFIG_DATADIR=${CONFIG_FIRMWAREDIR:-"/usr/lib/isdn"}
CONFIG_MANDIR=${mandir:-"/usr/man"}
CONFIG_SBINDIR=${CONFIG_CARD_SBINDIR:-"/sbin"}
MANDATE=`grep CHECKIN actctrl.man.in | awk '{print $4}'`

dnl Checks for programs.
AC_PROG_INSTALL
AC_PROG_CC

dnl Checks for libraries.

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(fcntl.h sys/ioctl.h unistd.h)

dnl Checks for library functions.
AC_FUNC_MMAP

dnl Optional firmware directory
AC_ARG_WITH(firmware,
	[  --with-firmware=DIR     Set dir where firmware installed. [/usr/lib/isdn]],
	AC_DEFINE(CONFIG_DATADIR, "${withval}", [directory where firmware installed]),
)

dnl Define which variables to substitute & which files to generate
AC_SUBST(MANDATE)
AC_SUBST(CONFIG_DATADIR)
AC_SUBST(CONFIG_SBINDIR)
AC_SUBST(CONFIG_MANDIR)
AC_SUBST(I4LCONFDIR)
AC_SUBST(I4LVERSION)

AC_OUTPUT(Makefile actctrl.man)
