/*
** $Id: lock.h,v 1.1 1997/02/25 17:40:49 michael Exp $
**
** Copyright (C) 1996, 1997 Michael 'Ghandi' Herold
*/

#ifndef _VBOX_LOCK_H
#define _VBOX_LOCK_H 1

/** Defines **************************************************************/

#define LOCK_MAX_LOCKNAME	256

#define LCK_MODEM		1
#define LCK_PID		2

/** Structures ***********************************************************/

struct locks
{
	int	type;
	int  *fd;
	char *file;
	char *desc;
};

/** Prototypes ***********************************************************/

extern int lock_type_lock(int);
extern int lock_type_unlock(int);

#endif /* _VBOX_LOCK_H */
