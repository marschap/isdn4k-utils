dnl Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])

AC_INIT(imon.c)
AC_PREFIX_DEFAULT([/usr])

I4LVERSION=${I4LVERSION:-"?.?"}
CONFIG_MANDIR=${mandir:-"/usr/man"}
CONFIG_SBINDIR=${sbindir:-"/sbin"}

dnl Checks for programs.
AC_PROG_INSTALL
AC_PROG_CC

dnl Checks for libraries.
AC_CHECK_LIB(ncurses, newpad,,
	AC_CHECK_LIB(curses, newpad,,
		AC_MSG_ERROR("Missing ncurses library")))

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(sys/time.h unistd.h)
AC_CHECK_HEADERS(ncurses.h,,
	[AC_CHECK_HEADERS(ncurses/curses.h,,
		[AC_CHECK_HEADERS(curses/ncurses.h,,
			[AC_CHECK_HEADERS(curses.h,,
				[AC_MSG_ERROR([Missing \(n\)curses.h])])])])])

dnl Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_TIME
AC_STRUCT_TM

dnl Checks for library functions.
AC_TYPE_SIGNAL
AC_CHECK_FUNCS(select)

dnl Define which variables to substitute & which files to generate
AC_SUBST(CONFIG_SBINDIR)
AC_SUBST(CONFIG_MANDIR)
AC_SUBST(I4LVERSION)
AC_CONFIG_HEADERS(config.h)

AC_OUTPUT(Makefile imon.man)
