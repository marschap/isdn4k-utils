##
## $Id: Makefile.am,v 1.2 1999/06/29 10:41:30 calle Exp $
##

CONFIG_KERNELDIR	= /usr/src/linux
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
