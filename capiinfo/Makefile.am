##
## $Id: Makefile.am,v 1.1 1999/04/16 15:40:43 calle Exp $
##

AUTOMAKE_OPTIONS	= foreign
CLEANFILES              = *~
MAINTAINERCLEANFILES    = configure aclocal.m4 Makefile.in config.h.in \
			  stamp-h.in comperr

INCLUDES = -I../capi20 $(all_includes)
CFLAGS = -Wall -O2
LDFLAGS = -L../capi20 $(all_libraries)
LDADD = -lcapi20

bin_PROGRAMS = capiinfo

capiinfo_SOURCES = capiinfo.c
