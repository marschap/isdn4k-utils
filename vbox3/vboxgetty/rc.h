/*
** $Id: rc.h,v 1.2 1998/07/06 09:05:29 michael Exp $
**
** Copyright 1996-1998 Michael 'Ghandi' Herold <michael@abadonna.mayn.de>
*/

#ifndef _VBOX_RC_H
#define _VBOX_RC_H 1

/** Defines **************************************************************/

#define VBOX_MAX_RCLINE_SIZE		255

/** Structures ***********************************************************/

struct vboxrc
{
	unsigned char *name;
	unsigned char *value;
};

/** Prototypes ***********************************************************/

extern int				 rc_read(struct vboxrc *, char *, char *);
extern void				 rc_free(struct vboxrc *);
extern unsigned char *rc_get_entry(struct vboxrc *, char *);
extern unsigned char *rc_set_entry(struct vboxrc *, char *, char *);
extern unsigned char *rc_set_empty(struct vboxrc *, char *, char *);

#endif /* _VBOX_RC_H */
