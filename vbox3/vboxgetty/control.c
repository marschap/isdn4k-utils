/*
** $Id: control.c,v 1.1 1998/07/06 09:05:18 michael Exp $
**
** Copyright 1996-1998 Michael 'Ghandi' Herold <michael@abadonna.mayn.de>
**
** $Log: control.c,v $
** Revision 1.1  1998/07/06 09:05:18  michael
** - New control file code added. The controls are not longer only empty
**   files - they can contain additional informations.
** - Control "vboxctrl-answer" added.
** - Control "vboxctrl-suspend" added.
** - Locking mechanism added.
** - Configuration parsing added.
** - Some code cleanups.
**
*/

#include "../config.h"

#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <unistd.h>

#include "control.h"
#include "stringutils.h"
#include "log.h"

/** Variablen ************************************************************/

static char ctrlpathname[PATH_MAX + 1];
static char ctrllastline[VBOX_CTRL_MAX_RCLINE + 1];

/*************************************************************************/
/** ctrl_exists():	Untersucht ob eine Kontrolldatei existiert und		**/
/**						gibt deren Inhalt zurück.									**/
/*************************************************************************/
/** => home				Homeverzeichnis des Benutzers.							**/
/** => name				Name der Kontrolldatei (ohne vboxctrl-).				**/
/**																							**/
/** <=					NULL wenn die Datei nicht gelesen werden konnte		**/
/**						oder ein Zeiger auf deren Initstring.					**/
/*************************************************************************/

char *ctrl_exists(char *home, char *name)
{
	FILE *cptr;
	char *stop;

	printstring(ctrlpathname, "%s/vboxctrl-%s", home, name);

	if ((cptr = fopen(ctrlpathname, "r")))
	{
		if (fgets(ctrllastline, VBOX_CTRL_MAX_RCLINE, cptr))
		{
			ctrllastline[strlen(ctrllastline) - 1] = '\0';

			if ((stop = index(ctrllastline, '\r'))) *stop = '\0';

			fclose(cptr);
			return(ctrllastline);
		}

		fclose(cptr);
	}

	return(NULL);
}

/*************************************************************************/
/** ctrl_create():	Erzeugt eine Kontrolldatei und schreibt den Init-	**/
/**						string.															**/
/*************************************************************************/
/** => home				Homeverzeichnis des Benutzers.							**/
/** => name				Name der Kontrolldatei (ohne vboxctrl-).				**/
/** => init				Initstring.														**/
/**																							**/
/** <=					0 wenn die Datei erzeugt werden konnte oder -1 bei	**/
/**						einem Fehler.													**/
/*************************************************************************/

int ctrl_create(char *home, char *name, char *init)
{
	FILE *cptr = NULL;
	int	loop = 5;

	printstring(ctrlpathname, "%s/vboxctrl-%s", home, name);

	while (loop > 0)
	{
		log_line(LOG_D, "Creating control \"vboxctrl-%s:%s\"...\n", name, init);

		if ((cptr = fopen(ctrlpathname, "w")))
		{
			fprintf(cptr, "%s\n", init);
			fclose(cptr);
			return(0);
		}

		usleep(500);
		
		loop--;
	}

	log_line(LOG_E, "Can't create \"%s\".\n", ctrlpathname);

	return(-1);
}

/*************************************************************************/
/** ctrl_remove():	Löscht eine Kontrolldatei.									**/
/*************************************************************************/
/** => home				Homeverzeichnis des Benutzers.							**/
/** => name				Name der Kontrolldatei (ohne vboxctrl-).				**/
/**																							**/
/** <=					0 wenn die Datei gelöscht werden konnte oder -1		**/
/**						bei einem Fehler.												**/
/*************************************************************************/

int ctrl_remove(char *home, char *name)
{
	int loop = 5;

	printstring(ctrlpathname, "%s/vboxctrl-%s", home, name);

	while (loop > 0)
	{
		log_line(LOG_D, "Removing control \"vboxctrl-%s\"...\n", name);

		if (remove(ctrlpathname) == 0) return(0);

		if (errno == ENOENT) return(0);

		usleep(500);

		loop--;
	}	

	log_line(LOG_E, "Can't remove \"%s\".\n", ctrlpathname);

	return(-1);
}
