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

dnl
dnl Check for definition of triggercps
dnl in struct isdn_net_ioctl_cfg in linux/isdn.h
dnl

AC_DEFUN(AC_CHECK_TRIGGERCPS, [
	OLD_CPPFLAGS="$CPPFLAGS"
	CPPFLAGS="-nostdinc -I${CONFIG_KERNELDIR}/include"
	have_triggercps="no"
	AC_MSG_CHECKING([for triggercps in ${CONFIG_KERNELDIR}/include/linux/isdn.h])
	AC_TRY_COMPILE([#include <linux/isdn.h>],isdn_net_ioctl_cfg x; int i = x.triggercps;,have_triggercps="yes",)
	AC_MSG_RESULT("${have_triggercps}")
	CPPFLAGS="$OLD_CPPFLAGS"
	if test "$have_triggercps" != "no" ; then
		AC_DEFINE(HAVE_TRIGGERCPS)
		AC_SUBST(HAVE_TRIGGERCPS)
	fi
])

AC_DEFUN(AC_CHECK_CISCOK, [
	OLD_CPPFLAGS="$CPPFLAGS"
	CPPFLAGS="-nostdinc -I${CONFIG_KERNELDIR}/include"
	have_ciscokeepalive="no"
	AC_MSG_CHECKING([for Cisco-Keepalive in ${CONFIG_KERNELDIR}/include/linux/isdn.h])
	AC_TRY_COMPILE([#include <linux/isdn.h>],int x = ISDN_NET_ENCAP_CISCOHDLCK;,have_ciscokeepalive="yes",)
	AC_MSG_RESULT("${have_ciscokeepalive}")
	CPPFLAGS="$OLD_CPPFLAGS"
	if test "$have_ciscokeepalive" != "no" ; then
		AC_DEFINE(HAVE_CISCOKEEPALIVE)
		AC_SUBST(HAVE_CISCOKEEPALIVE)
	fi
])

AC_DEFUN(AC_CHECK_TIMRU, [
	OLD_CPPFLAGS="$CPPFLAGS"
	CPPFLAGS="-nostdinc -I${CONFIG_KERNELDIR}/include"
	have_timru="no"
	AC_MSG_CHECKING([for TIMRU in ${CONFIG_KERNELDIR}/include/linux/isdn.h])
	AC_TRY_COMPILE([#include <linux/isdn.h>],int x = IIOCNETARU;,have_timru="yes",)
	AC_MSG_RESULT("${have_timru}")
	CPPFLAGS="$OLD_CPPFLAGS"
	if test "$have_timru" != "no" ; then
		AC_DEFINE(HAVE_TIMRU)
		AC_SUBST(HAVE_TIMRU)
	fi
])
