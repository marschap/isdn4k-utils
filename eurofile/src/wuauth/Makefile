#
# Makefile for Linux 1.2.13 with gcc 2.6.3
#
# $Id: Makefile,v 1.2 2006/01/15 15:08:55 keil Exp $
#
CC     = gcc 
AR     = ar cq
RANLIB = ranlib
LIBC   = /lib/libc.a
IFLAGS = 
LFLAGS = 
MYCFLAGS = -O6 -fomit-frame-pointer -fno-strength-reduce -pipe ${IFLAGS} ${LFLAGS}
CFLAGS = -g

SRCS   = strcasestr.c access.c divfunc.c private.c acl.c main.c sigfix.c  \
#	extensions.c

OBJS   = strcasestr.o access.o divfunc.o private.o acl.o main.o sigfix.o \
#	extensions.o
 
libwuauth.a: $(OBJS)
	-rm -f libwuauth.a
	${AR} libwuauth.a $(OBJS)
	${RANLIB} libwuauth.a
 
clean:
	-rm -f *.o libwuauth.a *~ #*#

ftp.h:
	install -c -m 444 ftp.h /usr/include/arpa

.c.o:
	${CC} ${CFLAGS} ${MYCFLAGS} -c $<

strcasestr.o: strcasestr.c
	${CC} ${CFLAGS} ${MYCFLAGS} -c strcasestr.c

authuser.o: authuser.c
	${CC} ${CFLAGS} ${MYCFLAGS} -c authuser.c

snprintf.o: snprintf.c
	${CC} ${CFLAGS} ${MYCFLAGS} -c snprintf.c

