/*
 * Linux configuration file
 * $Id: config.h,v 1.2 1999/10/05 21:23:23 he Exp $
 */
#include <linux/version.h>
#if defined(LINUX_VERSION_CODE)
#if LINUX_VERSION_CODE >= 66061
/* 1.2.13 or later */
#define HAVE_DIRFD
#endif
#endif
#undef  BSD
#define HAVE_DIRENT
#define HAVE_FLOCK
#define HAVE_FTW
#define HAVE_GETCWD
#define HAVE_GETDTABLESIZE
#undef  HAVE_PSTAT
#define HAVE_ST_BLKSIZE
#undef  HAVE_SYSINFO
#define HAVE_SYSCONF
#define HAVE_UT_UT_HOST
#define HAVE_VPRINTF
#define HAVE_SNPRINTF
#define LINUX
#define OVERWRITE
#undef  REGEX
#define SPT_TYPE SPT_REUSEARGV
#define  SHADOW_PASSWORD
#define UPLOAD
#undef  USG
#define SVR4
#define FACILITY LOG_DAEMON
#define HAVE_SYS_VFS
#define HAVE_SYMLINK
#define UTMAXTYPE
/* #define USE_ETC */
#define USE_LOG
#define USE_VAR
#define VAR_RUN
#define VIRTUAL
#define NEED_SIGFIX

#if !defined(__USE_POSIX)
#define __USE_POSIX
#endif
#include <limits.h>
#ifndef NBBY
#define NBBY 8
#endif
#ifndef NCARGS
#ifdef _POSIX_ARG_MAX
#define NCARGS _POSIX_ARG_MAX
#endif
#endif

#include <stdlib.h>		/* here instead of in all the srcs.  _H*/
#include <unistd.h>

typedef void SIGNAL_TYPE;

#include "../config.h"

#define realpath realpath_on_steroids	/* hack to work around unistd.h */
