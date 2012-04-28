dnl Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])

test -f ../.config && source ../.config

AC_INIT([I4L], [?])
AM_INIT_AUTOMAKE
AC_PREFIX_DEFAULT([/usr])

I4LVERSION=${I4LVERSION:-"?.?"}

dnl Checks for programs.
AC_PROG_INSTALL
AC_PROG_AWK
AC_PROG_SED

dnl Define which variables to substitute & which files to generate
AC_SUBST(I4LVERSION)

AC_OUTPUT(Makefile ttyI.4 isdninfo.4 isdn_audio.4 isdnctrl.4 isdn_cause.7)
