##
## Makefile for library for isdn4linux
##

##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2, or (at your option)
## any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

ifndef ROOTDIR
export ROOTDIR=$(shell pwd)
PREFIXDIR =.
else
PREFIXDIR =$(ROOTDIR)
endif

######################################################################
# USER CONFIGURATION AREA
######################################################################

CFLAGS      = -g -Wall -pipe #-O6
DEFS        =
ifndef _CC
export _CC  = gcc
endif
CC          = $(_CC)

export AVON = avon
export I4LCONFDIR

######################################################################
# DON'T EDIT BELOW THIS LINE
######################################################################

AREADIR   = ../areacode
LIB_AREA  = $(AREADIR)/areacode.o

ifeq ($(LIBAREA),1)
DEFS     += -DLIBAREA
endif

AVONDIR   = avon
LIB_AVON += $(AVONDIR)/createDB.o
DEFS     += -DAVON=\"$(AVON)\"

SUBDIRS  = $(AVONDIR) #$(AREADIR)

DEFS    += -I$(PREFIXDIR)

%.o: %.c
	$(CC) $(CFLAGS) $(DEFS) -c -o $@ $<

all: .depend libs libisdn.a

libs:
		set -e; for i in `echo $(SUBDIRS)`; do $(MAKE) -C $$i; done

depend dep: .depend

.depend:
		$(CPP) -M $(CFLAGS) $(DEFS) *.c >.depend
		set -e; for i in `echo $(SUBDIRS)`; do $(MAKE) -C $$i depend; done

clean:
		-rm -f *.o *.a
		set -e; for i in `echo $(SUBDIRS)`; do $(MAKE) -C $$i clean; done

distclean: clean
		-rm -f .depend policy.h
		set -e; for i in `echo $(SUBDIRS)`; do $(MAKE) -C $$i distclean; done

install: depend all
		set -e; for i in `echo $(SUBDIRS)`; do $(MAKE) -C $$i depend; done
		set -e; for i in `echo $(SUBDIRS)`; do $(MAKE) -C $$i all; done
ifneq ($(CONFIG_LIB_AREACODE),y)
		set -e; for i in `echo $(AVONDIR)`; do $(MAKE) -C $$i install; done
#else
#		set -e; for i in `echo $(AREADIR)`; do $(MAKE) -C $$i install; done
endif

deinstall: distclean
		set -e; for i in `echo $(SUBDIRS)`; do $(MAKE) -C $$i deinstall; done


libisdn.a: conffile.o isdntools.o libtools.o $(LIB_AREA) $(LIB_AVON)
		ar rs libisdn.a isdntools.o conffile.o libtools.o $(LIB_AREA) $(LIB_AVON)
		ranlib libisdn.a

ifeq (.depend,$(wildcard .depend))
include .depend
#HEADERS := $(HEADERS) .depend
endif

