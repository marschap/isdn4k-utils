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
		-test $(AVON) -ot $(I4LCONFDIR)/$(AVON) || ( $(INSTALL) -m 644 $(AVON) $(I4LCONFDIR) )
		# -rm -f $(I4LCONFDIR)/$(AVON).pag $(I4LCONFDIR)/$(AVON).dir
	        
deinstall: distclean
		-rm -f $(I4LCONFDIR)/$(AVON)*


createDB.o:   createDB.c createDB.h

