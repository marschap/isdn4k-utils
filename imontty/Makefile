# tty line ISDN status monitor
#
# (c) 1995-97 Volker Götz
#
# $Id: Makefile,v 1.1 1997/03/03 04:10:10 fritz Exp $

ifeq (../.config,$(wildcard ../.config))
	#
	# Automatic config with isdn4k-utils
	#
	include ../.config
	MAN8PATH        := $(CONFIG_MANDIR)/man8
else
	#
	# Manual config standalone
	#
	CONFIG_SBINDRIR := /sbin
	MAN8PATH        := /usr/man/man8
endif

CCFLAGS=-O2
#CCFLAGS=-I$(ISDN_INCLUDE) -g

# nothing to change behind this line

PROGS=imontty

all: $(PROGS)

imontty: imontty.c imontty.h
	cc $(CCFLAGS) -o imontty imontty.c

rootperm:
	@if [ `id -u` != 0 ] ; then \
		echo -e "\n\n      Need root permission for (de)installation!\n\n"; \
		exit 1; \
	fi

install: $(PROGS) rootperm
	install -s -o 0 -g 0 -m 0755 $(PROGS) $(CONFIG_SBINDIR)

uninstall: rootperm
	for i in $(PROGS) ; do rm -f $(CONFIG_SBINDIR)/$$i; done

clean:
	rm -f *.o $(PROGS) *~

distclean: clean

#
# dummy for isdn4k-utils package
#
config:
