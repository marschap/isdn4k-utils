## $Id: Makefile,v 1.5 1997/03/24 22:51:59 luethje Exp $
##
## ISDN accounting for isdn4linux.
##
## Copyright 1995, 1997 by Andreas Kool (akool@Kool.f.EUnet.de)
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2, or (at your option)
## any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
##
## $Log: Makefile,v $
## Revision 1.5  1997/03/24 22:51:59  luethje
## isdnrep completed.
##
## Revision 1.4  1997/03/23 21:04:03  luethje
## some bugfixes
##
## Revision 1.3  1997/03/23 19:22:32  fritz
## Make isdnlog configurable.
##
## Revision 1.2  1997/03/16 20:58:07  luethje
## Added the source code isdnlog. isdnlog is not working yet.
## A workaround for that problem:
## copy lib/policy.h into the root directory of isdn4k-utils.
##
## Revision 1.1  1997/03/03 04:37:33  fritz
## Added files in isdnlog
##
## Revision 2.6.37  1997/02/11  17:53:19  akool

ifndef ROOTDIR
export ROOTDIR=$(shell pwd)
PREFIXDIR=.
MAKELIB  =1
else
PREFIXDIR=$(ROOTDIR)
endif

PREFIXDIR=..
LIBAREA=1

LIBISDNDIR  = $(PREFIXDIR)/lib
ifdef MAKELIB
SUBDIRS     = $(LIBISDNDIR)
endif

ifndef _CC
export _CC  = gcc -g -Wall -pipe -m486 #-O6 -fomit-frame-pointer -fforce-mem -fforce-addr -funroll-loops -fstrength-reduce
endif

CC          = $(_CC)

INCLUDE     = -I./connect -I./tools -I$(PREFIXDIR) -I$(LIBISDNDIR)

ifndef LIBAREA
ifdef NEED_GDBM
LIB         = -lgdbm
else
LIB         = -ldbm
endif
endif

XDEF        = -ansi -Dlinux -D__i386__ -D_POSIX_SOURCE -D_BSD_SOURCE -D_GNU_SOURCE -DX_LOCALE  -DNO_MESSAGE_CATALOG  -DFUNCPROTO=15 -DNARROWPROTO
XLIB        = -L/usr/X11/lib -llibXm.a -llibXpm.a -lXt -lXext -lX11 -lc
ifdef NEED_ICE
XLIB       += -lICE -lSM
endif
XINCLUDE    = -I/usr/X11/include -I./client -I./client/Xm

VPATH       = .

POSTGRESDIR = /usr/local/postgres95

OLDCONFFILE = isdnlog.conf

USERFILE    = isdnlog.users
CHARGEFILE  = charge.dat

RELOADCMD   = reload
STOPCMD     = stop
REBOOTCMD   = /sbin/reboot

LOGFILE     = /var/log/isdn.log

SERV_PORT   = 20011
SERVICEFILE = /etc/services


######################################################################
# DON'T EDIT BELOW THIS LINE
######################################################################

VERSION     = 2.99.1

ifdef POSTGRES
CFLAGS     += -DPOSTGRES
INCLUDE    += -I$(POSTGRESDIR)/include
LIB        += -L$(POSTGRESDIR)/lib -lpq
endif

ifdef ISDN_NL
CFLAGS     += -DISDN_NL
endif

ifdef ISDN_CH
CFLAGS     += -DISDN_CH
endif

ifdef DBMALLOC
CFLAGS     += -DDBMALLOC=1
LIB        += -ldbmalloc
endif

CFLAGS       += -DOLDCONFFILE=\"$(OLDCONFFILE)\" \
                -DUSERFILE=\"$(USERFILE)\"       \
                -DLOGFILE=\"$(LOGFILE)\"         \
                -DRELOADCMD=\"$(RELOADCMD)\"     \
                -DSTOPCMD=\"$(STOPCMD)\"         \
                -DVERSION=\"$(VERSION)\"   \
                -DCHARGEFILE=\"$(CHARGEFILE)\"   \
                -DSERV_PORT=$(SERV_PORT) 	 \
                -DREBOOTCMD=\"$(REBOOTCMD)\"     \
                $(INCLUDE) $(XINCLUDE)

ISDNLOG_OBJS =  isdnlog/isdnlog.o isdnlog/processor.o isdnlog/functions.o \
                isdnlog/server.o isdnlog/start_prog.o isdnlog/messages.o  \
                connect/connect.o connect/socket.o tools/tools.o      	  \
                connect/conv_address.o isdnlog/user_access.o 		  \
                isdnrep/cheap.o tools/isdnconf.o $(LIBISDNDIR)/libisdn.a


ifdef POSTGRES
ISDNLOG_OBJS += isdnlog/postgres.o
endif

ifdef TESTCENTER
ISDNLOG_OBJS += isdnlog/test_center.o
endif

ISDNREP_OBJS =  isdnrep/isdnrep.o isdnrep/cheap.o tools/tools.o           \
                tools/isdnconf.o isdnlog/messages.o isdnrep/function.o    \
                $(LIBISDNDIR)/libisdn.a

ISDNCONF_OBJS=  isdnconf/isdnconf.o tools/tools.o tools/isdnconf.o        \
                $(LIBISDNDIR)/libisdn.a

XISDN_OBJS   =  client/Xm/main_create.o client/Xm/isdn_call.o             \
                client/Xm/isdn_list.o client/Xm/isdn_prot.o               \
                connect/connect.o client/client.o tools/tools.o           \
                connect/socket.o client/Xm/xisdn.o client/isdn_list_update.o \
                client/Xm/xconnect.o connect/conv_address.o               \
	        client/Xm/isdn_display.o tools/isdnconf.o                 \
                $(LIBISDNDIR)/lib.a

#		client/Xmt/XmStringCvt.o \
#               client/Xmt/Localize.o client/Xmt/Lookup.o

ISDNLOG      =  bin/isdnlog
ISDNCONF     =  bin/isdnconf
ISDNREP      =  bin/isdnrep
XISDN        =  bin/xisdn

MODS         =  *.o */*.o */*/*.o

PROGS        =  $(ISDNLOG) $(ISDNREP) $(ISDNCONF)

all:            libs $(PROGS)

xall:           $(XISDN) all

libs:
		set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i; done

clean:
		-rm -f $(MODS)
		set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean; done

distclean:      clean
		-rm -f $(PROGS)
		-rm -f .depend
		set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i distclean; done

xdistclean:     clean
		-rm -f $(PROGS) $(XISDN)

ifeq (.depend,$(wildcard .depend))
include .depend
HEADERS := $(HEADERS) .depend
endif

install: depend all
		@if [ `id -u` != "0" ]; then echo ""; echo 'Do "make install" as root!' ;echo ""; false; fi
		-test -d $(I4LCONFDIR) || ( mkdir -p -m 755 $(I4LCONFDIR) )
		-test -d $(BINDIR) || ( mkdir -p -m 755 $(BINDIR) )
		-test -d $(SBINDIR) || ( mkdir -p -m 755 $(SBINDIR) )
		-test -d $(MAN1DIR) || ( mkdir -p -m 755 $(MAN1DIR) )
		-test -d $(MAN8DIR) || ( mkdir -p -m 755 $(MAN8DIR) )
		if ps -x | fgrep $(ISDNLOG) >/dev/null; then kill `cat /var/run/isdnlog.pid`; fi
		$(INSTALL) -m 700 $(ISDNLOG)  $(SBINDIR)
		$(INSTALL) -m 755 $(ISDNREP)  $(BINDIR)
		$(INSTALL) -m 755 $(ISDNCONF)  $(BINDIR)
		$(INSTALL) -m 755 isdnlog/isdnlog.8 $(MAN8DIR)
		$(INSTALL) -m 755 isdnrep/isdnrep.1 $(MAN1DIR)
ifdef MAKELIB
		set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i distclean; done
endif
		@echo ""
		@echo "Don't forget to create $(I4LCONFDIR)/$(USERFILE)"
		@echo ""

xinstall:       xall install
		-test -d $(X11BIN) || ( mkdir -p -m 755 $(X11BIN) )
		$(INSTALL) -m 755 $(XISDN)  $(X11BIN)
		-test -d $(APPDEF) || ( mkdir -p -m 755 $(APPDEF) )
		$(INSTALL) -m 644 Isdn  $(APPDEF)
		@(grep isdnlog $(SERVICEFILE) >/dev/null) || \
		(echo "";echo "";echo "Add a line to the file $(SERVICEFILE)" ;echo "";echo ""; \
		echo "isdnlog $(SERV_PORT)/tcp        isdnlog" >> $(SERVICEFILE))
#               @echo ""
#               @echo ""
#               @echo "Don't forget to export XENVIRONMENT=$(APPDEF)/Isdn"
#               @echo ""
#               @echo ""

distrib:        xdistclean
		cd .. && tar cf /tmp/isdnlog-$(VERSION).tar    \
		  isdnlog-$(I4LVERSION)/Makefile       \
		  isdnlog-$(I4LVERSION)/Isdn           \
		  isdnlog-$(I4LVERSION)/README         \
		  isdnlog-$(I4LVERSION)/NEWS           \
		  isdnlog-$(I4LVERSION)/COPYING        \
		  isdnlog-$(I4LVERSION)/BUGS           \
		  isdnlog-$(I4LVERSION)/TODO           \
		  isdnlog-$(I4LVERSION)/FAQ            \
		  isdnlog-$(I4LVERSION)/samples        \
		  isdnlog-$(I4LVERSION)/client         \
		  isdnlog-$(I4LVERSION)/tools          \
		  isdnlog-$(I4LVERSION)/isdnconf       \
		  isdnlog-$(I4LVERSION)/isdnlog        \
		  isdnlog-$(I4LVERSION)/isdnrep        \
		  isdnlog-$(I4LVERSION)/connect        \
		  isdnlog-$(I4LVERSION)/lib            \
		  isdnlog-$(I4LVERSION)/contrib	    \
		  isdnlog-$(I4LVERSION)/bin
		gzip -f9 /tmp/isdnlog-$(I4LVERSION).tar
#		uuencode /tmp/isdnlog-$(I4LVERSION).tar.gz isdnlog-$(I4LVERSION).tar.gz > /tmp/isdnlog-$(I4LVERSION).uue

xdistrib:
		cd .. && tar cf /tmp/isdnlog-X11-$(I4LVERSION).tar \
                  isdnlog-$(I4LVERSION)/Isdn	     		\
		  isdnlog-$(I4LVERSION)/X11            \
		  isdnlog-$(I4LVERSION)/sounds         \
		  isdnlog-$(I4LVERSION)/doc
		gzip -f9 /tmp/isdnlog-X11-$(I4LVERSION).tar
		uuencode /tmp/isdnlog-X11-$(I4LVERSION).tar.gz isdnlog-X11-$(I4LVERSION).tar.gz > /tmp/isdnlog-X11-$(I4LVERSION).uue

$(ISDNLOG):     $(ISDNLOG_OBJS)
		$(CC) -o $(ISDNLOG) $(LFLAGS) $(ISDNLOG_OBJS) $(LIB)

$(ISDNREP):     $(ISDNREP_OBJS)
		$(CC) -o $(ISDNREP) $(LFLAGS) $(ISDNREP_OBJS) $(LIB)

$(ISDNCONF):    $(ISDNCONF_OBJS)
		$(CC) -o $(ISDNCONF) $(LFLAGS) $(ISDNCONF_OBJS) $(LIB)

$(XISDN):       $(XISDN_OBJS)
		$(CC) -o $(XISDN) $(LFLAGS) $(XISDN_OBJS) $(XLIB) $(LIB)



tools/tools.h: $(LIBISDNDIR)/libisdn.h $(PREFIXDIR)/policy.h
		touch tools/tools.h

isdnlog/isdnlog.o:         isdnlog/isdnlog.c isdnlog/isdnlog.h tools/tools.h \
			   connect/socket.h

isdnlog/processor.o:       isdnlog/processor.c isdnlog/isdnlog.h tools/tools.h \
			   connect/socket.h

isdnlog/functions.o:       isdnlog/functions.c isdnlog/isdnlog.h tools/tools.h \
			   connect/socket.h

isdnlog/server.o:          isdnlog/server.c  isdnlog/isdnlog.h tools/tools.h \
			   connect/socket.h

isdnlog/start_prog.o:      isdnlog/start_prog.c isdnlog/isdnlog.h tools/tools.h

isdnlog/user_access.o:     isdnlog/user_access.c isdnlog/isdnlog.h tools/tools.h

tools/tools.o:             tools/tools.c     tools/tools.h

tools/isdnconf.o:          tools/isdnconf.c  tools/tools.h

isdnrep/isdnrep.o:         isdnrep/isdnrep.c isdnrep/isdnrep.h tools/tools.h

isdnconf/isdnconf.o:       isdnconf/isdnconf.c isdnconf/isdnconf.h tools/tools.h

isdnrep/cheap.o:           isdnrep/cheap.c   isdnrep/isdnrep.h tools/tools.h

connect/conv_address.o:    connect/conv_address.c    connect/socket.h tools/tools.h
connect/socket.o:          connect/socket.c          connect/socket.h tools/tools.h
connect/connect.o:         connect/connect.c         connect/socket.h tools/tools.h

client/client.o:           client/client.c           connect/socket.h tools/tools.h
client/isdn_display.o:     client/isdn_display.c     connect/socket.h tools/tools.h
client/isdn_list_update.o: client/isdn_list_update.c tools/tools.h
client/Xm/xconnect.o:      client/Xm/xconnect.c      connect/socket.h tools/tools.h
client/Xm/xisdn.o:         client/Xm/xisdn.c         connect/socket.h tools/tools.h
client/Xm/main_create.o:   client/Xm/main_create.c   tools/tools.h
client/Xm/isdn_call.o:     client/Xm/isdn_call.c     tools/tools.h
client/Xm/isdn_list.o:     client/Xm/isdn_list.c     tools/tools.h
client/Xm/isdn_prot.o:     client/Xm/isdn_prot.c     tools/tools.h
client/Xm/isdn_display.o:  client/Xm/isdn_display.c  tools/tools.h
# client/Xmt/XmStringCvt.o: client/Xmt/XmStringCvt.c
# client/Xmt/Lookup.o: client/Xmt/Lookup.c
# client/Xmt/Localize.o: client/Xmt/Localize.c
#client/Xmt/All.o: client/Xmt/All.c
#client/Xmt/Create.o: client/Xmt/Create.c
#client/Xmt/Initialize.o: client/Xmt/Initialize.c
#	$(CC) $(XDEF) -c client/Xmt/XmStringCvt.c
#client/Xmt/xconnect.o:     client/Xmt/xconnect.c     connect/socket.h $(LIBISDNDIR)/libisdn.h tools/tools.h
#client/Xmt/xisdn.o:        client/Xmt/xisdn.c        connect/socket.h $(LIBISDN)/libisdn.h tools/tools.h
#client/Xmt/main_create.o:  client/Xmt/main_create.c  tools/tools.h
#client/Xmt/isdn_call.o:    client/Xmt/isdn_call.c    tools/tools.h
#client/Xmt/isdn_list.o:    client/Xmt/isdn_list.c    tools/tools.h
#client/Xmt/isdn_prot.o:    client/Xmt/isdn_prot.c    tools/tools.h
#client/Xmt/isdn_display.o: client/Xmt/isdn_display.c tools/tools.h

depend: .depend

.depend:
		$(CPP) -M $(CFLAGS) */*.c >.depend
#		$(CPP) -M $(CFLAGS) */*.c */*/*.c >.depend
