# $Id: Makefile,v 1.2 1999/06/30 21:05:21 he Exp $

SHELL = /bin/sh


-include ../Rules.make

CURRENT_DIR := $(shell pwd)

#
# passed down to sub-makefiles to communicate location of eft
# related libraries and header files  
#
EFT_LIBDIR  = $(CURRENT_DIR)/eft
EFT_INCDIR  = $(CURRENT_DIR)/eft/include
EFT_AUTHDIR = $(CURRENT_DIR)/wuauth
KERNEL_INCDIR = $(CONFIG_KERNELDIR)/include

CFLAGS = -g -Wall

export EFT_LIBDIR EFT_INCDIR EFT_AUTHDIR CONFIG_EFTP_READLINE CONFIG_EFTD_WUAUTH CRYPTLIB CFLAGS KERNEL_INCDIR

LIBDIRS    = eft  wuauth
CLIENTDIRS = eftp
SERVERDIRS = eftd 
TESTDIRS   = testing
EFTDIRS    = $(CLIENTDIRS) $(SERVERDIRS) 

SUBDIRS := $(LIBDIRS) $(EFTDIRS)
ALL_SUBDIRS := $(SUBDIRS) $(TESTDIRS)

DEFAULT_SUBDIRS := $(LIBDIRS)

ifeq ($(CONFIG_EFTP),y)
DEFAULT_SUBDIRS += eftp
endif

ifeq ($(CONFIG_EFTD),y)
DEFAULT_SUBDIRS += eftd
endif

default: $(DEFAULT_SUBDIRS)

all: $(ALL_SUBDIRS)

$(CLIENTDIRS): eft

$(SERVERDIRS) testing: $(LIBDIRS)

$(ALL_SUBDIRS): FORCE
	$(MAKE) -C $@

FORCE:

clean:
	for i in $(ALL_SUBDIRS); do $(MAKE) -C $$i clean; done

distclean: clean
	rm -f config.h `find . \( -name '*~' -o -name '#*#' -o -name core \)`

install: eftp/eftp eftd/eftd
	cp eftp/eftp /usr/local/bin
	cp eftd/eftd /usr/local/sbin

new_version:
	make -C eft new_version

.PHONY: default all clean distclean install FORCE new_version

