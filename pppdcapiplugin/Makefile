# Makefile for the capiplugin for pppd(8).
#
# Copyright 2000 Carsten Paeth (calle@calle.in-berlin.de)
# Copyright 2000 AVM GmbH Berlin (info@avm.de)
#
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public License
#  as published by the Free Software Foundation; either version
#  2 of the License, or (at your option) any later version.

CC	= gcc
CFLAGS	= -O2 -Wall -fPIC -I../capi20 -L../capi20
LDFLAGS	= -shared -L../capi20

PLUGINDIR=${DESTDIR}/etc/ppp/plugins
PEERDIR=${DESTDIR}/etc/ppp/peers/isdn
PEERS= arcor otelo talkline avm avm-ml leased
INSTALL=./install-sh -c

ALL = capiplugin.so userpass.so

all:	$(ALL)

capiplugin.so: capiplugin.o capiconn.o
	$(CC) -o $@ $(LDFLAGS) capiplugin.o capiconn.o -lcapi20dyn

userpass.so: userpass.c
	$(CC) -o $@ $(LDFLAGS) $(CFLAGS) -nostdlib userpass.c

distclean: clean

clean:
	$(RM) *.so *.o comperr

install: $(ALL)
	./mkinstalldirs $(PLUGINDIR)
	@for i in $(ALL); do \
		echo $(INSTALL) $$i $(PLUGINDIR); \
		$(INSTALL) $$i $(PLUGINDIR); \
	done
	./mkinstalldirs $(PEERDIR)
	@for i in $(PEERS); do \
		echo $(INSTALL) peers/$$i $(PEERDIR); \
		$(INSTALL) peers/$$i $(PEERDIR); \
	done
	for i in /usr/share/man /usr/man; do \
		if [ -d $$i/man8 ] ; then \
			echo $(INSTALL) capiplugin.8 $$i/man8; \
			$(INSTALL) capiplugin.8 $$i/man8; \
		fi ; \
	done

config:
	@echo nothing to configure
