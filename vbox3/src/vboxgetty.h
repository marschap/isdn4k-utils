/*
** $Id: vboxgetty.h,v 1.1 1998/06/10 13:31:59 michael Exp $
**
** Copyright 1997-1998 by Michael 'Ghandi' Herold
*/

#ifndef _VBOXGETTY_H
#define _VBOXGETTY_H 1

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define VBOX_DEFAULT_SPOOLDIR		"/var/spool/vbox"

extern char temppathname[PATH_MAX + 1];

extern struct vboxmodem vboxmodem;



extern void quit_program(int);
extern long vbox_strtol(char *, long);


#define printstring sprintf


#endif /* _VBOXGETTY_H */
