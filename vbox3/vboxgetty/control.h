/*
** $Id: control.h,v 1.1 1998/07/06 09:05:20 michael Exp $
**
** Copyright 1996-1998 Michael 'Ghandi' Herold <michael@abadonna.mayn.de>
*/

#ifndef _VBOX_CONTROL_H
#define _VBOX_CONTROL_H 1

/** Defines **************************************************************/

#define VBOX_CTRL_MAX_RCLINE	64

/** Prototypes ***********************************************************/

extern char *ctrl_exists(char *, char *);
extern int	 ctrl_create(char *, char *, char *);
extern int	 ctrl_remove(char *, char *);

#endif /* _VBOX_CONTROL_H */
