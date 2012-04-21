dnl Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])

AC_INIT(mkzonedb.c)
AC_PREFIX_DEFAULT([/usr])

CONFIG_MANDIR=${mandir:-"/usr/man"}
CONFIG_SBINDIR=${sbindir:-"/sbin"}
MANDATE=`grep CHECKIN mkzonedb.man.in | awk '{print $4}'`

dnl Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_CHECK_PROGS(BZIP2, [bzip2], [bzip2_not_found_or_installed])
if test "$BZIP2" = "bzip2_not_found_or_installed" ; then
	AC_MSG_ERROR([cannot find bzip2 - please install it.])
fi

AC_DEFINE_UNQUOTED(CDBEXTRALIBS, [], [extra library for CDB as database])
AC_DEFINE_UNQUOTED(DBEXT, [], [file extension for the library used])
AC_DEFINE_UNQUOTED(RDBEXT,[], [file extension for the library used])

if test -e ../cdb/i4l_cdb.c ; then
	AC_DEFINE(USE_CDB, 1, [using CDB as database])
	CDBEXTRALIBS="../cdb/i4l_cdb.a"
	DBEXT=".cdb"
	RDBEXT=".cdb"
	AC_MSG_RESULT(Using 'cdb' as database)
else
dnl Checks for libraries.
dnl Berkeley DB appends RDBEXT
CDBEXTRALIBS=""
AC_CHECK_LIB(gdbm, gdbm_open,
	[DBMLIB=-lgdbm
	 DBEXT=".gdbm"
	 RDBEXT=".gdbm"
	 AC_DEFINE(HAVE_LIBGDBM, 1, [using GDBM as database])],
	[AC_CHECK_LIB(dbm, dbm_open,
		[DBMLIB=-ldbm
		 RDBEXT=".db"
		 AC_DEFINE(HAVE_LIBDBM, 1, [using DBM as database])],
		[AC_CHECK_LIB(db, dbm_open,
			[DBMLIB=-ldb
			 RDBEXT=".db"
			 AC_DEFINE(HAVE_LIBDB, 1, [using DB as database])],
			[AC_MSG_ERROR([No db found])])])])
fi

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(unistd.h)

dnl Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

dnl Only 1,2,4 will work by now
AC_CHECK_SIZEOF(char,1)
AC_CHECK_SIZEOF(short,2)
AC_CHECK_SIZEOF(long,4)
AC_CHECK_SIZEOF(int,4)
AC_C_BIGENDIAN

dnl Checks for library functions.
AC_CHECK_FUNCS(strdup strtoul)

dnl Define which variables to substitute & which files to generate
AC_SUBST(DBMLIB)
AC_SUBST(DBEXT)
AC_SUBST(RDBEXT)
AC_SUBST(CDBEXTRALIBS)
AC_SUBST(CONFIG_SBINDIR)
AC_SUBST(CONFIG_MANDIR)
AC_SUBST(WORDS_BIGENDIAN)
AC_SUBST(MANDATE)
AC_CONFIG_HEADERS(config.h)

AC_OUTPUT(Makefile mkzonedb.man)
