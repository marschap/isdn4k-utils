/*
** $Id: stringutils.h,v 1.2 1998/07/06 09:05:30 michael Exp $
**
** Copyright 1996-1998 Michael 'Ghandi' Herold <michael@abadonna.mayn.de>
*/

#ifndef _VBOX_STRINGUTILS_H
#define _VBOX_STRINGUTILS_H 1

/** Defines **************************************************************/

#define printstring	sprintf

/** Prototypes ***********************************************************/

extern void xstrncpy(char *, char *, int);
extern long xstrtol(char *, long);
extern long xstrtoo(char *, long);

#endif /* _VBOX_STRINGUTILS_H */
