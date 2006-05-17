/*
 * gtk GUI functions
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
#include <stdio.h>
#include <signal.h>
#ifdef HAVE_STDLIB_H
  #include <stdlib.h>
#endif
#include <string.h>
#include <time.h>

/* GTK */
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

/* own header files */
#include "globals.h"
#include "gtk.h"
#include "session.h"
#include "isdn.h"
#include "util.h"
#include "sound.h"
#include "callerid.h"
#include "llcheck.h"
#include "settings.h"
#include "gtksettings.h"
#include "controlpad.h"

/* graphical symbols */
#include "hangup.xpm"
#include "pickup.xpm"
#include "aboutlogo.xpm"
/*
#include "icon16x16.xpm"
#include "icon32x32.xpm"
*/
#include "icon48x48.xpm"
/*
#include "icon64x64.xpm"
*/

/* call timeout_callback each <this number> of milliseconds */
#define TIMER_DELAY 300

volatile int interrupted = 0; /* the caught signal will be stored here */

/*
 * the quit (e.g. menu entry) callback
 * prototype is this way to standardize to GtkItemFactoryCallback2
 *
 * this function will always be called when gtk is to finish
 *
 * input: data: session
 */
static void quit(GtkWidget *widget _U_, gpointer data, guint action _U_) {
  session_t *session = (session_t *) data;
  
  settings_history_write(session); /* write history */
  settings_callerid_write(session); /* write callerid history */
  
  gtk_handle_hang_up_button(NULL, data); /* simulate hang_up_button */

  /* some (GUI) de-initialization, not directly session related */
  llcheck_bar_deinit(session->llcheck_in);
  llcheck_bar_deinit(session->llcheck_out);

  gtk_main_quit();
}

/*
 * main window delete_event callback
 *
 * input: data: session
 */
gint delete_event(GtkWidget *widget,
		  GdkEvent *event _U_,
		  gpointer data)
{
  quit(widget, data, 0);
  return FALSE; /* continue event handling */
}

/* handler for SIGTERM and SIGINT */
void terminate_signal_callback(int sig) {
  interrupted = sig;
}

/*
 * periodically (e.g. each 300 milliseconds) from gtk main loop called function
 */
gint timeout_callback(gpointer data) {
  session_t *session = (session_t *) data;

  if (interrupted) {
    switch(interrupted) {
    case SIGINT:
      if (debug)
	fprintf(stderr, "Ctrl-C caught.\n");
      break;
    case SIGTERM:
      if (debug)
	fprintf(stderr, "SIGTERM caught.\n");
      break;
    default:
      fprintf(stderr, "Warning: Unknown signal caught.\n");
    }
    quit(NULL, data, 0);
  }

  if (session->state == STATE_CONVERSATION) {
    char *timediff = timediff_str(time(NULL), session->vcon_time);
    char *buf;

    if (0 > asprintf(&buf, "%s %s",
		     state_data[session->state].status_bar, timediff))
      fprintf(stderr, "Warning: timeout_callback: asprintf error.\n");
    
    gtk_statusbar_pop(GTK_STATUSBAR(session->status_bar),
		      session->phone_context_id);
    gtk_statusbar_push(GTK_STATUSBAR(session->status_bar),
		       session->phone_context_id,
		       buf);
    free(buf);
    free(timediff);
  }
  
  if (session->state == STATE_PLAYBACK) {
    char *timediff = timediff_str(time(NULL),
	                          session->effect_playback_start_time);
    char *buf;

    if (0 > asprintf(&buf, "%s %s",
		     state_data[session->state].status_bar, timediff))
      fprintf(stderr, "Warning: timeout_callback: asprintf error.\n");
    
    gtk_statusbar_pop(GTK_STATUSBAR(session->status_bar),
		      session->phone_context_id);
    gtk_statusbar_push(GTK_STATUSBAR(session->status_bar),
		       session->phone_context_id,
		       buf);
    free(buf);
    free(timediff);
  }
  
  return TRUE; /* call it again */
}


/*
 * some GUI tools
 */

/*
 * return a dialog window with a (big) label and an ok button to close
 * (good for displaying a note)
 *
 * justification: justification of label (e.g. GTK_JUSTIFY_LEFT)
 *
 * NOTE: caller has to show the window itself with gtk_widget_show()
 *       and maybe want to make it modal with
 *         gtk_window_set_modal(GTK_WINDOW(window), TRUE);
 */
GtkWidget *ok_dialog_get(char *title, char *contents,
			 GtkJustification justification) {
  GtkWidget *window;
  GtkWidget *button_box;
  GtkWidget *button;
  GtkWidget *label;

  window = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(window), title);

  /* vbox area */
  gtk_container_set_border_width(GTK_CONTAINER(GTK_DIALOG(window)->vbox), 0);
  label = gtk_label_new(contents);
  gtk_label_set_justify(GTK_LABEL(label), justification);
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(window)->vbox), label,
		     TRUE, FALSE, 0);
  gtk_misc_set_padding(GTK_MISC(label), 10, 10);
  gtk_widget_show(label);

  /* action area */
  button_box = gtk_hbutton_box_new();
  gtk_container_add(GTK_CONTAINER(GTK_DIALOG(window)->action_area),
		    button_box);
  gtk_widget_show(button_box);

  /* OK button */
  button = gtk_button_new_with_label(_("OK"));
  gtk_box_pack_start_defaults(GTK_BOX(button_box), button);
  gtk_signal_connect_object(GTK_OBJECT(button), "clicked",
			    GTK_SIGNAL_FUNC(gtk_widget_destroy),
			    GTK_OBJECT(window));
  gtk_widget_show(button);

  /* caller has to show the window itself */

  return window;
}

/*
 * shortcut to display a note about audio devices not available
 */
void show_audio_error_dialog(void) {
  GtkWidget *dialog;

  dialog = ok_dialog_get(_("ANT Note"),
			 _("Can't open audio device.\n"
			   "Please stop other applications using\n"
			   "the audio device(s) or check your\n"
			   "device settings and try again."),
			 GTK_JUSTIFY_CENTER);
  gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
  gtk_widget_show(dialog);
}

/**********************
 * some GUI callbacks *
 **********************/

/*
 * File Menu entries
 */

struct info_row_t {
  gchar *tag;
  gchar *value;
};

/* Info window */
static void cb_info_window(GtkWidget *widget _U_, gpointer data,
			   guint action _U_) {
  session_t *session = (session_t *) data;
  int inactive = session->option_release_devices &&
    (session->state == STATE_READY || session->state == STATE_RINGING_QUIET);
  struct info_row_t rows[] = {
    { N_("Sound input device:"), strdup(session->audio_device_name_in)},
    { N_("Input speed:"), inactive ? strdup(_("[inactive]")) :
      ltostr(session->audio_speed_in) },
    { N_("Input sample size (bits):"), inactive ?
       strdup(_("[inactive]")) : ltostr(session->audio_sample_size_in * 8)},
    { N_("Input fragment size (samples):"), inactive ? strdup(_("[inactive]")) :
      ltostr(session->fragment_size_in / session->audio_sample_size_in)},
    { N_("Input channels:"), inactive ? strdup(_("[inactive]")) : ltostr(1) },
    { "", strdup("") },

    { N_("Sound output device:"), strdup(session->audio_device_name_out)},
    { N_("Output speed:"), inactive ? strdup(_("[inactive]")) :
      ltostr(session->audio_speed_out) },
    { N_("Output sample size (bits):"), inactive ?
      strdup(_("[inactive]")) : ltostr(session->audio_sample_size_out*8)},
    { N_("Input fragment size (samples):"), inactive ? strdup(_("[inactive]")) :
      ltostr(session->fragment_size_out / session->audio_sample_size_out)},
    { N_("Output channels:"), inactive ? strdup(_("[inactive]")) : ltostr(1) },
    { "", strdup("") },

    { N_("ISDN device:"), strdup(session->isdn_device_name) },
    { N_("ISDN speed (samples):"), ltostr(8000) },
    { N_("ISDN sample size (bits):"), ltostr(8) },
    { N_("ISDN fragment size (bytes):"), ltostr(255) }
  };
  unsigned int i;

  GtkWidget *window; /* the window itself, actually a GtkDialog */
  GtkWidget *table;  /* for tag->value pairs */
  GtkWidget *label;
  GtkWidget *button_box;
  GtkWidget *button;
  GtkWidget *separator;

  window = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(window), _("ANT Info"));

  /* the info area */
  table = gtk_table_new(sizeof(rows) / sizeof(struct info_row_t), 2, FALSE);
  gtk_container_set_border_width(GTK_CONTAINER(table), 10);
  gtk_table_set_col_spacings(GTK_TABLE(table), 10);
  gtk_container_add(GTK_CONTAINER(GTK_DIALOG(window)->vbox), table);
  gtk_widget_show(table);

  for (i = 0; i < sizeof(rows) / sizeof(struct info_row_t); i++) {
    if (strcmp(rows[i].tag, "")) { /* normal data */
      label = gtk_label_new(_(rows[i].tag));
      gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 1, i, i + 1);
      gtk_misc_set_alignment(GTK_MISC(label), 1, 0.5);
      gtk_widget_show(label);

      label = gtk_label_new(rows[i].value);
      gtk_table_attach_defaults(GTK_TABLE(table), label, 1, 2, i, i + 1);
      gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
      gtk_widget_show(label);
    } else { /* separator */
      separator = gtk_hseparator_new();
      gtk_table_attach(GTK_TABLE(table), separator, 0, 2, i, i + 1,
		       GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, 0, 5);
      gtk_widget_show(separator);
    }
    
    free(rows[i].value); /* free stdrup() mem */
  }

  /* action area */
  button_box = gtk_hbutton_box_new();
  gtk_container_add(GTK_CONTAINER(GTK_DIALOG(window)->action_area),
		    button_box);
  gtk_widget_show(button_box);

  button = gtk_button_new_with_label(_("OK"));
  gtk_box_pack_start_defaults(GTK_BOX(button_box), button);
  gtk_signal_connect_object(GTK_OBJECT(button), "clicked",
			    GTK_SIGNAL_FUNC(gtk_widget_destroy),
			    GTK_OBJECT(window));
  gtk_widget_show(button);
  
  gtk_window_set_modal(GTK_WINDOW(window), TRUE);

  /* show everything */
  gtk_widget_show(window);
}

/*
 * Options menu entries
 */
/*
 * Help menu entries
 */

/* the about menu entry callback */
static void cb_about(GtkWidget *widget _U_, gpointer data _U_, guint action _U_) {
  GtkWidget *window;
  GtkWidget *button_box;
  GtkWidget *button;
  GtkWidget *label;

  GdkPixmap* pixmap;
  GdkBitmap* mask;
  GtkStyle* style;
  GtkWidget* pixmapwidget;

  char *message;

  window = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(window), _("About ANT"));

  /* vbox area */
  gtk_container_set_border_width(GTK_CONTAINER(GTK_DIALOG(window)->vbox), 0);

  gtk_widget_realize(window);
  style = gtk_widget_get_style(window);
  pixmap = gdk_pixmap_create_from_xpm_d(window->window,
                                        &mask,
                                        NULL,
                                        (gchar**) aboutlogo_xpm);
  pixmapwidget = gtk_pixmap_new(pixmap, mask);
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(window)->vbox),
                     pixmapwidget, TRUE, TRUE, 0);
  gtk_widget_show(pixmapwidget);

  asprintf(&message, _("ANT (ANT is Not a Telephone) Version %s\n"
		       "Copyright 2002, 2003 Roland Stigge\n\n"
		       "This is an ISDN telephone application\n"
		       "written for GNU/Linux and ISDN4Linux for\n"
		       "communicating via a full duplex soundcard (or\n"
		       "multiple sound devices if you like) and an\n"
		       "audio capable ISDN4Linux ISDN device\n\n"
		       "Contact:\n"
		       "Roland Stigge, stigge@antcom.de\n"
		       "http://www.antcom.de/\n"
		       "Mailing list: ant-phone-devel@nongnu.org"), VERSION);
  label = gtk_label_new(message);
  free(message);
  gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_CENTER);
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(window)->vbox), label,
		     TRUE, FALSE, 0);
  gtk_misc_set_padding(GTK_MISC(label), 10, 10);
  gtk_widget_show(label);

  /* action area */
  button_box = gtk_hbutton_box_new();
  gtk_container_add(GTK_CONTAINER(GTK_DIALOG(window)->action_area),
		    button_box);
  gtk_widget_show(button_box);

  /* OK button */
  button = gtk_button_new_with_label(_("OK"));
  gtk_box_pack_start_defaults(GTK_BOX(button_box), button);
  gtk_signal_connect_object(GTK_OBJECT(button), "clicked",
			    GTK_SIGNAL_FUNC(gtk_widget_destroy),
			    GTK_OBJECT(window));
  gtk_widget_show(button);
  
  gtk_widget_show(window);
}

/* the about menu entry callback */
static void cb_license(GtkWidget *widget _U_, gpointer data _U_, guint action _U_) {
  GtkWidget *window = ok_dialog_get(_("ANT License"),
    _("ANT (ANT is Not a Telephone)\n"
      "Copyright (C) 2002, 2003 Roland Stigge\n"
      "\n"
      "This program is free software; you can redistribute it and/or\n"
      "modify it under the terms of the GNU General Public License\n"
      "as published by the Free Software Foundation; either version 2\n"
      "of the License, or (at your option) any later version.\n"
      "\n"
      "This program is distributed in the hope that it will be useful,\n"
      "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
      "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
      "GNU General Public License for more details.\n"
      "\n"
      "You should have received a copy of the GNU General Public License\n"
      "along with this program; if not, write to the Free Software\n"
      "Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, "
      "USA."), GTK_JUSTIFY_CENTER);

  gtk_widget_show(window);
}

/*
 * get the main menu widget
 *
 * input: window: parent window to integrate accelerators
 * returns: the widget yet to pack
 *
 * NOTE: assumes other parts to be initialized (maybe not shown already)
 */
GtkWidget *get_main_menu(GtkWidget *window, session_t *session) {
  /* The main menu structure */
  GtkItemFactoryEntry main_menu_items[] = {
/*path                  accel.    callb.         cb par. kind           extra */
{_("/Phon_e"),            NULL,     NULL,          0,      "<Branch>",    NULL},
{_("/Phone/_Info Window"),NULL,     cb_info_window,0,      NULL,          NULL},
{_("/Phone/_Line Level Check"),
                          NULL,     llcheck,       0,      NULL,      NULL},
{_("/Phone/"),            NULL,     NULL,          0,      "<Separator>", NULL},
{_("/Phone/_Quit"),       "<alt>X", quit,          0,      NULL,          NULL},

{_("/_View"),             NULL,     NULL,          0,      "<Branch>",    NULL},
{_("/View/_Caller ID Monitor"),
                          NULL,     cid_toggle_cb, 0,      "<CheckItem>", NULL},
{_("/View/_Line Level Meters"),
                          NULL,     llcheck_toggle_cb,0,   "<CheckItem>", NULL},
{_("/View/Control _Pad"), NULL,     controlpad_toggle_cb,0,"<CheckItem>", NULL},

{_("/_Options"),          NULL,     NULL,          0,      "<Branch>",    NULL},
{_("/Options/_Settings"), NULL,     gtksettings_cb,0,      NULL,          NULL},

{_("/_Help"),             NULL,     NULL,          0,      "<LastBranch>",NULL},
{_("/Help/_About"),       NULL,     cb_about,      0,      NULL,          NULL},
{_("/Help/_License"),     NULL,     cb_license,    0,      NULL,          NULL}
    /*        set to session by ..._create_items_ac -^                       */
  };

  GtkItemFactory *item_factory;
  GtkAccelGroup *accel_group;
  char* temp;

  gint nmenu_items = sizeof(main_menu_items) / sizeof(main_menu_items[0]);

  accel_group = gtk_accel_group_new();
  item_factory = gtk_item_factory_new(GTK_TYPE_MENU_BAR, "<main>",
				      accel_group);
  gtk_item_factory_create_items_ac(item_factory, nmenu_items, main_menu_items,
				session, 2);
  gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);
  
  /* save connection to menu item(s), set defaults */
  session->cid_check_menu_item = gtk_item_factory_get_item(item_factory,
		   temp = stripchr(_("/View/_Caller ID Monitor"), '_'));
  free(temp);
  session->llcheck_check_menu_item = gtk_item_factory_get_item(item_factory,
		   temp = stripchr(_("/View/_Line Level Meters"), '_'));
  free(temp);
  session->controlpad_check_menu_item = gtk_item_factory_get_item(item_factory,
		   temp = stripchr(_("/View/Control _Pad"), '_'));
  free(temp);
  session->menuitem_settings = gtk_item_factory_get_item(item_factory,
		   temp = stripchr(_("/Options/_Settings"), '_'));
  free(temp);
  session->menuitem_line_check = gtk_item_factory_get_item(item_factory,
		   temp = stripchr(_("/Phone/_Line Level Check"), '_'));
  free(temp);
  gtk_check_menu_item_set_active(
    GTK_CHECK_MENU_ITEM(session->cid_check_menu_item),
    session->option_show_callerid);
  gtk_check_menu_item_set_active(
    GTK_CHECK_MENU_ITEM(session->llcheck_check_menu_item),
    session->option_show_llcheck);
  gtk_check_menu_item_set_active(
    GTK_CHECK_MENU_ITEM(session->controlpad_check_menu_item),
    session->option_show_controlpad);
  
  return gtk_item_factory_get_widget(item_factory, "<main>");
}

/*
 * called on key pressed in combo box entry
 */
static gint entry_key_cb(GtkWidget *entry, GdkEventKey *event, gpointer data) {
  session_t *session = (session_t *) data;

  if (event->keyval == GDK_KP_Enter) { /* catch keyboard keypad Enter */
    gtk_button_clicked(GTK_BUTTON(session->pick_up_button));
    /* the keyboard keypad Enter generates an unneeded character, so
       discard it: */
    gtk_signal_emit_stop_by_name(GTK_OBJECT(entry), "key-press-event");
    return TRUE; /* event handled */
  } else {
    return FALSE;
  }
}

/*
 * Get the basic dial box with entry and dial / hang up buttons
 * sets dial box members in session
 * NOTE: caller has to gtk_widget_show it itself
 */
GtkWidget *get_dial_box(session_t *session) {
  GtkWidget *frame;
  GtkWidget *hbox;
  GtkWidget *label;
  GtkWidget *label_hbox;
  GdkPixmap *pixmap;
  GdkBitmap *mask;
  GtkStyle *style;
  GtkWidget *pixmapwid;

  frame = gtk_frame_new(_("Dialing"));
  gtk_container_set_border_width(GTK_CONTAINER(frame), 8);

  /* dial hbox */
  hbox = gtk_hbox_new(FALSE, 10);
  gtk_container_add(GTK_CONTAINER(frame), hbox);
  gtk_container_set_border_width(GTK_CONTAINER(hbox), 8);
  gtk_widget_show(hbox);

  /* dial label */
  label = gtk_label_new(_("Number:"));
  gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
  gtk_widget_show(label);

  /* dial number combo box */
  session->dial_number_box = gtk_combo_new();
  gtk_combo_set_popdown_strings(GTK_COMBO(session->dial_number_box),
				session->dial_number_history);
  gtk_combo_set_use_arrows_always(GTK_COMBO(session->dial_number_box), TRUE);
  gtk_widget_set_size_request(session->dial_number_box, 180, -1);
  gtk_box_pack_start(GTK_BOX(hbox), session->dial_number_box,
		     TRUE, TRUE, 0);
  gtk_widget_show(session->dial_number_box);
  gtk_signal_disconnect(GTK_OBJECT(GTK_COMBO(session->dial_number_box)->entry),
			GTK_COMBO(session->dial_number_box)->activate_id);

  /* pick up button */
  session->pick_up_button = gtk_button_new();
  gtk_box_pack_start(GTK_BOX(hbox), session->pick_up_button, FALSE, FALSE, 0);
  gtk_signal_connect(GTK_OBJECT(session->pick_up_button), "clicked",
		     GTK_SIGNAL_FUNC(gtk_handle_pick_up_button),
		     (gpointer) session);
  gtk_widget_show(session->pick_up_button);

  /* activate dial button when pressing enter in entry widget */
  gtk_signal_connect_object(GTK_OBJECT(GTK_COMBO(session->dial_number_box)
				       ->entry),
			    "activate",
			    GTK_SIGNAL_FUNC(gtk_button_clicked),
			    GTK_OBJECT(session->pick_up_button));
  /* handle special keys */
  gtk_signal_connect(GTK_OBJECT(GTK_COMBO(session->dial_number_box)->entry),
                     "key-press-event", GTK_SIGNAL_FUNC(entry_key_cb), session);

  /* pick up button hbox */
  label_hbox = gtk_hbox_new(FALSE, 0);
  gtk_container_add(GTK_CONTAINER(session->pick_up_button), label_hbox);
  gtk_widget_show(label_hbox);

  /* pick up button symbol */
  style = gtk_widget_get_style(session->main_window);
  pixmap = gdk_pixmap_create_from_xpm_d(session->main_window->window,
					&mask,
					&style->bg[GTK_STATE_NORMAL],
					(gchar **) pickup_xpm);

  pixmapwid = gtk_pixmap_new(pixmap, mask);
  gtk_box_pack_start(GTK_BOX(label_hbox), pixmapwid, FALSE, FALSE, 2);
  gtk_widget_show(pixmapwid);
  
  /* pick up button label */
  session->pick_up_label = gtk_label_new(NULL);
  gtk_box_pack_start(GTK_BOX(label_hbox), session->pick_up_label,
		     TRUE, FALSE, 16); /* expand but fill up outside label */
  gtk_widget_show(session->pick_up_label);

   /* hang up button */
  session->hang_up_button = gtk_button_new();
  gtk_box_pack_start(GTK_BOX(hbox), session->hang_up_button, FALSE, FALSE, 0);
  gtk_signal_connect(GTK_OBJECT(session->hang_up_button), "clicked",
		     GTK_SIGNAL_FUNC(gtk_handle_hang_up_button),
		     (gpointer) session);
  gtk_widget_show(session->hang_up_button);

  /* pick up button hbox */
  label_hbox = gtk_hbox_new(FALSE, 0);
  gtk_container_add(GTK_CONTAINER(session->hang_up_button),
		    label_hbox);
  gtk_widget_show(label_hbox);

  /* pick up button symbol*/
  /* style = gtk_widget_get_style(session->main_window); */
    /* (already done for pickup button) */

  pixmap = gdk_pixmap_create_from_xpm_d(session->main_window->window,
					&mask,
					&style->bg[GTK_STATE_NORMAL],
					(gchar **) hangup_xpm);

  pixmapwid = gtk_pixmap_new(pixmap, mask);
  gtk_box_pack_start(GTK_BOX(label_hbox), pixmapwid, FALSE, FALSE, 2);
  gtk_widget_show(pixmapwid);
  
  /* hang up button label */
  session->hang_up_label = gtk_label_new(NULL);
  gtk_box_pack_start(GTK_BOX(label_hbox), session->hang_up_label,
		     TRUE, FALSE, 16); /* expand but fill up outside label */
  gtk_widget_show(session->hang_up_label);
  
  return frame;
}

/*
 * main function for gtk GUI
 *
 * returns int to be returned from main()
 */
int main_gtk(session_t *session) {
  GList* icon_list = NULL;
  
  GtkWidget *main_window;
  GtkWidget *main_vbox;
  GtkWidget *main_menu;

  GtkWidget *dialbox;
  GtkWidget *cidbox;

  GtkWidget *dummy_label; /* needed to calculate the length of a text label */
  GtkRequisition requisition;

  GtkRcStyle *rc_style;
  GdkColor color;

  /* the main window */
  session->main_window = main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(main_window), "ANT " VERSION);

  /* seems to cause window managers only to use the first one as icon (e.g.FVWM)
  icon_list = g_list_append(icon_list,
          (gpointer) gdk_pixbuf_new_from_xpm_data((const char**) icon16x16));
  icon_list = g_list_append(icon_list,
          (gpointer) gdk_pixbuf_new_from_xpm_data((const char**) icon32x32));
  */
  icon_list = g_list_append(icon_list,
          (gpointer) gdk_pixbuf_new_from_xpm_data((const char**) icon48x48));
  /*
  icon_list = g_list_append(icon_list,
          (gpointer) gdk_pixbuf_new_from_xpm_data((const char**) icon64x64));
  */
  gtk_window_set_icon_list(GTK_WINDOW(session->main_window), icon_list);
  
  gtk_signal_connect(GTK_OBJECT(main_window), "delete_event",
		     GTK_SIGNAL_FUNC(delete_event), (gpointer) session);

  gtk_container_set_border_width(GTK_CONTAINER(main_window), 0);
  gtk_widget_realize(main_window);

  gtk_object_set(GTK_OBJECT(main_window), "allow_shrink", FALSE, NULL);
  gtk_object_set(GTK_OBJECT(main_window), "allow_grow", TRUE, NULL);

  /* set some defaults */
  gtk_hbutton_box_set_spacing_default(16); /* space between buttons */
  gtk_hbutton_box_set_layout_default(GTK_BUTTONBOX_SPREAD);

  /* main vbox */
  main_vbox = gtk_vbox_new(FALSE, 0); /* not homogeneous, spacing = 0 */
  gtk_container_set_border_width(GTK_CONTAINER(main_vbox), 0);
  gtk_container_add(GTK_CONTAINER(main_window), main_vbox);
  gtk_widget_show(main_vbox);

  /* contents of main vbox (wrong order to let menu depend on others): */

  /* the dial hbox */
  dialbox = get_dial_box(session);
  gtk_widget_show(dialbox);

  /* key pad */
  session->controlpad = controlpad_new(session); /* show later */
  
  /* the caller id section */
  cidbox = cid_new(session); /* show later */

  /* status bar */
  session->status_bar = gtk_statusbar_new();
  gtk_statusbar_set_has_resize_grip(GTK_STATUSBAR(session->status_bar), FALSE);
  gtk_widget_show(session->status_bar);
  session->phone_context_id =
    gtk_statusbar_get_context_id(GTK_STATUSBAR(session->status_bar), "phone");
  gtk_statusbar_push(GTK_STATUSBAR(session->status_bar),
		     session->phone_context_id, "");

  /* line level check inside status bar */
  session->llcheck = gtk_vbox_new(FALSE, 0);
  gtk_box_pack_end(GTK_BOX(session->status_bar),
		   session->llcheck, FALSE, FALSE, 0);

  session->llcheck_in =
    llcheck_bar_new(66, 10, 0, 200, 0);
  gtk_box_pack_start(GTK_BOX(session->llcheck),
		   session->llcheck_in, FALSE, FALSE, 0);
  gtk_widget_show(session->llcheck_in);
  session->llcheck_out =
    llcheck_bar_new(66, 10, 200, 0, 0);
  gtk_box_pack_start(GTK_BOX(session->llcheck),
		   session->llcheck_out, FALSE, FALSE, 0);
  gtk_widget_show(session->llcheck_out);

  /* audio warning inside status bar */
  session->audio_warning = gtk_statusbar_new();
  gtk_statusbar_set_has_resize_grip(GTK_STATUSBAR(session->audio_warning),
      FALSE);
  gdk_color_parse("#B00000", &color);
  rc_style = gtk_rc_style_new();
  rc_style->fg[GTK_STATE_NORMAL] = color;
  rc_style->color_flags[GTK_STATE_NORMAL] |= GTK_RC_FG;
  gtk_widget_modify_style(GTK_STATUSBAR(session->audio_warning)->label,
			  rc_style);
  gtk_rc_style_unref(rc_style);
  gtk_box_pack_end(GTK_BOX(session->status_bar),
		   session->audio_warning, FALSE, FALSE, 0);
  session->audio_context_id =
    gtk_statusbar_get_context_id(GTK_STATUSBAR(session->audio_warning),
				 "audio");
  gtk_statusbar_push(GTK_STATUSBAR(session->audio_warning),
		     session->audio_context_id, "");
  /* mute warning inside status bar */
  session->muted_warning = gtk_statusbar_new();
  gtk_statusbar_set_has_resize_grip(GTK_STATUSBAR(session->muted_warning),
      FALSE);
  gdk_color_parse("#008000", &color);
  rc_style = gtk_rc_style_new();
  rc_style->fg[GTK_STATE_NORMAL] = color;
  rc_style->color_flags[GTK_STATE_NORMAL] |= GTK_RC_FG;
  gtk_widget_modify_style(GTK_STATUSBAR(session->muted_warning)->label,
			  rc_style);
  gtk_rc_style_unref(rc_style);
  gtk_box_pack_end(GTK_BOX(session->status_bar),
		   session->muted_warning, FALSE, FALSE, 0);
  session->muted_context_id =
    gtk_statusbar_get_context_id(GTK_STATUSBAR(session->muted_warning),
				 "muted");
  gtk_statusbar_push(GTK_STATUSBAR(session->muted_warning),
		     session->muted_context_id, _("MUTED"));
  dummy_label = gtk_label_new(_("MUTED"));
  gtk_widget_show(dummy_label);
  gtk_widget_size_request(dummy_label, &requisition);
  gtk_widget_set_size_request(session->muted_warning,
      requisition.width + 4, -1);

  /* main menu bar */
  main_menu = get_main_menu(main_window, session); /* main menu */
  gtk_widget_show(main_menu);

  /* pack items into main vbox (after main menu bar creation) */
  gtk_box_pack_start(GTK_BOX(main_vbox), main_menu, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(main_vbox), dialbox, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(main_vbox), session->controlpad, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(main_vbox), cidbox, TRUE, TRUE, 0);
  gtk_box_pack_end(GTK_BOX(main_vbox), session->status_bar, FALSE, FALSE, 2);

  /* show items on demand */
  if (GTK_CHECK_MENU_ITEM(session->controlpad_check_menu_item)->active)
    gtk_widget_show(session->controlpad);
  if (GTK_CHECK_MENU_ITEM(session->cid_check_menu_item)->active)
    gtk_widget_show(cidbox);
  if (GTK_CHECK_MENU_ITEM(session->llcheck_check_menu_item)->active)
    gtk_widget_show(session->llcheck);
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(session->mute_button)))
    gtk_widget_show(session->muted_warning);

  /* show everything */
  gtk_widget_show(main_window);

  settings_history_read(session); /* get history from home dir dotfile */
  gtk_clist_freeze(GTK_CLIST(session->cid_list));
  settings_callerid_read(session); /* get callerid history */
  if (session->option_calls_merge)
    cid_calls_merge(session); /* merge history from isdnlog */
  gtk_clist_thaw(GTK_CLIST(session->cid_list));
  cid_jump_to_end(session);
  cid_jump_to_end(session);

  /* connect isdn and sound input handlers to gdk pseudo select */
  session_io_handlers_start(session);

  /* set up for initial state */
  session_set_state(session, STATE_READY); /* state is already in session */

  /* set up additional handlers */
  gtk_timeout_add(TIMER_DELAY, timeout_callback, (gpointer) session);
  signal(SIGINT, &terminate_signal_callback);
  signal(SIGTERM, &terminate_signal_callback);

  /* gtk main loop */
  gtk_main();
  
  /* some deinit */
  session_io_handlers_stop(session);

  return 0;
}

