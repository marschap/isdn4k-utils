sinclude(../etc/ackernel.m4)dnl

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
	fi
	AC_SUBST(HAVE_TIMRU)
])
