dnl
dnl Check for definition of triggercps
dnl in struct isdn_net_ioctl_cfg in linux/isdn.h
dnl

AC_DEFUN(AC_CHECK_TRIGGERCPS, [
	have_triggercps="no"
	AC_MSG_CHECKING([for triggercps in linux/isdn.h])
	AC_TRY_COMPILE([#include <linux/isdn.h>],isdn_net_ioctl_cfg x; int i = x.triggercps;,have_triggercps="yes",)
	AC_MSG_RESULT("${have_triggercps}")
	if test "$have_triggercps" != "no" ; then
		AC_MSG_RESULT("yes")
		AC_DEFINE(HAVE_TRIGGERCPS)
		AC_SUBST(HAVE_TRIGGERCPS)
	fi
])