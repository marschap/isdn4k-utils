dnl Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])

AC_INIT([capiiniti], [1.0])
AM_INIT_AUTOMAKE
AC_PREFIX_DEFAULT([/])

I4LCONFDIR=${syconfdir:-"/etc/isdn"}

AC_DEFINE_UNQUOTED(I4LCONFDIR, "${I4LCONFDIR}", [directory for configuration files])

dnl Checks for programs.
AC_PROG_INSTALL
AC_PROG_CC

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(linux/capi.h,,
	[AC_MSG_ERROR([Missing $ac_hdr. Kernel source installed?])])
AC_CHECK_HEADERS(capi20.h)

dnl Define which variables to substitute & which files to generate
AC_SUBST(I4LCONFDIR)
AM_CONFIG_HEADER(config.h)

AC_OUTPUT(Makefile capiinit.8)
