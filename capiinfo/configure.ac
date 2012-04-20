dnl Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])

AC_INIT([capiinfo], [1.0])
AM_INIT_AUTOMAKE
AC_PREFIX_DEFAULT([/usr])

I4LCONFDIR=`eval echo ${CONFIG_I4LCONFDIR:-"/etc/isdn"}`
I4LVERSION=${I4LVERSION:-"?.?"}
CONFIG_SBINDIR=${sbindir:-"/sbin"}
CONFIG_MANDIR=${mandir:-"/usr/man"}
#MANDATE=`grep CHECKIN isdnctrl.man.in | awk '{print $4}'`

dnl Checks for programs.
AC_PROG_INSTALL
AC_PROG_CC

dnl Checks for libraries.

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(linux/capi.h,,
	[AC_MSG_ERROR([Missing $ac_hdr. Kernel source installed?])])
dnl not used AC_CHECK_HEADERS(capi20.h)

dnl Define which variables to substitute & which files to generate
AC_SUBST(I4LCONFDIR)
AC_SUBST(I4LVERSION)
AC_SUBST(CONFIG_SBINDIR)
AC_SUBST(CONFIG_MANDIR)
AC_CONFIG_HEADERS(config.h)

AC_OUTPUT(Makefile)
