/*
** $Id: audio.h,v 1.2 1998/08/31 10:43:01 michael Exp $
**
** Copyright 1996-1998 Michael 'Ghandi' Herold <michael@abadonna.mayn.de>
*/

#ifndef _VBOX_AUDIO_H
#define _VBOX_AUDIO_H 1

/** Prototypes ***********************************************************/

extern int audio_open_dev(unsigned char *);
extern int audio_close_dev(int);

#endif /* _VBOX_AUDIO_H */
