dnl Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])

AC_INIT
AC_PREFIX_DEFAULT([/usr])

I4LCONFDIR=${I4LCONFDIR:-"/etc/isdn"}
I4LVERSION=${I4LVERSION:-"?.?"}
CONFIG_MANDIR=${mandir:-"/usr/man"}

dnl Checks for programs.
AC_PROG_INSTALL
AC_PROG_AWK
AC_PROG_SED

dnl Define which variables to substitute & which files to generate
AC_SUBST(I4LCONFDIR)
AC_SUBST(I4LVERSION)
AC_SUBST(CONFIG_MANDIR)

AC_OUTPUT(Makefile ttyI.man isdninfo.man isdn_cause.man isdn_audio.man isdnctrl.man)
