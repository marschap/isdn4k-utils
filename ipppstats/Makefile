#
# pppstats makefile
# $Id: Makefile,v 1.2 1997/03/23 19:55:50 fritz Exp $
#

PPPSTATSRCS = ipppstats.c
PPPSTATOBJS = ipppstats.o

CC = gcc
COPTS = -O2
COMPILE_FLAGS = -m486 -fomit-frame-pointer -I../include
LIBS =

INSTALL= install -o root -g daemon

CFLAGS = $(COPTS) $(COMPILE_FLAGS)

all: ipppstats

install: ipppstats
	$(INSTALL) -s -c ipppstats $(BINDIR)/ipppstats
	$(INSTALL) -c -m 444 ipppstats.8 $(MANDIR)/man8/ipppstats.8

ipppstats: $(PPPSTATSRCS)
	$(CC) $(CFLAGS) -o ipppstats ipppstats.c $(LIBS)

clean:
	rm -f ipppstats *~ #* core

distclean: clean

depend:
	cpp -M $(CFLAGS) $(PPPSTATSRCS) >.depend
#	makedepend $(CFLAGS) $(PPPSTATSRCS)
