/*
** $Id: vboxgetty.h,v 1.4 1998/08/31 10:43:17 michael Exp $
**
** Copyright 1996-1998 Michael 'Ghandi' Herold <michael@abadonna.mayn.de>
*/

#ifndef _VBOXGETTY_H
#define _VBOXGETTY_H 1

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>

/** Defines **************************************************************/

#define VBOX_ROOT_UMASK "0022"

/** Variables ************************************************************/

extern unsigned char temppathname[PATH_MAX + 1];
extern unsigned char savettydname[NAME_MAX + 1];

/** Structures ***********************************************************/

extern struct vboxmodem vboxmodem;

/** Prototypes ***********************************************************/

extern void	 quit_program(int);
extern int	 set_process_permissions(uid_t, gid_t, int);

#endif /* _VBOXGETTY_H */
