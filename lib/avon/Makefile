##
## Makefile for avon for isdn4linux
##
## Copyright 1996, 1997 by Stefan Luethje (luethje@sl-gw.lake.de)
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

ifndef _CC
export _CC = gcc -Wall -pipe -O6
endif
CC         = $(_CC)

######################################################################
# DON'T EDIT BELOW THIS LINE
######################################################################

INSTALL =install
INSTALL_DIR     := $(INSTALL) -m 0755 -o 0 -g 0 -d
INSTALL_AVON    := $(INSTALL) -m 0644 $(AVON) $(DESTDIR)$(I4LCONFDIR)

#CFLAGS   = -I$(PREFIXDIR)

all: createDB.o

clean:
		rm -f *.o *.a

depend: .depend

.depend:
		$(CPP) -M $(CFLAGS) createDB.c >.depend

distclean: clean
		rm -f .depend

install: depend all
  ifneq ($(CONFIG_LIB_AREACODE),y)
		-$(INSTALL_DIR) $(DESTDIR)$(I4LCONFDIR)
		-test $(AVON) -ot $(DESTDIR)$(I4LCONFDIR)/$(AVON) || ( $(INSTALL_AVON) )
		-@rm -f $(DESTDIR)$(I4LCONFDIR)/$(AVON).pag $(DESTDIR)$(I4LCONFDIR)/$(AVON).dir
  endif
	        
deinstall: distclean
		-rm -f $(DESTDIR)$(I4LCONFDIR)/$(AVON)*

createDB.o:   createDB.c createDB.h

