# $Id: Makefile,v 1.1 1998/06/10 13:31:48 michael Exp $



#-- Compiler flags -------------------------------------------------------#

CC			= gcc
CFLAGS			= -O2 -m486 -Wall

#-- All we have to do ----------------------------------------------------#

all:			libvboxmodem vboxgetty

#-- All objects ----------------------------------------------------------#

libvboxmodem.o:		libvboxmodem.h libvboxmodem.c
			$(CC) $(CFLAGS) -c libvboxmodem.c

vboxgetty.o:		vboxgetty.h vboxgetty.c
			$(CC) $(CFLAGS) -c vboxgetty.c

log.o:			log.h log.c
			$(CC) $(CFLAGS) -c log.c

rc.o:			rc.h rc.c
			$(CC) $(CFLAGS) -c rc.c

modem.o:		modem.h modem.c
			$(CC) $(CFLAGS) -c modem.c

voice.o:		voice.h voice.c
			$(CC) $(CFLAGS) -c voice.c

tclscript.o:		tclscript.h tclscript.c
			$(CC) $(CFLAGS) -c tclscript.c

#-- Modem library -------------------------------------------------------#

LIBVBOXMODEMOBJ	= libvboxmodem.o

libvboxmodem:		$(LIBVBOXMODEMOBJ)
			ar cru libvboxmodem.a $(LIBVBOXMODEMOBJ)
			ranlib libvboxmodem.a

#-- vboxgetty -----------------------------------------------------------#

VBOXGETTYOBJ	=	vboxgetty.o \
			log.o \
			rc.o \
			modem.o \
			voice.o \
			tclscript.o

vboxgetty:		$(VBOXGETTYOBJ)
			$(CC) $(CFLAGS) $(VBOXGETTYOBJ) -o vboxgetty -L. -lvboxmodem -ldl -lm -ltcl




install:		ignore
			@install -s -m 700 vboxgetty /usr/local/sbin






#-- Internal junk --------------------------------------------------------#


clean:		  	ignore
		  	@rm -f *~ *.[oas]

distclean:	  	clean
		  	@rm -f vboxgetty vbox

ignore:
