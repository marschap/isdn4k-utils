dnl Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])

AC_INIT([isdnctrl], [?.?])
AC_PREFIX_DEFAULT(/usr)

I4LCONFDIR=${CONFIG_I4LCONFDIR:-"/etc/isdn"}
I4LCONFFILE=${CONFIG_CONFFILE:-"/etc/isdn"}
I4LVERSION=${I4LVERSION:-"?.?"}
CONFIG_SBINDIR=${sbindir:-"/sbin"}
CONFIG_MANDIR=${mandir:-"/usr/man"}
CONFIG_ISDNCTRL_CONF=${CONFIG_ISDNCTRL_CONF:-"n"}
CONFIG_ISDNCTRL_DWABC_UDP_INFO=${CONFIG_ISDNCTRL_DWABC_UDP_INFO:-"n"}
MANDATE=`grep CHECKIN isdnctrl.man.in | awk '{print $4}'`

dnl Checks for programs.
AC_PROG_INSTALL
AC_PROG_CC

dnl Checks for libraries.
AC_TYPE_SIGNAL

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(sys/ioctl.h unistd.h)


dnl Optional config-file-option
AC_ARG_ENABLE(cfile,
	[  --enable-cfile          Enable configfile option [no]],
	CONFIG_ISDNCTRL_CONF=y
	AC_DEFINE(CONFIG_ISDNCTRL_CONF, [y], [Enable configfile option]),
)

dnl Define which variables to substitute & which files to generate
AC_SUBST(MANDATE)
AC_SUBST(I4LCONFDIR)
AC_SUBST(I4LCONFFILE)
AC_SUBST(I4LVERSION)
AC_SUBST(CONFIG_ISDNCTRL_CONF)
AC_SUBST(CONFIG_ISDNCTRL_DWABC_UDP_INFO)
AC_SUBST(CONFIG_SBINDIR)
AC_SUBST(CONFIG_MANDIR)
AC_CONFIG_HEADERS(config.h)

AC_OUTPUT(Makefile isdnctrl.man .isdnctrl_conf.man)
