/*
** $Id: tclscript.h,v 1.1 1998/06/10 13:31:57 michael Exp $
**
** Copyright 1997-1998 by Michael 'Ghandi' Herold
*/

#ifndef _VBOX_TCLSCRIPT_H
#define _VBOX_TCLSCRIPT_H 1

#include <tcl.h>

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


#define VBOX_TCLFUNC_PROTO ClientData, Tcl_Interp *, int, Tcl_Obj *CONST []
#define VBOX_TCLFUNC			ClientData data, Tcl_Interp *intp, int objc, Tcl_Obj *CONST objv[]


/** Prototypes ***********************************************************/

extern int	 scr_create_interpreter(void);
extern void  scr_remove_interpreter(void);
extern int	 scr_execute(char *, char *);
extern int	 scr_init_variables(struct vbox_tcl_variable *);


extern char *scr_tcl_version(void);

#endif /* _VBOX_TCLSCRIPT_H */
