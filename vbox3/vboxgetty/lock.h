/*
** $Id: lock.h,v 1.1 1998/07/06 09:05:24 michael Exp $
**
** Copyright 1996-1998 Michael 'Ghandi' Herold <michael@abadonna.mayn.de>
*/

#ifndef _VBOX_LOCK_H
#define _VBOX_LOCK_H 1

/** Prototypes ***********************************************************/

extern int	 lock_create(char *);
extern int	 lock_remove(char *);

#endif /* _VBOX_LOCK_H */
