/*
** $Id: control.h,v 1.2 1998/08/28 13:06:13 michael Exp $
**
** Copyright 1996-1998 Michael 'Ghandi' Herold <michael@abadonna.mayn.de>
*/

#ifndef _VBOX_CONTROL_H
#define _VBOX_CONTROL_H 1

/** Defines **************************************************************/

#define VBOX_CTRL_MAX_RCLINE	64

/** Prototypes ***********************************************************/

extern char *ctrl_exists(char *, char *, char *);
extern int	 ctrl_create(char *, char *, char *, char *);
extern int	 ctrl_remove(char *, char *, char *);

#endif /* _VBOX_CONTROL_H */
