# $Id: Makefile,v 1.3 1997/02/22 14:58:02 fritz Exp $
#
# Toplevel Makefile for isdn4k-utils
#
export I4LVERSION=2.1
export I4LCONFDIR=/etc/isdn

SUBDIRS=`find . -type d -maxdepth 1`

all:
	@set -e; allow_null_glob_expansion=1; \
	NOMAKE=true; \
	for i in */Makefile ; do \
		$(MAKE) -C `dirname $$i` all; \
		NOMAKE=false; \
	done; \
	if $$NOMAKE ; then \
		echo 'Please configure the package with "make config" first!'; \
	fi

install:
	@set -e; allow_null_glob_expansion=1; \
	for i in */Makefile ; do \
		$(MAKE) -C `dirname $$i` install ;\
	done

uninstall:
	@set -e; allow_null_glob_expansion=1; \
	for i in */Makefile ; do \
		$(MAKE) -C `dirname $$i` uninstall ;\
	done

clean:
	@set -e; allow_null_glob_expansion=1; \
	for i in */Makefile ; do \
		$(MAKE) -C `dirname $$i` clean ;\
	done
	@ rm -f *~

distclean: clean
	@set -e; allow_null_glob_expansion=1; \
	for i in */Makefile ; do \
		$(MAKE) -C `dirname $$i` distclean ;\
	done

config:
	@set -e; allow_null_glob_expansion=1; \
	for i in */Makefile.in ; do \
		$(MAKE) -C `dirname $$i` -f Makefile.in config ;\
	done

mrproper: distclean

archive: distclean
	@(cd .. ;\
	mv isdn4k-utils isdn4k-utils-$(I4LVERSION) ;\
	tar cvzf distisdn/isdn4k-utils-$(I4LVERSION).tar.gz isdn4k-utils-$(I4LVERSION) ;\
	mv isdn4k-utils-$(I4LVERSION) isdn4k-utils )
