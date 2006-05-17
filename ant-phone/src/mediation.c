/*
 * functions for mediation between ISDN and OSS
 *
 * This file is part of ANT (Ant is Not a Telephone)
 *
 * Copyright 2002, 2003 Roland Stigge
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
 *
 *
 * NOTE:
 *   * for performance reasons, separate recording buffers are filled while
 *     mediating
 */

#include "config.h"

/* regular GNU system includes */
#include <stdio.h>
#ifdef HAVE_STDLIB_H
  #include <stdlib.h>
#endif
#ifdef HAVE_UNISTD_H
  #include <unistd.h>
#endif
#include <errno.h>
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#include <math.h>

/* own header files */
#include "globals.h"
#include "session.h"
/* ulaw conversion (LUT) */
#include "g711.h"
#include "isdn.h"
#include "sound.h"
#include "util.h"
#include "mediation.h"
#include "llcheck.h"
#include "fxgenerator.h"
#include "recording.h"

/*
 * allocate memory and build look-up-tables for audio <-> isdn conversion
 *
 * input:
 *   format_in, LUT_in: used audio format and pointer to look-up-table
 *                      for conversion of isdn -> audio
 *   format_out, LUT_out: the same for audio -> isdn
 *   LUT_generate: table for conversion of 8 bit unsigned -> isdn
 *   LUT_analyze: table for conversion of isdn -> 8 bit unsigned
 *
 * return: 0 on success, -1 otherwise
 *
 * NOTE: the caller has to free the memory of LUT_* itself
 */
int mediation_makeLUT(int format_in, unsigned char **LUT_in,
		      int format_out, unsigned char **LUT_out,
		      unsigned char **LUT_generate,
		      unsigned char **LUT_analyze,
		      short **LUT_ulaw2short) {
  int sample_size_in;
  int sample_size_out;
  int buf_size_in;
  int buf_size_out;
  int sample;
  int i;
  short s;
  
  /* Allocation */
  sample_size_in = sample_size_from_format(format_in); /* isdn -> audio */
  if (sample_size_in == 0 ||
      !(*LUT_in = (unsigned char *)malloc(buf_size_in = sample_size_in * 256)))
    return -1;
  
  sample_size_out = sample_size_from_format(format_out); /* audio -> isdn */
  if (sample_size_out == 0 ||
      !(*LUT_out =
	(unsigned char *)malloc(buf_size_out =
				(1 + (sample_size_out - 1) * 255) * 256)))
    return -1;

  if (!(*LUT_generate = (unsigned char*) malloc (256)))
    return -1;
  if (!(*LUT_analyze = (unsigned char*) malloc (256)))
    return -1;
  if (!(*LUT_ulaw2short = (short*) malloc (256*sizeof(short))))
    return -1;
  
  /* Calculation */
  for (i = 0; i < buf_size_in; i += sample_size_in) { /* isdn -> audio */
    switch(format_in) {
    case AFMT_U8:
      (*LUT_in)[i] = (unsigned char)((ulaw2linear((unsigned char)i) / 256 &
				     0xff) ^ 0x80);
      break;

    case AFMT_S8:
      (*LUT_in)[i] = (unsigned char)(ulaw2linear((unsigned char)i) / 256 &
				     0xff);
      break;

    case AFMT_MU_LAW:
      (*LUT_in)[i] = (unsigned char)i;
      break;

    case AFMT_S16_LE:
      sample = ulaw2linear((unsigned char)(i / 2));
      (*LUT_in)[i] = (unsigned char)(sample & 0xff);
      (*LUT_in)[i+1] = (unsigned char)(sample >> 8 & 0xff);
      break;

    case AFMT_S16_BE:
      sample = ulaw2linear((unsigned char)(i / 2));
      (*LUT_in)[i+1] = (unsigned char)(sample & 0xff);
      (*LUT_in)[i] = (unsigned char)(sample >> 8 & 0xff);
      break;

    case AFMT_U16_LE:
      sample = ulaw2linear((unsigned char)(i / 2));
      (*LUT_in)[i] = (unsigned char)(sample & 0xff);
      (*LUT_in)[i+1] = (unsigned char)((sample >> 8 & 0xff) ^ 0x80);
      break;

    case AFMT_U16_BE:
      sample = ulaw2linear((unsigned char)(i / 2));
      (*LUT_in)[i+1] = (unsigned char)(sample & 0xff);
      (*LUT_in)[i] = (unsigned char)((sample >> 8 & 0xff) ^ 0x80);
      break;

    default:
      fprintf(stderr, 
	      "Error: "
	      "Unsupported format appeared while building input LUT.\n");
      return -1;
    }
  }
	
  for (i = 0; i < buf_size_out; i++) { /* audio -> isdn */
    switch(format_out) {
    case AFMT_U8:
      (*LUT_out)[i] = linear2ulaw((i - 128) * 256);
      break;

    case AFMT_S8:
      (*LUT_out)[i] = linear2ulaw(i * 256);
      break;

    case AFMT_MU_LAW:
      (*LUT_out)[i] = (unsigned char)i;
      break;

    /* next 4 cases:
       input int i stores first buffer byte in low byte */
    case AFMT_S16_LE:
      (*LUT_out)[i] = linear2ulaw((int)(signed char)(i >> 8) << 8 |
				  (int)(i & 0xff));
      break;
      
    case AFMT_S16_BE:
      (*LUT_out)[i] = linear2ulaw((int)(signed char)(i & 0xff) << 8 |
				  (int)(i >> 8));
      break;

    case AFMT_U16_LE:
      (*LUT_out)[i] = linear2ulaw(i - 32768);
      break;

    case AFMT_U16_BE:
      (*LUT_out)[i] = linear2ulaw(((i & 0xff) << 8 | i >> 8) - 32768);
      break;

    default:
      fprintf(stderr, 
	      "Error: "
	      "Unsupported format appeared while building output LUT.\n");
      return -1;
    }
  }

  for (i = 0; i < 256; i++) { /* 8 bit unsigned -> isdn -> 8 bit unsigned */
    (*LUT_generate)[i] = linear2ulaw((i - 128) * 256);
    (*LUT_ulaw2short)[i] = s = ulaw2linear((unsigned char)i); /* ulaw->short */
    (*LUT_analyze)[i] = (unsigned char)((s / 256 & 0xff) ^ 0x80);
  }

  return 0;
}

/*
 * writes buffer carefully out to file (ttyI / audio device)
 *
 * returns 0 on success, -1 otherwise (write error)
 */
int write_buf(int fd, unsigned char *outbuf, int outbuf_size) {
  int towrite = outbuf_size;
  int written = 0;

  /* write until everything has been written */
  while (towrite && (written != -1 || errno == EAGAIN)) {
    written = write(fd, &outbuf[outbuf_size - towrite], towrite);
    if (debug >= 2)
      fprintf(stderr, "Wrote %d bytes to device.\n", written);
    if (written != -1)
      towrite -= written;
    else
      if (errno == EAGAIN) {
	if (debug)
	  fprintf(stderr, "write_buf: EAGAIN\n");
	ant_sleep(SHORT_INTERVAL);
      }
  }
  if (written == -1) {
    perror("write_buf");
    return -1;
  }
  return 0;
}

/* XXX: smooth samples when converting speeds in next 2 functions */

/*
 * process isdn input from ttyI to sound device
 *
 * to be called after select found block to read in isdn file descriptor
 */
void process_isdn_source(session_t *session) {
  int got, i, j;
  unsigned char inbyte;  /* byte read from ttyI */
  int to_process; /* number of samples to process
                     (according to ratio / ratio_support_count) */
  unsigned char sample; /* 8 bit unsigned sample */
  int max = 0; /* for llcheck */

  short s; /* libsndfile sample data */
  
  got = read(session->isdn_fd, session->isdn_inbuf,
	     session->isdn_inbuf_size);

  if (debug >= 2)
    fprintf(stderr, "From isdn: got %d bytes.\n", got);

  if (got != -1) {
    for (i = 0; i < got; i++) {
      inbyte = session->isdn_inbuf[i];
      if (!session->escape == (inbyte != DLE)) {
	/* normal mode or last byte was an escape in DLE mode */
	
	/* input line level check */
	sample = session->audio_LUT_analyze[inbyte];
	if (abs((int)sample - 128) > max)
	  max = abs((int)sample - 128);

	/* recording */
	if (session->option_record) {
	  if (session->option_record_remote)
	    s = session->audio_LUT_ulaw2short[inbyte];
	  else
	    s = 0;
	  session->rec_buf_remote[session->rec_buf_remote_index++] = s;
	  if (session->rec_buf_remote_index >= session->rec_buf_remote_size) {
	    if (recording_write(session->recorder, session->rec_buf_remote,
	                        session->rec_buf_remote_size, RECORDING_REMOTE))
	      fprintf(stderr, "Warning: Recording (remote) error.\n");
	    session->rec_buf_remote_index = 0;
	  }
	}

	/* touchtone to audio: after llcheck to monitor other end */
	if (session->touchtone_countdown_audio > 0) {
	  inbyte = fxgenerate(session, EFFECT_TOUCHTONE,
			      session->touchtone_index,
			      (double)session->touchtone_countdown_audio /
			      ISDN_SPEED); /* playing reverse is ok */
	  session->touchtone_countdown_audio--;
	}

	/* mediation */
	to_process = (int)floor((double)(session->samples_in + 1) *
				session->ratio_in) -
	             (int)floor((double)session->samples_in *
				session->ratio_in);
	/* printf("isdn -> audio: to_process == %d\n", to_process); */
	for (j = 0; j < to_process; j++) {
	  if (session->audio_sample_size_out == 1) {
	    session->audio_outbuf[session->audio_outbuf_index++] =
	      session->audio_LUT_in[(int)inbyte];
	  } else { /* audio_sample_size == 2 */
	    session->audio_outbuf[session->audio_outbuf_index++] =
	      session->audio_LUT_in[(int)inbyte * 2];
	    session->audio_outbuf[session->audio_outbuf_index++] =
	      session->audio_LUT_in[(int)inbyte * 2 + 1];
	  }
	  if (session->audio_outbuf_index >= session->fragment_size_out) {
	    if (write_buf(session->audio_fd_out, session->audio_outbuf,
			  session->fragment_size_out))
	      session->aborted = 1;
	    session->audio_outbuf_index = 0;
	  }
	}

	session->samples_in++;

	if (session->escape) {
	  session->escape = 0;
	  if (debug) fprintf(stderr, "debug: escape mode off after 2x DLE.\n");
	}
      } else if (!session->escape && inbyte == DLE) { /* new escape: DLE */
	session->escape = 1;
	if (debug)
	  fprintf(stderr, "debug: ttyI DLE escape mode on.\n");
      } else /* i.e. if (*escape) */ {
	if (inbyte == DC4 || inbyte == ETX) {
	  session->hangup = 1;
	} else {/* else: must be a touchtone: ignored */
	  if (debug) {
	    if ((inbyte >= '0' && inbyte <= '9') || inbyte == '#' ||
		inbyte =='*' || (inbyte >= 'A' && inbyte <= 'D'))
	      fprintf(stderr, "Touchtone %c received.\n", inbyte);
	    else
	      fprintf(stderr, "Warning: Unknown escape sequence received.\n");
	  }
	}
	
	session->escape = 0;
	if (debug) fprintf(stderr,
			   "debug: escape mode off after special char.\n");
      }
    }

    llcheck_bar_set(session->llcheck_in, (double)max / 128);

  } else {
    fprintf(stderr, "process_isdn_source: read error (return -1).\n");
  }
}

/*
 * process audio input from sound device to isdn tty
 *
 * to be called after select found fragment(s) to read from
 */
void process_audio_source(session_t *session) {
  int i, j, got, n;
  unsigned char sample; /* the ulaw sample */
  short s = 0; /* libsndfile sample data */
  /* the ulaw sample when muted: */
  unsigned char zero = session->audio_LUT_generate[128];
  int to_process; /* number of samples to process
                     (according to ratio / ratio_support_count) */
  unsigned char sampleu8; /* 8 bit unsigned sample */
  int max = 0; /* for llcheck */

  got = read(session->audio_fd_in, session->audio_inbuf,
	     session->fragment_size_in);
  
  if (debug >= 2)
    fprintf(stderr, "From audio: got %d bytes.\n", got);

  if (got != -1) {
    for (i = 0; i < got;
	 i += session->audio_sample_size_in, session->samples_out++) {
      
      to_process = (int)floor((double)(session->samples_out + 1)
			      * session->ratio_out) -
	           (int)floor((double)session->samples_out
			      * session->ratio_out);
      /* printf("audio -> isdn: to_process == %d\n", to_process); */
      for (j = 0; j < to_process; j++) {
	if (session->audio_sample_size_in == 1) {
	  sample = session->audio_LUT_out[(int)(session->audio_inbuf[i])];
	} else { /* audio_sample_size == 2 */
	  /* multiple byte samples are used "little endian" in int
	     to look up in LUT (see mediation_makeLUT) */
	  sample = session->audio_LUT_out[(int)(session->audio_inbuf[i]) |
					  (int)(session->audio_inbuf[i+1])
					  << 8];
	}
	
	/* touchtone to isdn: before llcheck to monitor it */
	if (session->touchtone_countdown_isdn > 0) {
	  sample = fxgenerate(session, EFFECT_TOUCHTONE,
			      session->touchtone_index,
			      (double)session->touchtone_countdown_isdn /
			      ISDN_SPEED /* playing reverse is ok */ );
	  session->touchtone_countdown_isdn--;
	}

	if (session->option_muted) /* zero if muted */
	  sample = zero;

	/* input line level check */
	sampleu8 = session->audio_LUT_analyze[sample];
	if (abs((int)sampleu8 - 128) > max)
	  max = abs((int)sampleu8 - 128);
	
	/* recording */
	if (session->option_record) {
	  if (session->option_record_local)
	    s = session->audio_LUT_ulaw2short[sample];
	  else
	    s = 0;
	  session->rec_buf_local[session->rec_buf_local_index++] = s;
	  if (session->rec_buf_local_index >= session->rec_buf_local_size) {
	    if (recording_write(session->recorder, session->rec_buf_local,
	                        session->rec_buf_local_size, RECORDING_LOCAL))
	      fprintf(stderr, "Warning: Recording (local) error.\n");
	    session->rec_buf_local_index = 0;
	  }
	}

	n = (sample == DLE) ? 2 : 1; /* again if DLE escape */
	while (n > 0) {
	  session->isdn_outbuf[session->isdn_outbuf_index++] = sample;
	  if (session->isdn_outbuf_index >= session->isdn_outbuf_size) {
	    /* write outbuf out */
	    if (write_buf(session->isdn_fd, session->isdn_outbuf,
			  session->isdn_outbuf_size))
	      session->aborted = 1;
	    session->isdn_outbuf_index = 0;
	  }
	  n--;
	}
      }
    }

    llcheck_bar_set(session->llcheck_out, (double)max / 128);

  } else {
    switch (errno) {
    case EAGAIN:
      if (debug)
	fprintf(stderr,
		"process_audio_source: "
		"EAGAIN - no data immediately available (that's ok).\n");
      break;
    case EBADF:
      fprintf(stderr,
	      "process_audio_source: EBADF - invalid file descriptor.\n");
      break;
    case EINTR:
      fprintf(stderr,
	      "process_audio_source: EINTR - interrupted by signal.\n");
      break;
    case EIO:
      fprintf(stderr,
	      "process_audio_source: EIO - hardware error.\n");
      break;
    }
  }
}
