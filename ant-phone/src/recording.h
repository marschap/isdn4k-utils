/*
 * recording functions
 *
 * This file is part of ANT (Ant is Not a Telephone)
 *
 * Copyright 2003 Roland Stigge
 *
 * ANT is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * ANT is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ANT; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef _ANT_RECORDING_H
#define _ANT_RECORDING_H

#include "config.h"

/* regular GNU system includes */
#include <stdio.h>

/* sndfile audio file reading/writing library */
#include <sndfile.h>

enum recording_format_t {
  RECORDING_FORMAT_WAV = 0x10,
  RECORDING_FORMAT_AIFF = 0x20,
  
  RECORDING_FORMAT_ULAW = 0x01,
  RECORDING_FORMAT_S16 = 0x02,

  RECORDING_FORMAT_MAJOR = 0xF0,
  RECORDING_FORMAT_MINOR = 0x0F
};

enum recording_channel_t {
  RECORDING_LOCAL = 0,
  RECORDING_REMOTE = 1
};

struct recqueue_t {
  struct recqueue_t *next;
  short *buf;
};

struct recorder_t {
  SNDFILE *sf;      /* sndfile state */
  char *filename;   /* audio file name */
  struct recqueue_t *queue; /* buffer queue for communication with libsndfile */
  struct recqueue_t *current_local; /* current queue element for local input */
  struct recqueue_t *current_remote;/* current queue element for remote input */
  int localindex;  /* indices into current block in queue */
  int remoteindex;
};

int recording_open(struct recorder_t *recorder, char *filename,
                   enum recording_format_t format);
int recording_write(struct recorder_t *recorder, short *buf, int size,
		    enum recording_channel_t channel);
int recording_close(struct recorder_t *recorder);

#endif /* recording.h */
