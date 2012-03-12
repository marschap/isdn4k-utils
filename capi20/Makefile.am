## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = no-dependencies foreign
CLEANFILES              = *~ comperr
DISTCLEANFILES			= intltool-extract intltool-merge intltool-update
MAINTAINERCLEANFILES    = configure aclocal.m4 Makefile.in config.h.in
			  stamp-h.in
include_HEADERS = capi20.h capiutils.h capicmd.h capi_mod.h capi_debug.h
lib_LTLIBRARIES = libcapi20.la
lib_LIBRARIES   = libcapi20dyn.a

libcapi20_la_SOURCES = capi20.c capifunc.c convert.c
libcapi20_la_LDFLAGS = -version-info ${LIBCAPI_VERSION}:${LIBCAPI_VERSION_RELEASE}:${LIBCAPI_VERSION_AGE} -lrt -lc -ldl -rdynamic
libcapi20_la_CFLAGS = -fno-strict-aliasing -DLIBDIR=\"$(libdir)/capi\"

modulesdir = $(libdir)/capi
modules_LTLIBRARIES = lib_capi_mod_std.la lib_capi_mod_fritzbox.la lib_capi_mod_rcapi.la

lib_capi_mod_std_la_SOURCES = capi_mod_std.c
lib_capi_mod_std_la_CFLAGS = -fno-strict-aliasing
lib_capi_mod_std_la_LDFLAGS = -shared -version-info @CAPI_MODULE_LOADER_VERSION@:0:0
lib_capi_mod_std_la_LIBADD = libcapi20.la

lib_capi_mod_fritzbox_la_SOURCES = capi_mod_fritzbox.c
lib_capi_mod_fritzbox_la_CFLAGS = -fno-strict-aliasing
lib_capi_mod_fritzbox_la_LDFLAGS = -shared -version-info @CAPI_MODULE_LOADER_VERSION@:0:0
lib_capi_mod_fritzbox_la_LIBADD = libcapi20.la

lib_capi_mod_rcapi_la_SOURCES = capi_mod_rcapi.c
lib_capi_mod_rcapi_la_CFLAGS = -fno-strict-aliasing
lib_capi_mod_rcapi_la_LDFLAGS = -shared -version-info @CAPI_MODULE_LOADER_VERSION@:0:0
lib_capi_mod_rcapi_la_LIBADD = libcapi20.la

libcapi20dyn_a_SOURCES = capidyn.c
libcapi20dyn_a_CFLAGS = -fPIC
# libcapi20_la_LIBADD = $(LIBADD_DL)


pcdir = ${libdir}/pkgconfig
pc_DATA = capi20.pc

$(OBJECTS): libtool

export SED

libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck
