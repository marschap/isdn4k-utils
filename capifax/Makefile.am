##
## $Id: Makefile.am,v 1.2 1999/10/20 16:43:18 calle Exp $
##

AUTOMAKE_OPTIONS	= foreign
CLEANFILES              = *~ comperr
MAINTAINERCLEANFILES    = configure aclocal.m4 Makefile.in config.h.in \
			  stamp-h.in

INCLUDES = -I../capi20 $(all_includes)
CFLAGS = -Wall -O2
LDFLAGS = -L../capi20/.libs -L../capi20 $(all_libraries)
LDADD = -lcapi20

bin_PROGRAMS = capifax capifaxrcvd

common = c20msg.c capi.c connect.c contr.c data.c id.c init.c fax.c

capifax_SOURCES = $(common) capifax.c

capifaxrcvd_SOURCES = $(common) capifaxrcvd.c

