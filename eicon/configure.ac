dnl Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])

AC_INIT(eiconctrl.c)
AC_PREFIX_DEFAULT([/usr])

I4LVERSION=${I4LVERSION:-"?.?"}
CONFIG_DATADIR=${CONFIG_FIRMWAREDIR:-"/usr/lib/isdn/eicon"}
CONFIG_SBINDIR=${sbindir:-"/usr/sbin"}
CONFIG_CARD_SBINDIR=${CONFIG_CARD_SBINDIR:-"/sbin"}
CONFIG_MANDIR=${mandir:-"/usr/man"}
CONFIG_EICONCTRL_DEBUG=${CONFIG_EICONCTRL_DEBUG:-"n"}
MANDATE=`grep CHECKIN eiconctrl.man.in | awk '{print $4}'`

dnl Checks for programs.
AC_PROG_INSTALL
AC_PROG_CC

dnl Checks for libraries.
AC_CHECK_LIB(ncurses, newpad,,
	[AC_CHECK_LIB(curses, newpad,,
		[AC_MSG_ERROR([Missing ncurses library])])])

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(fcntl.h sys/ioctl.h unistd.h)
AC_CHECK_HEADERS(ncurses.h,,
	[AC_CHECK_HEADERS(ncurses/curses.h,,
		[AC_CHECK_HEADERS(curses/ncurses.h,,
			[AC_CHECK_HEADERS(curses.h,,
				[AC_MSG_ERROR([Missing \(n\)curses.h])])])])])

dnl Checks for library functions.
AC_FUNC_MMAP

dnl Optional firmware directory
AC_ARG_WITH(firmware,
	[  --with-firmware=DIR     Set dir where firmware istalled. [/usr/lib/isdn]],
	AC_DEFINE(CONFIG_DATADIR,"${withval}", [eicon firmware directory]),
)

dnl Check, if user wants dump option for debugging
AC_ARG_ENABLE(dump,
	[  --enable-dump           Enable dump feature for debugging [no]],
	CONFIG_EICONCTRL_DEBUG="y"
)

AC_DEFINE(HAVE_XLOG, [], [define if we have XLOG capability])
AC_SUBST(HAVE_XLOG)

AC_DEFINE(HAVE_TRACE, [], [define if we have TRACE capability])
AC_SUBST(HAVE_TRACE)

AC_DEFINE(HAVE_NPCI, [], [NPCI supported])
AC_SUBST(HAVE_NPCI)

AC_SUBST(HAVE_NATIVE, [n])

AC_SUBST(INSTALL)
AC_SUBST(MANDATE)
AC_SUBST(CONFIG_EICONCTRL_DEBUG)
AC_SUBST(CONFIG_DATADIR)
AC_SUBST(CONFIG_KERNELDIR)
AC_SUBST(CONFIG_SBINDIR)
AC_SUBST(CONFIG_CARD_SBINDIR)
AC_SUBST(CONFIG_MANDIR)
AC_SUBST(I4LVERSION)
AC_CONFIG_HEADER(config.h)

AC_OUTPUT(Makefile eiconctrl.man divaload.man divalog.man divalogd.man)
