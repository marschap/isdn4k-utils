/*
** $Id: rc.c,v 1.1 1998/06/17 16:38:49 michael Exp $
**
** Copyright 1997-1998 by Michael Herold <michael@abadonna.mayn.de>
**
** $Log: rc.c,v $
** Revision 1.1  1998/06/17 16:38:49  michael
** - First part of the automake/autoconf implementation. Currently vbox will
**   *not* compile!
**
** Revision 1.1  1998/06/10 13:31:55  michael
** Source added.
**
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "log.h"
#include "rc.h"

/*************************************************************************/
/** rc_read():		Reads a configuration into the structure.					**/
/*************************************************************************/
/** => rc			Configuration structure											**/
/** => rcname		Name of the configuration to read							**/
/** => section		Name of the section to jump to								**/
/** <=				 0 success															**/
/** <=				-1 unable to open													**/
/** <=				-2 section not found												**/
/*************************************************************************/

int rc_read(struct vboxrc *rc, char *rcname, char *section)
{
	char	rctmpln[VBOX_MAX_RCLINE_SIZE + 1];
	FILE *rctxtio;
	int	rcerror;
	int	rcsjump;
	char *stop;
	char *name;
	char *args;

	log_line(LOG_D, "Parsing \"%s\"...\n", rcname);

	rcerror = 0;
	rcsjump = 0;
	
	if ((rctxtio = fopen(rcname, "r")))
	{
		while ((fgets(rctmpln, VBOX_MAX_RCLINE_SIZE, rctxtio)) && (rcerror == 0))
		{
			rctmpln[strlen(rctmpln) - 1] = '\0';

			if ((stop = rindex(rctmpln, '#'))) *stop = '\0';

			if (rctmpln[0] == '\0') continue;

			if ((section) && (rcsjump == 0))
			{

				continue;
			}

			name = strtok(rctmpln, "\t ");
			args = strtok(NULL   , "\t ");

			rc_set_entry(rc, name, args);
		}

		fclose(rctxtio);
	}
	else rcerror = -1;

	if ((rcerror == 0) && (section) && (rcsjump == 0))
	{
		log_line(LOG_E, "Section \"%s\" not found.\n", section);

		rcerror = -2;
	}

	return(rcerror);
}

/*************************************************************************/
/** **/
/*************************************************************************/

void rc_free(struct vboxrc *rc)
{
	int i = 0;
	
	while (rc[i].name)
	{
		if (rc[i].value) free(rc[i].value);
		
		rc[i].value = NULL;
	
		i++;
	}
}

unsigned char *rc_get_entry(struct vboxrc *rc, char *name)
{
	int i = 0;

	while (rc[i].name)
	{
		if (strcasecmp(rc[i].name, name) == 0) return(rc[i].value);
	
		i++;
	}
	
	return(NULL);
}

/*************************************************************************/
/** **/
/*************************************************************************/

unsigned char *rc_set_entry(struct vboxrc *rc, char *name, char *value)
{
	int				i = 0;
	unsigned char *v = NULL;

	log_line(LOG_D, "Setting \"%s\" to \"%s\"...\n", (name ? name : "???"), (value ? value : "???"));

	if ((!name) || (!value))
	{
		if (!name ) log_line(LOG_W, "Configuration variable name not set (ignored).\n");
		if (!value) log_line(LOG_W, "Configuration argument not set (ignored).\n");

		return(NULL);
	}
	
	while (rc[i].name)
	{
		if (strcasecmp(rc[i].name, name) == 0)
		{
			v = strdup(value);
			
			if (v)
			{
				if (rc[i].value) free(rc[i].value);
				
				rc[i].value = v;
			}
			else log_line(LOG_E, "Can't set \"%s\"'s value.\n", name);

			return(v);
		}

		i++;
	}

	log_line(LOG_W, "Unknown entry \"%s\" ignored.\n", name);

	return(NULL);
}

unsigned char *rc_set_empty(struct vboxrc *rc, char *name, char *value)
{
	int i = 0;

	while (rc[i].name)
	{
		if (strcasecmp(rc[i].name, name) == 0)
		{
			if (rc[i].value) return(rc[i].value);

			return(rc_set_entry(rc, name, value));
		}

		i++;
	}

	return(NULL);
}

