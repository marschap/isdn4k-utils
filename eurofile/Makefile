# $Id: Makefile,v 1.6 1999/06/30 21:59:53 he Exp $

SHELL = /bin/sh

CVS = cvs -z 4

SCRIPTS = scripts/eftd.sh scripts/eftp.sh scripts/eftp_setup

#
#additions to the CHANGES file shall automatically increase the compiled-in
#version string 
#
E4L_EXTRA_VERSION_DEP := $(shell pwd)/CHANGES
export E4L_EXTRA_VERSION_DEP

CONF_DEPS = ./configure scripts/eftd.sh scripts/eftp.sh scripts/eftp_setup src/config.h Rules.make

#this target is requested from isdn4k-utils default target
all: $(CONF_DEPS)
	make -C src

really_all:	$(CONF_DEPS)
	make -C src all

config src/config.h  Rules.make $(SCRIPTS): configure 
	./configure
	chmod a+x scripts/eftd.sh scripts/eftp.sh scripts/eftp_setup

configure: configure.in
	autoconf

permissions: scripts/eftd.sh scripts/eftp.sh scripts/eftp_setup
	chown root scripts/eftp_setup
	chmod u+s  scripts/eftp_setup
	chmod a+x $(SCRIPTS)

clean:
	rm -f  xx[0-9][0-9] CHANGES.tmp
	make -C src clean

distclean: clean
	rm -f config.* *~ '#'*'#' $(SCRIPTS)
	make -C src distclean
	rm -f  Rules.make eftp4linux*.tar.gz
	rm -f `find . \( -name core -o -name '*.orig' -o -name '*.rej' -o \
		-name '*~' -o -name '*.bak' -o -name '#*#' -o -name '.*.orig'\
		-o -name '.*.rej' -o -name '.SUMS' -o -size 0 \) -print` TAGS

configureclean: distclean
	rm -f configure

commit: distclean
	$(CVS) commit .

update:
	$(CVS) update -dP .

new_version:
	@echo current version is `grep E4L_VERSION src/eft/include/tdu_user.h|sed 's/^.*VERSION//'`
	@if [ -z "$(NEW_VERSION)" ]; then echo "You must define the variable NEW_VERSION when making a new version!"; exit 1; fi
	@echo new version is $(NEW_VERSION)
# Make a new release. The variable NEW_VERSION must be set in the
# make command's arguments or environment.
# : update
release: new_version commit
	make -C src new_version
	rm -f xx[0-9][0-9]
	csplit CHANGES '/$$Log:/+1'
	mv -f xx00 CHANGES.tmp
	echo '===============' >> CHANGES.tmp
	cat xx[0-9][0-9] >> CHANGES.tmp
	rm -f xx[0-9][0-9]
	mv -f CHANGES CHANGES.old
	mv CHANGES.tmp CHANGES
	$(CVS) commit -m "Version $(NEW_VERSION)" .
	$(CVS) tag `echo eftp4linux-$(NEW_VERSION)|sed s/\\\\./_/g` .
	tar -czf ../eftp4linux-$(NEW_VERSION).tar.gz .

.PHONY: all config permissions clean distclean release configureclean commit update default

