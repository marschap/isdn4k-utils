/*
** $Id: libvboxmodem.h,v 1.1 1998/06/10 13:31:51 michael Exp $
**
** Copyright 1997-1998 by Michael 'Ghandi' Herold
*/

#ifndef _VBOX_LIBMODEM_H
#define _VBOX_LIBMODEM_H 1

#include <unistd.h>

/** Defines **************************************************************/

#define VBOXMODEM_BUFFER_SIZE	255				/* Modem input buffer size	*/

/** Variables ************************************************************/

typedef struct termios TIO;

struct vboxmodem
{
	int				fd;
	char			  *devicename;
	unsigned char *input;
	int				inputpos;
	int				inputlen;
	int				nocarrier;
	int				nocarrierpos;
	char			  *nocarriertxt;
};

/** Internal junk ********************************************************/

#define set_modem_error(A)		strcpy(lastmodemerrmsg, A)

/** Prototypes ***********************************************************/

extern int				 vboxmodem_open(struct vboxmodem *, unsigned char *);
extern void				 vboxmodem_close(struct vboxmodem *);
extern unsigned char *vboxmodem_error(void);
extern int				 vboxmodem_raw_read(struct vboxmodem *, unsigned char *, int);
extern size_t			 vboxmodem_raw_write(struct vboxmodem *, unsigned char *, int);
extern int				 vboxmodem_set_termio(struct vboxmodem *, TIO *);
extern int				 vboxmodem_get_termio(struct vboxmodem *, TIO *);

#endif /* _VBOX_LIBMODEM_H */
