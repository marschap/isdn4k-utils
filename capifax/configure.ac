dnl Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])

AC_INIT([capifax], [1.0])
AM_INIT_AUTOMAKE
AC_PREFIX_DEFAULT([/usr])

CONFIG_SBINDIR=${sbindir:-"/sbin"}
CONFIG_MANDIR=${mandir:-"/usr/man"}

dnl Checks for programs.
AC_PROG_INSTALL
AC_PROG_CC

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(sys/ioctl.h unistd.h)
AC_CHECK_FILE([../capi20/capi20.h], CPPFLAGS="-I ../capi20 $CPPFLAGS")
AC_CHECK_HEADER([capi20.h],[],[AC_MSG_ERROR([capi20.h not found])])

dnl Checks for typedefs, structures, and compiler characteristics.
CS_TEST_ALERT
CS_TEST_GLOBALCONFIG

dnl Define which variables to substitute & which files to generate
AC_SUBST(CONFIG_SBINDIR)
AC_SUBST(CONFIG_MANDIR)
AM_CONFIG_HEADER(config.h)

AC_OUTPUT(Makefile)
