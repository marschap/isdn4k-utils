/*
** $Id: stringutils.c,v 1.4 1998/08/31 10:43:11 michael Exp $
**
** Copyright 1996-1998 Michael 'Ghandi' Herold <michael@abadonna.mayn.de>
**
** $Log: stringutils.c,v $
** Revision 1.4  1998/08/31 10:43:11  michael
** - Changed "char" to "unsigned char".
**
** Revision 1.3  1998/07/06 09:05:30  michael
** - New control file code added. The controls are not longer only empty
**   files - they can contain additional informations.
** - Control "vboxctrl-answer" added.
** - Control "vboxctrl-suspend" added.
** - Locking mechanism added.
** - Configuration parsing added.
** - Some code cleanups.
**
** Revision 1.2  1998/06/17 17:01:23  michael
** - First part of the automake/autoconf implementation. Currently vbox will
**   *not* compile!
**
*/

#ifdef HAVE_CONFIG_H
#  include "../config.h"
#endif

#include <string.h>

#include "stringutils.h"

/*************************************************************************/
/** xstrncpy():																			**/
/*************************************************************************/

void xstrncpy(unsigned char *dest, unsigned char *source, int max)
{
	strncpy(dest, source, max);
   
	dest[max] = '\0';
}

/*************************************************************************/
/** xstrtol():																				**/
/*************************************************************************/
      
long xstrtol(unsigned char *string, long number)
{
	long           back;
	unsigned char *stop;

	if (string)
	{
		back = strtol(string, &stop, 10);
		
		if (*stop == '\0') return(back);
	}

	return(number);
}

/*************************************************************************/
/** xstrtoo():																				**/
/*************************************************************************/

long xstrtoo(unsigned char *string, long number)
{
	long           back;
	unsigned char *stop;

	if (string)
	{
		back = strtol(string, &stop, 8);
		
		if (*stop == '\0') return(back);
	}

	return(number);
}
