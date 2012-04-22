dnl Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])

AC_INIT([capiinfo], [1.0])
AM_INIT_AUTOMAKE
AC_PREFIX_DEFAULT([/usr])

dnl Checks for programs.
AC_PROG_INSTALL
AC_PROG_CC

dnl Checks for libraries.

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(linux/capi.h,,
	[AC_MSG_ERROR([Missing $ac_hdr. Kernel source installed?])])

dnl Define which variables to substitute & which files to generate
AC_OUTPUT(Makefile)
