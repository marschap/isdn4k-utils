# $Id: Makefile,v 1.5 1997/03/03 04:00:33 fritz Exp $
#
# Toplevel Makefile for isdn4k-utils
#

export I4LVERSION = 2.1

all:	do-it-all

#
# Make "config" the default target if there is no configuration file.
#
ifeq (.config,$(wildcard .config))
include .config
do-it-all:      subtargets
else
CONFIGURATION = config
do-it-all:      config
endif

SUBDIRS :=
ifeq ($(CONFIG_LIB_AREACODE),y)
	SUBDIRS := $(SUBDIRS) areacode lib
endif
ifeq ($(CONFIG_ISDNCTRL),y)
	SUBDIRS := $(SUBDIRS) isdnctrl
endif
ifeq ($(CONFIG_IPROFD),y)
	SUBDIRS := $(SUBDIRS) iprofd
endif
ifeq ($(CONFIG_ICNCTRL),y)
	SUBDIRS := $(SUBDIRS) icn
endif
ifeq ($(CONFIG_PCBITCTL),y)
	SUBDIRS := $(SUBDIRS) pcbit
endif
ifeq ($(CONFIG_TELESCTRL),y)
	SUBDIRS := $(SUBDIRS) teles
else
	ifeq ($(CONFIG_HISAXCTRL),y)
		SUBDIRS := $(SUBDIRS) teles
	endif
endif
ifeq ($(CONFIG_IMON),y)
	SUBDIRS := $(SUBDIRS) imon
endif
ifeq ($(CONFIG_IMONTTY),y)
	SUBDIRS := $(SUBDIRS) imontty
endif
ifeq ($(CONFIG_ISDNLOG),y)
	SUBDIRS := $(SUBDIRS) isdnlog
endif
ifeq ($(CONFIG_XMONISDN),y)
	SUBDIRS := $(SUBDIRS) xmonisdn
endif
ifeq ($(CONFIG_XISDNLOAD),y)
	SUBDIRS := $(SUBDIRS) xisdnload
endif
ifeq ($(CONFIG_VBOX),y)
	SUBDIRS := $(SUBDIRS) vbox
endif
ifeq ($(CONFIG_GENMAN),y)
	SUBDIRS := $(SUBDIRS) doc
endif
ifeq ($(CONFIG_FAQ),y)
	SUBDIRS := $(SUBDIRS) FAQ
endif
ifneq ($(SUBDIRS),)
	ifeq ($(filter lib,$(SUBDIRS)),)
		SUBDIRS := lib $(SUBDIRS)
	endif
endif

subtargets: $(CONFIGURATION)
	set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i all; done

rootperm:
	@if [ `id -u` != 0 ] ; then \
		echo -e "\n\n      Need root permission for (de)installation!\n\n"; \
		exit 1; \
	fi

install: rootperm
	set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install; done

uninstall: rootperm
	set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i uninstall; done

#
# targets clean and distclean go through ALL directories
# regardless of cofigured options.
#
clean:
	-set -e; allow_null_glob_expansion=1; \
	for i in */Makefile; do $(MAKE) -i -C `dirname $$i` clean; done
	rm -f *~ *.o

distclean: clean
	-$(MAKE) -C scripts/lxdialog clean
	-set -e; allow_null_glob_expansion=1; \
	for i in */Makefile; do $(MAKE) -i -C `dirname $$i` distclean; done
	rm -f *~ .config .config.old scripts/autoconf.h .menuconfig \
		Makefile.tmp .menuconfig.log scripts/defconfig.old

scripts/lxdialog/lxdialog:
	@$(MAKE) -C scripts/lxdialog all

#
# Next target makes three attempts to configure:
#  - if a Makefile already exists, make config
#  - if a configure script exists, execute it
#  - if a Makefile.in exists, make -f Makefile.in config
#
subconfig:
	@echo Selected subdirs: $(SUBDIRS)
	@set -e; for i in $(SUBDIRS); do \
		if [ -x $$i/configure ] ; then \
			echo -e "\nRunning configure in $$i ...\n"; sleep 1; \
			(cd $$i; ./configure); \
		else \
			if [ -f $$i/Makefile.in ] ; then \
				echo -e "\nRunning make -f Makefile.in config in $$i ...\n"; sleep 1; \
				$(MAKE) -C $$i -f Makefile.in config; \
			else \
				if [ -f $$i/Makefile ] ; then \
					echo -e "\nRunning make config in $$i ...\n"; sleep 1; \
					$(MAKE) -C $$i config; \
				fi; \
			fi; \
		fi; \
	done

#
# Next target uses a second tempory Makefile
# because new .config has to be re-included.
#
menuconfig: scripts/lxdialog/lxdialog
	@scripts/Menuconfig scripts/config.in
	@cp Makefile Makefile.tmp
	$(MAKE) -f Makefile.tmp subconfig
	@rm -f Makefile.tmp

config: menuconfig

mrproper: distclean

archive: distclean
	@(cd .. ;\
	mv isdn4k-utils isdn4k-utils-$(I4LVERSION) ;\
	tar cvzf distisdn/isdn4k-utils-$(I4LVERSION).tar.gz isdn4k-utils-$(I4LVERSION) ;\
	mv isdn4k-utils-$(I4LVERSION) isdn4k-utils )

distarch: distclean
	@(cd .. ;\
	mv isdn4k-utils isdn4k-utils-$(I4LVERSION) ;\
	tar -cvzf --exclude=CVS distisdn/isdn4k-utils-$(I4LVERSION).tar.gz \
	isdn4k-utils-$(I4LVERSION) ;\
	mv isdn4k-utils-$(I4LVERSION) isdn4k-utils )

dist: distarch
