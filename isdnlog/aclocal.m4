dnl
dnl Check for postgres
dnl

AC_DEFUN(AC_CHECK_POSTGRES, [
	POSTGRESDIR=""
	pqdir="no"

	AC_ARG_WITH(postgres,
		[  --with-postgres=DIR     Set postgres directory []],
		tst_postgresdir="${withval}")

	if test "$tst_postgresdir" != "" || test "$CONFIG_ISDNLOG_POSTGRES" = "y" ; then
		AC_MSG_CHECKING([for postgres])
		if test "${tst_postgresdir}" != "" ; then
			CPPFLAGS="-nostdinc -I${tst_postgresdir}/include"
			AC_TRY_CPP([#include <libpq-fe.h>], pqdir=${tst_postgresdir})
		fi
		if test "$pqdir" = "no" ; then
			CPPFLAGS="-nostdinc -I/lib/postgres95/include"
			AC_TRY_CPP([#include <libpq-fe.h>], pqdir=/lib/postgres95)
		fi
		if test "$pqdir" = "no" ; then
			CPPFLAGS="-nostdinc -I/usr/lib/postgres95/include"
			AC_TRY_CPP([#include <libpq-fe.h>], pqdir=/usr/lib/postgres95)
		fi
		if test "$pqdir" = "no" ; then
			CPPFLAGS="-nostdinc -I/usr/local/postgres95/include"
			AC_TRY_CPP([#include <libpq-fe.h>], pqdir=/usr/local/postgres95)
		fi
		if test "$pqdir" = "no" ; then
			CPPFLAGS="-nostdinc -I/usr/local/lib/postgres95/include"
			AC_TRY_CPP([#include <libpq-fe.h>], pqdir=/usr/local/lib/postgres95)
		fi
		AC_MSG_RESULT("$pqdir")
	fi
	if test "$pqdir" != "no" ; then
		POSTGRES=1
		AC_DEFINE_UNQUOTED(POSTGRES,1)
	else
		pqdir=""
	fi
	POSTGRESDIR="$pqdir"
	AC_DEFINE_UNQUOTED(POSTGRESDIR,"$pqdir")
	AC_SUBST(POSTGRES)
	AC_SUBST(POSTGRESDIR)
])
