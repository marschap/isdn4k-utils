/*
** $Id: vboxgetty.h,v 1.2 1998/07/06 09:05:36 michael Exp $
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

extern char temppathname[PATH_MAX + 1];

/** Structures ***********************************************************/

extern struct vboxmodem vboxmodem;

/** Prototypes ***********************************************************/

extern void	 quit_program(int);
extern int	 set_process_permissions(uid_t, gid_t, int);

#endif /* _VBOXGETTY_H */
