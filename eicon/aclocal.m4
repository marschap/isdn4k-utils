sinclude(../etc/ackernel.m4)dnl

dnl
dnl Check for XLOG
dnl

AC_DEFUN(AC_CHECK_XLOG, [
        OLD_CPPFLAGS="$CPPFLAGS"
        CPPFLAGS="-nostdinc -I${CONFIG_KERNELDIR} -I/usr/include"
        have_xlog="no"
        AC_MSG_CHECKING([for XLOG in ${CONFIG_KERNELDIR}/drivers/isdn/eicon/eicon.h])
        AC_TRY_COMPILE([#include <linux/types.h>
			#include <drivers/isdn/eicon/eicon.h>],int x = XLOG_OK;,have_xlog="yes",)
        AC_MSG_RESULT("${have_xlog}")
        CPPFLAGS="$OLD_CPPFLAGS"
        if test "$have_xlog" != "no" ; then
                AC_DEFINE(HAVE_XLOG)
        fi
        AC_SUBST(HAVE_XLOG)
])


dnl
dnl Check for PCI in native driver 
dnl

AC_DEFUN(AC_CHECK_NPCI, [
        OLD_CPPFLAGS="$CPPFLAGS"
        CPPFLAGS="-nostdinc -I${CONFIG_KERNELDIR} -I/usr/include"
        have_npci="no"
        AC_MSG_CHECKING([for pci code in ${CONFIG_KERNELDIR}/drivers/isdn/eicon/eicon.h])
        AC_TRY_COMPILE([#include <linux/types.h>
			#include <drivers/isdn/eicon/eicon.h>],eicon_pci_codebuf *p = NULL;,have_npci="yes",)
        AC_MSG_RESULT("${have_npci}")
        CPPFLAGS="$OLD_CPPFLAGS"
        if test "$have_npci" != "no" ; then
                AC_DEFINE(HAVE_NPCI)
        fi
        AC_SUBST(HAVE_NPCI)
])

