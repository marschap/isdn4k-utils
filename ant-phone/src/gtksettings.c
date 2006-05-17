/*
 * settings gtk GUI functions
 *
 * This file is part of ANT (Ant is Not a Telephone)
 *
 * Copyright 2002, 2003 Roland Stigge
 *
 */

#include "config.h"

/* regular GNU system includes */
#include <stdio.h>
#include <string.h>
#ifdef HAVE_STDLIB_H
  #include <stdlib.h>
#endif

/* GTK */
#include <gtk/gtk.h>

/* own header files */
#include "globals.h"
#include "session.h"
#include "isdn.h"
#include "sound.h"
#include "gtk.h"
#include "settings.h"
#include "recording.h"

/*
 * Load all settings from widgets into session
 * -> used by gtksettings_cb_ok and gtksettings_cb_save
 *
 * input: widget: the widget with associated data widgets and session inside
 *
 * returns 0 on success, -1 otherwise (bad audio settings)
 */
static int gtksettings_try(GtkWidget *widget) {
  session_t *session = gtk_object_get_data(GTK_OBJECT(widget),
						  "session");
  /* save current settings */
  char *old_msn = session->msn;
  char *old_msns = session->msns;
  int successful = 1;
  GtkWidget *entry;
  GtkWidget *button;
  GSList *list;

  /* exec_on_incoming */
  entry = (GtkWidget *) gtk_object_get_data(GTK_OBJECT(widget),
					    "exec_on_incoming_entry");
  if (entry) {
    free(session->exec_on_incoming);
    session->exec_on_incoming = strdup(gtk_entry_get_text(GTK_ENTRY(entry)));
  } else
    fprintf(stderr, "gtksettings_cb_ok: Error getting exec_on_incoming.\n");

  /* popup checkbutton */
  button = (GtkWidget *) gtk_object_get_data(GTK_OBJECT(widget),
					     "popup_checkbutton");
  if (button)
    session->option_popup =
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button));
  else
    fprintf(stderr, "gtksettings_cb_ok: Error getting popup state.\n");
  
  /* recording_format */
  list = (GSList *) gtk_object_get_data(GTK_OBJECT(widget), "recording_format");
  if (list) {
    while (list) {
      if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(list->data))) {
	session->option_recording_format = (enum recording_format_t)
	    gtk_object_get_data(GTK_OBJECT(list->data), "rec_format");
      }
      list = list->next;
    }
  } else
    fprintf(stderr, "gtksettings_cb_ok: Error getting recording_format.\n");

  /* msn */
  entry = (GtkWidget *) gtk_object_get_data(GTK_OBJECT(widget), "msn_entry");
  if (entry)
    session->msn = strdup(gtk_entry_get_text(GTK_ENTRY(entry)));
  else
    fprintf(stderr, "gtksettings_cb_ok: Error getting msn.\n");

  /* msns */
  entry = (GtkWidget *) gtk_object_get_data(GTK_OBJECT(widget), "msns_entry");
  if (entry)
    session->msns = strdup(gtk_entry_get_text(GTK_ENTRY(entry)));
  else
    fprintf(stderr, "gtksettings_cb_ok: Error getting msns.\n");
       
  /* history_entry */
  entry = (GtkWidget *) gtk_object_get_data(GTK_OBJECT(widget),
					    "history_entry");
  if (entry)
    session->dial_number_history_maxlen =
      strtol(gtk_entry_get_text(GTK_ENTRY(entry)), NULL, 0);
  else
    fprintf(stderr, "gtksettings_cb_ok: Error getting history.\n");
  session->dial_number_history_pointer = 0;

  /* cid_max_entry */
  entry = (GtkWidget *) gtk_object_get_data(GTK_OBJECT(widget),
					    "cid_max_entry");
  if (entry)
    session->cid_num_max =
      strtol(gtk_entry_get_text(GTK_ENTRY(entry)), NULL, 0);
  else
    fprintf(stderr, "gtksettings_cb_ok: "
	    "Error getting caller id maximum rows.\n");

  /* cid_calls_merge_checkbutton */
  button = (GtkWidget *) gtk_object_get_data(GTK_OBJECT(widget),
					     "cid_calls_merge_checkbutton");
  if (button)
    session->option_calls_merge =
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button));
  else
    fprintf(stderr,
	    "gtksettings_cb_ok: Error getting isdnlog calls_merge state.\n");
  /* cid_calls_merge_max_entry */
  entry = (GtkWidget *) gtk_object_get_data(GTK_OBJECT(widget),
					    "cid_calls_merge_max_entry");
  if (entry)
    session->option_calls_merge_max_days =
      strtol(gtk_entry_get_text(GTK_ENTRY(entry)), NULL, 0);
  else
    fprintf(stderr, "gtksettings_cb_ok: "
	    "Error getting maximum number of days for isdnlog retrieval.\n");

  /* save checkbutton */
  button = (GtkWidget *) gtk_object_get_data(GTK_OBJECT(widget),
					     "save_checkbutton");
  if (button)
    session->option_save_options =
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button));
  else
    fprintf(stderr,
	    "gtksettings_cb_ok: Error getting save_options state.\n");

  /*
   * audio stop/start
   */

  if (session->audio_device_name_in) { /* shut down if defined */
    session_io_handlers_stop(session);
    if (!session->option_release_devices) /* audio_close if normal mode */
      session_audio_deinit(session);
    free(session->audio_device_name_in);
    free(session->audio_device_name_out);
  }

  /* audio_device_name_in */
  entry = (GtkWidget *) gtk_object_get_data(GTK_OBJECT(widget),
					    "audio_device_name_in_entry");
  if (entry)
    session->audio_device_name_in =
      strdup(gtk_entry_get_text(GTK_ENTRY(entry)));
  else
    fprintf(stderr,"gtksettings_cb_ok: Error getting audio_device_name_in.\n");

  /* audio_device_name_out */
  entry = (GtkWidget *) gtk_object_get_data(GTK_OBJECT(widget),
					    "audio_device_name_out_entry");
  if (entry)
    session->audio_device_name_out =
      strdup(gtk_entry_get_text(GTK_ENTRY(entry)));
  else
    fprintf(stderr,
	    "gtksettings_cb_ok: Error getting audio_device_name_out.\n");

  /* release checkbutton */
  button = (GtkWidget *) gtk_object_get_data(GTK_OBJECT(widget),
					     "release_checkbutton");
  if (button)
    session->option_release_devices =
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button));
  else
    fprintf(stderr,
	    "gtksettings_cb_ok: Error getting release_devices state.\n");

  if (!session->option_release_devices) {
    if (session_audio_init(session)) {
      successful = 0;
      free(session->audio_device_name_in);
      free(session->audio_device_name_out);
      session->audio_device_name_in = NULL; /* flag for audio shut down */
    }
  }

  if (session->audio_device_name_in) { /* OK or unknown */
    session_io_handlers_start(session);
    session_set_state(session, STATE_READY); /* update everything */
  }

  /* try to apply msn settings */
  if (isdn_setMSN(session->isdn_fd, session->msn) ||
      isdn_setMSNs(session->isdn_fd, session->msns)) {
    /* got some problem */
    free(session->msn);
    free(session->msns);
    session->msn = old_msn;
    session->msns = old_msns;
    isdn_setMSN(session->isdn_fd, session->msn);
    isdn_setMSNs(session->isdn_fd, session->msns);
    successful = 0;
  } else {
    /* everything's fine */
    free(old_msn);
    free(old_msns);
  }

  return !!successful - 1;
}

/*
 * clicked "ok" at gtksettings_cb
 * input: window: the widget with associated data widgets and session inside
 */
static void gtksettings_cb_ok(GtkWidget *window) {
  GtkWidget *ok_window;

  if (!gtksettings_try(window)) { /* changed settings */
    gtk_widget_destroy(window);
  } else { /* settings error -> restored old settings -> display note */
    ok_window = ok_dialog_get(_("ANT Note"),
			 _("Bad isdn/sound device settings, please try again."),
			 GTK_JUSTIFY_CENTER);
    gtk_window_set_modal(GTK_WINDOW(ok_window), TRUE);
    gtk_widget_show(ok_window);
  }
}

/* clicked "Save" button at settings dialog (gtksettings_cb) */
static void gtksettings_cb_save(GtkWidget *widget) {
  session_t *session = gtk_object_get_data(GTK_OBJECT(widget),
						  "session");
  gtksettings_try(widget);
  settings_options_write(session);
}

/* clicked "Cancel" button at settings dialog (gtksettings_cb) */
static void gtksettings_cb_cancel(GtkWidget *window) {
  session_t *session = gtk_object_get_data(GTK_OBJECT(window),
						  "session");
  if (session->audio_device_name_in) /* audio not shut off */
    gtk_widget_destroy(window);
}

/*
 * callback for setting widget sensitivity on toggle button change
 */
static void toggle_sensitive_register(GtkToggleButton *button,
                                      GtkWidget *widget) {
  gtk_widget_set_sensitive(widget, gtk_toggle_button_get_active(button));
}

/* Settings menu entry callback */
void gtksettings_cb(GtkWidget *widget _U_, gpointer data, guint action _U_) {
  session_t *session = (session_t *) data;
  GtkWidget *window;   /* the dialog window itself */
  GtkWidget *button_box; /* box for buttons in standard way */
  GtkWidget *button;   /* action area buttons */
  GtkWidget *table;    /* tables in frames */
  GtkWidget *label;    /* different labels */
  GtkWidget *frame;    /* the frames to separate msn/msns from sound devices */
  GtkWidget *notebook; /* to separate options into topics */
  GtkWidget *vbox; /* the vbox inside notebook with frames */
  GtkWidget *vbox2; /* vbox inside frames */

  /* widgets to save (to evaluate later) */
  GtkWidget *save_checkbutton;           /* program page */
  GtkWidget *popup_checkbutton;
  GtkWidget *exec_on_incoming_entry; 
  GtkWidget *msn_entry;                  /* phone page */
  GtkWidget *msns_entry;
  GtkWidget *history_entry;
  GtkWidget *cid_max_entry;
  GtkWidget *audio_device_name_in_entry; /* sound devices page */
  GtkWidget *audio_device_name_out_entry;
  GtkWidget *release_checkbutton; 
  GtkWidget *recformat_radiobutton; /* recording format */

  GtkWidget *cid_calls_merge_checkbutton;
  GtkWidget *cid_calls_merge_max_entry;

  char *s; /* temporary string */

  window = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(window), _("ANT Settings"));

  /* the notebook inside the window / vbox area */
  notebook = gtk_notebook_new();
  gtk_container_set_border_width(GTK_CONTAINER(notebook), 5);
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(window)->vbox), notebook,
		     TRUE, TRUE, 0);
  gtk_widget_show(notebook);

  /* Program page */
  vbox = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox);
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox,
			   gtk_label_new(_("Application")));

  frame = gtk_frame_new(_("Options"));
  gtk_container_set_border_width(GTK_CONTAINER(frame), 10);
  gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 0);
  gtk_widget_show(frame);

  table = gtk_table_new(3, 2, FALSE); /* rows, columns, not homogeneous */
  gtk_container_add(GTK_CONTAINER(frame), table);
  gtk_container_set_border_width(GTK_CONTAINER(table), 5);
  gtk_table_set_row_spacings(GTK_TABLE(table), 5);
  gtk_table_set_col_spacings(GTK_TABLE(table), 5);
  gtk_widget_show(table);

  save_checkbutton = gtk_check_button_new_with_label(_("Save options on exit"));
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(save_checkbutton),
			       session->option_save_options);
  gtk_table_attach_defaults(GTK_TABLE(table), save_checkbutton, 0,2,0,1);
  gtk_widget_show(save_checkbutton);

  popup_checkbutton =
    gtk_check_button_new_with_label(_("Popup main window on incoming call"));
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(popup_checkbutton),
			       session->option_popup);
  gtk_table_attach_defaults(GTK_TABLE(table), popup_checkbutton, 0,2,1,2);
  gtk_widget_show(popup_checkbutton);

  label = gtk_label_new(_("Execute on incoming call:"));
  gtk_table_attach_defaults(GTK_TABLE(table), label, 0,1,2,3);
  gtk_misc_set_alignment(GTK_MISC(label), 1, 0.5);
  gtk_widget_show(label);

  exec_on_incoming_entry = gtk_entry_new();
  gtk_table_attach_defaults(GTK_TABLE(table), exec_on_incoming_entry, 1,2,2,3);
  gtk_entry_set_text(GTK_ENTRY(exec_on_incoming_entry),
		     session->exec_on_incoming);
  gtk_widget_show(exec_on_incoming_entry);

  frame = gtk_frame_new(_("Recording Format"));
  gtk_container_set_border_width(GTK_CONTAINER(frame), 10);
  gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 0);
  gtk_widget_show(frame);

  vbox2 = gtk_vbox_new(FALSE, 0);
  gtk_container_add(GTK_CONTAINER(frame), vbox2);
  gtk_widget_show(vbox2);

  recformat_radiobutton = gtk_radio_button_new_with_label(NULL,
      _("Microsoft WAV, uLaw"));
  gtk_box_pack_start(GTK_BOX(vbox2), recformat_radiobutton, FALSE, FALSE, 0);
  gtk_widget_show(recformat_radiobutton);
  gtk_object_set_data(GTK_OBJECT(recformat_radiobutton),
      "rec_format", (gpointer) (RECORDING_FORMAT_WAV | RECORDING_FORMAT_ULAW));
  if (session->option_recording_format == (enum recording_format_t)
      gtk_object_get_data(GTK_OBJECT(recformat_radiobutton), "rec_format"))
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(recformat_radiobutton),TRUE);

  recformat_radiobutton = gtk_radio_button_new_with_label_from_widget(
    GTK_RADIO_BUTTON(recformat_radiobutton), _("Microsoft WAV, 16-bit signed"));
  gtk_box_pack_start(GTK_BOX(vbox2), recformat_radiobutton, FALSE, FALSE, 0);
  gtk_widget_show(recformat_radiobutton);
  gtk_object_set_data(GTK_OBJECT(recformat_radiobutton),
      "rec_format", (gpointer) (RECORDING_FORMAT_WAV | RECORDING_FORMAT_S16));
  if (session->option_recording_format == (enum recording_format_t)
      gtk_object_get_data(GTK_OBJECT(recformat_radiobutton), "rec_format"))
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(recformat_radiobutton),TRUE);

  recformat_radiobutton = gtk_radio_button_new_with_label_from_widget(
      GTK_RADIO_BUTTON(recformat_radiobutton), _("Apple/SGI AIFF, uLaw"));
  gtk_box_pack_start(GTK_BOX(vbox2), recformat_radiobutton, FALSE, FALSE, 0);
  gtk_widget_show(recformat_radiobutton);
  gtk_object_set_data(GTK_OBJECT(recformat_radiobutton),
      "rec_format", (gpointer) (RECORDING_FORMAT_AIFF | RECORDING_FORMAT_ULAW));
  if (session->option_recording_format == (enum recording_format_t)
      gtk_object_get_data(GTK_OBJECT(recformat_radiobutton), "rec_format"))
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(recformat_radiobutton),TRUE);

  recformat_radiobutton = gtk_radio_button_new_with_label_from_widget(
   GTK_RADIO_BUTTON(recformat_radiobutton), _("Apple/SGI AIFF, 16-bit signed"));
  gtk_box_pack_start(GTK_BOX(vbox2), recformat_radiobutton, FALSE, FALSE, 0);
  gtk_widget_show(recformat_radiobutton);
  gtk_object_set_data(GTK_OBJECT(recformat_radiobutton),
      "rec_format", (gpointer) (RECORDING_FORMAT_AIFF | RECORDING_FORMAT_S16));
  if (session->option_recording_format == (enum recording_format_t)
      gtk_object_get_data(GTK_OBJECT(recformat_radiobutton), "rec_format"))
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(recformat_radiobutton),TRUE);

  /* Phone page */
  vbox = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox);
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox,
			   gtk_label_new(_("Phone")));

  frame = gtk_frame_new(_("ISDN"));
  gtk_container_set_border_width(GTK_CONTAINER(frame), 10);
  gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 0);
  gtk_widget_show(frame);

  table = gtk_table_new(2, 2, FALSE); /* rows, columns, not homogeneous */
  gtk_container_add(GTK_CONTAINER(frame), table);
  gtk_container_set_border_width(GTK_CONTAINER(table), 5);
  gtk_table_set_row_spacings(GTK_TABLE(table), 5);
  gtk_table_set_col_spacings(GTK_TABLE(table), 5);
  gtk_widget_show(table);

  label = gtk_label_new(_("Identifying MSN:"));
  gtk_table_attach_defaults(GTK_TABLE(table), label, 0,1,0,1);
  gtk_misc_set_alignment(GTK_MISC(label), 1, 0.5);
  gtk_widget_show(label);

  msn_entry = gtk_entry_new();
  gtk_table_attach_defaults(GTK_TABLE(table), msn_entry, 1,2,0,1);
  gtk_entry_set_text(GTK_ENTRY(msn_entry), session->msn);
  gtk_widget_show(msn_entry);

  label = gtk_label_new(_("Listen to MSNs:"));
  gtk_table_attach_defaults(GTK_TABLE(table), label, 0,1,1,2);
  gtk_misc_set_alignment(GTK_MISC(label), 1, 0.5);
  gtk_widget_show(label);

  msns_entry = gtk_entry_new();
  gtk_table_attach_defaults(GTK_TABLE(table), msns_entry, 1,2,1,2);
  gtk_entry_set_text(GTK_ENTRY(msns_entry), session->msns);
  gtk_widget_show(msns_entry);

  frame = gtk_frame_new(_("Dialing"));
  gtk_container_set_border_width(GTK_CONTAINER(frame), 10);
  gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 0);
  gtk_widget_show(frame);

  table = gtk_table_new(1, 2, FALSE); /* rows, columns, not homogeneous */
  gtk_container_add(GTK_CONTAINER(frame), table);
  gtk_container_set_border_width(GTK_CONTAINER(table), 5);
  gtk_table_set_row_spacings(GTK_TABLE(table), 5);
  gtk_table_set_col_spacings(GTK_TABLE(table), 5);
  gtk_widget_show(table);

  label = gtk_label_new(_("Dial history size:"));
  gtk_table_attach_defaults(GTK_TABLE(table), label, 0,1,0,1);
  gtk_misc_set_alignment(GTK_MISC(label), 1, 0.5);
  gtk_widget_show(label);

  history_entry = gtk_entry_new();
  gtk_table_attach_defaults(GTK_TABLE(table), history_entry, 1,2,0,1);
  asprintf(&s, "%u", session->dial_number_history_maxlen);
  gtk_entry_set_text(GTK_ENTRY(history_entry), s);
  free(s);
  gtk_widget_show(history_entry);

  frame = gtk_frame_new(_("Caller ID"));
  gtk_container_set_border_width(GTK_CONTAINER(frame), 10);
  gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 0);
  gtk_widget_show(frame);

  table = gtk_table_new(3, 2, FALSE); /* rows, columns, not homogeneous */
  gtk_container_add(GTK_CONTAINER(frame), table);
  gtk_container_set_border_width(GTK_CONTAINER(table), 5);
  gtk_table_set_row_spacings(GTK_TABLE(table), 5);
  gtk_table_set_col_spacings(GTK_TABLE(table), 5);
  gtk_widget_show(table);

  label = gtk_label_new(_("Maximum CID rows:"));
  gtk_table_attach_defaults(GTK_TABLE(table), label, 0,1,0,1);
  gtk_misc_set_alignment(GTK_MISC(label), 1, 0.5);
  gtk_widget_show(label);

  cid_max_entry = gtk_entry_new();
  gtk_table_attach_defaults(GTK_TABLE(table), cid_max_entry, 1,2,0,1);
  if (session->cid_num_max == 0) {
    s = strdup(_("[no limit]"));
  } else {
    asprintf(&s, "%u", session->cid_num_max);
  }
  gtk_entry_set_text(GTK_ENTRY(cid_max_entry), s);
  free(s);
  gtk_widget_show(cid_max_entry);

  cid_calls_merge_checkbutton = gtk_check_button_new_with_label(
                                    _("Read isdnlog data on startup"));
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cid_calls_merge_checkbutton),
			       session->option_calls_merge);
  gtk_table_attach_defaults(GTK_TABLE(table), cid_calls_merge_checkbutton,
                            0,2,1,2);
  gtk_widget_show(cid_calls_merge_checkbutton);
  
  label = gtk_label_new(_("Maximum days to read from isdnlog:"));
  gtk_table_attach_defaults(GTK_TABLE(table), label, 0,1,2,3);
  gtk_misc_set_alignment(GTK_MISC(label), 1, 0.5);
  gtk_widget_set_sensitive(label, session->option_calls_merge);
  gtk_signal_connect(GTK_OBJECT(cid_calls_merge_checkbutton), "toggled",
                     GTK_SIGNAL_FUNC(toggle_sensitive_register), label);
  gtk_widget_show(label);

  cid_calls_merge_max_entry = gtk_entry_new();
  gtk_table_attach_defaults(GTK_TABLE(table), cid_calls_merge_max_entry,
                            1,2,2,3);
  if (session->option_calls_merge_max_days == 0) {
    s = strdup(_("[no limit]"));
  } else {
    asprintf(&s, "%u", session->option_calls_merge_max_days);
  }
  gtk_entry_set_text(GTK_ENTRY(cid_calls_merge_max_entry), s);
  free(s);
  gtk_widget_set_sensitive(cid_calls_merge_max_entry,
                           session->option_calls_merge);
  gtk_signal_connect(GTK_OBJECT(cid_calls_merge_checkbutton), "toggled",
                     GTK_SIGNAL_FUNC(toggle_sensitive_register),
		     cid_calls_merge_max_entry);
  gtk_widget_show(cid_calls_merge_max_entry);

  /* Sound devices page */
  vbox = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox);
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox,
			   gtk_label_new(_("Sound Devices")));

  frame = gtk_frame_new(_("OSS"));
  gtk_container_set_border_width(GTK_CONTAINER(frame), 10);
  gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 0);
  gtk_widget_show(frame);

  table = gtk_table_new(3,2, FALSE); /* rows, columns, not homogeneous */
  gtk_container_add(GTK_CONTAINER(frame), table);
  gtk_container_set_border_width(GTK_CONTAINER(table), 5);
  gtk_table_set_row_spacings(GTK_TABLE(table), 5);
  gtk_table_set_col_spacings(GTK_TABLE(table), 5);
  gtk_widget_show(table);

  label = gtk_label_new(_("Input sound device:"));
  gtk_table_attach_defaults(GTK_TABLE(table), label, 0,1,0,1);
  gtk_misc_set_alignment(GTK_MISC(label), 1, 0.5);
  gtk_widget_show(label);

  audio_device_name_in_entry = gtk_entry_new();
  gtk_table_attach_defaults(GTK_TABLE(table), audio_device_name_in_entry,
			    1,2,0,1);
  gtk_entry_set_text(GTK_ENTRY(audio_device_name_in_entry),
		     session->audio_device_name_in);
  gtk_widget_show(audio_device_name_in_entry);

  label = gtk_label_new(_("Output sound device:"));
  gtk_table_attach_defaults(GTK_TABLE(table), label, 0,1,1,2);
  gtk_misc_set_alignment(GTK_MISC(label), 1, 0.5);
  gtk_widget_show(label);

  audio_device_name_out_entry = gtk_entry_new();
  gtk_table_attach_defaults(GTK_TABLE(table), audio_device_name_out_entry,
			    1,2,1,2);
  gtk_entry_set_text(GTK_ENTRY(audio_device_name_out_entry),
		     session->audio_device_name_out);
  gtk_widget_show(audio_device_name_out_entry);

  release_checkbutton =
    gtk_check_button_new_with_label(_("Release unused devices"));
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(release_checkbutton),
			       session->option_release_devices);
  gtk_table_attach_defaults(GTK_TABLE(table), release_checkbutton, 0,2,2,3);
  gtk_widget_show(release_checkbutton);

  /* action area */
  button_box = gtk_hbutton_box_new();
  gtk_container_add(GTK_CONTAINER(GTK_DIALOG(window)->action_area),
  		    button_box);
  gtk_widget_show(button_box);

  button = gtk_button_new_with_label(_("OK"));
  gtk_box_pack_start_defaults(GTK_BOX(button_box), button);

  gtk_object_set_data(GTK_OBJECT(window), "session", (gpointer) session);

  gtk_object_set_data(GTK_OBJECT(window), "save_checkbutton",
		      (gpointer) save_checkbutton);
  gtk_object_set_data(GTK_OBJECT(window), "popup_checkbutton",
		      (gpointer) popup_checkbutton);
  gtk_object_set_data(GTK_OBJECT(window), "exec_on_incoming_entry",
		      (gpointer) exec_on_incoming_entry);

  gtk_object_set_data(GTK_OBJECT(window), "recording_format",
    (gpointer) gtk_radio_button_group(GTK_RADIO_BUTTON(recformat_radiobutton)));
    
  gtk_object_set_data(GTK_OBJECT(window), "msn_entry", (gpointer) msn_entry);
  gtk_object_set_data(GTK_OBJECT(window), "msns_entry", (gpointer) msns_entry);
  gtk_object_set_data(GTK_OBJECT(window), "history_entry",
		      (gpointer) history_entry);
  gtk_object_set_data(GTK_OBJECT(window), "cid_max_entry",
		      (gpointer) cid_max_entry);
  gtk_object_set_data(GTK_OBJECT(window), "cid_calls_merge_checkbutton",
		      (gpointer) cid_calls_merge_checkbutton);
  gtk_object_set_data(GTK_OBJECT(window), "cid_calls_merge_max_entry",
		      (gpointer) cid_calls_merge_max_entry);

  gtk_object_set_data(GTK_OBJECT(window), "audio_device_name_in_entry",
		      (gpointer) audio_device_name_in_entry);
  gtk_object_set_data(GTK_OBJECT(window), "audio_device_name_out_entry",
		      (gpointer) audio_device_name_out_entry);
  gtk_object_set_data(GTK_OBJECT(window), "release_checkbutton",
		      (gpointer) release_checkbutton);

  gtk_signal_connect_object(GTK_OBJECT(button), "clicked",
			    GTK_SIGNAL_FUNC(gtksettings_cb_ok),
			    GTK_OBJECT(window));
  gtk_widget_show(button);

  button = gtk_button_new_with_label(_("Save"));
  gtk_box_pack_start_defaults(GTK_BOX(button_box), button);
  gtk_signal_connect_object(GTK_OBJECT(button), "clicked",
			    GTK_SIGNAL_FUNC(gtksettings_cb_save),
			    GTK_OBJECT(window));
  gtk_widget_show(button);

  button = gtk_button_new_with_label(_("Cancel"));
  gtk_box_pack_start_defaults(GTK_BOX(button_box), button);
  gtk_signal_connect_object(GTK_OBJECT(button), "clicked",
			    GTK_SIGNAL_FUNC(gtksettings_cb_cancel),
			    GTK_OBJECT(window));
  gtk_signal_connect_object(GTK_OBJECT(window), "delete_event",
			    GTK_SIGNAL_FUNC(gtksettings_cb_cancel),
			    GTK_OBJECT(window));
  gtk_widget_show(button);
  
  gtk_window_set_modal(GTK_WINDOW(window), TRUE);

  /* show everything */
  gtk_widget_show(window);
}

