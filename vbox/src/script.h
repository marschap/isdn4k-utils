/*
** $Id: script.h,v 1.4 1997/05/10 10:58:50 michael Exp $
**
** Copyright (C) 1996, 1997 Michael 'Ghandi' Herold
*/

#ifndef _VBOX_SCRIPT_H
#define _VBOX_SCRIPT_H 1

#include "lists.h"

/** Prototypes ***********************************************************/

extern struct minlist breaklist;

extern int script_run(char *);
extern int script_check_interpreter(void);

#endif /* _VBOX_SCRIPT_H */
