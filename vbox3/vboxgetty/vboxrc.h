/*
** $Id: vboxrc.h,v 1.1 1998/07/06 09:05:38 michael Exp $
**
** Copyright 1996-1998 Michael 'Ghandi' Herold <michael@abadonna.mayn.de>
*/

#ifndef _VBOXRC_H
#define _VBOXRC_H 1

/** Defines **************************************************************/

#define VBOXRC_MAX_RCLINE		255

/** Structures ***********************************************************/

struct vboxcall
{
	char	section[VBOXRC_MAX_RCLINE + 1];
	char	name[VBOXRC_MAX_RCLINE + 1];
	char	script[VBOXRC_MAX_RCLINE + 1];
	int	ringring;
	int	tollring;
	int	savetime;
};

/** Prototypes ***********************************************************/

#endif /* _VBOXRC_H */
                  