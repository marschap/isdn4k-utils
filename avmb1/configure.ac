dnl Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])

AC_INIT([avmcapictrl],[1.17])
AM_INIT_AUTOMAKE
AC_PREFIX_DEFAULT([/usr])

dnl Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL

I4LVERSION=${I4LVERSION:-"?.?"}

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(fcntl.h sys/ioctl.h unistd.h)

dnl Checks for library functions.
AC_CHECK_FUNCS(mmap)

dnl Define which variables to substitute & which files to generate
AC_SUBST(I4LVERSION)

AC_OUTPUT(Makefile avmcapictrl.8)
