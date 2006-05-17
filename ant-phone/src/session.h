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

#ifndef _ANT_SESSION_H
#define _ANT_SESSION_H

#include "config.h"

/* regular GNU system includes */
#ifdef HAVE_TERMIOS_H
  #include <termios.h>
#endif
#include <time.h>

/* GTK */
#include <gtk/gtk.h>

/* own header files */
#include "recording.h"

#define SESSION_PRESET_SIZE 4

enum state_t {
  STATE_READY,          /* completely idle */
  STATE_RINGING,        /* somebody's calling  */
  STATE_RINGING_QUIET,  /* same as above, audio off (device blocked) */
  STATE_DIALING,        /* we are dialing out */
  STATE_CONVERSATION,   /* we are talking */
  STATE_SERVICE,        /* special mode (llcheck) */
  STATE_PLAYBACK,       /* sound playback, usually recorded conversation */

  STATE_NUMBER          /* dummy to calculate size */
};

enum effect_t {
  EFFECT_NONE,     /* nothing is played currently */
  EFFECT_RING,     /* somebody's calling */
  EFFECT_RINGING,  /* waiting for the other end to pick up the phone */
  EFFECT_TEST,     /* play test sound (e.g. line level check) */
  EFFECT_TOUCHTONE,/* play a touchtone */
  EFFECT_SOUNDFILE /* play sound from file */
};

/*
 * Data needed for setting the session state (the state is the index)
 */
struct state_data_t {
  char *status_bar;
  char *pick_up_label;
  int pick_up_state;
  char *hang_up_label;
  int hang_up_state;
};

struct state_data_t state_data[STATE_NUMBER];

/*
 * session data
 */
typedef struct {
  /* audio device data */
  char *audio_device_name_in; /* allocated memory! */
  char *audio_device_name_out; /* allocated memory! */
  int audio_fd_in;        /* audio device file descriptors */
  int audio_fd_out;
  unsigned int audio_speed_in;     /* sound device recording speed */
  unsigned int audio_speed_out;    /* sound device playback speed */
  int fragment_size_in;   /* sound device buffer fragment sizes in bytes */
  int fragment_size_out;
  int *format_priorities; /* 0-terminated sorted list of preferenced formats */
  int audio_format_in;    /* the actual formats */
  int audio_format_out;
  int audio_sample_size_in; /* number of bytes of a sample */
  int audio_sample_size_out;
  unsigned char *audio_inbuf;  /* buffer: should have size fragment_size_in */
  unsigned char *audio_outbuf; /* buffer: should have size fragment_size_out */
  int audio_outbuf_index;   /* needed for conversation mode output memory */

  /* isdn data */
  char* isdn_device_name; /* "/dev/ttyIxx", xx == 0 .. 63 */
  int isdn_fd;
  char* isdn_lockfile_name;
  int isdn_inbuf_size;
  int isdn_outbuf_size;
  unsigned char *isdn_inbuf;   /* 8 bit ulaw */
  unsigned char *isdn_outbuf;
  int isdn_inbuf_len;      /* index of '\0' in audio_inbuf in command mode,
			       if char at this index != 0 -> reading line */
  int isdn_outbuf_index;   /* needed for conversation mode output memory */
  int escape; /* escape mode/state RECEIVING isdn data */
  int no_input; /* after this many select calls without isdn input
                   get back from blockmode */
  struct termios isdn_backup; /* saved state to restore on exit */

  char *from; /* caller's number */
  char *to;   /* callee's number */

  /* mediation data */
  /* Look-up-tables for audio <-> isdn conversion: */
  unsigned char *audio_LUT_in;  /* isdn -> audio */
  unsigned char *audio_LUT_out; /* audio -> isdn */
  unsigned char *audio_LUT_generate; /* 8 bit unsigned -> isdn */
  unsigned char *audio_LUT_analyze;  /* isdn -> 8 bit unsigned */
  short *audio_LUT_ulaw2short;  /* unsigned char (ulaw) -> short */
  double ratio_in;        /* ratio: audio output rate / isdn input rate */
  double ratio_out;       /* ratio: isdn output rate / audio input rate */
  unsigned int samples_in;     /* ring counter of samples: from isdn */
  unsigned int samples_out;    /* ...                ... : from sound device */

  /* recording data */
  short *rec_buf_local; /* pointers to recording buffers */
  short *rec_buf_remote;
  int rec_buf_local_index;  /* number of shorts in local recording buffer */
  int rec_buf_remote_index;  /* number of shorts in local recording buffer */
  int rec_buf_local_size;  /* recording buffer sizes */
  int rec_buf_remote_size;
  struct recorder_t *recorder; /* recorder internal data */

  /* GUI elements in this session (GTK specific) */
  GtkWidget *main_window;         /* the main window (with style ...) */
  GtkWidget *pick_up_button; /* the pick up button to enable / disable */
  GtkWidget *pick_up_label;  /* the label on the pick up button */
  GtkWidget *hang_up_button; /* the hang up button to enable / disable */
  GtkWidget *hang_up_label;  /* the label on the hang up button */
  GtkWidget *dial_number_box; /* the dial number combo box */
  GList *dial_number_history; /* the last called numbers */
  unsigned int dial_number_history_maxlen; /* how many numbers to remember */
  unsigned int dial_number_history_pointer; /* which one to use next if req */
  GtkWidget *status_bar; /* the status bar */
  gint phone_context_id; /* a context for the status bar */
  GtkWidget *audio_warning; /* inside status bar */
  gint audio_context_id; /* a context for audio_warning */

  GtkWidget *llcheck; /* line level check widget inside status bar */
  GtkWidget *llcheck_in; /* input level meter */
  GtkWidget *llcheck_out; /* output level meter */
  GtkWidget *llcheck_check_menu_item; /* state of line levels (status bar) */

  guint gtk_isdn_input_tag; /* these tags are saved to later remove handlers */
  guint gtk_audio_input_tag;
  
  GtkWidget *controlpad; /* key pad etc. */
  GtkWidget *controlpad_check_menu_item; /* display state of control pad */
  GtkWidget *mute_button; /* toggle button */
  GtkWidget *muted_warning; /* show in status bar if muted */
  gint muted_context_id; /* a context for the status bar */

  GtkWidget *record_checkbutton; /* recording checkbutton */
  GtkWidget *record_checkbutton_local; /* local recording checkbutton */
  GtkWidget *record_checkbutton_remote; /* remote recording checkbutton */
  
  /* caller id related */
  GtkWidget *cid; /* the caller id widget itself (to show/hide) */
  GtkWidget *cid_check_menu_item; /* to handle state of cid monitor (show?) */
  GtkWidget *cid_list; /* the list to hold the individual call data */
  GtkWidget *cid_scrolled_window; /* the home of the clist with adjustments */
  gint cid_num; /* number of rows in list */
  gint cid_num_max; /* maximum number of rows in list */
  time_t vcon_time; /* the start of conversation mode (for duration calc.) */
  time_t ring_time; /* the first sign of the conversation (dial/ring) */
  /* the symbols for the CList */
  GdkPixmap *symbol_in_pixmap;
  GdkBitmap *symbol_in_bitmap;
  GdkPixmap *symbol_out_pixmap;
  GdkBitmap *symbol_out_bitmap;
  GdkPixmap *symbol_record_pixmap;
  GdkBitmap *symbol_record_bitmap;

  GtkWidget *menuitem_settings; /* Menu items to select / deselect */
  GtkWidget *menuitem_line_check;

  /* ringing etc. */
  guint effect_tag; /* remove this callback after e.g. ringing */
  enum effect_t effect; /* which effect is currently been played? */
  unsigned int effect_pos; /* sample position in effect */
  char* effect_filename;   /* the file to playback */
  SNDFILE* effect_sndfile; /* the handle */
  SF_INFO effect_sfinfo;   /* info struct about effect_sndfile */
  short* effect_sndfile_buffer;/*temporary buffer to construct playback output*/
  int effect_sndfile_buffer_frames;/*number of frames of effect_sndfile_buffer*/
  time_t effect_playback_start_time; /* start time of playback */
  int touchtone_countdown_isdn;  /* number of samples yet to play */
  int touchtone_countdown_audio;
  int touchtone_index; /* which touchtone */

  /* phone specific */
  enum state_t state; /* which state we are currently in */
  int hangup; /* remote hangup */
  int aborted; /* i/o error (isdn or audio) */

  char* msn;  /* originating msn, allocated memory! */
  char* msns; /* comma-separated list of msns to listen on, allocated memory!*/

  int unanswered; /* unanswered calls for this session */

  /* some options (useful for options file handling) */
  int option_save_options;    /* save options on exit */
  int option_release_devices; /* close sound devices while not needed */
  int option_show_llcheck;    /* show line level checks in main window  */
  int option_show_callerid;   /* show callerid part in main window */
  int option_show_controlpad; /* show control pad (key pad etc.) */
  int option_muted;           /* mute microphone (other party gets zeros) */
  int option_record;          /* record to file */
  int option_record_local;    /* record local channel */
  int option_record_remote;   /* record remote channel */
  enum recording_format_t option_recording_format; /* recording file format */

  int option_calls_merge;     /* merge isdnlog */
  int option_calls_merge_max_days;

  char *exec_on_incoming; /* string with command to execute on incoming call */
  int option_popup; /* push main window to foreground on incoming call */

  char* preset_names[SESSION_PRESET_SIZE]; /* names and numbers for */
  char* preset_numbers[SESSION_PRESET_SIZE]; /*  preset buttons */

  int local_sock; /* unix domain socket for local server functionality */
  guint gtk_local_input_tag; /* gtk tag for gtk main loop select */
} session_t;

extern session_t session;

int session_set_state(session_t *session, enum state_t state);
void session_io_handlers_start(session_t *session);
void session_io_handlers_stop(session_t *session);
int session_reset_audio(session_t *session);
int session_audio_open(session_t *session);
int session_audio_close(session_t *session);
int session_audio_init(session_t *session);
int session_audio_deinit(session_t *session);

int session_init(session_t *session,
		 char *audio_device_name_in,
		 char *audio_device_name_out,
		 char *msn, char *msns);
void session_audio_free(session_t *session);
int session_deinit(session_t *session);

void session_effect_start(session_t *session, enum effect_t kind);
void session_effect_stop(session_t *session);

void gtk_handle_isdn_input(gpointer data, gint fd,
			   GdkInputCondition condition);
void gtk_handle_audio_input(gpointer data, gint fd,
			   GdkInputCondition condition);
void session_make_call(session_t *session, char *number);
void gtk_handle_pick_up_button(GtkWidget *widget, gpointer data);
void gtk_handle_hang_up_button(GtkWidget *widget, gpointer data);

void session_history_add(session_t *session, const char *number);
void session_history_append(session_t *session, char *number);
#endif /* session.h */
