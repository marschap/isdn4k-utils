/*
** $Id: libvbox.c,v 1.4 1997/02/26 20:33:51 michael Exp $
**
** Copyright (C) 1996, 1997 Michael 'Ghandi' Herold
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "libvbox.h"                     

/** Variables ************************************************************/

char *compressions[] =
{
   "?", "?", "ADPCM-2", "ADPCM-3", "ADPCM-4", "ALAW", "ULAW"
};

/*************************************************************************
 ** ctrl_create():	Creates a vbox control file. The file is created	**
 **						with the permissions -rw-rw-rw-.							**
 *************************************************************************
 ** path					Path to the spool directory of the current user.	**
 ** file					Name of the control file to create.						**
 ** <return>			0 on error; 1 on success.									**
 *************************************************************************/

int ctrl_create(char *path, char *file)
{
	char	location[PATH_MAX + 1];
	int	fd;
	
	xstrncpy(location, path, PATH_MAX);
	xstrncat(location, "/" , PATH_MAX);
	xstrncat(location, file, PATH_MAX);

	if ((fd = open(location, O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)) != -1)
	{
		chmod(location, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
		chmod(location, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
		close(fd);
		
		returnok();
	}

	returnerror();
}

/*************************************************************************
 ** ctrl_remove():	Removes a vbox control file.								**
 *************************************************************************
 ** path					Path to the spool directory of the current user.	**
 ** file					Name of the control file to remove.						**
 ** <return>			0 on error; 1 on success.									**
 *************************************************************************/

int ctrl_remove(char *path, char *file)
{
	char	location[PATH_MAX + 1];
	int	loop;
	
	xstrncpy(location, path, PATH_MAX);
	xstrncat(location, "/" , PATH_MAX);
	xstrncat(location, file, PATH_MAX);

	loop = 5;

	while (loop > 0)
	{
		unlink(location);
		unlink(location);
		
		if (!ctrl_ishere(path, file)) returnok();

		xpause(1000);
		
		loop--;
	}

	returnerror();
}

/*************************************************************************
 ** ctrl_ishere():	Checks if a control file exists.							**
 *************************************************************************
 ** path					Path to the spool directory of the current user.	**
 ** file					Name of the control file to check.						**
 ** <return>			0 don't exists; 1 exists.									**
 *************************************************************************/

int ctrl_ishere(char *path, char *file)
{
	char location[PATH_MAX + 1];
	
	xstrncpy(location, path, PATH_MAX);
	xstrncat(location, "/" , PATH_MAX);
	xstrncat(location, file, PATH_MAX);

	if (access(location, F_OK) == 0) returnok();

	returnerror();
}

/*************************************************************************
 ** xstrncpy():	Copys one string to another.									**
 *************************************************************************
 ** dest				Pointer to the destination.									**
 ** source			Pointer to the source.											**
 ** max				Max length of destination.										**
 *************************************************************************/

void xstrncpy(char *dest, char *source, int max)
{
	strncpy(dest, source, max);
	
	dest[max] = '\0';
}

/*************************************************************************
 ** xstrncat():	Cats one string to another.									**
 *************************************************************************
 ** dest				Pointer to the destination.									**
 ** source			Pointer to the source.											**
 ** max				Max length of destination.										**
 *************************************************************************/

void xstrncat(char *dest, char *source, int max)
{
	if ((max - strlen(dest)) > 0) strncat(dest, source, max - strlen(dest));

	dest[max] = '\0';
}

/*************************************************************************
 ** xpause():	Waits some miliseconds.												**
 *************************************************************************
 ** ms			Miliseconds to wait.													**
 *************************************************************************/

void xpause(unsigned long ms)
{
	usleep(ms * 1000);
}

/*************************************************************************
 ** xstrtol():	Converts a string to a long number, using a default on	**
 **				error.																	**
 *************************************************************************
 ** str			String to convert to long.											**
 ** use			Default value if string can't converted.						**
 ** <return>	Converted string value on success; default on error.		**
 *************************************************************************/

long xstrtol(char *str, long use)
{
	char *stop;
	long	line;

	line = strtol(str, &stop, 10);
	
	if ((line < 0) || (*stop != 0)) line = use;

	return(line);
}

/*************************************************************************
 ** header_put():	Writes the vbox audio header.									**
 *************************************************************************
 ** fd				File descriptor used to write.								**
 ** header			Pointer to a filled vbox audio header.						**
 ** <return>		0 on error; 1 on success.										**
 *************************************************************************/

int header_put(int fd, vaheader_t *header)
{
	if (write(fd, header, sizeof(vaheader_t)) != sizeof(vaheader_t))
	{
		returnerror();
	}

	returnok();
}

/*************************************************************************
 ** header_get():	Reads a vbox audio header.										**
 *************************************************************************
 ** fd				File descriptor used to read.									**
 ** header			Pointer to a vbox audio header.								**
 ** <return>		0 on error; 1 on success.										**
 *************************************************************************/

int header_get(int fd, vaheader_t *header)
{
	vaheader_t dummy;

	if (read(fd, &dummy, VAH_MAX_MAGIC) == VAH_MAX_MAGIC)
	{
		if (strncmp(dummy.magic, VAH_MAGIC, VAH_MAX_MAGIC) == 0)
		{
			if (lseek(fd, 0, SEEK_SET) == 0)
			{
				if (read(fd, header, sizeof(vaheader_t)) == sizeof(vaheader_t))
				{
					returnok();
				}
			}
		}
	}
	
	returnerror();
}
