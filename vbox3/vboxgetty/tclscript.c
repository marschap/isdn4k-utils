/*
** $Id: tclscript.c,v 1.3 1998/06/18 12:38:17 michael Exp $
**
** Copyright 1997-1998 by Michael Herold <michael@abadonna.mayn.de>
**
** $Log: tclscript.c,v $
** Revision 1.3  1998/06/18 12:38:17  michael
** - 2nd part of the automake/autoconf implementation (now compiles again).
**
** Revision 1.2  1998/06/17 17:01:24  michael
** - First part of the automake/autoconf implementation. Currently vbox will
**   *not* compile!
**
*/

#include <tcl.h>
#include <string.h>
#include <unistd.h>

#include "config.h"
#include "vboxgetty.h"
#include "log.h"
#include "modem.h"
#include "tclscript.h"

static Tcl_Interp *interpreter = NULL;





int vbox_block(VBOX_TCLFUNC_PROTO);
int vbox_log(VBOX_TCLFUNC_PROTO);
int vbox_modem_command(VBOX_TCLFUNC_PROTO);


static struct vbox_tcl_function vbox_tcl_functions[] =
{
	{ "exit", vbox_block },
	{ "vbox_log", vbox_log },
	{ "vbox_modem_command", vbox_modem_command },
	{ NULL, NULL }
};


static int scr_init_functions(void);








int scr_create_interpreter(void)
{
	log_line(LOG_D, "Initializing tcl interpreter...\n");

	if (TCL_MAJOR_VERSION >= 8)
	{
		if ((interpreter = Tcl_CreateInterp()))
		{
			if (scr_init_functions() == 0) return(0);
		}
	}

	return(-1);
}

void scr_remove_interpreter(void)
{
	int rc;

	if (interpreter)
	{
		log_line(LOG_D, "Removing tcl interpreter...\n");

		if ((rc = Tcl_InterpDeleted(interpreter)) == 0)
		{
			Tcl_DeleteInterp(interpreter);
		}
		else log_line(LOG_W, "Tcl interpreter can't be removed (returns %d).\n", rc);
	}
}

/*************************************************************************/
/** scr_execute():	Executes a tcl script.										**/
/*************************************************************************/
/** => name				Name of the script to execute								**/
/** => user				User name (if set, script is first searched in		**/
/**						users spooldir)												**/
/** <=					0 on success or -1 on error								**/
/*************************************************************************/

int scr_execute(char *name, char *user)
{
	int canrun = 0;

	if (user)
	{

		canrun = 0;
	}

	if (!canrun)
	{
		printstring(temppathname, "%s/%s/%s", DATADIR, PACKAGE, name);

		if (access(temppathname, F_OK|R_OK) == 0) canrun = 1;
	}

	if (canrun)
	{
		log_line(LOG_A, "Running \"%s\"...\n", temppathname);

		if (Tcl_EvalFile(interpreter, temppathname) == TCL_OK)
		{
			log_line(LOG_D, "Tcl script returns without errors.\n");
			
			return(0);
		}
		else log_line(LOG_E, "%s: %s [line %d]!\n", name, Tcl_GetStringResult(interpreter), interpreter->errorLine);
	}
	else log_line(LOG_E, "Tcl script \"%s\" not found.\n", name);

	return(-1);
}

/*************************************************************************/
/** scr_tcl_version():	Returns current tcl version number.					**/
/*************************************************************************/
/** <=						Tcl version string										**/
/*************************************************************************/

char *scr_tcl_version(void)
{
	return(TCL_VERSION);
}

/*************************************************************************/
/** scr_init_functions():	Adds the vbox functions to the interpreter.	**/
/*************************************************************************/
/** <=							0 on success or -1 on error						**/
/*************************************************************************/

static int scr_init_functions(void)
{
	int i = 0;
	
	while (vbox_tcl_functions[i].name)
	{
		if (!Tcl_CreateObjCommand(interpreter, vbox_tcl_functions[i].name, vbox_tcl_functions[i].proc, NULL, NULL))
		{
			log_line(LOG_E, "Can't add new tcl command \"%s\".\n", vbox_tcl_functions[i].name);

			return(-1);
		}
		else log_line(LOG_D, "New tcl command \"%s\" added.\n", vbox_tcl_functions[i].name);

		i++;
	}

	return(0);
}

/*************************************************************************/
/** scr_init_variables():	Initialize global tcl variables.					**/
/*************************************************************************/
/** => vars						Pointer to a filled variable structure			**/
/** <=							0 on success or -1 on error						**/
/*************************************************************************/

int scr_init_variables(struct vbox_tcl_variable *vars)
{
	int i = 0;
	
	while (vars[i].name)
	{
		if (Tcl_VarEval(interpreter, "set ", vars[i].name, " \"", vars[i].args, "\"", NULL) != TCL_OK)
		{
			log_line(LOG_E, "Can't set tcl variable \"%s\".\n", vars[i].name);

			return(-1);
		}
		else log_line(LOG_D, "Tcl variable \"%s\" set to \"%s\".\n", vars[i].name, vars[i].args);
	
		i++;
	}

	return(0);
}

















int vbox_block(VBOX_TCLFUNC)
{
	log_line(LOG_W, "Tcl command \"%s\" is blocked!\n", Tcl_GetStringFromObj(objv[0], NULL));

	return(TCL_OK);
}







int vbox_log(VBOX_TCLFUNC)
{
	char *levelv;
	int	levelc;

	if (objc == 3)
	{
		levelv = Tcl_GetStringFromObj(objv[1], NULL);
		levelc = LOG_E;
		
		switch (levelv[0])
		{
			case 'W':
				levelc = LOG_W;
				break;

			case 'I':
				levelc = LOG_I;
				break;

			case 'A':
				levelc = LOG_A;
				break;

			case 'D':
				levelc = LOG_D;
				break;
		}

		log_line(levelc, "%s\n", Tcl_GetStringFromObj(objv[2], NULL));
	}
	else log_line(LOG_E, "Bad vars %d\n", objc);

	return(TCL_OK);
}

int vbox_modem_command(VBOX_TCLFUNC)
{
	int i;

	if (objc == 3)
	{
		i = modem_command(&vboxmodem, Tcl_GetStringFromObj(objv[1], NULL), Tcl_GetStringFromObj(objv[2], NULL));

		printstring(temppathname, "%d", i);
		
		Tcl_SetResult(intp, temppathname, NULL);
	}
	else log_line(LOG_E, "Bad vars %d\n", objc);

	return(TCL_OK);
}















