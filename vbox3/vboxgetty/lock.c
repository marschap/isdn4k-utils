/*
** $Id: lock.c,v 1.1 1998/07/06 09:05:23 michael Exp $
**
** Copyright 1996-1998 Michael 'Ghandi' Herold <michael@abadonna.mayn.de>
**
** $Log: lock.c,v $
** Revision 1.1  1998/07/06 09:05:23  michael
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
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>

#include "lock.h"
#include "log.h"

/*************************************************************************/
/** lock_create():	Erzeugt einen Lock.											**/
/*************************************************************************/
/** => name				Name der Lockdatei.											**/
/**																							**/
/** <=					0 wenn der Lock erzeugt werden konnte oder -1 wenn	**/
/**						nicht.															**/
/*************************************************************************/

int lock_create(char *name)
{
	char  line[32];
	FILE *lptr;
	long	lock;
	int	loop;

	log_line(LOG_D, "Creating lock \"%s\"...\n", name);

	lock = -1;
	loop =  5;

		/* Pr�fen ob der Lock bereits existiert. Wenn ja wird die	*/
		/* PID eingelesen.														*/

	if (access(name, F_OK) == 0)
	{
		if ((lptr = fopen(name, "r")))
		{
			if (fgets(line, 32, lptr))
			{
				line[strlen(line) - 1] = '\0';

				lock = xstrtol(line, -1);
			}

			fclose(lptr);
		}
		else
		{
			log_line(LOG_E, "Lock exists but can not be opened.\n");
			
			return(-1);
		}
	}

		/* Wenn der Lock existiert, wird ein Signal 0 an den ent-	*/
		/* sprechenden Proze� geschickt um herauszufinden ob dieser	*/
		/* noch existiert.														*/

	if (lock > 1)
	{
		if (kill(lock, 0) == 0)
		{
			log_line(LOG_E, "Lock exists - pid %ld is running.\n", lock);
			
			return(-1);
		}
		else log_line(LOG_D, "Lock exists - pid %ld not running.\n", lock);
	}

		/* Der Lock existierte nicht, die Datei wird neu ange-	*/
		/* legt.																	*/

	while (loop > 0)
	{
		if ((lptr = fopen(name, "w")))
		{
			fprintf(lptr, "%010ld\n", getpid());
			fclose(lptr);
			return(0);
		}

		usleep(500);
		
		loop--;
	}

	log_line(LOG_E, "Can't create lock \"%s\".\n", name);
	
	return(-1);
}

/*************************************************************************/
/** lock_remove():	Entfernt einen Lock.											**/
/*************************************************************************/
/** => name				Name der Lockdatei.											**/
/**																							**/
/** <=					0 wenn der Lock entfernt werden konnte oder -1		**/
/**						wenn nicht.														**/
/*************************************************************************/

int lock_remove(char *name)
{
	int loop = 5;

	log_line(LOG_D, "Removing lock \"%s\"...\n", name);

	while (loop > 0)
	{
		if (remove(name) == 0) return(0);

		if (errno == ENOENT) return(0);

		usleep(500);
		
		loop--;
	}

	log_line(LOG_E, "Can't remove lock \"%s\".\n", name);

	return(-1);
}