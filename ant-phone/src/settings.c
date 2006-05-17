/*
 * functions for dotfile handling (options, history, ...)
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
 * To add a new option, do the following:
 *    -> set default in session_init()
 *    -> add entry in settings_option_set()
 *    -> add entry in settings_options_write()
 *    -> of course, set it in gtksettings (also set_data()!) / whatever
 *    -> get it back in gtksettings_try()
 */

/* regular GNU system includes */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <limits.h>

/* own header files */
#include "globals.h"
#include "session.h"
#include "settings.h"
#include "calleridparser.h"
#include "util.h"
#include "callerid.h"

#include "isdn.h"
#include "isdnlexer.h"
#include "isdnparser.h"
#include "isdntree.h"

extern FILE *callerid_in;
int callerid_parse(void *session);
extern FILE *isdn_in;
int isdn_parse();

/*
 * sets an option for the specified session
 *
 * used as callback from yyparse()
 */
void settings_option_set(session_t *session, char *option, char *value) {
  int i_value = INT_MIN; /* set value to 0 or 1 if appropriate */

  if (!strcmp(value, "0") || !strcasecmp(value, "false") ||
      !strcasecmp(value, "off")) {
    i_value = 0;
  } else
  if (!strcmp(value, "1") || !strcasecmp(value, "true") ||
      !strcasecmp(value, "on")) {
    i_value = 1;
  } else
    i_value = strtol(value, NULL, 0);

  /* Settings */
  if (i_value != INT_MIN) {
    if (!strcmp(option, "HistorySize")) {
      session->dial_number_history_maxlen = i_value;
    }
    if (!strcmp(option, "CallerIDSize")) {
      session->cid_num_max = i_value;
    }

    if (!strcmp(option, "SaveOptions")) {
      session->option_save_options = (i_value == 0 ? 0 : 1);
    }

    if (!strcmp(option, "ExecOnIncoming")) {
      free(session->exec_on_incoming);
      session->exec_on_incoming = strdup(value);
    }
    if (!strcmp(option, "PopupOnIncoming")) {
      session->option_popup = (i_value == 0 ? 0 : 1);
    }

    if (!strcmp(option, "RecordingFormat")) {
      if (!strcasecmp(value, "aiff")) {
	session->option_recording_format =
	  (session->option_recording_format & ~RECORDING_FORMAT_MAJOR)
	  | RECORDING_FORMAT_AIFF;
      } else { /* wav */
	session->option_recording_format =
	  (session->option_recording_format & ~RECORDING_FORMAT_MAJOR)
	  | RECORDING_FORMAT_WAV;
      }
    }
    if (!strcmp(option, "RecordingEncoding")) {
      if (!strcasecmp(value, "s16")) {
	session->option_recording_format =
	  (session->option_recording_format & ~RECORDING_FORMAT_MINOR)
	  | RECORDING_FORMAT_S16;
      } else { /* ulaw */
	session->option_recording_format =
	  (session->option_recording_format & ~RECORDING_FORMAT_MINOR)
	  | RECORDING_FORMAT_ULAW;
      }
    }

    if (!strncmp(option, "PresetName", 10)) {
      int num = strtol(&option[10], NULL, 0);
      if (0 <= num && num < SESSION_PRESET_SIZE) {
        free(session->preset_names[num]);
	session->preset_names[num] = strdup(value);
      }
    }
    if (!strncmp(option, "PresetNumber", 12)) {
      int num = strtol(&option[12], NULL, 0);
      if (0 <= num && num < SESSION_PRESET_SIZE) {
        free(session->preset_numbers[num]);
	session->preset_numbers[num] = strdup(value);
      }
    }

    if (!strcmp(option, "MergeIsdnlog")) {
      session->option_calls_merge = (i_value == 0 ? 0 : 1);
    }
    if (!strcmp(option, "MergeIsdnlogDays")) {
      session->option_calls_merge_max_days = i_value;
    }

    if (!strcmp(option, "AudioDeviceIn") &&
	!strcmp(session->audio_device_name_in, "")) { /* may be overridden */
      free(session->audio_device_name_in);
      session->audio_device_name_in = strdup(value);
    }
    if (!strcmp(option, "AudioDeviceOut") &&
	!strcmp(session->audio_device_name_out, "")) { /* may be overridden */
      free(session->audio_device_name_out);
      session->audio_device_name_out = strdup(value);
    }
    if (!strcmp(option, "ReleaseAudioDevices")) {
      session->option_release_devices = (i_value == 0 ? 0 : 1);
    }
    if (!strcmp(option, "IdentifyingMSN") &&
	!strcmp(session->msn, "")) { /* may be overridden */
      free(session->msn);
      session->msn = strdup(value);
    }
    if (!strcmp(option, "ListenOnMSNs") &&
	!strcmp(session->msns, "")) { /* may be overridden */
      free(session->msns);
      session->msns = strdup(value);
    }

    /* GUI state */
    if (!strcmp(option, "ShowCallerID")) {
      session->option_show_callerid = (i_value == 0 ? 0 : 1);
    }
    if (!strcmp(option, "ShowLLCheckers")) {
      session->option_show_llcheck = (i_value == 0 ? 0 : 1);
    }
    if (!strcmp(option, "ShowControlPad")) {
      session->option_show_controlpad = (i_value == 0 ? 0 : 1);
    }
    if (!strcmp(option, "Muted")) {
      session->option_muted = (i_value == 0 ? 0 : 1);
    }
    if (!strcmp(option, "RecordToFile")) {
      session->option_record = (i_value == 0 ? 0 : 1);
    }
    if (!strcmp(option, "RecordLocalChannel")) {
      session->option_record_local = (i_value == 0 ? 0 : 1);
    }
    if (!strcmp(option, "RecordRemoteChannel")) {
      session->option_record_remote = (i_value == 0 ? 0 : 1);
    }

  }
}

/*
 * Read options from options file and isdn4linux config
 *
 * Here, we parse the config file and _change_ options. The defaults
 * will be all set at init time
 */
void settings_options_read(session_t *session) {
  char *homedir;
  char *filename;

  /* read isdn4linux config */
  isdn_lexer_init(ISDN_CONFIG_FILENAME);
  if (!isdn_in) {
    fprintf(stderr, "Warning: Couldn't read ISDN config file.\n");
  } else {
    isdn_tree_node_t* node;
    
    isdn_parse();
    isdn_lexer_deinit();
    if (debug > 1) {
      printf("ISDN config file (%s) contents:\n", ISDN_CONFIG_FILENAME);
      isdn_tree_dump();
    }
    /* get calls file name if possible */
    node = isdn_tree;
    while (node != NULL) {
      if (node->type == ISDN_NODE_TYPE_SECTION &&
	  !strcmp(node->name, "ISDNLOG"))
      {
	isdn_tree_node_t* entry = node->content.section;

	while (entry != NULL ) {
	  if (entry->type == ISDN_NODE_TYPE_ENTRY &&
	      !strcmp(entry->name, "LOGFILE"))
	  {
	    if (isdn_calls_filename_from_config)
	      free(isdn_calls_filename_from_config);
	    isdn_calls_filename_from_config = strdup(entry->content.value);
	  }
	  entry = entry->next;
	}
      }
      node = node->next;
    }
    
    isdn_tree_free();
  }

  if (!(homedir = get_homedir())) {
    fprintf(stderr, "Warning: Couldn't get home dir.\n");
    return;
  }

  /* dotfile */
  if (asprintf(&filename, "%s/." PACKAGE "/%s",
	       homedir, SETTINGS_OPTIONS_FILENAME) < 0)
  {
    fprintf(stderr,
	    "Warning: Couldn't allocate memory for options filename.\n");
    return;
  }

  isdn_lexer_init(filename);
  if (!isdn_in) {
    fprintf(stderr, "Warning: No options file available.\n");
  } else {
    isdn_tree_node_t* node;

    isdn_parse();
    isdn_lexer_deinit();
    node = isdn_tree;
    if (debug > 1) {
      printf("Options file (%s) contents:\n", filename);
      isdn_tree_dump();
    }
    /* actually set options */
    while (node != NULL) {
      switch (node->type) {
        case ISDN_NODE_TYPE_ENTRY:
	  if (debug)
	    printf("Setting \"%s\" to \"%s\"...\n",
		   node->name, node->content.value);
	  settings_option_set(session, node->name, node->content.value);
	  break;
        case ISDN_NODE_TYPE_SECTION:
	  fprintf(stderr, "Warning: Unexpected section \"%s\".\n", node->name);
	  break;
        case ISDN_NODE_TYPE_SUBSECTION:
	  fprintf(stderr,
	          "Warning: Unexpected subsection \"%s\".\n", node->name);
          break;
        default:
	  fprintf(stderr, "Unknown ISDN_NODE_TYPE\n");
      }
    
      node = node->next;
    }

    isdn_tree_free();
  }
  free(filename);
}

/*
 * write options to options file (in dotfile directory)
 */
void settings_options_write(session_t *session) {
  int i;
  char *homedir;
  char *filename;
  FILE *f;

  if (!(homedir = get_homedir())) {
    fprintf(stderr, "Warning: Couldn't get home dir.\n");
    return;
  }

  if (touch_dotdir())
    return;

  if (asprintf(&filename, "%s/." PACKAGE "/%s",
	       homedir, SETTINGS_OPTIONS_FILENAME) < 0) {
    fprintf(stderr,
	    "Warning: Couldn't allocate memory for options filename.\n");
    return;
  }

  if ((f = fopen(filename, "w"))) {
    fprintf(f, "# " PACKAGE " options file \n\n");

    fprintf(f, "#\n# Number of dialed numbers to remember\n#\n");
    fprintf(f, "HistorySize = %u\n\n", session->dial_number_history_maxlen);

    fprintf(f, "#\n# Maximum number of rows Caller ID window\n#\n");
    fprintf(f, "CallerIDSize = %u\n\n", session->cid_num_max);

    fprintf(f, "#\n# Automatically save options on exit?\n#\n");
    fprintf(f, "SaveOptions = %d\n\n", session->option_save_options);

    fprintf(f, "#\n# Execute an arbitrary command on incoming call\n#\n");
    fprintf(f, "ExecOnIncoming = \"%s\"\n\n", session->exec_on_incoming);

    fprintf(f, "#\n# When activated, the main window will be pushed to "
	       "foreground\n# on incoming call\n#\n");
    fprintf(f, "PopupOnIncoming = \"%d\"\n\n", session->option_popup);

    fprintf(f, "#\n# Audio recording device\n#\n");
    fprintf(f, "AudioDeviceIn = %s\n\n", session->audio_device_name_in);

    fprintf(f, "#\n# Audio playback device, "
	    "may be the same as AudioDeviceIn\n#\n");
    fprintf(f, "AudioDeviceOut = %s\n\n", session->audio_device_name_out);

    fprintf(f, "#\n# Release audio devices in idle (\"Ready\") mode "
	    "(when not needed)\n#\n");
    fprintf(f, "ReleaseAudioDevices = %d\n\n",session->option_release_devices);

    fprintf(f, "#\n# MSN (Multiple Subscriber Number) to send to identify\n"
	    "# ourselves at called party\n#\n");
    fprintf(f, "IdentifyingMSN = %s\n\n", session->msn);

    fprintf(f, "#\n# These are the MSNs we want to listen on "
	    "(and accept calls)\n#\n");
    fprintf(f, "ListenOnMSNs = %s\n\n", session->msns);

    fprintf(f, "#\n# Show Caller ID frame in main window?\n#\n");
    fprintf(f, "ShowCallerID = %d\n\n", session->option_show_callerid);

    fprintf(f, "#\n# Show Line Level Checkers in main window?\n#\n");
    fprintf(f, "ShowLLCheckers = %d\n\n", session->option_show_llcheck);

    fprintf(f, "#\n# Show control pad (key pad etc.) in main window?\n#\n");
    fprintf(f, "ShowControlPad = %d\n\n", session->option_show_controlpad);

    fprintf(f, "#\n# Turn on to make the other party receive silence\n#\n");
    fprintf(f, "Muted = %d\n\n", session->option_muted);

    fprintf(f, "#\n# Record Audio stream to file?\n#\n");
    fprintf(f, "RecordToFile = %d\n\n", session->option_record);

    fprintf(f, "#\n# When RecordToFile is set, record local channel?\n#\n");
    fprintf(f, "RecordLocalChannel = %d\n\n", session->option_record_local);

    fprintf(f, "#\n# When RecordToFile is set, record remote channel?\n#\n");
    fprintf(f, "RecordRemoteChannel = %d\n\n", session->option_record_remote);

    fprintf(f, "#\n# Recording file format\n"
               "# (\"wav\" for Microsoft WAV / "
	         "\"aiff\" for Apple/SGI AIFF)\n#\n");
    fprintf(f, "RecordingFormat = \"%s\"\n\n",
	(session->option_recording_format & RECORDING_FORMAT_MAJOR) ==
	RECORDING_FORMAT_WAV ? "wav" : "aiff");
    fprintf(f, "#\n# Recording file encoding\n"
	       "# (\"ulaw\" for uLaw / \"s16\" for 16-bit signed)\n#\n");
    fprintf(f, "RecordingEncoding = \"%s\"\n\n",
	(session->option_recording_format & RECORDING_FORMAT_MINOR) ==
	RECORDING_FORMAT_S16 ? "s16" : "ulaw");

    fprintf(f, "#\n# Preset Names and Numbers\n#\n");
    for (i = 0; i < SESSION_PRESET_SIZE; i++) {
      fprintf(f, "PresetName%d = \"%s\"\n", i, session->preset_names[i]);
      fprintf(f, "PresetNumber%d = \"%s\"\n", i, session->preset_numbers[i]);
    }
    fprintf(f, "\n");

    fprintf(f,
      "#\n# Merge isdnlog callerid history to local history at startup?\n#\n");
    fprintf(f, "MergeIsdnlog = %d\n\n", session->option_calls_merge);
    fprintf(f, "#\n# Get data from this number of last days when retrieving "
	       "data from\n"
	       "# isdnlog at startup\n"
	       "# (set to 0 if you prefer an unlimited number of days)\n#\n");
    fprintf(f, "MergeIsdnlogDays = %d\n\n",
	       session->option_calls_merge_max_days);

    if (fclose(f) == EOF) {
      fprintf(stderr, "Warning: Couldn't close options file.\n");
    }
  } else if (debug) {
    fprintf(stderr, "Warning: Can't write to options file.\n");
  }
  free(filename);
}

/* Read history file and set dial combo box */
void settings_history_read(session_t *session) {
  char *homedir;
  char *filename;
  FILE *f;
  char *lineptr = NULL;
  size_t linesize = 0;
  ssize_t got;

  if (!(homedir = get_homedir())) {
    fprintf(stderr, "Warning: Couldn't get home dir.\n");
    return;
  }

  if (asprintf(&filename, "%s/." PACKAGE "/%s",
	       homedir, SETTINGS_HISTORY_FILENAME) < 0) {
    fprintf(stderr,
	    "Warning: Couldn't allocate memory for history filename.\n");
    return;
  }

  if (debug)
    fprintf(stdout, "Info: History Filename: %s.\n", filename);
  if ((f = fopen(filename, "r"))) {
    do {
      got = getline(&lineptr, &linesize, f);
      if (lineptr[got - 1] == '\n') {
	lineptr[got - 1] = '\0';
      }
      if (got > 0 && strlen(lineptr) > 0) {
	session_history_append(session, lineptr);
	if (debug)
	  fprintf(stdout, "Info: History Number: %s.\n", lineptr);
      }
    } while (got > 0);
    
    if (fclose(f) == EOF) {
      fprintf(stderr, "Warning: Couldn't close history file.\n");
    }
  } else if (debug) {
    fprintf(stderr, "Warning: No history file available.\n");
  }

  free(filename);
  
  if (lineptr) free(lineptr);
}

/*
 * helper function writing the specified line s to stream f
 * used as callback by settings_history_write
 */
static void settings_history_write_line(gpointer s, gpointer f) {
  if (strcmp(s, "")) {
    fprintf(f, "%s\n", (char*)s);
  }
}

/*
 * write out dial history file
 */
void settings_history_write(session_t *session) {
  char *homedir;
  char *filename;
  FILE *f;

  if (!(homedir = get_homedir())) {
    fprintf(stderr, "Warning: Couldn't get home dir.\n");
    return;
  }
  
  if (touch_dotdir())
    return;

  if (asprintf(&filename, "%s/." PACKAGE "/%s",
	       homedir, SETTINGS_HISTORY_FILENAME) < 0) {
    fprintf(stderr,
	    "Warning: Couldn't allocate memory for history filename.\n");
    return;
  }

  if ((f = fopen(filename, "w"))) {
    g_list_foreach(session->dial_number_history, 
		   settings_history_write_line, f);

    if (fclose(f) == EOF) {
      fprintf(stderr, "Warning: Couldn't close history file.\n");
    }
  } else if (debug) {
    fprintf(stderr, "Warning: Can't write to history file.\n");
  }

  free(filename);
}

/* Read callerid history file */
void settings_callerid_read(session_t *session) {
  char *homedir;
  char *filename;

  if (!(homedir = get_homedir())) {
    fprintf(stderr, "Warning: Couldn't get home dir.\n");
    return;
  }

  if (asprintf(&filename, "%s/." PACKAGE "/%s",
	       homedir, SETTINGS_CALLERID_HISTORY_FILENAME) < 0) {
    fprintf(stderr, "Warning: "
	    "Couldn't allocate memory for caller id history filename.\n");
    return;
  }

  if ((callerid_in = fopen(filename, "r"))) {
    callerid_parse(session);
    if (fclose(callerid_in) == EOF) {
      fprintf(stderr, "Warning: Couldn't close callerid history file.\n");
    }
  } else if (debug) {
    fprintf(stderr, "Warning: No caller id history file available.\n");
  }

  free(filename);
}

/*
 * write out callerid history file
 */
void settings_callerid_write(session_t *session) {
  char *homedir;
  char *filename;
  FILE *f;
  int akt_line;
  gchar *date;
  gchar *type;
  gchar *from;
  gchar *to;
  gchar *duration;

  if (!(homedir = get_homedir())) {
    fprintf(stderr, "Warning: Couldn't get home dir.\n");
    return;
  }

  if (touch_dotdir())
    return;

  if (asprintf(&filename, "%s/." PACKAGE "/%s",
	       homedir, SETTINGS_CALLERID_HISTORY_FILENAME) < 0) {
    fprintf(stderr, "Warning: "
	    "Couldn't allocate memory for callerid history filename.\n");
    return;
  }
  
  if ((f = fopen(filename, "w"))) {
    for (akt_line = 0; akt_line < session->cid_num; akt_line++)
    {
      gtk_clist_get_text(GTK_CLIST(session->cid_list), akt_line,
	                 CID_COL_TIME, &date);
      gtk_clist_get_pixtext(GTK_CLIST(session->cid_list), akt_line,
	                    CID_COL_TYPE, &type, NULL, NULL, NULL);
      gtk_clist_get_text(GTK_CLIST(session->cid_list), akt_line,
	                 CID_COL_FROM, &from);
      gtk_clist_get_text(GTK_CLIST(session->cid_list), akt_line,
	                 CID_COL_TO, &to);
      gtk_clist_get_text(GTK_CLIST(session->cid_list), akt_line,
	                 CID_COL_DURATION, &duration);
      
      fprintf (f, "%-19s|%-3s|%-20s|%-20s|%-10s\n",
	       date, type, from, to, duration);
      if (debug > 1)
      fprintf(stderr, "%-19s|%-3s|%-20s|%-20s|%-10s\n",
		date, type, from, to, duration);
      
    }

    if (fclose(f) == EOF) {
      fprintf(stderr, "Warning: Couldn't close callerid history file.\n");
    }
  } else if (debug) {
    fprintf(stderr, "Warning: Can't write to callerid history file.\n");
  }

  free(filename);

}

