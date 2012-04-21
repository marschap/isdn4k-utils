dnl Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])

AC_INIT(makedest)
AC_PREFIX_DEFAULT([/usr])

CONFIG_MANDIR=`eval echo ${mandir:-"/usr/man"}`
CONFIG_SBINDIR=`eval echo ${sbindir:-"/sbin"}`

dnl Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_CHECK_PROGS(BZIP2, [bzip2], [bzip2_not_found_or_installed])
if test "$BZIP2" = "bzip2_not_found_or_installed" ; then
	AC_MSG_ERROR([cannot find bzip2 - please install it.])
fi

if test -e ../cdb/i4l_cdb.c  ; then
	AC_DEFINE(USE_CDB)
	CDBEXTRALIBS="../cdb/i4l_cdb.a"
	DBEXT=".cdb"
	RDBEXT=".cdb"
	AC_MSG_RESULT(Using 'cdb' as database)
else
dnl Checks for libraries.
dnl Berkeley DB appends RDBEXT
AC_CHECK_LIB(gdbm, gdbm_open,
	[DBMLIB=-lgdbm;DBEXT=".gdbm"; RDBEXT=".gdbm"; AC_DEFINE(HAVE_LIBGDBM)],
	[AC_CHECK_LIB(dbm, dbm_open,
		[DBMLIB=-ldbm; RDBEXT=".db"; AC_DEFINE(HAVE_LIBDBM)],
		[AC_CHECK_LIB(db, dbm_open,
			[DBMLIB=-ldb; RDBEXT=".db"; AC_DEFINE(HAVE_LIBDB)],
			[AC_MSG_ERROR([No db found])])])])
fi

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(unistd.h)

dnl Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

dnl Checks for library functions.
AC_CHECK_FUNCS(strdup strtoul)

dnl Define which variables to substitute & which files to generate
AC_SUBST(DBMLIB)
AC_SUBST(DBEXT)
AC_SUBST(RDBEXT)
AC_SUBST(CDBEXTRALIBS)
AC_SUBST(CC)
AC_SUBST(INSTALL)
AC_SUBST(CONFIG_SBINDIR)
AC_SUBST(CONFIG_MANDIR)

AC_OUTPUT(Makefile)
