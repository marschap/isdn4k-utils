/*
** $Id: voice.h,v 1.1 1998/06/17 16:38:56 michael Exp $
**
** Copyright 1997-1998 by Michael 'Ghandi' Herold
*/

#ifndef _VBOX_VOICE_H
#define _VBOX_VOICE_H 1

#define VBOXVOICE_BUFFER_SIZE	32

#define VBOXVOICE_STAT_OK			0
#define VBOXVOICE_STAT_TIMEOUT	1
#define VBOXVOICE_STAT_HANGUP		2
#define VBOXVOICE_STAT_TOUCHTONE	4

#define ETX  (0x03)
#define NL   (0x0A)
#define CR   (0x0D)
#define DLE  (0x10)
#define XON  (0x11)
#define XOFF (0x13)
#define DC4  (0x14)
#define CAN  (0x18)


extern int voice_init(void);
extern int voice_save(int);
extern int voice_hear(int);
extern int voice_wait(int);

#endif /* _VBOX_VOICE_H */
