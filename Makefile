# $Id: Makefile,v 1.16 1997/05/25 23:36:23 fritz Exp $
#
# Toplevel Makefile for isdn4k-utils
#

export I4LVERSION = 2.1b1

all:	do-it-all

#
# Make "config" the default target if there is no configuration file.
#

# Following line is important for lib and isdnlog (sl).
export ROOTDIR=$(shell pwd)

ifeq (.config,$(wildcard .config))
include .config
do-it-all:      subtargets
else
CONFIGURATION = config
do-it-all:      config
endif

SUBDIRS :=
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
ifeq ($(CONFIG_AVMCAPICTRL),y)
	SUBDIRS := $(SUBDIRS) avmb1
endif
ifeq ($(CONFIG_LOOPCTRL),y)
	SUBDIRS := $(SUBDIRS) loop
endif
ifeq ($(CONFIG_IMON),y)
	SUBDIRS := $(SUBDIRS) imon
endif
ifeq ($(CONFIG_IMONTTY),y)
	SUBDIRS := $(SUBDIRS) imontty
endif
ifeq ($(CONFIG_ISDNLOG),y)
	SUBDIRS := $(SUBDIRS) areacode lib isdnlog
else
	ifeq ($(CONFIG_LIB_AREACODE),y)
		SUBDIRS := $(SUBDIRS) areacode
	endif
endif
ifeq ($(CONFIG_IPPPSTATS),y)
	SUBDIRS := $(SUBDIRS) ipppstats
endif
ifeq ($(CONFIG_XMONISDN),y)
	SUBDIRS := $(SUBDIRS) xmonisdn
endif
ifeq ($(CONFIG_XISDNLOAD),y)
	SUBDIRS := $(SUBDIRS) xisdnload
endif
ifeq ($(CONFIG_IPPPD),y)
	SUBDIRS := $(SUBDIRS) ipppd
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
	set -e; for i in `echo $(SUBDIRS)`; do $(MAKE) -C $$i all; done

rootperm:
	@echo 'main(int argc,char**argv){unlink(argv[0]);return(getuid()==0);}'>g
	@if gcc -x c -o G g && rm -f g && ./G ; then \
		echo -e "\n\n      Need root permission for (de)installation!\n\n"; \
		exit 1; \
	fi

install: rootperm
	set -e; for i in `echo $(SUBDIRS)`; do $(MAKE) -C $$i install; done

uninstall: rootperm
	set -e; for i in `echo $(SUBDIRS)`; do $(MAKE) -C $$i uninstall; done

#
# targets clean and distclean go through ALL directories
# regardless of configured options.
#
clean:
	-set -e; \
	for i in `echo ${wildcard */GNUmakefile}`; do \
		$(MAKE) -i -C `dirname $$i` clean; \
	done;
	-set -e; \
	for i in `echo ${wildcard */Makefile}`; do \
		$(MAKE) -i -C `dirname $$i` clean; \
    done;
	-rm -f *~ *.o

distclean: clean
	-$(MAKE) -C scripts/lxdialog clean
	-set -e; \
	for i in `echo ${wildcard */GNUmakefile}`; do \
		$(MAKE) -i -C `dirname $$i` distclean; \
	done;
	-set -e; \
	for i in `echo ${wildcard */Makefile}`; do \
		if [ -f $$i ] ; then \
			$(MAKE) -i -C `dirname $$i` distclean; \
		fi ; \
	done;
	-rm -f *~ .config .config.old scripts/autoconf.h .menuconfig \
		Makefile.tmp .menuconfig.log scripts/defconfig.old .#* scripts/.#*

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
	@set -e; for i in `echo $(SUBDIRS)`; do \
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

#
# For testing: runs Menuconfig only
#
testconfig: scripts/lxdialog/lxdialog
	@scripts/Menuconfig scripts/config.in

config: menuconfig

mrproper: distclean

archive: distclean
	@(cd .. ;\
	ln -nfs isdn4k-utils isdn4k-utils-$(I4LVERSION) ;\
	tar cvhzf distisdn/isdn4k-utils-$(I4LVERSION).tar.gz isdn4k-utils-$(I4LVERSION) ;\
	rm isdn4k-utils-$(I4LVERSION) )

distarch: distclean
	(cd .. ;\
	ln -nfs isdn4k-utils isdn4k-utils-$(I4LVERSION) ;\
	tar -cvhz --exclude=CVS -f distisdn/isdn4k-utils-$(I4LVERSION).tar.gz \
	isdn4k-utils-$(I4LVERSION) ;\
	rm isdn4k-utils-$(I4LVERSION) )

dist: distarch
