/*
** $Id: userrc.h,v 1.1 1998/07/06 09:05:34 michael Exp $
**
** Copyright 1996-1998 Michael 'Ghandi' Herold <michael@abadonna.mayn.de>
*/

#ifndef _USERRC_H
#define _USERRC_H 1

#include <limits.h>

/** Defines **************************************************************/

#define USERRC_MAX_RCLINE			255

#define VBOXUSER_CALLID				64
#define VBOXUSER_NUMBER				64
#define VBOXUSER_USERNAME			64

/** Structures ***********************************************************/

struct vboxuser
{
	uid_t	uid;
	gid_t	gid;
	int	umask;
	long	space;
	char	incomingid[VBOXUSER_CALLID + 1];
	char	localphone[VBOXUSER_NUMBER + 1];
	char	name[VBOXUSER_USERNAME + 1];
	char	home[PATH_MAX + 1];
};

/** Prototypes ***********************************************************/

extern int userrc_parse(struct vboxuser *, char *, char *);

#endif /* _USERRC_H */
