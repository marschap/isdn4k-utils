##
## $Id: Makefile.am,v 1.1 1999/10/20 16:43:19 calle Exp $
##

AUTOMAKE_OPTIONS	= foreign
CLEANFILES              = *~ comperr
MAINTAINERCLEANFILES    = configure aclocal.m4 Makefile.in config.h.in \
			  stamp-h.in comperr

INCLUDES = -I../capi20 -I$(CONFIG_KERNELDIR)/drivers/isdn/avmb1 $(all_includes)
CFLAGS = -Wall -O2
LDFLAGS = -L../capi20/.libs -L../capi20 $(all_libraries)
LDADD = -lcapi20

bin_PROGRAMS = rcapid

rcapid_SOURCES = rcapid.c
