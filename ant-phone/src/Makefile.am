## Process this file with automake to produce Makefile.in

PACKAGE = @PACKAGE@
VERSION = @VERSION@

bin_PROGRAMS = ant-phone

ant_phone_SOURCES = \
	ant-phone.c \
	callerid.c \
	g711.c \
	gtk.c \
	isdn.c \
	llcheck.c \
	mediation.c \
	session.c \
	sound.c \
	util.c \
	settings.c \
	calleridlexer.l \
	calleridparser.y \
	isdnlexer.l \
	isdnparser.y \
	gtksettings.c \
	controlpad.c \
	fxgenerator.c \
	server.c \
	client.c \
	recording.c \
	isdntree.c

noinst_HEADERS = \
	callerid.h \
	g711.h \
	gtk.h \
	isdn.h \
	llcheck.h \
	mediation.h \
	session.h \
	sound.h \
	util.h \
	settings.h \
	gtksettings.h \
	controlpad.h \
	fxgenerator.h \
	server.h \
	client.h \
	recording.h \
	globals.h \
	gettext.h \
	isdnlexer.h \
	isdntree.h

EXTRA_DIST = \
	pickup.xpm \
	hangup.xpm \
	in.xpm \
	out.xpm \
	backspace.xpm \
	redial.xpm \
	mute.xpm \
	aboutlogo.xpm \
	record.xpm \
	icon16x16.xpm \
	icon32x32.xpm \
	icon48x48.xpm \
	icon64x64.xpm

top_srcdir = @top_srcdir@
subdir = src

datadir = @datadir@
localedir = $(datadir)/locale
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@

LIBS = @LIBINTL@ @LIBS@

INCLUDES = -I../intl -I$(top_srcdir)/intl @DEPS_CFLAGS@
AM_CFLAGS = -DVERSION='"@VERSION@"' -DPACKAGE='"@PACKAGE@"' @CFLAGS@
AM_LDFLAGS = @DEPS_LIBS@
LDADD = @LEXLIB@

AM_YFLAGS = -d
##YFLAGS = -d
AM_LFLAGS=-olex.yy.c
##LFLAGS=-Pcallerid

BUILT_SOURCES = isdnparser.h calleridparser.h
