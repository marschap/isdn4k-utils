# tty line ISDN status monitor
#
# (c) 1995-97 Volker Götz
#
# $Id: Makefile,v 1.3 1998/09/26 18:28:06 akool Exp $

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
	@echo 'main(int argc,char**argv){unlink(argv[0]);return(getuid()==0);}'>g
	@if gcc -x c -o G g && rm -f g && ./G ; then \
		echo -e "\n\n      Need root permission for (de)installation!\n\n"; \
		exit 1; \
	fi

install: $(PROGS) rootperm
	install -s -o 0 -g 0 -m 0755 $(PROGS) $(DESTDIR)$(CONFIG_SBINDIR)

uninstall: rootperm
	for i in $(PROGS) ; do rm -f $(DESTDIR)$(CONFIG_SBINDIR)/$$i; done

clean:
	rm -f *.o $(PROGS) *~

distclean: clean

#
# dummy for isdn4k-utils package
#
config:
