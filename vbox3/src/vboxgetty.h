/*
** $Id: vboxgetty.h,v 1.2 1998/06/17 12:20:39 michael Exp $
**
** Copyright 1997-1998 by Michael 'Ghandi' Herold
*/

#ifndef _VBOXGETTY_H
#define _VBOXGETTY_H 1

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>

#define VBOX_DEFAULT_SPOOLDIR		"/var/spool/vbox"

#define VBOX_RCLINE_SIZE			255

#define VBOXUSER_CALLID				64
#define VBOXUSER_NUMBER				64

extern char temppathname[PATH_MAX + 1];

extern struct vboxmodem vboxmodem;

struct vboxuser
{
	uid_t	uid;
	gid_t	gid;
	int	umask;
	long	space;
	char	incomingid[VBOXUSER_CALLID + 1];
	char	localphone[VBOXUSER_NUMBER + 1];
	char	home[PATH_MAX + 1];
};










extern void quit_program(int);

#define printstring sprintf


#endif /* _VBOXGETTY_H */
