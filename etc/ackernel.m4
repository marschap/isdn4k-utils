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
		CPPFLAGS="-nostdinc -I${tst_kerneldir}/include -I${tst_kerneldir}/drivers/isdn"
		AC_TRY_CPP([#include <isdn_common.h>], lxdir=${tst_kerneldir},
		AC_MSG_RESULT("no"))
	fi
	if test "$lxdir" = "no" ; then
		AC_MSG_CHECKING([for linux kernel source in /usr/src/linux])
		CPPFLAGS="-nostdinc -I/usr/src/linux/include -I/usr/src/linux/drivers/isdn"
		AC_TRY_CPP([#include <isdn_common.h>], lxdir=/usr/src/linux)
	fi
	if test "$lxdir" = "no" ; then
		AC_MSG_RESULT("$lxdir")
		AC_MSG_CHECKING([for linux kernel source in /usr/local/src/linux])
		CPPFLAGS="-nostdinc -I/usr/local/src/linux/include -I/usr/local/src/linux/drivers/isdn"
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

