#
# pppstats makefile
# $Id: Makefile,v 1.1 1997/03/07 15:57:03 hipp Exp $
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

depend:
	cpp -M $(CFLAGS) $(PPPSTATSRCS) >.depend
#	makedepend $(CFLAGS) $(PPPSTATSRCS)
