##
## $Id: Makefile.am,v 1.4 1998/08/30 17:32:05 michael Exp $
##

## system admin executables to install ####################################

sbin_PROGRAMS		= vboxgetty

###########################################################################

packagelogdir		= @packagelogdir@
packagelockdir		= @packagelockdir@
packagepiddir		= @packagepiddir@

tclpackagelibs		= @LINK_TCL_LIBS@
tclpackageinclude	= @LINK_TCL_INCL@

DEFS		       += -DSYSCONFDIR='"$(sysconfdir)"' -DPKGDATADIR='"$(pkgdatadir)"' -DLOGDIR='"$(packagelogdir)"' -DLOCKDIR='"$(packagelockdir)"' -DPIDDIR='"$(packagepiddir)"'
INCLUDES		= $(all_includes) $(tclpackageinclude)

## vbox low level modem library ###########################################

noinst_LIBRARIES	= libvboxmodem.a

libvboxmodem_a_SOURCES	= libvboxmodem.c libvboxmodem.h

## vboxgetty ##############################################################

vboxgetty_LDADD		= $(tclpackagelibs) -L. -lvboxmodem

vboxgetty_SOURCES	= log.c log.h \
			  modem.c modem.h \
			  rc.c rc.h \
			  vboxrc.c vboxrc.h \
			  userrc.c userrc.h \
			  stringutils.c stringutils.h \
			  tclscript.c tclscript.h \
			  vboxgetty.c vboxgetty.h \
			  voice.c voice.h \
			  control.c control.h \
			  lock.c lock.h \
			  audio.c audio.h

## other directories to process ###########################################

SUBDIRS			= examples

## cleanup ################################################################

CLEANFILES		= *~
MAINTAINERCLEANFILES	= Makefile.in

## Local install routines ################################################

install-exec-local:
			  $(mkinstalldirs) $(sbindir)
			  $(INSTALL) -s -m 700 vboxgetty $(sbindir)/
