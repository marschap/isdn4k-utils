/*
** $Id: lock.c,v 1.3 1997/02/26 13:10:40 michael Exp $
**
** Copyright (C) 1996, 1997 Michael 'Ghandi' Herold
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#include "runtime.h"
#include "lock.h"
#include "log.h"
#include "init.h"
#include "perms.h"
#include "libvbox.h"

/** Variables ************************************************************/

static int modemfd = -1;
static int gettyfd = -1;

static struct locks locks[] =
{
	{ LCK_MODEM, &modemfd, LOCKFILE, "modem port" },
	{ LCK_PID  , &gettyfd, PIDFILE , "vbox"       },
	{ 0        , NULL    , NULL    , NULL         }
};

/** Prototypes ***********************************************************/

static int lock_locking(int, int);
static int lock_unlocking(int, int);

/*************************************************************************
 ** lock_type_lock():	Create vbox lock.											**
 *************************************************************************/

int lock_type_lock(int type)
{
	char  name[LOCK_MAX_LOCKNAME + 1];
	char	temp[14];
	char *device;
	int	i;

	if (!(device = rindex(setup.modem.device, '/')))
	{
		device = setup.modem.device;
	}
	else device++;
	
	i = 0;
	
	while (TRUE)
	{
		if (locks[i].file == NULL)
		{
			log(L_WARN, "Lock setup for type %d not found.\n", type);

			returnok();
		}

		if (locks[i].type == type) break;

		i++;
	}

	if ((strlen(locks[i].file) + strlen(device)) < LOCK_MAX_LOCKNAME)
	{
		sprintf(name, locks[i].file, device);

		log(L_DEBUG, "Locking %s '%s'...\n", locks[i].desc, name);

		if (*(locks[i].fd) == -1)
		{
			if ((*(locks[i].fd) = open(name, O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)) != -1)
			{
				if (lock_locking(*(locks[i].fd), 5))
				{
					sprintf(temp, "%10d\n", getpid());

					write(*(locks[i].fd), temp, strlen(temp));

						/* Set permissions but make the locks readable to	*/
						/* all (overrides umask).									*/

					permissions_set(name, setup.users.uid, setup.users.gid, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH, (setup.users.umask & ~(S_IRUSR|S_IRGRP|S_IROTH)));

					returnok();
				}
				else log(L_FATAL, "Can't lock '%s'.\n", name);
			}
			else log(L_FATAL, "Can't create '%s'.\n", name);
		}
		else
		{
			log(L_WARN, "Use existing lock for '%s' (%d).\n", name, *(locks[i].fd));
					
			returnok();
		}
	}
	else log(L_FATAL, "Lockname too long. Set 'LOCK_MAX_LOCKNAME' in 'lock.h' to a higher value.\n");
				
	returnerror();
}

/*************************************************************************
 ** lock_type_unlock():	Deletes vbox lock.										**
 *************************************************************************/

int lock_type_unlock(int type)
{
	char  name[LOCK_MAX_LOCKNAME + 1];
	char *device;
	int	i;

	if (!(device = rindex(setup.modem.device, '/')))
	{
		device = setup.modem.device;
	}
	else device++;
	
	i = 0;
	
	while (TRUE)
	{
		if (locks[i].file == NULL)
		{
			log(L_WARN, "Lock setup for type %d not found.\n", type);

			returnok();
		}

		if (locks[i].type == type) break;

		i++;
	}

	if ((strlen(locks[i].file) + strlen(device)) < LOCK_MAX_LOCKNAME)
	{
		sprintf(name, locks[i].file, device);

		log(L_DEBUG, "Unlocking %s '%s'...\n", locks[i].desc, name);

		if (*(locks[i].fd) != -1)
		{
			lock_unlocking(*(locks[i].fd), 5);

			close(*(locks[i].fd));

			*(locks[i].fd) = -1;
		}

		unlink(name);
		unlink(name);
	}
	else log(L_FATAL, "Lockname too long. Set 'LOCK_MAX_LOCKNAME' in 'lock.h' to a higher value.\n");
				
	returnerror();
}

/*************************************************************************
 ** lock_locking():	Locks a file descriptor with delay.						**
 *************************************************************************/

static int lock_locking(int fd, int trys)
{
	while (trys > 0)
	{
		if (flock(fd, LOCK_EX|LOCK_NB) == 0) returnok();
            
		xpause(1000);
                  
		trys--;
	}

	returnerror();
}

/*************************************************************************
 ** lock_unlocking():	Unlocks a file descriptor with delay.				**
 *************************************************************************/

static int lock_unlocking(int fd, int trys)
{
	while (trys > 0)
	{
		if (flock(fd, LOCK_UN) == 0) returnok();
            
		xpause(1000);
                  
		trys--;
	}

	returnerror();
}

