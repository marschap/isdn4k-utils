# Process this file with autoconf to produce a configure script.
AC_INIT(ant-phone)
AM_INIT_AUTOMAKE(ant-phone, "0.1.12", ant-phone-devel@nongnu.org)
AC_PREREQ(2.53)
AC_CONFIG_SRCDIR([config.h.in])
AM_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL

AM_PROG_LEX
if test "$LEX" != flex; then
  LEX="$SHELL $missing_dir/missing flex"
  AC_SUBST(LEX_OUTPUT_ROOT, lex.yy)
  AC_SUBST(LEXLIB, '')
fi
AC_PROG_YACC

# Checks for libraries.
AC_CHECK_LIB([m], [floor])
AC_CHECK_LIB([sndfile], [sf_open],, AC_MSG_ERROR(You need the libsndfile headers to build this package))

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h limits.h math.h pwd.h stddef.h stdlib.h string.h sys/ioctl.h sys/stat.h sys/time.h sys/types.h termios.h unistd.h sndfile.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_HEADER_TIME

# Checks for library functions.
AC_PROG_GCC_TRADITIONAL
AC_FUNC_MALLOC
AC_TYPE_SIGNAL
AC_FUNC_STRFTIME
AC_CHECK_FUNCS([floor select strdup strstr strtol mkdir strcasecmp])

# GTK+ 2.0:
PKG_CHECK_MODULES(DEPS, gtk+-2.0)
AC_SUBST(DEPS_CFLAGS)
AC_SUBST(DEPS_LIBS)

#
# If we're running gcc, add '-Wall -W' to CFLAGS, and add
# '-D_U_="__attribute__((unused))"' as well, so we can use _U_ to
# flag unused function arguments and not get warnings about them.
#
# Otherwise, add '-D_U_=""', so that _U_ used to flag an unused function
# argument will compile with non-GCC compilers.
#
AC_MSG_CHECKING(to see if we can add '-Wall -W' to CFLAGS)
if test x$GCC != x ; then
  CFLAGS="$CFLAGS -D_U_=\"__attribute__((unused))\" -Wall -W -D_GNU_SOURCE -O3"
  AC_MSG_RESULT(yes)
else
  CFLAGS="-D_U_=\"\" $CFLAGS"
  AC_MSG_RESULT(no)
fi

# GNU gettext
AM_GNU_GETTEXT
AM_GNU_GETTEXT_VERSION(0.12.1)

# directory containing configuration support files
# defaults to package root
#AC_CONFIG_AUX_DIR

AC_CONFIG_FILES([Makefile
		 intl/Makefile
		 po/Makefile.in
		 doc/Makefile
		 src/Makefile])
AC_OUTPUT

