/*
** $Id: tclscript.h,v 1.2 1998/07/06 09:05:32 michael Exp $
**
** Copyright 1996-1998 Michael 'Ghandi' Herold <michael@abadonna.mayn.de>
*/

#ifndef _VBOX_TCLSCRIPT_H
#define _VBOX_TCLSCRIPT_H 1

#include <tcl.h>
#include "userrc.h"

/** Structures ***********************************************************/

struct vbox_tcl_function
{
	char				*name;
	Tcl_ObjCmdProc	*proc;
};

struct vbox_tcl_variable
{
	char *name;
	char *args;
};

/** Defines **************************************************************/

#define VBOX_TCLFUNC_PROTO ClientData, Tcl_Interp *, int, Tcl_Obj *CONST []
#define VBOX_TCLFUNC			ClientData data, Tcl_Interp *intp, int objc, Tcl_Obj *CONST objv[]

/** Prototypes ***********************************************************/

extern int	 scr_create_interpreter(void);
extern void  scr_remove_interpreter(void);
extern int	 scr_execute(char *, struct vboxuser *);
extern int	 scr_init_variables(struct vbox_tcl_variable *);
extern char *scr_tcl_version(void);

#endif /* _VBOX_TCLSCRIPT_H */
