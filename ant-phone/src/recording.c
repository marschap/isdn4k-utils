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

#include "config.h"

/* regular GNU system includes */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* sndfile audio file reading/writing library */
#include <sndfile.h>

/* own header files */
#include "globals.h"
#include "isdn.h"
#include "recording.h"
#include "util.h"

/* recorder internal buffer size (number of items, 1 item = 2 shorts): */
#define RECORDING_BUFSIZE 16384

/*
 * Carefully opens a file and prepares recorder
 * * when the file exists, new data will be appended
 *
 * File format: 
 *
 * input:
 *   recorder: struct to be filled with recorder state for recording session
 *             until recording_close()
 *   filename: the base file name. It will be expanded
 *             with full path and extension
 *
 * returns: 0 on success, -1 otherwise
 */
int recording_open(struct recorder_t *recorder, char *filename,
                   enum recording_format_t format) {
  SF_INFO sfinfo;
  char *homedir;
  char *fn;

  /* prepare path and file */
  touch_dotdir();
  
  if (!(homedir = get_homedir())) {
    fprintf(stderr, "Warning: Couldn't get home dir.\n");
    return -1;
  }
  if (asprintf(&fn, "%s/." PACKAGE "/recordings", homedir) < 0) {
    fprintf(stderr, "Warning: "
	    "recording_open: Couldn't allocate memory for directory name.\n");
    return -1;
  }
  if (touch_dir(fn) < 0) {
    if (debug)
      fprintf(stderr,
	      "Warning: recording_open: Can't reach directory %s.\n", fn);
    return -1;
  }
  free(fn);

  if (asprintf(&fn, "%s/." PACKAGE "/recordings/%s.%s", homedir, filename, format & RECORDING_FORMAT_WAV ? "wav" : "aiff")
      < 0) {
    fprintf(stderr, "Warning: "
	    "recording_open: Couldn't allocate memory for directory name.\n");
    return -1;
  }
 
  if (access(fn, F_OK)) { /* file doesn't exist */
    sfinfo.format = 
      (format & RECORDING_FORMAT_WAV ? SF_FORMAT_WAV : SF_FORMAT_AIFF) |
      (format & RECORDING_FORMAT_S16 ? SF_FORMAT_PCM_16 : SF_FORMAT_ULAW);
    sfinfo.channels = 2;
    sfinfo.samplerate = ISDN_SPEED;
    if (!(recorder->sf = sf_open(fn, SFM_WRITE, &sfinfo))) {
      fprintf(stderr, "recording_open: sf_open (file creation) error.\n");
      return -1;
    }
  } else { /* file already exists */
    sfinfo.format = 0;
    if (!(recorder->sf = sf_open(fn, SFM_RDWR, &sfinfo))) {
      fprintf(stderr, "recording_open: sf_open (reopen) error.\n");
      return -1;
    }
    if (sf_seek(recorder->sf, 0, SEEK_END) == -1) {
      fprintf(stderr, "recording_open: sf_seek error.\n");
      return -1;
    }
  }
  recorder->filename = fn;
  if (!(recorder->queue =
	(struct recqueue_t *) malloc (sizeof(struct recqueue_t)))) {
    fprintf(stderr, "recording_open: recorder->queue malloc error.\n");
    return -1;
  }
  if (!(recorder->queue->buf =
	(short *) malloc (RECORDING_BUFSIZE * sizeof(short) * 2))) {
    fprintf(stderr, "recording_open: recorder->queue->buf malloc error.\n");
    return -1;
  }
  recorder->queue->next = NULL;
  recorder->current_local = recorder->current_remote = recorder->queue;
  recorder->localindex = 0;
  recorder->remoteindex = 0;
  return 0;
}

/*
 * writes specified number of shorts to file
 *
 * input:
 *   recorder: struct with sound file state
 *   buf, size: the buffer of shorts with its size (number of shorts)
 *   channel: RECORDING_LOCAL or RECORDING_REMOTE
 *
 * returns: 0 on success, -1 otherwise
 */
int recording_write(struct recorder_t *recorder, short *buf, int size,
		    enum recording_channel_t channel) {
  int i;
  struct recqueue_t **link_this; /* alias for current_local or current_remote */
  int *buf_index_this; /* alias for localindex or remoteindex */
  struct recqueue_t *temp;
  
  if (channel == RECORDING_LOCAL) {
    link_this = &recorder->current_local;
    buf_index_this = &recorder->localindex;
  } else {
    link_this = &recorder->current_remote;
    buf_index_this = &recorder->remoteindex;
  }

  for (i = 0; i < size; i++) {
    (*link_this)->buf[(*buf_index_this)++ * 2 + channel] = buf[i];
    if (*buf_index_this >= RECORDING_BUFSIZE) { /* one buffer full */
      if ((*link_this)->next == NULL) { /* expand list */
	if (!((*link_this)->next =
	      (struct recqueue_t *) malloc (sizeof(struct recqueue_t)))) {
	  fprintf(stderr, "recording_write: buffer allocation error.\n");
	  return -1;
	}
	if (!((*link_this)->next->buf =
	      (short *) malloc (RECORDING_BUFSIZE * sizeof(short) * 2))) {
	  fprintf(stderr, "recording_write: buffer allocation error.\n");
	  return -1;
	}
	(*link_this)->next->next = NULL;
      }
      
      *link_this = (*link_this)->next; /* go further in list */
      *buf_index_this = 0;

      if (recorder->queue != recorder->current_local &&
	  recorder->queue != recorder->current_remote) {
	/* write out buffer */
	sf_writef_short(recorder->sf, recorder->queue->buf, RECORDING_BUFSIZE);
	
	temp = recorder->queue;
	recorder->queue = recorder->queue->next;
	free(temp->buf);
	free(temp);
      }
    }
  }
  return 0;
}

/*
 * finishes recording to file, writes remaining data from queue to file
 * (eventually filling a dangling channeln with silence)
 *
 * input:
 *   recorder: struct with sound file state
 *
 * returns: 0 on success, -1 otherwise
 */
int recording_close(struct recorder_t *recorder) {
  struct recqueue_t **link_this; /* aliases for the links and their buffer - */
  struct recqueue_t **link_other;
  int *buf_index_this;           /*   indices to traverse */
  int *buf_index_other;
  enum recording_channel_t channel;
  struct recqueue_t *temp;
  
  /* set last, unavailable samples to zero */
  if (recorder->queue == recorder->current_local) { /* traverse local samples */
    link_this = &recorder->current_local;
    link_other = &recorder->current_remote;
    buf_index_this = &recorder->localindex;
    buf_index_other = &recorder->remoteindex;
    channel = RECORDING_LOCAL;
  } else { /* traverse remote samples */
    link_this = &recorder->current_remote;
    link_other = &recorder->current_local;
    buf_index_this = &recorder->remoteindex;
    buf_index_other = &recorder->localindex;
    channel = RECORDING_REMOTE;
  }

  while (*link_this != NULL) { /* traverse all chain links */
    while ((*link_this != *link_other && *buf_index_this < RECORDING_BUFSIZE)
	   || *buf_index_this < *buf_index_other) { /* all remaining samples */
      (*link_this)->buf[(*buf_index_this)++ * 2 + channel] = 0;
    }
    sf_writef_short(recorder->sf, (*link_this)->buf, *buf_index_this);
    
    *buf_index_this = 0;
    temp = *link_this;
    *link_this = (*link_this)->next;
    free(temp->buf);
    free(temp);
  }
  
  if (!sf_close(recorder->sf)) return -1;
  free(recorder->filename);
  return 0;
}

