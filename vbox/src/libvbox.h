/*
** $Id: libvbox.h,v 1.1 1997/02/25 17:40:47 michael Exp $
**
** Copyright (C) 1996, 1997 Michael 'Ghandi' Herold
*/

#ifndef _VBOX_LIBVBOX_H
#define _VBOX_LIBVBOX_H 1

/** Defines **************************************************************/

#define CTRL_NAME_STOP			"vboxctrl-stop"
#define CTRL_NAME_ANSWERNOW	"vboxctrl-answernow"
#define CTRL_NAME_ANSWERALL	"vboxctrl-answerall"
#define CTRL_NAME_REJECT		"vboxctrl-reject"

#define VAH_MAGIC					"VBOX"							/* Magic id	*/
#define VAH_MAX_MAGIC			(4)
#define VAH_MAX_NAME				(32)
#define VAH_MAX_CALLERID		(32)
#define VAH_MAX_PHONE			(32)
#define VAH_MAX_LOCATION		(64)

#ifdef TRUE
#undef TRUE
#endif

#ifdef FALSE
#undef FALSE
#endif

#define TRUE					(1)
#define FALSE					(0)

#define returnerror()		return(FALSE)
#define returnok()			return(TRUE)
#define printstring			sprintf

/** Structures ***********************************************************/

typedef struct
{
	char					magic[4];
	unsigned long int	time;
	unsigned long int	compression;
	char					callerid[VAH_MAX_CALLERID + 1];
	char					name[VAH_MAX_NAME + 1];
	char					phone[VAH_MAX_PHONE + 1];
	char					location[VAH_MAX_LOCATION + 1];
} vaheader_t;

/** Variables ************************************************************/

extern char *compressions[];

/** Prototypes ***********************************************************/

extern int ctrl_create(char *, char *);
extern int ctrl_remove(char *, char *);
extern int ctrl_ishere(char *, char *);

extern void xstrncpy(char *, char *, int);
extern void xstrncat(char *, char *, int);
extern void xpause(unsigned long);
extern long xstrtol(char *, long);

extern int header_put(int, vaheader_t *);
extern int header_get(int, vaheader_t *);

#endif /* _VBOX_LIBVBOX_H */
