/*
** $Id: voice.h,v 1.3 1998/08/28 13:06:19 michael Exp $
**
** Copyright 1996-1998 Michael 'Ghandi' Herold <michael@abadonna.mayn.de>
*/

#ifndef _VBOX_VOICE_H
#define _VBOX_VOICE_H 1

#ifdef HAVE_CONFIG_H
#	include "../config.h"
#endif

#if TIME_WITH_SYS_TIME
#   include <sys/time.h>
#   include <time.h>
#else
#   if HAVE_SYS_TIME_H
#      include <sys/time.h>
#   else
#      include <time.h>
#   endif
#endif

#include "userrc.h"
#include "vboxrc.h"

/** Defines **************************************************************/

#define VBOXVOICE_BUFEXP   5
#define VBOXVOICE_NUMFRAGS 4
#define VBOXVOICE_FRAGFACT 5
#define VBOXVOICE_BUFSIZE  ((1 << VBOXVOICE_BUFEXP) * VBOXVOICE_FRAGFACT)

#define VBOXVOICE_STAT_OK			0
#define VBOXVOICE_STAT_TIMEOUT	1
#define VBOXVOICE_STAT_HANGUP		2
#define VBOXVOICE_STAT_TOUCHTONE	4
#define VBOXVOICE_STAT_SUSPEND	8

#define ETX  (0x03)
#define NL   (0x0A)
#define CR   (0x0D)
#define DLE  (0x10)
#define XON  (0x11)
#define XOFF (0x13)
#define DC4  (0x14)
#define CAN  (0x18)

#define VBOXSAVE_NAME				64
#define VBOXSAVE_ID					64

/** Structures ***********************************************************/

struct vboxsave
{
	time_t	time;
	char		name[VBOXSAVE_NAME + 1];
	char		id[VBOXSAVE_ID + 1];
};

/** Prototypes ***********************************************************/

extern int voice_init(struct vboxuser *, struct vboxcall *);
extern int voice_save(int);
extern int voice_hear(int);
extern int voice_wait(int);

#endif /* _VBOX_VOICE_H */
