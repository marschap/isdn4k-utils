##
## vboxgetty/Makefile.am - (C) 1997-1998 Michael Herold
##

## system admin executables to install ####################################

sbin_PROGRAMS		= vboxgetty

###########################################################################

packagelogdir		= @packagelogdir@
packagelockdir		= @packagelockdir@
packagepiddir		= @packagepiddir@

tclpackagelibs		= @LINK_TCL_LIBS@
tclpackageinclude	= @LINK_TCL_INCL@

DEFS		       += -DSYSCONFDIR='"$(sysconfdir)"' -DDATADIR='"$(datadir)"' -DLOGDIR='"$(packagelogdir)"' -DLOCKDIR='"$(packagelockdir)"' -DPIDDIR='"$(packagepiddir)"'
INCLUDES		= $(all_includes) $(tclpackageinclude)

## vbox low level modem library ###########################################

noinst_LIBRARIES	= libvboxmodem.a

libvboxmodem_a_SOURCES	= libvboxmodem.c libvboxmodem.h

## vboxgetty ##############################################################

vboxgetty_LDADD		= $(tclpackagelibs) -L. -lvboxmodem

vboxgetty_SOURCES	= log.c log.h \
			  modem.c modem.h \
			  rc.c rc.h \
			  stringutils.c stringutils.h \
			  tclscript.c tclscript.h \
			  vboxgetty.c vboxgetty.h \
			  voice.c voice.h

## other directories to process ###########################################

SUBDIRS			= examples

## cleanup ################################################################

CLEANFILES		= *~
MAINTAINERCLEANFILES	= Makefile.in
