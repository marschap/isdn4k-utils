#$Id: Makefile,v 1.1 1999/06/30 17:19:08 he Exp $
# This Makefile uses GNU-make extensions

default: eftp


#
# If EFT_LIBDIR or EFT_INCDIR are set (the top level makefile
# does so) this makefile will generate dependencies and search pathes
# (refering to the libraries and corresponding header files).
#
# Otherwise (for stand-alone compilation), no such dependencies are generated
# and libraries and includes are only searched in the standard location only.
#

ifdef EFT_LIBDIR
EFT_LIBDIR_FLAGS := -L $(EFT_LIBDIR) 
eftp: $(EFT_LIBDIR)/libeft.a 
endif

ifdef EFT_INCDIR
EFT_INCDIR_FLAGS := -I $(EFT_INCDIR) 
eftp.o: $(EFT_INCDIR)/tdu_user.h $(EFT_INCDIR)/eft.h
endif

ifeq ($(CONFIG_EFTP_READLINE),y)
READLINE_CFLAGS=-DCONFIG_EFTP_READLINE
# only needed for old implementation
# READLINE_O=read_line.o
# read_line.o: read_line.c read_line.h
#	cc $(CFLAGS) -c $(READLINE_CFLAGS) read_line.c -o read_line.o
#
#
#READLINE_LDFLAGS=$(READLINE_O) -L/usr/local/lib -lncurses -lreadline
READLINE_LDFLAGS= -lncurses -lreadline
endif

eftp.o:	eftp.c
	cc $(CFLAGS) $(EFT_INCDIR_FLAGS) -c $(READLINE_CFLAGS) eftp.c -o eftp.o

eftp: eftp.o $(READLINE_O)
	cc -g eftp.o $(EFT_LIBDIR_FLAGS) $(READLINE_LDFLAGS) -left -o eftp

clean:
	rm -f eftp *.o 

.PHONY: clean default
