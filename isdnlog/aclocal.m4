dnl
dnl Check for postgres
dnl

AC_DEFUN(AC_CHECK_POSTGRES, [
	POSTGRESDIR=""
	pqdir="no"
	tst_postgresdir="$CONFIG_ISDNLOG_POSTGRESDIR"

	AC_ARG_WITH(postgres,
		[  --with-postgres=DIR     Set postgres directory []],
		tst_postgresdir="${withval}")

	if test "$tst_postgresdir" != "" || test "$CONFIG_ISDNLOG_POSTGRES" = "y" ; then
		AC_MSG_CHECKING([for postgres in ${tst_postgresdir}])
		if test "${tst_postgresdir}" != "" ; then
			AC_EGREP_HEADER(PGconn,${tst_postgresdir}/include/libpq-fe.h,
			pqdir=${tst_postgresdir})
		fi
		if test "$pqdir" = "no" ; then
			AC_MSG_RESULT("$pqdir")
			AC_MSG_CHECKING([for postgres in /lib/postgres95])
			AC_EGREP_HEADER(PGconn,/lib/postgre95/include/libpq-fe.h,
			pqdir=/lib/postgres95)
		fi
		if test "$pqdir" = "no" ; then
			AC_MSG_RESULT("$pqdir")
			AC_MSG_CHECKING([for postgres in /usr/lib/postgres95])
			AC_EGREP_HEADER(PGconn,/usr/lib/postgre95/include/libpq-fe.h,
			pqdir=/usr/lib/postgres95)
		fi
		if test "$pqdir" = "no" ; then
			AC_MSG_RESULT("$pqdir")
			AC_MSG_CHECKING([for postgres in /usr/local/postgres95])
			AC_EGREP_HEADER(PGconn,/usr/lib/local/postgre95/include/libpq-fe.h,
			pqdir=/usr/local/postgres95)
		fi
		if test "$pqdir" = "no" ; then
			AC_MSG_RESULT("$pqdir")
			AC_MSG_CHECKING([for postgres in /usr/local/lib/postgres95])
			AC_EGREP_HEADER(PGconn,/usr/lib/local/lib/postgre95/include/libpq-fe.h,
			pqdir=/usr/local/lib/postgres95)
		fi
	fi
	if test "$pqdir" != "no" ; then
		AC_MSG_RESULT("yes")
		POSTGRES=1
		AC_DEFINE_UNQUOTED(POSTGRES,1)
	else
		AC_MSG_RESULT("no POSTGRES DISABLED")
		pqdir=""
	fi
	POSTGRESDIR="$pqdir"
	AC_DEFINE_UNQUOTED(POSTGRESDIR,"$pqdir")
	AC_SUBST(POSTGRES)
	AC_SUBST(POSTGRESDIR)
])
dnl
dnl Try finding linux sourcetree
dnl

AC_DEFUN(AC_FIND_KERNEL, [
	OLD_CPPFLAGS="$CPPFLAGS"
	lxdir="no"
	eval tst_kerneldir=$CONFIG_KERNELDIR

	AC_ARG_WITH(kernel,
		[  --with-kernel=DIR       Set kernel source directory [/usr/src/linux]],
		DOTEST="y"; tst_kerneldir="${withval}")

	if test "$DOTEST" = "y" || test "$CONFIG_KERNELDIR" != "" ; then
		AC_MSG_CHECKING([for linux kernel source in ${tst_kerneldir}])
		CPPFLAGS="-nostdinc -I${tst_kerneldir}/drivers/isdn"
		AC_TRY_CPP([#include <isdn_common.h>], lxdir=${tst_kerneldir},
		AC_MSG_RESULT("no"))
	fi
	if test "$lxdir" = "no" ; then
		AC_MSG_CHECKING([for linux kernel source in /usr/src/linux])
		CPPFLAGS="-nostdinc -I/usr/src/linux/drivers/isdn"
		AC_TRY_CPP([#include <isdn_common.h>], lxdir=/usr/src/linux)
	fi
	if test "$lxdir" = "no" ; then
		AC_MSG_RESULT("$lxdir")
		AC_MSG_CHECKING([for linux kernel source in /usr/local/src/linux])
		CPPFLAGS="-nostdinc -I/usr/local/src/linux/drivers/isdn"
		AC_TRY_CPP([#include <isdn_common.h>], lxdir=/usr/local/src/linux)
	fi
	if test "$lxdir" != "no" ; then
		AC_MSG_RESULT("yes")
	else
		lxdir=""
		AC_MSG_ERROR("Kernel source not found. You MUST specify a correct path to the linux source in the configuration.")
	fi
	CONFIG_KERNELDIR="$lxdir"
	CPPFLAGS="$OLD_CPPFLAGS"
	AC_DEFINE_UNQUOTED(CONFIG_KERNELDIR,"$lxdir")
	AC_SUBST(CONFIG_KERNELDIR)
])
