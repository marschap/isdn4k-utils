## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = no-dependencies foreign
CLEANFILES              = *~ comperr
MAINTAINERCLEANFILES    = configure aclocal.m4 Makefile.in config.h.in
			  stamp-h.in
include_HEADERS = capi20.h capiutils.h capicmd.h
lib_LTLIBRARIES = libcapi20.la
lib_LIBRARIES   = libcapi20dyn.a

libcapi20_la_SOURCES = capi20.c capifunc.c convert.c
libcapi20_la_LDFLAGS = -version-info ${LIBCAPI_VERSION}:${LIBCAPI_VERSION_RELEASE}:${LIBCAPI_VERSION_AGE}
libcapi20_la_CFLAGS = -fno-strict-aliasing

libcapi20dyn_a_SOURCES = capidyn.c
libcapi20dyn_a_CFLAGS = -fPIC
# libcapi20_la_LIBADD = $(LIBADD_DL)

$(OBJECTS): libtool

export SED

libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck
