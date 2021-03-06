#
# old capiutils.h missed the sending_complete parameter
#
#
AC_DEFUN([CS_TEST_ALERT],
[AC_MSG_CHECKING([for capi20 ALERT with sending complete])
  AC_TRY_COMPILE([#include <capiutils.h>],
    [_cmsg cm; ALERT_REQ(&cm, 1, 1, 1, NULL, NULL, NULL, NULL, NULL);],
    [
      AC_DEFINE([HAVE_ALERT_SENDING_COMPLETE],1,[we have SENDING_COMPLETE in ALERT_REQ])
      AC_MSG_RESULT([yes])
    ],
    [AC_MSG_RESULT([no])]
)]) dnl CS_TEST_ALERT
#
# old capiutils.h missed the Globalconfiguration in BProtocol
#
#
AC_DEFUN([CS_TEST_GLOBALCONFIG],
[AC_MSG_CHECKING([for capi20 ALERT with sending complete])
  AC_TRY_COMPILE([#include <capiutils.h>],
    [_cmsg cm;void *p; p=CONNECT_REQ_GLOBALCONFIGURATION(&cm);],
    [
      AC_DEFINE([HAVE_GLOBALCONFIGURATION],1,[we have GLOBALCONFIGURATION in BProtocol])
      AC_MSG_RESULT([yes])
    ],
  [AC_MSG_RESULT([no])]
)]) dnl CS_TEST_ALERT
