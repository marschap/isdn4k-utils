/*
** $Id: modem.h,v 1.2 1998/06/17 12:20:37 michael Exp $
**
** Copyright 1997-1998 by Michael 'Ghandi' Herold
*/

#ifndef _VBOX_MODEM_H
#define _VBOX_MODEM_H 1

#include "libvboxmodem.h"

#define VBOXMODEM_STAT_INIT		0
#define VBOXMODEM_STAT_WAIT		1
#define VBOXMODEM_STAT_RING		2
#define VBOXMODEM_STAT_TEST		3
#define VBOXMODEM_STAT_EXIT		255





struct modemsetup
{
	int echotimeout;
	int commandtimeout;
	int ringtimeout;
	int alivetimeout;
	int toggle_dtr_time;




};





extern void modem_set_timeout(int);
extern int	modem_get_timeout(void);
extern int	modem_get_sequence(struct vboxmodem *, char *);
extern void modem_flush(struct vboxmodem *, int);
extern int	modem_command(struct vboxmodem *, char *, char *);
extern int	modem_hangup(struct vboxmodem *);
extern int	modem_wait(struct vboxmodem *);
extern void modem_set_nocarrier(struct vboxmodem *, int);
extern int	modem_get_nocarrier(struct vboxmodem *);
extern int	modem_read(struct vboxmodem *, char *, int);

#endif /* _VBOX_MODEM_H */
