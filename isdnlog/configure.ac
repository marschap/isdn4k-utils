dnl Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])

AC_INIT(isdnlog/isdnlog.c)
AC_PREFIX_DEFAULT([/usr])

dnl i4l package setup, get variables from central config-snippet .config
dnl If a variable is not set there, set default also.

if test -r ../.config ; then
	source ../.config
fi

I4LVERSION=${I4LVERSION:-"?.?"}
I4LCONFDIR=${CONFIG_I4LCONFDIR:-"/etc/isdn"}
OLDCONFDIR=${CONFIG_ISDNLOG_OLDI4LCONFDIR:-"/etc/isdnlog"}
OLDCONFFILE=${CONFIG_ISDNLOG_OLDI4LCONFFILE:-"isdnlog.conf"}
CONFFILE=${CONFIG_CONFFILE:-"isdn.conf"}
CALLERIDFILE=${CONFIG_CALLERIDFILE:-"callerid.conf"}
RUNDIR=${CONFIG_RUNDIR:-"/var/run"}
SERV_PORT=${CONFIG_ISDNLOG_SERV_PORT:-20011}
USERFILE=${CONFIG_ISDNLOG_USERFILE:-"isdnlog.users"}
LOGFILE=${CONFIG_ISDNLOG_LOGFILE:-"/var/log/isdn.log"}
CHARGEFILE=${CONFIG_ISDNLOG_CHARGEFILE:-"charge.dat"}
RELOADCMD=${CONFIG_ISDNLOG_RELOADCMD:-"reload"}
STOPCMD=${CONFIG_ISDNLOG_STOPCMD:-"stop"}
REBOOTCMD=${CONFIG_ISDNLOG_REBOOTCMD:-"/sbin/reboot"}
COUNTRYCODE=${CONFIG_COUNTRYCODE:-"49"}
COUNTRYPREFIX=${CONFIG_COUNTRY_PREFIX:-"+"}
AREACODE=${CONFIG_AREACODE:-""}
AREAPREFIX=${CONFIG_AREA_PREFIX:-"0"}
CONFIG_LIB_AREACODE=${CONFIG_LIB_AREACODE:-"y"}
CONFIG_ISDNLOG_POSTGRESDIR=${CONFIG_ISDNLOG_POSTGRESDIR:-""}
CONFIG_ISDNLOG_MYSQLDIR=${CONFIG_ISDNLOG_MYSQLDIR:-""}
LIBISDNDIR="../lib"
bindir=${CONFIG_BINDIR:-"/usr/bin"}
sbindir=${CONFIG_SBINDIR:-"/sbin"}
mandir=${CONFIG_MANDIR:-"/usr/man"}
datadir=${CONFIG_DATADIR:-"/usr/lib/isdn"}
DOCDIR=${CONFIG_ISDNLOG_DOCDIR:-"/usr/doc/packages/i4l/isdnlog"}
DATADIR=$datadir
MANDATE_CALLERID=`grep CHECKIN isdnlog/callerid.conf.5.in | awk '{print $4}'`
MANDATE_ISDN_CONF=`grep CHECKIN isdnlog/isdn.conf.5.in | awk '{print $4}'`
MANDATE_RATE_CONF=`grep CHECKIN tools/rate.conf.5.in | awk '{print $4}'`
MANDATE_ISDNFORMAT=`grep CHECKIN isdnlog/isdnformat.5.in | awk '{print $4}'`
MANDATE_ISDNLOG5=`grep CHECKIN isdnlog/isdnlog.5.in | awk '{print $4}'`
MANDATE_ISDNLOG8=`grep CHECKIN isdnlog/isdnlog.8.in | awk '{print $4}'`
MANDATE_ISDNLOG_USERS=`grep CHECKIN isdnlog/isdnlog.users.5.in | awk '{print $4}'`
MANDATE_ISDNREP=`grep CHECKIN isdnrep/isdnrep.1.in | awk '{print $4}'`
MANDATE_ISDNBILL=`grep CHECKIN isdnrep/isdnbill.1.in | awk '{print $4}'`
MANDATE_ISDNCONF=`grep CHECKIN isdnconf/isdnconf.1.in | awk '{print $4}'`
ISDNLOG_VERSION=`grep "^VERSION \+= \+" Makefile.in | awk '{print $3}'`

if test "$CONFIG_LIB_AREACODE" = "y" ; then
	ALIB=area
fi

dnl
dnl To make it possible to set variables in policy.h
dnl they have to be defined with AC_DEFINE...
dnl
AC_DEFINE_UNQUOTED(OLDCONFDIR, ["$OLDCONFDIR"], [isdnlog old config directory])
AC_DEFINE_UNQUOTED(OLDCONFFILE, ["$OLDCONFFILE"], [isdnlog old config file])
AC_DEFINE_UNQUOTED(DATADIR, ["$datadir"], [isdnlog data directory])
AC_DEFINE_UNQUOTED(CONFIG_DATADIR, ["$CONFIG_DATADIR"], [isdnlog data directory])
AC_DEFINE_UNQUOTED(CONFIG_I4LCONFDIR, ["$CONFIG_I4LCONFDIR"], [isdnlog i4l config directory])
AC_DEFINE_UNQUOTED(SERV_PORT, [$SERV_PORT], [isdnlog server port])
AC_DEFINE_UNQUOTED(USERFILE, ["$USERFILE"], [isdnlog users file])
AC_DEFINE_UNQUOTED(LOGFILE, ["$LOGFILE"], [isdnlog log file])
AC_DEFINE_UNQUOTED(CHARGEFILE, ["$CHARGEFILE"], [isdnlog charge file])
AC_DEFINE_UNQUOTED(RELOADCMD, ["$RELOADCMD"], [isdnlog reload command])
AC_DEFINE_UNQUOTED(STOPCMD, ["$STOPCMD"], [sdnlog stop command])
AC_DEFINE_UNQUOTED(REBOOTCMD, ["$REBOOTCMD"], [isdnlog reboot command])

if test "$CONFIG_ISDN_LOG_DE" = "y" ; then
	NATION="de"
	NATION_MACRO="ISDN_DE"
	AC_DEFINE(ISDN_DE, 1, [ISDN in Germany])
fi
if test "$CONFIG_ISDN_LOG_AT" = "y" ; then
	NATION="at"
	NATION_MACRO="ISDN_AT"
	AC_DEFINE(ISDN_AT, 1, [ISDN in Austria])
fi
if test "$CONFIG_ISDN_LOG_CH" = "y" ; then
	NATION="ch"
	NATION_MACRO="ISDN_CH"
	AC_DEFINE(ISDN_CH, 1, [ISDN in Switzerland])
fi
if test "$CONFIG_ISDN_LOG_NL" = "y" ; then
	NATION="nl"
	NATION_MACRO="ISDN_NL"
	AC_DEFINE(ISDN_NL, 1, [ISDN in the Netherlands])
fi
if test "$CONFIG_ISDN_LOG_LU" = "y" ; then
	NATION="lu"
	NATION_MACRO="ISDN_LU"
	AC_DEFINE(ISDN_LU, 1, [ISDN in Luxemburg])
fi
if test "$CONFIG_ISDN_LOG_XX" = "y" ; then
	NATION=$CONFIG_ISDN_LOG_CC
	NATION_MACRO="ISDN_XX"
	AC_DEFINE(ISDN_XX, 1, [ISDN])
fi

dnl Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_AWK
AC_PATH_PROG(SED,sed)
AC_PROG_MAKE_SET
AC_PATH_PROGS(BZIP2, bzip2, /usr/bin/bzip2, $PATH:/bin:/usr/bin:/usr/local/bin)

dnl Checks for libraries.
if test -e tools/cdb/i4l_cdb.c ; then
    AC_DEFINE(USE_CDB, 1, [use CDB as database])
    CDBEXTRALIBS="tools/cdb/i4l_cdb.a"
    DBEXT=".cdb"
    RDBEXT=".cdb"
    AC_MSG_RESULT([Using CDB as database])
else
AC_CHECK_LIB(gdbm, gdbm_open,
	[DBMLIB=-lgdbm
	 AC_DEFINE(HAVE_LIBGDBM, 1, [use GDBM as database])],
	[AC_CHECK_LIB(dbm, dbm_open,
		[DBMLIB=-ldbm
		 AC_DEFINE(HAVE_LIBDBM, 1, [use DBM as database])],
		[AC_CHECK_LIB(db, dbm_open,
			[DBMLIB=-ldb
			 AC_DEFINE(HAVE_LIBDB, 1, [use DB as database])])])])
fi
AC_DEFINE_UNQUOTED(RDBEXT, ["$RDBEXT"], [extension of the respective DB files])

dnl log database support
AC_CHECK_POSTGRES
AC_CHECK_MYSQLDB
AC_CHECK_ORACLE

dnl TODO: replace main by a funcion within libdbmalloc
#AC_CHECK_LIB(dbmalloc, main)

dnl Checks for header files.
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS(fcntl.h limits.h sys/ioctl.h sys/time.h syslog.h unistd.h)

dnl Manual config
AC_ARG_WITH(isdnlib,
	[  --with-isdnlib=DIR      Set isdn library [../lib]],
	LIBISDNDIR=$withval; AC_DEFINE_UNQUOTED(LIBISDNDIR, ["$withval"], [ISDN library]))
AC_ARG_WITH(area-lib,
	[  --with-area-lib=STRING  Set area library type [area]],
	ALIB=$withval)
AC_ARG_WITH(charge,
	[  --with-charge=XX        Set country specific chargeinfo [DE]],
	CHARGECOUNTRY=$withval)
AC_ARG_WITH(oldconfdir,
	[  --with-oldconfdir=DIR   Set old config directory [/etc/isdnlog]],
	OLDCONFDIR=$withval; AC_DEFINE_UNQUOTED(OLDCONFDIR, ["$withval"], [isdnlog old config directory]))
AC_ARG_WITH(oldconf,
	[  --with-oldconf=NAME     Set old config file name [isdnlog.conf]],
	OLDCONFFILE=$withval; AC_DEFINE_UNQUOTED(OLDCONFFILE, ["$withval"], [isdnlog old config file]))
AC_ARG_WITH(datadir,
        [  --with-datadir=DIR      Set data directory [/usr/lib/isdn]],
        datadir=$withval; AC_DEFINE_UNQUOTED(DATADIR, ["$withval"], [isdnlog data directory]))
AC_ARG_WITH(sport,
	[  --with-sport=INT        Set server port [20011]],
	SERV_PORT=$withval; AC_DEFINE_UNQUOTED(SERV_PORT, [$withval], [isdnlog server port]))

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_STRUCT_TM

dnl Checks for C library functions.
AC_FUNC_MEMCMP
AC_TYPE_SIGNAL
AC_FUNC_UTIME_NULL
AC_CHECK_FUNCS(mktime select socket stime strdup strerror strftime strstr strtod strtol)

dnl Set variables in output files.

if test "$ALIB" = "area" ; then
	LIBAREA=1
	AC_DEFINE(LIBAREA, 1, [area library])
fi

case "$CHARGECOUNTRY" in
	DE)
		NATION="de"
		NATION_MACRO="ISDN_DE"
		AC_DEFINE(ISDN_DE, 1, [ISDN in Germany])
		;;
	AT)
		NATION="at"
		NATION_MACRO="ISDN_AT"
		AC_DEFINE(ISDN_AT, 1, [ISDN in Austria])
		;;
	CH)
		NATION="ch"
		NATION_MACRO="ISDN_CH"
		AC_DEFINE(ISDN_CH, 1, [ISDN in Switzerland])
		;;
	NL)
		NATION="nl"
		NATION_MACRO="ISDN_NL"
		AC_DEFINE(ISDN_NL, 1, [ISDN in the Netherlands])
		;;
esac

AC_SUBST(AREACODE)
AC_SUBST(AREAPREFIX)
AC_SUBST(CALLERIDFILE)
AC_SUBST(CDBEXTRALIBS)
AC_SUBST(CONFFILE)
AC_SUBST(COUNTRYCODE)
AC_SUBST(COUNTRYPREFIX)
AC_SUBST(DATADIR)
AC_SUBST(DBMLIB)
AC_SUBST(DOCDIR)
AC_SUBST(I4LCONFDIR)
AC_SUBST(I4LVERSION)
AC_SUBST(ISDNLOG_VERSION)
AC_SUBST(LIBAREA)
AC_SUBST(LIBISDNDIR)
AC_SUBST(LOGFILE)
AC_SUBST(MANDATE_CALLERID)
AC_SUBST(MANDATE_ISDNBILL)
AC_SUBST(MANDATE_ISDNCONF)
AC_SUBST(MANDATE_ISDN_CONF)
AC_SUBST(MANDATE_ISDNFORMAT)
AC_SUBST(MANDATE_ISDNLOG5)
AC_SUBST(MANDATE_ISDNLOG8)
AC_SUBST(MANDATE_ISDNLOG_USERS)
AC_SUBST(MANDATE_ISDNREP)
AC_SUBST(MANDATE_RATE_CONF)
AC_SUBST(NATION)
AC_SUBST(NATION_MACRO)
AC_SUBST(OLDCONFDIR)
AC_SUBST(OLDCONFFILE)
AC_SUBST(REBOOTCMD)
AC_SUBST(RELOADCMD)
AC_SUBST(RUNDIR)
AC_SUBST(SERV_PORT)
AC_SUBST(STOPCMD)
AC_SUBST(USERFILE)
AC_CONFIG_HEADERS(policy.h)

AC_OUTPUT(Makefile isdnlog/callerid.conf.5 isdnlog/isdn.conf.5 tools/rate.conf.5 isdnlog/isdnformat.5 isdnlog/isdnlog.5 isdnlog/isdnlog.8 isdnlog/isdnlog.users.5 isdnrep/isdnrep.1 isdnrep/isdnbill.1 isdnconf/isdnconf.1 samples/isdn.conf samples/isdn.conf.at samples/isdn.conf.de samples/isdn.conf.lu samples/isdn.conf.nl samples/isdn.conf.no samples/isdn.conf.pl)
