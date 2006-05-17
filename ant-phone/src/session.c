/*
 * definitions for runtime session specific data handling
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
 */

#include "config.h"

/* regular GNU system includes */
#include <string.h>
#include <stdio.h>
#ifdef HAVE_STDLIB_H
  #include <stdlib.h>
#endif
#ifdef HAVE_UNISTD_H
  #include <unistd.h>
#endif
#include <fcntl.h>
#include <math.h>
#include <time.h>

/* GTK */
#include <gtk/gtk.h>

/* libsndfile */
#include <sndfile.h>

/* own header files */
#include "globals.h"
#include "session.h"
#include "sound.h"
#include "isdn.h"
#include "mediation.h"
#include "gtk.h"
#include "util.h"
#include "callerid.h"
#include "llcheck.h"
#include "settings.h"
#include "fxgenerator.h"
#include "server.h"

/*
 * This is our session. Currently just one globally.
 */
session_t session;

struct state_data_t state_data[STATE_NUMBER] = {
{N_("Ready"),         N_("Dial"),   1,N_("Hang up"),0},/* STATE_READY         */
{N_("RING"),          N_("Answer"), 1,N_("Reject"), 1},/* STATE_RINGING       */
{N_("RING"),          N_("Answer"), 1,N_("Reject"), 1},/* STATE_RINGING_QUIET */
{N_("Dialing"),       N_("Pick up"),0,N_("Cancel"), 1},/* STATE_DIALING       */
{N_("B-Channel open"),N_("Pick up"),0,N_("Hang up"),1},/* STATE_CONVERSATION  */
{N_("Setup"),         N_("Pick up"),0,N_("Hang up"),0},/* STATE_SERVICE       */
{N_("Playback"),      N_("Pick up"),0,N_("Stop")   ,1} /* STATE_PLAYBACK      */
};

/*
 * Simply opens audio devices for specified session
 *
 * returns 0 on success, -1 on error
 */
int session_audio_open(session_t *session) {
  if (debug)
    fprintf(stderr, "session_audio_open: Opening audio device(s).\n");
  if (open_audio_devices(session->audio_device_name_in,
			 session->audio_device_name_out,
			 1,
			 session->format_priorities,
			 &session->audio_fd_in,
			 &session->audio_fd_out,
			 &session->fragment_size_in,
			 &session->fragment_size_out,
			 &session->audio_format_in,
			 &session->audio_format_out,
			 &session->audio_speed_in,
			 &session->audio_speed_out)) {
    return -1;
  }
  return 0;
}

/*
 * Closes audio devices for specified session
 *
 * returns 0 on success, -1 on error
 */
int session_audio_close(session_t *session) {
  if (debug)
    printf("session_audio_close: Closing audio device(s).\n");
  if (close_audio_devices(session->audio_fd_in, session->audio_fd_out)) {
    return -1;
  }
  return 0;
}

/*
 * init audio devices in session
 *
 * input: session->audio_device_name_in, session->audio_device_name_out
 *
 * returns 0 on success, -1 otherwise
 *
 * output: on success, session->audio_LUT* and session->audio_*buf are
 *         allocated (and set, if appropriate)
 */
int session_audio_init(session_t *session) {
  /* message: open audio device(s) */
  if (debug) {
    if (strcmp(session->audio_device_name_in, session->audio_device_name_out))
      {
	/* different devices */
	fprintf(stderr, "Initializing %s and %s...\n",
		session->audio_device_name_in, session->audio_device_name_out);
      } else {
	fprintf(stderr, "Initializing %s ...\n",
		session->audio_device_name_in);
      }
  }
  
  /* other options */
  session->audio_speed_in = ISDN_SPEED;  /* default audio speed */
  session->audio_speed_out = ISDN_SPEED;

  /* audio device buffer fragment sizes */
  session->fragment_size_in = DEFAULT_FRAGMENT_SIZE;
  session->fragment_size_out = DEFAULT_FRAGMENT_SIZE;

  session->format_priorities = default_audio_priorities;

  if (session_audio_open(session)) {
    fprintf(stderr, "Error initializing audio device(s).\n");
    return -1;
  }

  session->ratio_in = (double)session->audio_speed_out / ISDN_SPEED;
  session->ratio_out = (double)ISDN_SPEED / session->audio_speed_in;

  if (mediation_makeLUT(session->audio_format_out, &session->audio_LUT_in,
			session->audio_format_in, &session->audio_LUT_out,
			&session->audio_LUT_generate,
			&session->audio_LUT_analyze,
			&session->audio_LUT_ulaw2short)) {
    fprintf(stderr, "Error building conversion look-up-table.\n");
    return -1;
  }

  session->audio_sample_size_in =
    sample_size_from_format(session->audio_format_in);
  session->audio_sample_size_out =
    sample_size_from_format(session->audio_format_out);

  /* allocate buffers */
  if (!(session->audio_inbuf =
      (unsigned char *)malloc(session->fragment_size_in))) {
    return -1;
  }
  if (!(session->audio_outbuf =
      (unsigned char *)malloc(session->fragment_size_out))) {
    return -1;
  }

  return 0;
}

/*
 * init isdn in session
 *
 * input: session->msn, session->msns
 *
 * returns 0 on success, -1 otherwise
 */
int session_isdn_init(session_t *session) {
  /* open and init isdn device */
  if (debug)
    fprintf(stderr, "Initializing ISDN device...\n");
  if ((session->isdn_fd =
       open_isdn_device(&session->isdn_device_name,
			&session->isdn_lockfile_name)) < 0) {
    fprintf(stderr, "Error opening isdn device.\n");
    return -1;
  }
  if (init_isdn_device(session->isdn_fd, &session->isdn_backup)) {
    fprintf(stderr, "Error initializing isdn device.\n");
    return -1;
  }

  session->isdn_inbuf_size = session->isdn_outbuf_size = DEFAULT_ISDNBUF_SIZE;

  /* allocate buffers */
  if (!(session->isdn_inbuf =
      (unsigned char *)malloc(session->isdn_inbuf_size))) {
    return -1;
  }
  if (!(session->isdn_outbuf =
      (unsigned char *)malloc(session->isdn_outbuf_size))) {
    return -1;
  }
  
  if (isdn_setMSN(session->isdn_fd, session->msn) ||
      isdn_setMSNs(session->isdn_fd, session->msns)) {
    fprintf(stderr, "Error setting MSN properties.\n");
    return -1;
  }

  session->isdn_inbuf_len = 0;
  session->isdn_inbuf[0] = 1;

  return 0;
}

/*
 * init recording related things in session
 */
int session_recording_init(session_t *session) {
  /* mediation recording stuff */
  session->rec_buf_local_size = MEDIATION_RECBUFSIZE;
  session->rec_buf_remote_size = MEDIATION_RECBUFSIZE;
  session->rec_buf_local_index = 0;
  session->rec_buf_remote_index = 0;
  if (!(session->rec_buf_local =
    (short *) malloc(session->rec_buf_local_size * sizeof(short)))) {
    return -1;
  }
  if (!(session->rec_buf_remote =
    (short *) malloc(session->rec_buf_remote_size * sizeof(short)))) {
    return -1;
  }

  if (!(session->recorder =
	(struct recorder_t *)malloc(sizeof(struct recorder_t)))) {
    return -1;
  }

  return 0;
}

/*
 * deinit recording related things in session
 */
int session_recording_deinit(session_t *session) {
  free(session->rec_buf_local);
  free(session->rec_buf_remote);
  free(session->recorder);
  return 0;
}

/*
 * de-allocates memory used by audio buffers of session
 * -> prepares de-initialization
 * -> is part of session_audio_free()
 */
void session_audio_free(session_t *session) {
  free(session->audio_inbuf);
  free(session->audio_outbuf);

  free(session->audio_LUT_in);
  free(session->audio_LUT_out);
  free(session->audio_LUT_generate);
  free(session->audio_LUT_analyze);
  free(session->audio_LUT_ulaw2short);
}

/*
 * close audio device(s) and clean up (deallocate buffers)
 *
 * returns 0 on success, -1 otherwise
 */
int session_audio_deinit(session_t *session) {
  /* free allocated buffers */
  session_audio_free(session);
  
  /* close audio device(s) */
  if (debug)
    fprintf(stderr, "Closing sound device(s)...\n");
  if (session_audio_close(session)) {
    fprintf(stderr, "Error closing sound device(s).\n");
    return -1;
  }

  return 0;
}

/*
 * close isdn device and clean up (deallocate buffers)
 *
 * returns 0 on success, -1 otherwise
 */
int session_isdn_deinit(session_t *session) {
  /* free allocated buffers */
  free(session->isdn_inbuf);
  free(session->isdn_outbuf);
  
  if (debug)
    fprintf(stderr, "Closing ISDN device...\n");
  /* de-init / restore isdn device */
  if (deinit_isdn_device(session->isdn_fd, &session->isdn_backup)) {
    fprintf(stderr, "Error restoring ttyI state.\n");
  }
  /* close isdn device */
  if (close_isdn_device(session->isdn_fd, 
			session->isdn_device_name,
			session->isdn_lockfile_name)) {
    fprintf(stderr, "Error closing isdn device.\n");
  }
  
  return 0;
}

/*
 * initialize a session (isdn device, audio device) and read options file
 *
 * input:
 *   session: empty struct waiting for work
 *   audio_device_name_in, audio_device_name_out: name(s) of audio device(s)
 *   msn: msn to use
 *
 * NOTE: The latter 4 parameters are only the requested ones. When they are
 *       overridden by the options file, the resulting values will be different
 *
 * output: session: initialized session struct
 *
 * returns 0 on success, -1 otherwise
 */
int session_init(session_t *session,
		 char *audio_device_name_in,
		 char *audio_device_name_out,
		 char *msn, char *msns) {
  int i;
  
  /*
   * first: set all defaults possibly overridden by options file
  */

  session->dial_number_history = NULL;
  session->dial_number_history = g_list_append(session->dial_number_history,
  					       strdup(""));
  session->dial_number_history_maxlen = 10; /* config overrides this */
  session->cid_num_max = 100; /* 0 means no limit */

  /* options defaults */
  session->option_save_options = 1; /* save options automatically (on exit) */
  session->option_release_devices = 1;
  session->option_show_llcheck = 1;
  session->option_show_callerid = 1;
  session->option_show_controlpad = 1;
  session->option_muted = 0;
  session->option_record = 0;
  session->option_record_local = 1;
  session->option_record_remote = 1;
  session->option_recording_format =
    RECORDING_FORMAT_WAV | RECORDING_FORMAT_ULAW;
  session->option_popup = 1;

  session->option_calls_merge = 1;
  session->option_calls_merge_max_days = 10;

  session->exec_on_incoming = strdup("");

  for (i = 0; i < 4; i++) {
    asprintf(&session->preset_names[i], _("Preset %d"), i + 1);
    session->preset_numbers[i] = strdup("");
  }

  session->audio_device_name_in = strdup(audio_device_name_in);
  session->audio_device_name_out = strdup(audio_device_name_out);
  session->msn = strdup(msn);
  session->msns = strdup(msns);

  session->from = strdup("");
  session->to = strdup("");

  settings_options_read(session); /* override defaults analyzing options file */

  /* command line configurable parameters: set to hard coded defaults
     if no setting was made (either at command line or in options file) */
  if (!strcmp(session->audio_device_name_in, "")) {
    free(session->audio_device_name_in);
    session->audio_device_name_in = strdup(DEFAULT_AUDIO_DEVICE_NAME_IN);
  }
  if (!strcmp(session->audio_device_name_out, "")) {
    free(session->audio_device_name_out);
    session->audio_device_name_out = strdup(DEFAULT_AUDIO_DEVICE_NAME_OUT);
  }
  if (!strcmp(session->msn, "")) {
    free(session->msn);
    session->msn = strdup(DEFAULT_MSN);
  }
  if (!strcmp(session->msns, "")) {
    free(session->msns);
    session->msns = strdup(DEFAULT_MSNS);
  }

  /* other defaults */
  session->dial_number_history_pointer = 0;
  session->touchtone_countdown_isdn = 0;
  session->touchtone_countdown_audio = 0;
  session->touchtone_index = 0;

  session->ring_time = 0;
  session->unanswered = 0;

  /* setup audio and isdn */
  if (!session->option_release_devices)
    if (session_audio_init(session) < 0) return -1;
  if (session_isdn_init(session) < 0) return -1;
  if (session_recording_init(session) < 0) return -1;

  session->state = STATE_READY; /* initial state */

  session->effect = EFFECT_NONE;

  /* init server functionality */
  if (server_init(session) < 0) return -1;
  
  return 0;
}

/*
 * helper function to free single element data from g_list 
 * (used by session_deinit)
 */
void free_g_list_element(gpointer data, gpointer user_data _U_) {
  free(data);
}

/*
 * de-initialize a session (isdn device, audio device)
 *
 * input: session: session to de-initialize
 *
 * returns 0 on success, -1 otherwise
 */
int session_deinit(session_t *session) {
  int i;

  /* deinit server functionality */
  if (server_deinit(session) < 0) return -1;
  
  if (session->option_save_options) settings_options_write(session);

  /* free dial_number_history */
  g_list_foreach(session->dial_number_history, free_g_list_element, NULL);
  g_list_free(session->dial_number_history);

  /* close devices and clean up (buffers) */
  if (session_isdn_deinit(session) < 0) return -1;
  if (!session->option_release_devices)
    if (session_audio_deinit(session) < 0) return -1;

  if (session_recording_deinit(session) < 0) return -1;
  
  free(session->exec_on_incoming);

  /* clean up pre-set options */
  free(session->audio_device_name_in);
  free(session->audio_device_name_out);
  free(session->msn);
  free(session->msns);

  /* clean up rest */
  for (i = 0; i < 4; i++) {
    free(session->preset_names[i]);
    free(session->preset_numbers[i]);
  }
  free(session->from);
  free(session->to);

  return 0;
}

/*
 * set up handlers for audio/ISDN input
 */
void session_io_handlers_start(session_t *session) {
  if (!(session->option_release_devices &&
	(session->state == STATE_READY ||
	 session->state == STATE_RINGING_QUIET))) {
    session->gtk_audio_input_tag = gtk_input_add_full(session->audio_fd_in,
						      GDK_INPUT_READ,
						      gtk_handle_audio_input,
						      NULL,
						      (gpointer) session,
						      NULL);
  }
  session->gtk_isdn_input_tag = gtk_input_add_full(session->isdn_fd,
					           GDK_INPUT_READ,
					           gtk_handle_isdn_input,
						   NULL,
					           (gpointer) session,
						   NULL);

  /* server functionality */
  session->gtk_local_input_tag = gtk_input_add_full(session->local_sock,
					            GDK_INPUT_READ,
					            server_handle_local_input,
						    NULL,
					            (gpointer) session,
						    NULL);
}

/*
 * remove handlers for audio/ISDN input
 */
void session_io_handlers_stop(session_t *session) {
  if (!(session->option_release_devices &&
	(session->state == STATE_READY ||
	 session->state == STATE_RINGING_QUIET))) {
    gtk_input_remove(session->gtk_audio_input_tag);
  }
  gtk_input_remove(session->gtk_isdn_input_tag);
  gtk_input_remove(session->gtk_local_input_tag);
}

/*
 * Resets audio devices by closing and reopening
 *
 * assumes audio in open, initialized (possibly used) state
 *
 * WARNING: * Stop I/O handlers first!
 *          * Only resets currently used device(s). To use another device,
 *            use session_audio_deinit() and session_audio_init()
 *
 * returns 0 on success, -1 on error
 */
int session_reset_audio(session_t *session) {
  int result = 0;
  
  if (!(session->option_release_devices &&
	(session->state == STATE_READY ||
	 session->state == STATE_RINGING_QUIET))) {
    if (session_audio_close(session)) {
      fprintf(stderr, "Error closing audio device(s) while resetting.\n");
      result = -1;
    }
    if (session_audio_open(session)) {
      fprintf(stderr, "Error reopening audio device(s) while resetting.\n");
      result = -1;
    }
  }
  return result;
}

/*
 * Callback for new unhandled modem string
 *
 * This function will be called, whenever we got a new unhandled modem string.
 * That means that it's called unless modem answers were stolen by response
 * handlers in functions like isdn_setMSN or isdn_hangup where "OK\r\n" will
 * be checked for
 */
void session_new_modem_string_callback(session_t *session) {
  /* modem string is in session->isdn_inbuf */
  if (debug)
    /* new line is in isdn_inbuf */
    fprintf(stderr, "|%s", session->isdn_inbuf);
}

/*
 * Tries to read isdn device in command mode, returns immediately
 *   (non blocking). If we have a partially filled input buffer, continue
 *   with that
 *
 * input: session struct with open isdn_fd of ttyI in command mode
 *
 * output: command (or partial command) read in
 *           session->isdn_inbuf, session->isdn_inbuf_len
 * 
 * returns with 1 if we got a new line (else 0)
 *
 * NOTE: * completed lines are 0-terminated at session->isdn_inbuf_len,
 *         non-compleded lines are 1-terminated there
 *       * completed lines are actually terminated by "\r\n\0"
 */
static int isdn_try_read_line(session_t *session) {
  int total = 0; /* number of bytes read in this call */
  struct timeval tv;
  fd_set fds;
  int num; /* number of file descriptors with data (0/1) */

  tv.tv_sec = 0; /* return immediately */
  tv.tv_usec = 0;
  
  do {
    FD_ZERO(&fds);
    FD_SET(session->isdn_fd, &fds);
    
    num = select(FD_SETSIZE, &fds, 0, 0, &tv);
    if (num > 0) { /* got another char: append to buffer */
      
      if (session->isdn_inbuf[session->isdn_inbuf_len] == 0 ||
	  session->isdn_inbuf_len == session->isdn_inbuf_size - 1) {
	/* we have to start new line or buffer is full -> reset buffer */
	session->isdn_inbuf_len = 0;
	session->isdn_inbuf[0] = 1;
      }
      
      read(session->isdn_fd, &session->isdn_inbuf[session->isdn_inbuf_len], 1);
      total ++;
      session->isdn_inbuf[++session->isdn_inbuf_len] = 1;
      
      if (session->isdn_inbuf_len >= 2 &&
	  session->isdn_inbuf[session->isdn_inbuf_len - 2] == '\r' &&
	  session->isdn_inbuf[session->isdn_inbuf_len - 1] == '\n') {
	/* end of line */
	session->isdn_inbuf[session->isdn_inbuf_len] = 0;
      }
    }
  } while (num > 0 && session->isdn_inbuf[session->isdn_inbuf_len] != 0);
  if (session->isdn_inbuf[session->isdn_inbuf_len] == 0 && total > 0) {
    session_new_modem_string_callback(session);
    return 1;
  } else
    return 0;
}

/* do some initialization of full duplex conversation mode */
void session_init_conversation(session_t *session) {
  session->samples_in = 0;
  session->samples_out = 0;

  session->audio_outbuf_index = 0;
  session->isdn_outbuf_index = 0;
  
  session->escape = 0;
  
  session->hangup = 0;
  session->aborted = 0;
  
  session->no_input = 0;
}

/*
 * do some deinitialization of full duplex conversation mode
 *
 * return: self_hangup: 1 == we hangup, 0 == other side hung up
 */
void session_deinit_conversation(session_t *session, int self_hangup) {
  if (session->option_record) {
    recording_write(session->recorder, session->rec_buf_local,
                    session->rec_buf_local_index, RECORDING_LOCAL);
    recording_write(session->recorder, session->rec_buf_remote,
                    session->rec_buf_remote_index, RECORDING_REMOTE);
    recording_close(session->recorder);
  }
  session->rec_buf_local_index = 0;
  session->rec_buf_remote_index = 0;
  
  session_io_handlers_stop(session);
  session_reset_audio(session);
  session_io_handlers_start(session);
  
  if (isdn_blockmode(session->isdn_fd, 0))
    fprintf(stderr, "Warning: "
	    "Switching back to normal isdn tty mode not successful.\n");
  
  /* go back to command mode */
  if (isdn_stop_audio(session->isdn_fd, self_hangup)) {
    fprintf(stderr, "Error switching back to command mode.\n");
  }
  
  /* isdn hangup */
  if (isdn_hangup(session->isdn_fd)) {
    fprintf(stderr, "Error hanging up.\n");
  }

  session->isdn_inbuf_len = 0;
  session->isdn_inbuf[0] = 1;
}

/*
 * will be called directly after getting VCON from ttyI
 * includes state transition
 */
void session_start_conversation(session_t *session) {
  char *digits; /* time in digits form */
  
  if (isdn_set_full_duplex(session->isdn_fd)) {
    /* ISDN full duplex (REC start command) error */
    fprintf(stderr, "Error setting full duplex audio mode.\n");
    isdn_hangup(session->isdn_fd);
    session_set_state(session, STATE_READY);
    cid_set_duration(session, _("(HW ERROR)"));
  } else { /* full duplex ok: audio mode */
    session->vcon_time = time(NULL); /* for caller id monitor */
    cid_set_date(session, session->vcon_time);
    session_set_state(session, STATE_CONVERSATION);
    if (session->option_record) {
      if ((digits = util_digitstime(&session->vcon_time))) {
        if (recording_open(session->recorder, digits,
	                   session->option_recording_format)) {
          fprintf(stderr, "Error opening audio file.\n");
	  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(
				       session->record_checkbutton), FALSE);
	}
	free(digits);
	cid_row_mark_record(session, session->cid_num - 1);
      } else {
	fprintf(stderr, "Error generating audio filename.\n");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(
	                             session->record_checkbutton), FALSE);
      }
    }
    if (isdn_blockmode(session->isdn_fd, 1)) /* blockmode error */
      fprintf(stderr,
	      "Warning: Switching to ISDN blockmode not successful.\n");
    session_init_conversation(session); /* init some conversation variables */
    
    session_io_handlers_stop(session);
    session_reset_audio(session);
    session_io_handlers_start(session);

    /* start with first block to start recording */
    process_audio_source(session);
  }
}

/*
 * to be called by a timeout and everyone who wants to stop a running effect
 *   to go back to STATE_READY
 */
gint session_effect_stop_cb(gpointer data) {
  session_t *session = (session_t *) data;

  session_set_state(session, STATE_READY);
  
  return FALSE; /* don't call me regularly */
}

/*
 * To be called when we can write another block to sound device
 */
void session_effect_callback(gpointer data,
                             gint source _U_, GdkInputCondition condition _U_)
{
  session_t *session = (session_t *) data;
  int pos = 0; /* position in raw byte output buffer */
  int i;
  int index_out = 0; /* logical position (sample number) in output buffer */
  unsigned char x; /* generated ulaw sample */
  int just_read = 0;
  int sndfile_buffer_index = 0;
  double speed_factor = (double)session->effect_sndfile_buffer_frames /
    (session->fragment_size_out / session->audio_sample_size_out);
    /* factor from audio device to sndfile */

  if (session->effect == EFFECT_SOUNDFILE) {
    just_read = sf_readf_short(session->effect_sndfile,
	session->effect_sndfile_buffer, session->effect_sndfile_buffer_frames);
  }
  for (i = 0; i < session->effect_sfinfo.channels; i++)
    session->effect_sndfile_buffer[session->effect_sndfile_buffer_frames *
      session->effect_sfinfo.channels + i] =
      session->effect_sndfile_buffer[(session->effect_sndfile_buffer_frames-1) *
      session->effect_sfinfo.channels + i];
  
  /* fill buffer */
  while (pos < session->fragment_size_out) {
    if (session->effect == EFFECT_SOUNDFILE) {
      double sum = 0.0;
      double leftbound = speed_factor * index_out; /* sndfile bounds */
      double rightbound = speed_factor * (index_out + 1);
      double dummy;
      double frac = modf(leftbound, &dummy);
      
      while (leftbound < rightbound) {
        double frame_sum = 0;
	double weight;
	if (rightbound - leftbound < 1.0)
	  weight = rightbound - leftbound;
	else
	  weight = 1.0;
	  
        sndfile_buffer_index = (int)leftbound;
        if (sndfile_buffer_index < just_read) {
          int t_count;
          for (t_count=0; t_count < session->effect_sfinfo.channels; t_count++){
	    frame_sum += (1.0 - frac) * session->effect_sndfile_buffer[
	      sndfile_buffer_index * session->effect_sfinfo.channels + t_count]
	               + frac * session->effect_sndfile_buffer[
	      (sndfile_buffer_index+1)*session->effect_sfinfo.channels+t_count];
          }
          frame_sum /= session->effect_sfinfo.channels;
        } else {
	  frame_sum = 0;
        }
	sum += weight * frame_sum;
	leftbound += 1.0;
      }
      sum /= speed_factor;
      x = session->audio_LUT_generate[(int)sum / 256 + 128];
    } else {
      double seconds = (double)session->effect_pos / session->audio_speed_out;
      x = fxgenerate(session, session->effect, 0, seconds);
    }
    if (session->audio_sample_size_out == 1) {
      session->audio_outbuf[pos++] = session->audio_LUT_in[(int)x];
    } else { /* audio_sample_size == 2 */
      session->audio_outbuf[pos++] = session->audio_LUT_in[(int)x * 2];
      session->audio_outbuf[pos++] = session->audio_LUT_in[(int)x * 2 + 1];
    }
    index_out ++;
    session->effect_pos++;
  }
  
  /* play it! */
  write_buf(session->audio_fd_out,
	    session->audio_outbuf,
	    session->fragment_size_out);

  if (session->effect == EFFECT_SOUNDFILE &&
      just_read < session->effect_sndfile_buffer_frames &&
      session->effect_filename)
  {
    gtk_timeout_add(
      1000 * session->fragment_size_out / session->audio_sample_size_out *
          (audio_get_write_fragments_total(session->audio_fd_out) -
        audio_get_write_fragments_number(session->audio_fd_out)) /
      session->audio_speed_out,
      session_effect_stop_cb, session);
    /* session->effect_filename provides a flag if we already want to stop */
    free(session->effect_filename);
    session->effect_filename = NULL;
  }
}

/*
 * Start an effect (effect_t) playing on the sound device
 *
 * on kind == EFFECT_SOUNDFILE, session->effect_filename should be
 * initialized so that session_effect_stop can free() it afterwards
 */
void session_effect_start(session_t *session, enum effect_t kind) {
  if (kind == EFFECT_SOUNDFILE) {
    session->effect_sfinfo.format = 0;
    if (!(session->effect_sndfile =
      sf_open(session->effect_filename, SFM_READ, &session->effect_sfinfo)))
    {
      fprintf(stderr, "Error on opening sound file.\n");
    }
    session->effect_sndfile_buffer_frames =
      session->fragment_size_out / session->audio_sample_size_out *
      session->effect_sfinfo.samplerate / session->audio_speed_out;
    session->effect_sndfile_buffer = (short*) malloc (sizeof(short) *
      (session->effect_sndfile_buffer_frames + 1) * /* doubled dummy at end */
      session->effect_sfinfo.channels);

  }
  session->effect_tag = gtk_input_add_full(session->audio_fd_out,
				           GDK_INPUT_WRITE,
				           session_effect_callback,
					   NULL,
				           (gpointer) session,
					   NULL);

  session->effect = kind;
  session->effect_pos = 0;
}

/*
 * Reset sound device and unset callback
 */
void session_effect_stop(session_t *session) {
  if (session->effect != EFFECT_NONE) { /* stop only if already playing */
    if (session->effect == EFFECT_SOUNDFILE) {
      sf_close(session->effect_sndfile);
      free(session->effect_sndfile_buffer);
    }
    gtk_input_remove(session->effect_tag);
    session->effect = EFFECT_NONE;
  }
  session_io_handlers_stop(session);
  session_reset_audio(session);
  session_io_handlers_start(session);
}

/*
 * Sets status bar audio state (e.g. "AUDIO OFF")
 * hide if note is ""
 */
void session_audio_notify(session_t *session, char *note) {
  GtkWidget *dummy_label; /* needed to adjust size of sub-statusbar */
  GtkRequisition requisition;
  
  gtk_widget_hide(session->audio_warning);
  if (*note) {
    gtk_statusbar_pop(GTK_STATUSBAR(session->audio_warning),
		      session->audio_context_id);
    gtk_statusbar_push(GTK_STATUSBAR(session->audio_warning),
		       session->audio_context_id, note);
    
    dummy_label = gtk_label_new(note);
    gtk_widget_show(dummy_label);
    gtk_widget_size_request(dummy_label, &requisition);
    gtk_widget_set_size_request(session->audio_warning,
        requisition.width + 4, -1);

    gtk_widget_show(session->audio_warning);
  }
}

/*
 * Sets new state in session and GUI (also handles audio state)
 *
 * returns 0 on success, -1 otherwise (can't open audio device)
 */
int session_set_state(session_t *session, enum state_t state) {
  int result = 0;

  /* open / close audio when needed, set state */
  session_io_handlers_stop(session);
  if (session->option_release_devices && state != session->state) {
    if (state == STATE_READY && session->state != STATE_RINGING_QUIET) {
      /* release */
      session_audio_deinit(session);
    } else if ((session->state == STATE_READY ||
		session->state == STATE_RINGING_QUIET) &&
	       state != STATE_READY && state != STATE_RINGING_QUIET) {
      /* (try to) resume */
      if (session_audio_init(session)) {
	state = session->state;
	result = -1;
      }
    }
  }
  session->state = state;
  session_io_handlers_start(session);

  /* some menu items are selected only in STATE_READY */
  gtk_widget_set_sensitive(session->menuitem_settings, state == STATE_READY);
  gtk_widget_set_sensitive(session->menuitem_line_check, state == STATE_READY);

  /* start / stop effects when needed */
  switch (state) {
  case STATE_DIALING:
    if (session->effect == EFFECT_NONE)
      session_effect_start(session, EFFECT_RINGING);
    if (debug) fprintf(stderr, "New state: STATE_DIALING\n");
    break;
  case STATE_RINGING:
    if (session->option_popup) {
      gtk_window_present(GTK_WINDOW(session->main_window));
    }
    if (session->effect == EFFECT_NONE)
      session_effect_start(session, EFFECT_RING);
    if (debug) fprintf(stderr, "New state: STATE_RINGING\n");
    break;
  case STATE_RINGING_QUIET:
    if (session->option_popup) {
      gtk_window_present(GTK_WINDOW(session->main_window));
    }
    if (debug) fprintf(stderr, "New state: STATE_RINGING_QUIET\n");
    break;
  case STATE_READY:
    gtk_widget_grab_focus(GTK_WIDGET(GTK_COMBO(session->dial_number_box)
				     ->entry));
    if (session->effect != EFFECT_NONE)
      session_effect_stop(session);
    if (debug) fprintf(stderr, "New state: STATE_READY\n");
    break;
  case STATE_CONVERSATION:
    if (session->effect != EFFECT_NONE)
      session_effect_stop(session);
    session->touchtone_countdown_isdn = 0;
    session->touchtone_countdown_audio = 0;
    if (debug) fprintf(stderr, "New state: STATE_CONVERSATION\n");
    break;
  case STATE_SERVICE:
    if (debug) fprintf(stderr, "New state: STATE_SERVICE\n");
    break;
  case STATE_PLAYBACK:
    if (session->effect == EFFECT_NONE)
      session_effect_start(session, EFFECT_SOUNDFILE);
    if (debug) fprintf(stderr, "New state: STATE_PLAYBACK\n");
    break;
  default:
    fprintf(stderr, "Warning: session_set_state: Unhandled state.\n");
  }

  /* audio on / off notify */
  if (session->option_release_devices) {
    session_audio_notify(session,
			 state == STATE_READY || state == STATE_RINGING_QUIET ?
			 _("Audio OFF") : _("Audio ON"));
  } else {
    session_audio_notify(session, "");
  }

  /* status line */
  gtk_statusbar_pop(GTK_STATUSBAR(session->status_bar),
		    session->phone_context_id);
  gtk_statusbar_push(GTK_STATUSBAR(session->status_bar),
                     session->phone_context_id,
		     _(state_data[state].status_bar));

  gtk_label_set_text(GTK_LABEL(session->pick_up_label),
		     _(state_data[state].pick_up_label));
  gtk_widget_set_sensitive(session->pick_up_button,
			   state_data[state].pick_up_state);

  gtk_label_set_text(GTK_LABEL(session->hang_up_label),
		     _(state_data[state].hang_up_label));
  gtk_widget_set_sensitive(session->hang_up_button,
			   state_data[state].hang_up_state);

  if (state == STATE_READY) {
    llcheck_bar_reset(session->llcheck_in);
    llcheck_bar_reset(session->llcheck_out);
  }

  return result;
}

/*
 * Callback: reinitialize isdn input watchdog
 */
static gboolean gtk_isdn_input_defer_timeout(session_t* session) {
  session->gtk_isdn_input_tag = gtk_input_add_full(session->isdn_fd,
					           GDK_INPUT_READ,
					           gtk_handle_isdn_input,
						   NULL,
					           (gpointer) session,
						   NULL);
  return FALSE; /* don't call me regularly */
}

/* defer 300 milliseconds if appropriate */
#define DEFER_INTERVAL 300

/*
 * put a graceful delay into GTK main loop to prevent permanent isdn input
 * callback
 */
static void gtk_isdn_input_defer(session_t* session) {
  gtk_input_remove(session->gtk_isdn_input_tag);
  session->gtk_isdn_input_tag =
    gtk_timeout_add(DEFER_INTERVAL,
                    (GtkFunction) gtk_isdn_input_defer_timeout,
		    session);
}

/*
 * callback for gtk on isdn input
 *
 * input: data:      session (session_t *)
 *        fd:        file descriptor where we got the input from
 *        condition: will be GDK_INPUT_READ in this case
 */
void gtk_handle_isdn_input(gpointer data, gint fd _U_,
			   GdkInputCondition condition _U_) {
  session_t *session = (session_t *) data;
  char *temp;

  switch (session->state) {
  case STATE_READY: /* we are in command mode */
    if (isdn_try_read_line(session)){ /* got new line: something happened */
      if (!strncmp(session->isdn_inbuf, "RING/", 5)) { /* -> RINGING state */
	session->isdn_inbuf[session->isdn_inbuf_len - 2] = 0;
	/* caller id update */
	session->ring_time = time(NULL);

	/* save callee's number */
	free(session->to);
	session->to = strdup(&session->isdn_inbuf[5]);
	
	cid_add_line(session, CALL_IN, NULL, session->to);

	if (session_set_state(session, STATE_RINGING))
	  session_set_state(session, STATE_RINGING_QUIET);

      } else { /* something else */
	if (debug)
	  fprintf(stderr, "Unknown message from ISDN device.\n");
      }
    }
    break;
  case STATE_DIALING:
    if (isdn_try_read_line(session)){ /* a response to our dial request */
      if (strstr(session->isdn_inbuf, "BUSY\r\n")) { /* get back to READY  */
	session_set_state(session, STATE_READY);
	cid_set_duration(session, _("(BUSY)"));
      } else if (strstr(session->isdn_inbuf, "VCON\r\n")) { /* let's go! */
	session_start_conversation(session); /* including state transition */
      } else if (strstr(session->isdn_inbuf, "NO CARRIER\r\n")) {
	/* timeout? */
	session_set_state(session, STATE_READY);
	cid_set_duration(session, _("(TIMEOUT)"));
      } else { /* got some other modem answer string while dialing out */
	if (debug)
	  fprintf(stderr, "Unknown message from ISDN device.\n");
      }
    }
    break;
  case STATE_RINGING:
  case STATE_RINGING_QUIET:
    if (isdn_try_read_line(session)){ /* got new line: something happened */
      if (strstr(session->isdn_inbuf, "VCON\r\n")) { /* let's go! */
	/* will only come in STATE_RINGING */
	session_start_conversation(session); /* including state transition */
      } else if (strstr(session->isdn_inbuf, "CALLER NUMBER: ")) {
	/* got Caller ID */
	session->isdn_inbuf[session->isdn_inbuf_len - 2] = 0;
	
	/* save caller's number */
	free(session->from);
	session->from = strdup(&session->isdn_inbuf[15]);

	/* complete from field */
	cid_set_from(session, session->from);

	/* execute command if given */
	if (session->exec_on_incoming) {
	  temp = strdup(session->exec_on_incoming);
	  substitute(&temp, "%n", session->to);
	  substitute(&temp, "%s", session->from);
	  execute(temp);
	  free(temp);
	}
	
      } else if (strstr(session->isdn_inbuf, "RUNG\r\n")) {
	/* caller giving up */
	session_set_state(session, STATE_READY);
	cid_set_duration(session, _("(RUNG)"));
	cid_mark_row(session, session->cid_num - 1, TRUE);
      } else { /* got some other modem answer string while it rings */
	if (debug)
	  fprintf(stderr, "Unknown message from ISDN device.\n");
      }
    }
    break;
  case STATE_CONVERSATION:
    process_isdn_source(session);
    if (session->samples_in >= ISDN_SPEED)
      session->samples_in %= ISDN_SPEED;
    session->no_input = 0;

    if (session->aborted || session->hangup) { /* That's it! */
      
      if (session->hangup)
	session_deinit_conversation(session, 0); /* 0 == other side hung up */
      else
	session_deinit_conversation(session, 1); /* 1 == let's hang up (I/O error) */
      
      session_set_state(session, STATE_READY);
      cid_set_duration(session, NULL);
    }
    break;
  case STATE_SERVICE:
    if (debug)
      fprintf(stderr, "Note: Got ISDN input in service mode.\n");
    gtk_isdn_input_defer(session);
    break;
  case STATE_PLAYBACK:
    if (debug)
      fprintf(stderr, "Note: Got ISDN input in playback mode.\n");
    gtk_isdn_input_defer(session);
    break;
  default:
    fprintf(stderr,
	    "Warning: gtk_handle_isdn_input: Unknown session state.\n");
    gtk_isdn_input_defer(session);
  }
}

/*
 * callback for gtk on audio isdn input
 *
 * input: data:      session (session_t *)
 *        fd:        file descriptor where we got the input from
 *        condition: will be GDK_INPUT_READ in this case
 */
void gtk_handle_audio_input(gpointer data, gint fd _U_,
			    GdkInputCondition condition _U_) {
  session_t *session = (session_t *) data;

  switch (session->state) {
  case STATE_READY: /* we are in command mode */
    if (debug > 1)
      fprintf(stderr, "Warning: Got audio input in ready mode (ALSA?).\n");
    /* flush audio input */
    read(session->audio_fd_in,
	 session->audio_inbuf, session->fragment_size_in);
    break;
  case STATE_DIALING:
    if (debug > 1)
      fprintf(stderr, "Warning: Got audio input in dialing mode (ALSA?).\n");
    /* flush audio input */
    read(session->audio_fd_in,
	 session->audio_inbuf, session->fragment_size_in);
    break;
  case STATE_RINGING:
    if (debug > 1)
      fprintf(stderr, "Warning: Got audio input in ringing mode (ALSA?).\n");
    /* flush audio input */
    read(session->audio_fd_in,
	 session->audio_inbuf, session->fragment_size_in);
    break;
  case STATE_RINGING_QUIET:
    if (debug > 1)
      fprintf(stderr,
	      "Warning: Got audio input in QUIET ringing mode.\n");
    /* flush audio input */
    read(session->audio_fd_in,
	 session->audio_inbuf, session->fragment_size_in);
    break;
  case STATE_CONVERSATION:
    process_audio_source(session);
    if (session->samples_out >= session->audio_speed_in)
      session->samples_out %= session->audio_speed_in;
    session->no_input++;

    /* if no more input from isdn came, assume abort and switch back */
    if (session->no_input >= 10) {
      /* XXX: reasonable number? */
      if (isdn_blockmode(session->isdn_fd, 0))
	fprintf(stderr, "Error: Could not switching off isdn blockmode.\n");
      session->no_input = 0;
    }
    if (session->aborted) { /* That's it! */
      
      session_deinit_conversation(session, 1);
        /* 1 == let's hang up (I/O error) */
      
      session_set_state(session, STATE_READY);
      cid_set_duration(session, NULL);
    }
    break;
  case STATE_SERVICE:
    if (debug > 1)
      fprintf(stderr, "Warning: Got audio input in service mode (ALSA?).\n");
    /* flush audio input */
    read(session->audio_fd_in,
	 session->audio_inbuf, session->fragment_size_in);
    break;
  case STATE_PLAYBACK:
    if (debug > 1)
      fprintf(stderr, "Warning: Got audio input in playback mode (ALSA?).\n");
    /* flush audio input */
    read(session->audio_fd_in,
	 session->audio_inbuf, session->fragment_size_in);
    break;
  default:
    fprintf(stderr,
	    "Warning: gtk_handle_audio_input: Unknown session state.\n");
    /* flush audio input */
    read(session->audio_fd_in,
	 session->audio_inbuf, session->fragment_size_in);
  }
}

/*
 * initiates dialing to specified number
 * -> changes contents of dial entry and simulates pick up button
 */
void session_make_call(session_t *session, char *number) {
  gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(session->dial_number_box)->entry),
		     number);
  gtk_button_clicked(GTK_BUTTON(session->pick_up_button));
}

/*
 * callback for GTK on pick up button clicked
 *
 * input: widget: the button
 *        data:   will be a (session_t *)
 */
void gtk_handle_pick_up_button(GtkWidget *widget _U_, gpointer data) {
  session_t *session = (session_t *) data;
  char *s; /* the modem dial string */
  const char *number; /* the number to dial "inside" gtk (entry) */
  char *clear_number; /* number after un_vanity() */
  int result;
  
  switch (session->state) {
  case STATE_READY: /* we are in command mode and want to dial */
    number = gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(session->dial_number_box)
					  ->entry));
    /* replace letters with numbers ("Vanity" Numbers) */
    clear_number = un_vanity(strdup(number));
    if ((s = (char*) malloc(strlen(clear_number) + 5)) &&
	strcmp(clear_number, "")) {
      if (!session_set_state(session, STATE_DIALING)) {
	/* dial only if audio is on etc. */
	snprintf(s, strlen(clear_number) + 5, "ATD%s\n", clear_number);
	
	tty_clear(session->isdn_fd);
	result = tty_write(session->isdn_fd, s);
	free(s);
	if (result) {
	  fprintf(stderr, "Error dialing.\n");
	} else {

	  /* update dial combo box */
	  session_history_add(session, number);
	  
	  /* caller id update */
	  session->ring_time = time(NULL);
	  cid_add_line(session, CALL_OUT, session->msn, clear_number);
	  
	  /* save caller's and callee's number */
	  free(session->from);
	  session->from = strdup(session->msn);
	  free(session->to);
	  session->to = strdup(clear_number);
	}
      } else {
	show_audio_error_dialog();
      }
    }

    free(clear_number);
    break;

  case STATE_DIALING: /* already dialing! */
    break;
  case STATE_RINGING: /* we want to pick up the phone while it rings */
    tty_clear(session->isdn_fd);
    if (tty_write(session->isdn_fd, "ATA\n"))
      fprintf(stderr, "Error answering call.\n");
    break;
  case STATE_RINGING_QUIET:
    if (!session_set_state(session, STATE_RINGING)) {
      tty_clear(session->isdn_fd);
      if (tty_write(session->isdn_fd, "ATA\n"))
	fprintf(stderr, "Error answering call.\n");
    } else {
      show_audio_error_dialog();
    }
    break;
  case STATE_CONVERSATION: /* channel already working */
    fprintf(stderr,
	    "Non-sense warning: Pick up button pressed in conversation mode\n");
    break;
  case STATE_SERVICE:
    fprintf(stderr,
	    "Non-sense warning: Pick up button pressed in service mode\n");
    break;
  case STATE_PLAYBACK:
    fprintf(stderr,
	    "Non-sense warning: Pick up button pressed in playback mode\n");
    break;
  default:
    fprintf(stderr,
	    "Warning: gtk_handle_pick_up_button: Unknown session state.\n");
  }
}

/*
 * callback for GTK on hang up button clicked, !!! also called on exit !!!
 *
 * input: widget: the button, NULL when called directly (on exit)
 *        data:   will be a (session_t *)
 */
void gtk_handle_hang_up_button(GtkWidget *widget _U_, gpointer data) {
  session_t *session = (session_t *) data;

  switch (session->state) {
  case STATE_READY: /* we are already in command mode */
    break;
  case STATE_DIALING:/* abort dialing */
    tty_clear(session->isdn_fd);
    if (tty_write(session->isdn_fd, "ATH\n"))
      fprintf(stderr, "Error answering call.\n");
    session_set_state(session, STATE_READY);
    cid_set_duration(session, _("(ABORTED)"));
    break;
  case STATE_RINGING: /* reject call */
  case STATE_RINGING_QUIET: /* reject call */
    tty_clear(session->isdn_fd);
    if (tty_write(session->isdn_fd, "ATH\n"))
      fprintf(stderr, "Error answering call.\n");
    session_set_state(session, STATE_READY);
    cid_set_duration(session, _("(REJECTED)"));
    break;
  case STATE_CONVERSATION: /* hang up (while b-channel is open) */
    session_deinit_conversation(session, 1); /* 1 == we hang up ourselves ;) */
      
    session_set_state(session, STATE_READY);
    cid_set_duration(session, NULL);
    break;
  case STATE_SERVICE:
    fprintf(stderr,
	    "Non-sense warning: Hang up button pressed in service mode\n");
    break;
  case STATE_PLAYBACK:
    session_set_state(session, STATE_READY);
    break;
  default:
    fprintf(stderr,
	    "Warning: gtk_handle_hang_up_button: Unknown session state.\n");
  }
}

/*
 * cut session->dial_number_history to specified size
 * (session->dial_number_history_maxlen)
 * -> and redisplay in session->dial_number_box
 */
static void session_history_normalize(session_t *session) {
  /* cut size if needed */
  while (g_list_length(session->dial_number_history) >
	 session->dial_number_history_maxlen + 1) {
    free(g_list_nth_data(session->dial_number_history,
			 g_list_length(session->dial_number_history) - 1));
    session->dial_number_history = g_list_remove_link(
	    session->dial_number_history,
	    g_list_last(session->dial_number_history));
  }
  gtk_combo_set_popdown_strings(GTK_COMBO(session->dial_number_box),
				session->dial_number_history);
}

/*
 * Add line to history of dial number combo box at start (first row)
 * and care about maximum size of history
 *
 * number will be copied, so caller has to care about it's associated memory
 */
void session_history_add(session_t *session, const char *number) {
  char *temp = strdup(number);

  session->dial_number_history = g_list_insert(
				       session->dial_number_history, temp, 1);
  session_history_normalize(session);
}

/*
 * like session_history_add but _appending_ number to list
 */
void session_history_append(session_t *session, char *number) {
  char *temp = strdup(number);

  session->dial_number_history = g_list_append(
					  session->dial_number_history, temp);
  session_history_normalize(session);
}

