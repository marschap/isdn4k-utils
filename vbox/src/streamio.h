/*
** $Id: streamio.h,v 1.1 1997/02/25 17:40:56 michael Exp $
**
** Copyright (C) 1996, 1997 Michael 'Ghandi' Herold
*/

#ifndef _STREAMIO_H
#define _STREAMIO_H 1

#include <time.h>
#include <stdio.h>

/** Structures ***********************************************************/

typedef struct
{
	int		fd;													/* Filedescriptor	*/
	char	  *name;						    /* Pointer to the name of the file	*/
	char	  *file;			 						 /* Pointer to the first line	*/
	char	  *next;								 	  /* Pointer to the next line	*/
	time_t	time;									  	 /* Last modification time	*/
	long		line;											 /* Current line number	*/
} streamio_t;

/** Prototypes ***********************************************************/

extern streamio_t	*streamio_open(char *);
extern streamio_t *streamio_reopen(streamio_t *);
extern void			 streamio_close(streamio_t *);
extern char			*streamio_gets(char *, int, streamio_t *);

#endif /* _STREAMIO_H */
