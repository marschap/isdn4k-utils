/*
** $Id: rc.h,v 1.1 1998/06/10 13:31:56 michael Exp $
**
** Copyright 1997-1998 by Michael 'Ghandi' Herold
*/

#ifndef _VBOX_RC_H
#define _VBOX_RC_H 1

#define VBOX_MAX_RCLINE_SIZE		255

struct vboxrc
{
	unsigned char *name;
	unsigned char *value;
};

extern int				 rc_read(struct vboxrc *, char *, char *);
extern void				 rc_free(struct vboxrc *);
extern unsigned char *rc_get_entry(struct vboxrc *, char *);
extern unsigned char *rc_set_entry(struct vboxrc *, char *, char *);
extern unsigned char *rc_set_empty(struct vboxrc *, char *, char *);


#endif /* _VBOX_RC_H */
