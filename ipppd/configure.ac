dnl Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])

AC_INIT(auth.c)
dnl AC_PREFIX_DEFAULT([/usr])

I4LVERSION=${I4LVERSION:-"?.?"}
MANDATE=`grep CHECKIN ipppd.man.in | awk '{print $4}'`
CONFIG_MANDIR=${mandir:-"/usr/man"}
CONFIG_SBINDIR=${sbindir:-"/sbin"}
CONFIG_RUNDIR=${CONFIG_RUNDIR:-"/var/run"}
CONFIG_IPPPD_MSCHAP=${CONFIG_IPPPD_MSCHAP:-"n"}
CONFIG_IPPPD_RADIUS=${CONFIG_IPPPD_RADIUS:-"n"}
CONFIG_IPPP_FILTER=${CONFIG_IPPP_FILTER:-"n"}
CONFIG_RADIUS_WTMP_LOGGING=${CONFIG_RADIUS_WTMP_LOGGING:-"n"}
RADIUS_CLIENT_CONFIG_FILE=${RADIUS_CLIENT_CONFIG_FILE:-"/usr/local/lib/radiusclient/radiusclient.conf"}

dnl Checks for programs.
AC_PROG_INSTALL
AC_PROG_CC

dnl Checks for libraries.
AC_CHECK_LIB(bsd, daemon,
	HAVE_LIBBSD=1; AC_DEFINE(HAVE_LIBBSD, [], [libbsd available]),
	[AC_MSG_WARN(Could not find libbsd, build disabled)])
dnl Replace `main' with a function in -lcrypt:
AC_CHECK_LIB(crypt, main,
	[HAVE_LIBCRYPT=1; AC_DEFINE(HAVE_LIBCRYPT, [], [libcrypt available])])
AC_CHECK_LIB(des, des_ecb_encrypt,
	[HAVE_LIBDES=1; AC_DEFINE(HAVE_LIBDES, [], [libdes available])])
AC_CHECK_LIB(ssl, DES_ecb_encrypt,
	[HAVE_LIBSSL=1; AC_DEFINE(HAVE_LIBSSL, [], [libssl available])])

dnl Checks for header files.
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS(fcntl.h limits.h paths.h sys/file.h sys/ioctl.h sys/time.h syslog.h unistd.h)
AC_CHECK_HEADER(shadow.h,
	[HAVE_SHADOW_H=1; AC_DEFINE(HAVE_SHADOW_H, [], [shadow.h available])])
AC_CHECK_HEADER(linux/isdn_lzscomp.h,
	[HAVE_LZSCOMP_H=1; AC_DEFINE(HAVE_LZSCOMP_H, [], [isdn_lzscomp.h available])])
AC_CHECK_HEADER(pcap-bpf.h,
	[HAVE_PCAP_BPF_H=1; AC_DEFINE(HAVE_PCAP_BPF_H, [], [pcap-bpf.h availabe])],
	[AC_CHECK_HEADER(net/bpf.h,
		[HAVE_NET_BPF_H=1; AC_DEFINE(HAVE_NET_BPF_H, [], [net/bpf.h available])],
		[AC_MSG_ERROR(Could not find pcap header file)])])

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_PID_T
AC_HEADER_TIME
AC_TYPE_UID_T

dnl Checks for library functions.
AC_TYPE_GETGROUPS
AC_FUNC_MEMCMP
AC_FUNC_VPRINTF
AC_CHECK_FUNCS(gethostname gettimeofday select socket strdup strerror strtol strtoul uname)

dnl Optional /var/run directory
AC_ARG_WITH(rundir,
	[  --with-rundir=DIR       Set directory for .pid files. [/var/run]],
	CONFIG_RUNDIR="${withval}"
	AC_DEFINE(CONFIG_RUNDIR,"${withval}", [directory for run files]),
)

dnl Optional MSCHAP
AC_ARG_ENABLE(mschap,
	[  --enable-mschap         Enable Microsoft chap authentication [no]],
	CONFIG_IPPPD_MSCHAP="y"
	AC_DEFINE(CONFIG_IPPPD_MSCHAP,"y", [enable MSCHAP authentication]),
)

dnl Optional RADIUS
AC_ARG_ENABLE(radius,
	[  --enable-radius         Enable RADIUS authentication [no]],
	CONFIG_IPPPD_RADIUS="y"
	AC_DEFINE(CONFIG_IPPPD_RADIUS,"y", [enable radius authentication]),
)

dnl Optional radiusclient config file
AC_ARG_WITH(radiusclient_config,
	[  --with-radiusclient=FILE Set filename for radiusclient configuration],
	RADIUS_CLIENT_CONFIG_FILE="${withval}"
	AC_DEFINE(RADIUS_CLIENT_CONFIG_FILE,"${withval}",[radius client config file]),
)

dnl Optional RADIUS_WTMP_LOGGING
AC_ARG_ENABLE(radius_wtmp_logging,
	[  --enable-radius-wtmp       Enable RADIUS_WTMP_LOGGING authentication [no]],
	CONFIG_IPPPD_RADIUS_WTMP_LOGGING="y"
	AC_DEFINE(CONFIG_IPPPD_RADIUS_WTMP_LOGGING,"y", [enable radius wtmp logging]),
)

dnl Optional DEBUGGING
AC_ARG_ENABLE(debug,
	[  --enable-debug          Enable debugging [no]],
	CONFIG_IPPPD_DEBUGFLAGS="-DDEBUGALL"
	AC_DEFINE(CONFIG_IPPPD_DEBUGFLAGS,"-DDEBUGALL", [enable debugging]),
)

AC_ARG_ENABLE(ippp-filter,
	[  --enable-ippp-filter    Enable IPPP Filters (needs kernel supports) [no]],
	CONFIG_IPPP_FILTER="y"
	AC_DEFINE(CONFIG_IPPP_FILTER,"y", [IPPP filters enabled]),
)

dnl Define which variables to substitute & which files to generate
AC_SUBST(I4LVERSION)
AC_SUBST(MANDATE)
AC_SUBST(HAVE_LIBDES)
AC_SUBST(HAVE_LIBSSL)
AC_SUBST(HAVE_LIBCRYPT)
AC_SUBST(HAVE_LIBBSD)
AC_SUBST(HAVE_SHADOW_H)
AC_SUBST(HAVE_LZSCOMP_H)
AC_SUBST(HAVE_PCAP_BPF_H)
AC_SUBST(HAVE_NET_BPF_H)
AC_SUBST(CONFIG_RUNDIR)
AC_SUBST(CONFIG_SBINDIR)
AC_SUBST(CONFIG_MANDIR)
AC_SUBST(CONFIG_IPPPD_MSCHAP)
AC_SUBST(CONFIG_IPPPD_RADIUS)
AC_SUBST(RADIUS_CLIENT_CONFIG_FILE)
AC_SUBST(CONFIG_RADIUS_WTMP_LOGGING)
AC_SUBST(CONFIG_IPPPD_DEBUGFLAGS)
AC_SUBST(CONFIG_IPPP_FILTER)
AC_CONFIG_HEADERS(config.h)

AC_OUTPUT(Makefile ipppd.man pathnames.h)
