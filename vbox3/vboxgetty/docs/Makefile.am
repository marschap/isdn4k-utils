##
## $Id: Makefile.am,v 1.1 1998/09/30 09:51:46 michael Exp $
##

## other directories to process ###########################################

SUBDIRS			= examples docs

## configure options ######################################################

packagelogdir		= @packagelogdir@
packagelockdir		= @packagelockdir@
packagepiddir		= @packagepiddir@

tclpackagelibs		= @LINK_TCL_LIBS@
tclpackageinclude	= @LINK_TCL_INCL@

## system admin executables to build ######################################

sbin_PROGRAMS		= vboxgetty

INCLUDES		= $(all_includes) $(tclpackageinclude)
DEFS		       += -DSYSCONFDIR='"$(sysconfdir)"' -DPKGDATADIR='"$(pkgdatadir)"' -DLOGDIR='"$(packagelogdir)"' -DLOCKDIR='"$(packagelockdir)"' -DPIDDIR='"$(packagepiddir)"'

## vbox low level modem library ###########################################

noinst_LIBRARIES	= libvboxmodem.a

libvboxmodem_a_SOURCES	= libvboxmodem.c libvboxmodem.h

## vboxgetty ##############################################################

vboxgetty_LDADD		= $(tclpackagelibs) -L. -lvboxmodem

vboxgetty_SOURCES	= log.c log.h \
			  modem.c modem.h \
			  rc.c rc.h \
			  vboxrc.c vboxrc.h \
			  stringutils.c stringutils.h \
			  tclscript.c tclscript.h \
			  vboxgetty.c vboxgetty.h \
			  voice.c voice.h \
			  control.c control.h \
			  lock.c lock.h \
			  audio.c audio.h \
			  breaklist.c breaklist.h

## extra files to distribute ##############################################

EXTRA_DIST		= AUTHORS COPYING

## cleanup ################################################################

CLEANFILES		= *~
MAINTAINERCLEANFILES	= Makefile.in
