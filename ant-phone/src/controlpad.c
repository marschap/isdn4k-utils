/*
 * The key pad
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

/* regular GNU system includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* GTK */
#include <gtk/gtk.h>

/* own header files */
#include "globals.h"
#include "session.h"
#include "isdn.h"
#include "util.h"
#include "callerid.h"

/* graphical symbols */
#include "backspace.xpm"
#include "redial.xpm"
#include "mute.xpm"


/*
 * called on preset edit confirmation (OK button)
 * -> sets new preset values (name, number) and destroys window
 *
 * input: widget = the OK button, set data:
 *                    "entry_name" - the entry with the new preset name
 *                    "entry_number" - the entry with the new preset number
 *                    "button" - the preset button
 *                    "number" - the preset button number
 *                    "window" - the window to destroy after setting values
 *        data = the session
 */
static void controlpad_preset_edit_ok(GtkWidget *widget, gpointer *data) {
  session_t *session = (session_t *) data;
  GtkWidget *entry_name = gtk_object_get_data(GTK_OBJECT(widget),
                                                      "entry_name");
  GtkWidget *entry_number = gtk_object_get_data(GTK_OBJECT(widget),
                                                        "entry_number");
  GtkWidget *button = gtk_object_get_data(GTK_OBJECT(widget), "button");
  int n = GPOINTER_TO_INT(gtk_object_get_data(GTK_OBJECT(widget), "number"));

  free(session->preset_names[n]);
  session->preset_names[n] = strdup(gtk_entry_get_text(GTK_ENTRY(entry_name)));
  free(session->preset_numbers[n]);
  session->preset_numbers[n] = strdup(
    gtk_entry_get_text(GTK_ENTRY(entry_number)));
  gtk_label_set_text(GTK_LABEL(GTK_BIN(button)->child),
                               gtk_entry_get_text(GTK_ENTRY(entry_name)));

  gtk_widget_destroy(gtk_object_get_data(GTK_OBJECT(widget), "window"));
}

/*
 * called on right-click on preset button to configure
 *
 * input: widget = the preset button
 *        event = the event (we determine the clicked mouse button from it)
 *        data = the session
 */
static gint controlpad_preset_edit_cb(GtkWidget *widget, GdkEventButton *event,
                                      gpointer data) {
  if (event->button == 3) {
    session_t *session = (session_t *) data;
    char *buttoncode = (char *)gtk_object_get_data(GTK_OBJECT(widget), "desc");
    int n = strtol(&buttoncode[1], NULL, 0);
    GtkWidget *window;
    GtkWidget *button_box;
    GtkWidget *button;
    GtkWidget *label;
    GtkWidget *table;
    GtkWidget *entry_name;
    GtkWidget *entry_number;
    char *title;
    char *labeltext;
  
    window = gtk_dialog_new();
    asprintf(&title, _("Preset %c"), buttoncode[1] + 1);
    gtk_window_set_title(GTK_WINDOW(window), title);
    gtk_window_set_modal(GTK_WINDOW(window), TRUE);

    /* vbox area */
    gtk_container_set_border_width(GTK_CONTAINER(GTK_DIALOG(window)->vbox), 0);
    asprintf(&labeltext,
	_("Please input new preset data for button %c:"), buttoncode[1] + 1);
    label = gtk_label_new(labeltext);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(window)->vbox), label,
		     TRUE, FALSE, 0);
    gtk_misc_set_padding(GTK_MISC(label), 10, 10);
    gtk_widget_show(label);

    table = gtk_table_new(2, 2, FALSE);
    gtk_container_set_border_width(GTK_CONTAINER(table), 10);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(window)->vbox), table,
	               TRUE, FALSE, 0);
    gtk_widget_show(table);

    label = gtk_label_new(_("Name:"));
    gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 1, 0, 1);
    gtk_widget_show(label);
    label = gtk_label_new(_("Number:"));
    gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 1, 1, 2);
    gtk_widget_show(label);
   
    entry_name = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(entry_name), session->preset_names[n]);
    gtk_table_attach_defaults(GTK_TABLE(table), entry_name, 1, 2, 0, 1);
    gtk_widget_show(entry_name);
    entry_number = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(entry_number), session->preset_numbers[n]);
    gtk_table_attach_defaults(GTK_TABLE(table), entry_number, 1, 2, 1, 2);
    gtk_widget_show(entry_number);

    /* action area */
    button_box = gtk_hbutton_box_new();
    gtk_container_add(GTK_CONTAINER(GTK_DIALOG(window)->action_area),
		      button_box);
    gtk_widget_show(button_box);

    /* OK button */
    button = gtk_button_new_with_label(_("OK"));
    gtk_box_pack_start_defaults(GTK_BOX(button_box), button);
    gtk_object_set_data(GTK_OBJECT(button), "entry_name", entry_name);
    gtk_object_set_data(GTK_OBJECT(button), "entry_number", entry_number);
    gtk_object_set_data(GTK_OBJECT(button), "button", widget);
    gtk_object_set_data(GTK_OBJECT(button), "number", GINT_TO_POINTER(n));
    gtk_object_set_data(GTK_OBJECT(button), "window", window);
    gtk_signal_connect(GTK_OBJECT(button), "clicked",
		       GTK_SIGNAL_FUNC(controlpad_preset_edit_ok),
		       session);
    gtk_widget_show(button);

    /* cancel button */
    button = gtk_button_new_with_label(_("Cancel"));
    gtk_box_pack_start_defaults(GTK_BOX(button_box), button);
    gtk_signal_connect_object(GTK_OBJECT(button), "clicked",
			      GTK_SIGNAL_FUNC(gtk_widget_destroy),
			      GTK_OBJECT(window));
    gtk_widget_show(button);
    gtk_widget_show(window);

    free(labeltext);
    free(title);
    return TRUE; /* event handled */
  } else {
    return FALSE;
  }
}

/*
 * called when button in key pad clicked (except mute)
 */
static void controlpad_button_cb(GtkWidget *button, gpointer data) {
  session_t *session = (session_t *) data;
  char *c = (char *) gtk_object_get_data(GTK_OBJECT(button), "desc");
  char *temp;

  switch (session->state) {
  case STATE_READY: /* manipulate dial box */
    if ((*c >= '0' && *c <= '9') || *c == '*' || *c == '#') { /* new char */
      gtk_entry_append_text(GTK_ENTRY(GTK_COMBO(session->dial_number_box)
				      ->entry), c);
    } else if (*c == 'P') { /* preset button */
      gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(session->dial_number_box)->entry),
	                 session->preset_numbers[strtol(&c[1], NULL, 0)]);
    } else if (*c == 'B') { /* clear one byte (Backspace) */
      temp = strdup(gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(
				       session->dial_number_box)->entry)));
      if (strlen(temp) > 0) {
	temp[strlen(temp) - 1] = 0;
	gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(session->dial_number_box)
				     ->entry), temp);
      }
      free(temp);
    } else if (*c == 'C') { /* clear entry */
      gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(session->dial_number_box)
				   ->entry), "");
    } else if (*c == 'R') { /* Redial from history */
      do {
	temp = g_list_nth_data(session->dial_number_history,
			       session->dial_number_history_pointer);
	gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(session->dial_number_box)
				     ->entry), temp);
	if (session->dial_number_history_pointer <
	    session->dial_number_history_maxlen)
	  session->dial_number_history_pointer++;
	if (session->dial_number_history_pointer ==
	    session->dial_number_history_maxlen)
	  session->dial_number_history_pointer = 0;
      } while (*temp == '\0' && session->dial_number_history_pointer != 0);
    }
    gtk_widget_grab_focus(GTK_WIDGET(GTK_COMBO(session->dial_number_box)
				     ->entry));
    gtk_entry_set_position(GTK_ENTRY(GTK_COMBO(session->dial_number_box)
                                          ->entry), -1);
    break;
  case STATE_CONVERSATION: /* touchtones */
    if ((*c >= '0' && *c <= '9') || *c == '*' || *c == '#') { /* new tt */
#define TOUCHTONE_LENGTH 0.1
      session->touchtone_countdown_isdn = TOUCHTONE_LENGTH * ISDN_SPEED;
      session->touchtone_countdown_audio =
	TOUCHTONE_LENGTH * session->audio_speed_out;
      session->touchtone_index = 
	*c == '*' ? 9 : *c == '0' ? 10 : *c == '#' ? 11 : *c - '1';
    }
    break;
  default: /* other states */
    ;
  }
}

/*
 * called when mute button in key pad toggled
 */
static void controlpad_mute_cb(GtkWidget *button, gpointer data) {
  session_t *session = (session_t *) data;

  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button))) { /* muted */
    session->option_muted = 1;
    gtk_widget_show(session->muted_warning);
  } else { /* unmuted */
    session->option_muted = 0;
    gtk_widget_hide(session->muted_warning);
  }
}

/*
 * called when record / local / remote checkbutton has been toggled
 */
static void controlpad_record_cb(GtkWidget *button, gpointer data) {
  session_t *session = (session_t *) data;
  char *digits = NULL;

  if (button == session->record_checkbutton) {
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button))) { /* record! */
      session->option_record = 1;
      if (session->state == STATE_CONVERSATION) {
	if (recording_open(session->recorder,
	      		   digits = util_digitstime(&session->vcon_time),
			   session->option_recording_format))
	{
          fprintf(stderr, "Error opening audio file.\n");
	  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(
				       session->record_checkbutton), FALSE);
	  return;
	}
        free(digits);
	cid_row_mark_record(session, session->cid_num - 1);
      }
    } else { /* don't record! */
      session->option_record = 0;
      if (session->state == STATE_CONVERSATION) {
	recording_write(session->recorder, session->rec_buf_local,
	                session->rec_buf_local_index, RECORDING_LOCAL);
	recording_write(session->recorder, session->rec_buf_remote,
	                session->rec_buf_remote_index, RECORDING_REMOTE);
        recording_close(session->recorder);
        session->rec_buf_local_index = 0;
        session->rec_buf_remote_index = 0;
      }
    }
    gtk_widget_set_sensitive(session->record_checkbutton_local,
			     session->option_record);
    gtk_widget_set_sensitive(session->record_checkbutton_remote,
			     session->option_record);
  } else if (button == session->record_checkbutton_local) {
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button))) {
      /* record local channel (if recording is selected at all)! */
      session->option_record_local = 1;
    } else { /* don't record local channel! */
      session->option_record_local = 0;
    }
  } else { /* button == session->record_checkbutton_remote */
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button))) {
      /* record remote channel (if recording is selected at all)! */
      session->option_record_remote = 1;
    } else { /* don't record remote channel! */
      session->option_record_remote = 0;
    }
  }
}

/*
 * The key pad etc.
 */
GtkWidget *controlpad_new(session_t *session) {
  char *labels[4][5] = {{"1", "2", "3", "B", session->preset_names[0]},
			{"4", "5", "6", "C", session->preset_names[1]},
			{"7", "8", "9", "R", session->preset_names[2]},
			{"*", "0", "#", "M", session->preset_names[3]}};
  char *tips[4][5] = {
    {",-./", "abc",   "def",  N_("Backspace"),       N_("Preset 1")},
    {"ghi",  "jkl",   "mno",  N_("Clear Number"),    N_("Preset 2")},
    {"pqrs", "tuv",   "wxyz", N_("Redial"),          N_("Preset 3")},
    {"",     "0...9", "",     N_("Mute Microphone"), N_("Preset 4")}};
  char *presetcodes[4] = {"P0", "P1", "P2", "P3"};
  
  GtkWidget *frame;
  GtkWidget *hbox;
  GtkWidget *table;
  GtkWidget *button;

  GtkWidget *recordframe;
  GtkWidget *recordbox;
  GtkWidget *record_checkbutton;
  GtkWidget *record_checkbutton_local;
  GtkWidget *record_checkbutton_remote;

  GdkPixmap *pixmap;
  GdkBitmap *mask;
  GtkStyle *style;
  GtkWidget *pixmapwid;

  GtkTooltips *tooltips;

  int i, j;

  tooltips = gtk_tooltips_new();
  /* tooltips are often grey by default and have to be set up with a gtkrc
     file:

     style "gtk-tooltips-style" {
             bg[NORMAL] = "#ffffc0"
     }
     widget "gtk-tooltips" style "gtk-tooltips-style"
   */

  frame = gtk_frame_new(_("Control"));
  gtk_container_set_border_width(GTK_CONTAINER(frame), 8);

  hbox = gtk_hbox_new(FALSE, 15);
  gtk_container_set_border_width(GTK_CONTAINER(hbox), 5);
  gtk_container_add(GTK_CONTAINER(frame), hbox);
  gtk_widget_show(hbox);

  table = gtk_table_new(4, 3, FALSE);
  gtk_table_set_row_spacings(GTK_TABLE(table), 5);
  gtk_box_pack_start(GTK_BOX(hbox), table, TRUE, FALSE, 5);
  gtk_widget_show(table);

  for (i = 0; i < 5; i++) { /* columns */
    for (j = 0; j < 4; j++) { /* rows */
      if (i == 3 && j == 3) { /* mute  */
	button = gtk_toggle_button_new();
	style = gtk_widget_get_style(session->main_window);
	pixmap = gdk_pixmap_create_from_xpm_d(session->main_window->window,
					      &mask,
					      &style->bg[GTK_STATE_NORMAL],
					      (gchar **) mute_xpm);
	pixmapwid = gtk_pixmap_new(pixmap, mask);
	gtk_container_add(GTK_CONTAINER(button), pixmapwid);
	gtk_widget_show(pixmapwid);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button),
				     session->option_muted);
	session->mute_button = button;
	gtk_signal_connect(GTK_OBJECT(button), "toggled",
			   GTK_SIGNAL_FUNC(controlpad_mute_cb), session);
      } else { /* everything else (except mute) */
	if (i == 3 && j == 0) { /* backspace icon */
	  button = gtk_button_new();
	  style = gtk_widget_get_style(session->main_window);
	  pixmap = gdk_pixmap_create_from_xpm_d(session->main_window->window,
						&mask,
						&style->bg[GTK_STATE_NORMAL],
						(gchar **) backspace_xpm);
	  pixmapwid = gtk_pixmap_new(pixmap, mask);
	  gtk_container_add(GTK_CONTAINER(button), pixmapwid);
	  gtk_widget_show(pixmapwid);
	} else if (i == 3 && j == 2) { /* redial icon */
	  button = gtk_button_new();
	  style = gtk_widget_get_style(session->main_window);
	  pixmap = gdk_pixmap_create_from_xpm_d(session->main_window->window,
						&mask,
						&style->bg[GTK_STATE_NORMAL],
						(gchar **) redial_xpm);
	  pixmapwid = gtk_pixmap_new(pixmap, mask);
	  gtk_container_add(GTK_CONTAINER(button), pixmapwid);
	  gtk_widget_show(pixmapwid);
	} else { /* icon with letter (or description) */
	  button = gtk_button_new_with_label(labels[j][i]);
	  gtk_signal_connect(GTK_OBJECT(button), "button-press-event",
	                     GTK_SIGNAL_FUNC(controlpad_preset_edit_cb),
			     session);
	}
	/* connect callback to widget */
	gtk_object_set_data(GTK_OBJECT(button), "desc",
	                    i < 4 ? labels[j][i] : presetcodes[j]);
	gtk_signal_connect(GTK_OBJECT(button), "pressed",
			   GTK_SIGNAL_FUNC(controlpad_button_cb), session);
      }
      if (*tips[j][i])
	gtk_tooltips_set_tip(tooltips, button, _(tips[j][i]), NULL);
      gtk_table_attach_defaults(GTK_TABLE(table), button, i, i + 1, j, j + 1);
      gtk_widget_set_size_request(button, i < 3 ? 30 : i == 3 ? 20 : 100, -1);
      gtk_widget_show(button);
    }
    gtk_table_set_col_spacing(GTK_TABLE(table), i, i < 2 ? 5 : 15);
  }

  recordframe = gtk_frame_new(_("Recording"));
  gtk_box_pack_start(GTK_BOX(hbox), recordframe, TRUE, FALSE, 5);
  gtk_widget_show(recordframe);

  recordbox = gtk_table_new(2, 3, FALSE);
  gtk_table_set_row_spacings(GTK_TABLE(recordbox), 0);
  gtk_table_set_col_spacings(GTK_TABLE(recordbox), 20);
  gtk_container_add(GTK_CONTAINER(recordframe), recordbox);
  gtk_widget_show(recordbox);

  record_checkbutton =
    gtk_check_button_new_with_label(_("Record to file"));
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(record_checkbutton),
			       session->option_record);
  gtk_table_attach(GTK_TABLE(recordbox), record_checkbutton,
      		   0, 2, 0, 1, GTK_FILL | GTK_EXPAND, 0, 0, 0);
  gtk_widget_show(record_checkbutton);
  gtk_signal_connect(GTK_OBJECT(record_checkbutton), "toggled",
		     GTK_SIGNAL_FUNC(controlpad_record_cb), session);
  session->record_checkbutton = record_checkbutton;

  record_checkbutton_local =
    gtk_check_button_new_with_label(_("Record local channel"));
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(record_checkbutton_local),
			       session->option_record_local);
  gtk_table_attach(GTK_TABLE(recordbox), record_checkbutton_local,
      		   1, 2, 1, 2, GTK_FILL | GTK_EXPAND, 0, 0, 0);
  gtk_widget_show(record_checkbutton_local);
  gtk_signal_connect(GTK_OBJECT(record_checkbutton_local), "toggled",
		     GTK_SIGNAL_FUNC(controlpad_record_cb), session);
  session->record_checkbutton_local = record_checkbutton_local;

  record_checkbutton_remote =
    gtk_check_button_new_with_label(_("Record remote channel"));
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(record_checkbutton_remote),
			       session->option_record_remote);
  gtk_table_attach(GTK_TABLE(recordbox), record_checkbutton_remote,
      		   1, 2, 2, 3, GTK_FILL | GTK_EXPAND, 0, 0, 0);
  gtk_widget_show(record_checkbutton_remote);
  gtk_signal_connect(GTK_OBJECT(record_checkbutton_remote), "toggled",
		     GTK_SIGNAL_FUNC(controlpad_record_cb), session);
  session->record_checkbutton_remote = record_checkbutton_remote;

  if (!session->option_record) {
    gtk_widget_set_sensitive(record_checkbutton_local, FALSE);
    gtk_widget_set_sensitive(record_checkbutton_remote, FALSE);
  }

  return frame;
}

/* called when control pad state check button is toggled */
void controlpad_toggle_cb(GtkWidget *widget _U_,
			  gpointer data, guint action _U_) {
  session_t *session = (session_t *) data;

  if (GTK_CHECK_MENU_ITEM (session->controlpad_check_menu_item)->active) {
    gtk_widget_show(session->controlpad);
    session->option_show_controlpad = 1;
  } else {
    gtk_widget_hide(session->controlpad);
    session->option_show_controlpad = 0;
    /* shrink if no growing callerid monitor present */
    if (!session->option_show_callerid)
      gtk_window_resize(GTK_WINDOW(session->main_window), 1, 1);
  }
}

