/*
** $Id: log.h,v 1.1 1998/06/10 13:31:53 michael Exp $
**
** Copyright 1997-1998 by Michael 'Ghandi' Herold
*/

#ifndef _VBOX_LOG_H
#define _VBOX_LOG_H 1

/** Defines **************************************************************/

#define LOG_E	(0)														  /* Errors	*/
#define LOG_W	(1)														/* Warnings	*/
#define LOG_I	(2)												  /* Informations	*/
#define LOG_A	(4)														  /* Action	*/
#define LOG_D	(128)															/* Debug	*/
#define LOG_X	(255)													 /* Full debug	*/

/** Structures ***********************************************************/

struct logsequence
{
	char  code;
	char *text;
};

/** Prototypes ***********************************************************/

extern int	log_open(char *);
extern void	log_set_debuglevel(int);
extern void log_close(void);
extern void log_line(int, char *, ...);
extern void log_char(int, char);
extern void log_text(int, char *, ...);
extern void log_code(int, char *);

#endif /* _VBOX_LOG_H */
