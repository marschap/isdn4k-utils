/*
** $Id: stringutils.c,v 1.2 1998/06/17 17:01:23 michael Exp $
**
** Copyright 1997-1998 by Michael Herold <michael@abadonna.mayn.de>
**
** $Log: stringutils.c,v $
** Revision 1.2  1998/06/17 17:01:23  michael
** - First part of the automake/autoconf implementation. Currently vbox will
**   *not* compile!
**
*/

#include <string.h>

#include "stringutils.h"

/*************************************************************************/
/** xstrncpy():	Copy one string to another (length limited).				**/
/*************************************************************************/
/** => dest			Destination															**/
/** => source		Source																**/
/** => max			Destination buffer length										**/
/*************************************************************************/

void xstrncpy(char *dest, char *source, int max)
{
	strncpy(dest, source, max);
   
	dest[max] = '\0';
}

/*************************************************************************/
/** xstrtol():	Converts a string to a number.									**/
/*************************************************************************/
/** => string	String to convert														**/
/** => number	Default number if string can not converted					**/
/** <=			Number																	**/
/*************************************************************************/
      
long xstrtol(char *string, long number)
{
	long  back;
	char *stop;

	if (string)
	{
		back = strtol(string, &stop, 10);
		
		if (*stop == '\0') return(back);
	}

	return(number);
}
