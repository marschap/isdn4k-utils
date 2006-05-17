/*
 * caller id related functions
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
#ifdef HAVE_STDLIB_H
  #include <stdlib.h>
#endif
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <glob.h>
#include <fcntl.h>

/* GTK */
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

/* own header files */
#include "globals.h"
#include "callerid.h"
#include "session.h"
#include "util.h"
#include "isdn.h"

/* graphical symbols */
#include "in.xpm"
#include "out.xpm"
#include "record.xpm"

/*
 * returns a new data structure (for association with a row)
 * filled with default values
 */
static gpointer cid_row_new(void) {
  struct cid_row_t *rowdata =
    (struct cid_row_t *) malloc(sizeof(struct cid_row_t));
  
  rowdata->marked_unanswered = 0;
  return (gpointer) rowdata;
}

/*
 * callback to be called for additional row data on destruction of row
 */
static void cid_row_destroy(gpointer rowdata) {
  if (debug > 1)
    fprintf(stderr, "debug: destroying rowdata at %p.\n", rowdata);
  free(rowdata);
}

/* called when caller id monitor state check button is toggled */
void cid_toggle_cb(GtkWidget *widget _U_, gpointer data, guint action _U_) {
  session_t *session = (session_t *) data;

  if (GTK_CHECK_MENU_ITEM (session->cid_check_menu_item)->active) {
    gtk_widget_show(session->cid);
    session->option_show_callerid = 1;
  } else {
    gtk_widget_hide(session->cid);
    session->option_show_callerid = 0;
    gtk_window_resize(GTK_WINDOW(session->main_window), 1, 1);
  }
}

/*
 * Callback: called when a button was clicked in row delete dialog
 */
static void cid_delete_response_cb(GtkWidget* widget, gint response_id,
                                   gpointer data)
{
  session_t* session = (session_t*) data;
  guint row = GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(widget), "row"));
  char* filename = (char*) g_object_get_data(G_OBJECT(widget), "filename");
  GtkWidget* checkbutton =
    (GtkWidget*) g_object_get_data(G_OBJECT(widget), "checkbutton");

  if (response_id == GTK_RESPONSE_OK) {
    if (filename &&
	gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton)))
    {
      unlink(filename);
    }
    cid_mark_row(session, row, FALSE); /* to count unanswered calls */
    session->cid_num--;
    /* decrease before removal: "select-row" signal submission -> other cb */
    gtk_clist_remove(GTK_CLIST(session->cid_list), row);
  }
  
  if (filename) free(filename);
  gtk_widget_destroy(widget);
}

/*
 * called on cid delete request
 *
 * -> deletes specified row without confirmation
 */
static void cid_request_delete(GtkWidget *widget _U_, gpointer data,
                               guint row) {
  session_t* session = (session_t *) data;
  GtkWidget* vbox;
  GtkWidget* label;
  GtkWidget* checkbutton;
  char *filename = cid_get_record_filename(session, row);
  
  GtkWidget* dialog = gtk_dialog_new_with_buttons(_("Delete Entry"),
                         GTK_WINDOW(session->main_window),
			 GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL,
		         GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			 GTK_STOCK_OK, GTK_RESPONSE_OK,
			 NULL);

  vbox = gtk_vbox_new(FALSE, 16);
  gtk_container_set_border_width(GTK_CONTAINER(vbox), 16);
  gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(dialog)->vbox), vbox);
  gtk_widget_show(vbox);
  
  label = gtk_label_new(_("Are you sure you want to\ndelete this entry?"));
  gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_CENTER);
  gtk_box_pack_start_defaults(GTK_BOX(vbox), label);
  gtk_widget_show(label);

  checkbutton = gtk_check_button_new_with_label(_("Delete recording"));
  if (filename) { /* default: checked */
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton), TRUE);
  } else { /* if file doesn't exist, grey out */
    gtk_widget_set_sensitive(checkbutton, FALSE);
  }
  gtk_box_pack_start_defaults(GTK_BOX(vbox), checkbutton);
  gtk_widget_show(checkbutton);

  g_object_set_data(G_OBJECT(dialog), "row", GUINT_TO_POINTER(row));
  g_object_set_data(G_OBJECT(dialog), "filename", (gpointer) filename);
  g_object_set_data(G_OBJECT(dialog), "checkbutton", (gpointer) checkbutton);
  g_signal_connect(dialog, "response", G_CALLBACK(cid_delete_response_cb),
                   session);
  gtk_widget_show(dialog);
}

/*
 * called on key pressed in cid list
 */
static gint cid_key_cb(GtkWidget *widget, GdkEventKey *event, gpointer data) {
  
  switch (event->keyval) {
    case GDK_Delete:     /* Delete dialog */
    case GDK_KP_Delete:
      /* portability: GtkCList.selection is private! */
      if (GTK_CLIST(((session_t *) data)->cid_list)->selection) {
        cid_request_delete(widget, data,
	    GPOINTER_TO_INT(GTK_CLIST(((session_t *) data)->cid_list)->selection->data));
      }
      return TRUE; /* event handled */
    default:
      return FALSE;
  }
}

/*
 * Callback: called on playback request
 */
static void cid_playback(GtkWidget *widget _U_, gpointer data, guint row) {
  session_t *session = (session_t *) data;

  session->effect_filename = cid_get_record_filename(session, row);
  session->effect_playback_start_time = time(NULL);
  session_set_state(session, STATE_PLAYBACK);
}

/*
 * Callback: called on "OK" click in "Save as..." file selection
 */
static void cid_save_as_filename_cb(GtkWidget *fs) {
  char* sourcename = g_object_get_data(G_OBJECT(fs), "sourcename");
  const char* destbase =
    gtk_file_selection_get_filename(GTK_FILE_SELECTION(fs));
  if (*destbase && *basename(destbase)) {
    /* copy if something was entered */
    char* destination;
    char* extension = filename_extension(sourcename);
    if (0 <= asprintf(&destination, "%s.%s", destbase, extension)) {
      /* actually copy if source and destination known */
      int buffer_size = 65536;
      char* buffer = (char*) malloc(buffer_size);
      if (buffer) {
	int fd_in = open(sourcename, O_RDONLY);
	if (fd_in != -1) {
	  int fd_out = open(destination, O_WRONLY | O_CREAT | O_TRUNC,
	                    S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
	  if (fd_out != -1) {
	    int just_read;
	    do {
	      int to_write = just_read = read(fd_in, buffer, buffer_size);
	      int written = 1;
	      char* buffer_index = buffer;
	      while (to_write && written) {
		written = write(fd_out, buffer_index, to_write);
		to_write -= written;
		buffer_index += written;
	      }
	    } while (just_read);
	    if (close(fd_out)) {
	      fprintf(stderr, "Error on closing destination file.\n");
	    }
	  } else {
	    fprintf(stderr,
		    "Error on destination file (%s) open().\n", destination);
	  }
	  if (close(fd_in)) {
	    fprintf(stderr, "Error on source file.\n");
	  }
	} else {
	  fprintf(stderr,
	          "Error on source file (%s) open().\n", sourcename);
	}
	free(buffer);
      } else {
	fprintf(stderr, "Error on malloc().\n");
      }
      free(destination);
    } else {
      fprintf(stderr, "Error on asprintf().\n");
    }
  }
  free(sourcename);
  gtk_widget_destroy(GTK_WIDGET(fs));
}

/*
 * Callback: called on "Save as..." request
 */
static void cid_save_as(GtkWidget *widget _U_, gpointer data, guint row) {
  session_t* session = (session_t*) data;
  char* sourcename = cid_get_record_filename(session, row);
  char* extension = filename_extension(sourcename);

  if (extension) {
    GtkWidget* fs;
    char* title;
    
    if (0 > asprintf(&title, _("Enter the base filename for %s file"),
	             extension))
    {
      fprintf(stderr, "Error on asprintf().\n");
      return;
    }
    fs = gtk_file_selection_new(title);
    free(title);
    g_signal_connect_swapped(GTK_OBJECT(GTK_FILE_SELECTION(fs)->cancel_button),
                             "clicked", G_CALLBACK(gtk_widget_destroy),
			     (gpointer) fs);
    g_object_set_data(G_OBJECT(fs), "sourcename", sourcename);
    g_signal_connect_swapped(GTK_OBJECT(GTK_FILE_SELECTION(fs)->ok_button),
                           "clicked", G_CALLBACK(cid_save_as_filename_cb), fs);
    gtk_widget_show(fs);
  } else {
    fprintf(stderr, "Error: no filename extension found.\n");
  }
}

/*
 * Callback: called if something happened in the delete recording dialog
 */
static void cid_delete_rec_response_cb(GtkWidget* widget, gint response_id,
                                       gpointer data)
{
  session_t *session = (session_t *) data;

  if (response_id == GTK_RESPONSE_OK) {
    char* temp;
    guint row = GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(widget), "row"));
    unlink(temp = cid_get_record_filename(session, row));
    free(temp);
    cid_row_mark_record(session, row);
  }
  gtk_widget_destroy(widget);
}

/*
 * Callback: called on delete recording request
 */
static void cid_delete_rec(GtkWidget *widget _U_, gpointer data, guint row) {
  session_t *session = (session_t *) data;

  GtkWidget* dialog = gtk_message_dialog_new(GTK_WINDOW(session->main_window),
      GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_WARNING,
      GTK_BUTTONS_OK_CANCEL, _("Do you really want to delete this recording?"));
  g_signal_connect(dialog, "response", G_CALLBACK(cid_delete_rec_response_cb),
                   session);
  gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
  g_object_set_data(G_OBJECT(dialog), "row", GINT_TO_POINTER(row));
  gtk_widget_show(dialog);
}

/*
 * called on mouse button pressed in cid list
 */
static gint cid_mouse_cb(GtkWidget *widget _U_,
                         GdkEventButton *event, gpointer data) {
  session_t *session = (session_t *) data;
  
  if (event->button == 3) { /* popup menu */
    gint row;
    gint column;

    if (gtk_clist_get_selection_info(GTK_CLIST(session->cid_list),
	                             event->x, event->y, &row, &column)) {

      GtkItemFactoryEntry menu_items[] = {
/*path                   accel. callb.         cb param. kind           extra */
{_("/_Playback"),        NULL,  cid_playback,  row,      "",            NULL},
{_("/_Save as..."),      NULL,  cid_save_as,   row,      "",            NULL},
{_("/Delete _Recording"),NULL,  cid_delete_rec,row,      "",            NULL},
{  "/Sep",               NULL,  NULL,          0,        "<Separator>", NULL},
{_("/_Delete Row"),      NULL,  cid_request_delete, row, "",            NULL}
      };
      GtkMenu *menu = GTK_MENU(gtk_menu_new());
      GtkItemFactory *item_factory;
      GtkAccelGroup *accel_group;

      gint nmenu_items = sizeof(menu_items) / sizeof(menu_items[0]);

      GtkWidget *playback_item;
      GtkWidget *save_as_item;
      GtkWidget *delete_record_item;
      GtkWidget *delete_item;
      char* fn;
      char* temp;
  
      accel_group = gtk_accel_group_new();
      item_factory = gtk_item_factory_new(GTK_TYPE_MENU, "<popup>",
	                                  accel_group);
      gtk_item_factory_create_items_ac(item_factory, nmenu_items, menu_items,
				   session, 2);
      
      if (!(playback_item = gtk_item_factory_get_item(item_factory,
	  temp = stripchr(_("/_Playback"), '_'))))
        fprintf(stderr, "Error getting playback_item.\n");
      free(temp);
      if (!(save_as_item = gtk_item_factory_get_item(item_factory,
	  temp = stripchr(_("/_Save as..."), '_'))))
        fprintf(stderr, "Error getting save_as_item.\n");
      free(temp);
      if (!(delete_record_item = gtk_item_factory_get_item(item_factory,
	    temp = stripchr(_("/Delete _Recording"), '_'))))
        fprintf(stderr, "Error getting delete_record_item_item.\n");
      free(temp);
      if (!(delete_item = gtk_item_factory_get_item(item_factory,
	  temp = stripchr(_("/_Delete Row"), '_'))))
        fprintf(stderr, "Error getting delete_item.\n");
      free(temp);

      if (!(session->state == STATE_READY &&
	    (fn = cid_get_record_filename(session, row)))) {
        gtk_widget_set_sensitive(playback_item, FALSE);
        gtk_widget_set_sensitive(save_as_item, FALSE);
        gtk_widget_set_sensitive(delete_record_item, FALSE);
      } else {
	free(fn);
      }
  
      menu = GTK_MENU(gtk_item_factory_get_widget(item_factory, "<popup>"));
      gtk_menu_set_accel_group(menu, accel_group);

      gtk_menu_popup(menu, NULL, NULL, NULL, NULL, event->button, event->time);
    }
    return TRUE; /* event handled */
  } else {
    return FALSE;
  }
}

/*
 * called on row select (to unmark lines)
 */
static void cid_select_row_cb(GtkCList *list _U_, gint row, gint column _U_,
                              GdkEventButton *button _U_, gpointer data) {
  session_t *session = (session_t *) data;

  if (row < session->cid_num)
    cid_mark_row(session, row, FALSE);
}

/*
 * Returns the Caller ID section as GtkWidget,
 *   sets CID related members in session
 * NOTE: caller has to gtk_widget_show() this widget
 */
GtkWidget *cid_new(session_t *session) {
  GtkWidget *frame;           /* frame with comment */
  GtkWidget *scrolled_window; /* the home for the clist */
  GtkWidget *clist;           /* the list itself */
  GtkStyle *style;            /* needed for preparing symbols */
  GtkWidget *pixmapwidget;    /* record symbol */
  gchar *titles[CID_COL_NUMBER];

  style = gtk_widget_get_style(session->main_window);
  frame = gtk_frame_new(_("Caller ID"));

  titles[CID_COL_FLAGS   ] = "";
  titles[CID_COL_TIME    ] = _("Date/Time");
  titles[CID_COL_TYPE    ] = _("Type");
  titles[CID_COL_FROM    ] = _("From");
  titles[CID_COL_TO      ] = _("To");
  titles[CID_COL_DURATION] = _("Duration");
  
  gtk_container_set_border_width(GTK_CONTAINER(frame), 8);

  /* the scrolled window: no special adjustments */
  scrolled_window = gtk_scrolled_window_new(NULL, NULL);
  /* always vertical scroll bar, horizontal... well... automatic means no ;) */
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
				 GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
  gtk_container_add(GTK_CONTAINER(frame), scrolled_window);
  gtk_container_set_border_width(GTK_CONTAINER(scrolled_window), 8);
  gtk_widget_show(scrolled_window);

  /* the list widget */
  clist = gtk_clist_new_with_titles(CID_COL_NUMBER, titles);
  session->symbol_record_pixmap = gdk_pixmap_create_from_xpm_d(
					 session->main_window->window,
					 &session->symbol_record_bitmap,
					 &style->bg[GTK_STATE_NORMAL],
					 (gchar **) record_xpm);
  pixmapwidget = gtk_pixmap_new(session->symbol_record_pixmap,
                                session->symbol_record_bitmap);
  gtk_clist_set_column_widget(GTK_CLIST(clist), CID_COL_FLAGS, pixmapwidget);
  
  /* default: GTK_SELECTION_MULTIPLE: */
  gtk_clist_set_selection_mode(GTK_CLIST(clist), GTK_SELECTION_BROWSE);

  gtk_clist_set_shadow_type(GTK_CLIST(clist), GTK_SHADOW_IN);
  gtk_clist_set_column_width(GTK_CLIST(clist), CID_COL_FLAGS, 14);
  gtk_clist_set_column_width(GTK_CLIST(clist), CID_COL_TIME, 128);
  gtk_clist_set_column_width(GTK_CLIST(clist), CID_COL_TYPE, 30);
  gtk_clist_set_column_width(GTK_CLIST(clist), CID_COL_FROM, 100);
  gtk_clist_set_column_width(GTK_CLIST(clist), CID_COL_TO, 100);
  gtk_clist_set_column_width(GTK_CLIST(clist), CID_COL_DURATION, 80);
  /*gtk_clist_set_column_width(GTK_CLIST(clist), 5, 80);*/
  gtk_container_add(GTK_CONTAINER(scrolled_window), clist);
  gtk_clist_column_titles_show(GTK_CLIST(clist));
  gtk_widget_set_size_request(clist, 500, 128);

  gtk_signal_connect(GTK_OBJECT(clist), "key-press-event",
                     GTK_SIGNAL_FUNC(cid_key_cb), session);
  gtk_signal_connect(GTK_OBJECT(clist), "button-press-event",
                     GTK_SIGNAL_FUNC(cid_mouse_cb), session);
  gtk_signal_connect(GTK_OBJECT(clist), "select-row",
                     GTK_SIGNAL_FUNC(cid_select_row_cb), session);

  gtk_widget_show(clist);

  /* set up IN/OUT symbols */
  session->symbol_in_pixmap = gdk_pixmap_create_from_xpm_d(
				 session->main_window->window,
				 &session->symbol_in_bitmap,
				 &style->bg[GTK_STATE_NORMAL],
				 (gchar **) in_xpm);
  session->symbol_out_pixmap = gdk_pixmap_create_from_xpm_d(
				 session->main_window->window,
				 &session->symbol_out_bitmap,
				 &style->bg[GTK_STATE_NORMAL],
				 (gchar **) out_xpm);
  session->cid = frame;
  session->cid_list = clist;
  session->cid_scrolled_window = scrolled_window;

  return frame;
}

/*
 * select last item in cid list and adjust view to end of list
 */
void cid_jump_to_end(session_t *session) {
  GtkAdjustment *adj;

  gtk_clist_select_row(GTK_CLIST(session->cid_list),
		       session->cid_num - 1, 0);

  adj = gtk_scrolled_window_get_vadjustment(
	  GTK_SCROLLED_WINDOW(session->cid_scrolled_window));
  gtk_adjustment_set_value(adj, fmax(adj->lower, adj->upper - adj->page_size));
}

/*
 * remove unneeded entries from caller id list (shorten log)
 */
void cid_normalize(session_t *session) {
  while (session->cid_num_max && session->cid_num > session->cid_num_max) {
    gtk_clist_remove(GTK_CLIST(session->cid_list), 0);
    session->cid_num--;
  }
}

/*
 * returns a dynamically allocated string in cid conformant time format
 * caller has got to take care of freeing this memory after usage
 */
static char *cid_timestring(time_t t) {
  char *date = (char *) malloc(20);
  int len;
  
  date[0] = '\1';
  len = strftime(date, 20, "%Y-%m-%d %H:%M:%S", localtime(&t));
  if (len == 0 && date[0] != '\0') {
    fprintf(stderr, "cid: Error calculating time with strftime.\n");
    return NULL;
  }

  return date;
}

/*
 * insert new entry at end of list
 *
 * from == NULL, if still unknown
 */
void cid_add_line(session_t *session,
		  enum call_type_t ct, gchar *from, gchar *to) {
  gchar *line[CID_COL_NUMBER];
  char *typestring;

  GdkPixmap *pixmap;
  GdkBitmap *bitmap;
 
  /* First: date */
  line[CID_COL_TIME] = cid_timestring(session->ring_time);

  line[CID_COL_FLAGS] = "";
  line[CID_COL_TYPE] = ""; /* call type */

  if (from == NULL) /* the from field */
    line[CID_COL_FROM] = "???";
  else
    line[CID_COL_FROM] = from;
  line[CID_COL_TO] = to;

  line[CID_COL_DURATION] = "???"; /* duration */

  /* create line */
  session->cid_num = gtk_clist_append(GTK_CLIST(session->cid_list), line) + 1;
  gtk_clist_set_row_data_full(GTK_CLIST(session->cid_list),
                              session->cid_num - 1,
			      cid_row_new(), cid_row_destroy);

  cid_normalize(session);

  /* actually set pixmap */
  if (ct == CALL_IN) {
    pixmap = session->symbol_in_pixmap;
    bitmap = session->symbol_in_bitmap;
    typestring = "IN";
  } else {
    pixmap = session->symbol_out_pixmap;
    bitmap = session->symbol_out_bitmap;
    typestring = "OUT";
  }
  gtk_clist_set_pixtext(GTK_CLIST(session->cid_list), session->cid_num - 1 ,
                        CID_COL_TYPE, typestring, 50, pixmap, bitmap);

  cid_jump_to_end(session);

  /* clean up */
  free(line[CID_COL_TIME]);
}

/*
 * set "date" field in last line
 */
void cid_set_date(session_t *session, time_t date) {
  if (date) {
    char *temp;
    gtk_clist_set_text(GTK_CLIST(session->cid_list),
	               session->cid_num - 1, CID_COL_TIME,
		       temp = cid_timestring(date));
    free(temp);
  }
}

/*
 * set "from" field in last line
 */
void cid_set_from(session_t *session, gchar *from) {
  if (from)
    gtk_clist_set_text(GTK_CLIST(session->cid_list),
		       session->cid_num - 1, CID_COL_FROM, from);
}

/*
 * complete last line with "duration"
 *
 * if message == NULL, current time will be used to calculate a duration
 * if message != NULL, this string will be displayed in the Duration field (#4)
 */
void cid_set_duration(session_t *session, gchar *message) {
  char *buf = timediff_str(time(NULL), session->vcon_time);
  
  if (message)
    gtk_clist_set_text(GTK_CLIST(session->cid_list), session->cid_num - 1,
	               CID_COL_DURATION, message);
  else
    gtk_clist_set_text(GTK_CLIST(session->cid_list), session->cid_num - 1,
	               CID_COL_DURATION, buf);
  free(buf);
}

/*
 * Add a line to clist (e.g. from saved caller id file)
 */
void cid_add_saved_line(session_t *session, char *date, char *type,
			char *from, char *to, char *duration) {
  if (debug > 1)
    fprintf(stderr, "Caller ID add:\n"
	    "Date: |%s|, Type: |%s|, From: |%s|, To: |%s|, Dur: |%s|\n",
            date, type, from, to, duration);
  
  cid_add_line(session,
	       !strncmp(type, "IN", 2) ? CALL_IN : CALL_OUT, from, to);
  gtk_clist_set_text(GTK_CLIST(session->cid_list),
		     session->cid_num - 1, CID_COL_TIME, date);
  gtk_clist_set_text(GTK_CLIST(session->cid_list),
		     session->cid_num - 1, CID_COL_DURATION, duration);

  cid_row_mark_record(session, session->cid_num - 1);
  
}

/*
 * Merges isdnlog data into existing cid list
 */
void cid_calls_merge(session_t *session) {
  int low, high, mid; /* bounds and helper index */
  time_t start = time(NULL)
                        - 60 * 60 * 24 * session->option_calls_merge_max_days;
  int callstime; /* time of calls file line */
  char *cidstr; /* time of cid_list entry */
  size_t linelength = 100; /* buffer size */ 
  char *line = (char *) malloc (linelength); /* initial buffer for getline() */
  FILE *f = NULL;

  char from[41]; /* data to read from calls file */
  char to[41];
  int duration;
  int date;
  char type[4];
  
  char *temp;
  int rel;
  int cidline; /* index into cid list */
  gchar *linearr[CID_COL_NUMBER]; /* line to insert to cid_list */
  GdkPixmap *pixmap;
  GdkBitmap *bitmap;

  char *cache; /* tracks successive dates */

  char* calls_filename;
  
  /* try to find isdnlog data file */
  calls_filename = isdn_get_calls_filename();
  if (calls_filename && (f = fopen(calls_filename, "r"))) {
    if (debug) {
      fprintf(stderr, "Using %s as source for isdnlog data.\n", calls_filename);
    }
    if (session->option_calls_merge_max_days) {
      /* binary search on the file for the desired starting time if needed */
      if (debug >= 3) {
	fprintf(stderr, "Binary search in calls file...\n");
      }
      low = 0;
      fseek(f, 0, SEEK_END);
      high = ftell(f);
      
      while (high - low > 200) {
	if (debug >= 3) {
	  fprintf(stderr, "low = %d, high = %d\n", low, high);
	}
        mid = (low + high) / 2;
        fseek(f, mid, SEEK_SET);
	getline(&line, &linelength, f); /* discard potentially partial line */
	if (-1 != getline(&line, &linelength, f)) {
	  if (!sscanf(line, "%*40[^|]|%*40[^|]|%*40[^|]|%*40[^|]|%*40[^|]|%d",
	              &callstime))
	    break;
	  if (callstime < start)
	    low = mid;
	  else
	    high = mid;
	} else
	  break;
      }
      fseek(f, low, SEEK_SET);
      getline(&line, &linelength, f);
    }
    
    /* read in all remaining lines: merge */
    cidline = 0;
    cache = strdup("");
    while (!feof(f)) {
      /* get new calls line */
      if (-1 == getline(&line, &linelength, f))	break;
      if (sscanf(line,
	         "%*40[^|]|%40[^|]|%40[^|]|%d|%*40[^|]|%d|%*40[^|]|%1c",
	         from, to, &duration, &date, type) != 5) {
	fprintf(stderr, "Warning: Incomplete data input from calls file.\n");
	break;
      }
      if ((temp = strchr(from, ' '))) *temp = '\0';
      if ((temp = strchr(to, ' '))) *temp = '\0';
      if (type[0] == 'I') {
	type[1] = 'N';
	type[2] = '\0';
        pixmap = session->symbol_in_pixmap;
        bitmap = session->symbol_in_bitmap;
      } else {
	type[1] = 'U';
	type[2] = 'T';
	type[3] = '\0';
        pixmap = session->symbol_out_pixmap;
        bitmap = session->symbol_out_bitmap;
      }
      
      /* prepare (text) line to merge */
      linearr[CID_COL_TIME] = cid_timestring((time_t)date);
      linearr[CID_COL_TYPE] = type;
      if (*from)
	linearr[CID_COL_FROM] = from;
      else
	linearr[CID_COL_FROM] = "???";
      if (*to)
	linearr[CID_COL_TO] = to;
      else
	linearr[CID_COL_TO] = "???";
      if (duration < 0) {
	linearr[CID_COL_DURATION] = strdup(_("(UNKNOWN)"));
      } else {
	linearr[CID_COL_DURATION] = timediff_str(duration, (time_t) 0);
      }

      /* where to merge it in? */
      rel = 1; /* init to merge by default */
      while (cidline < session->cid_num &&
        (gtk_clist_get_text(GTK_CLIST(session->cid_list),
			    cidline, CID_COL_TIME, &cidstr),
	 (rel = strcmp(cidstr, linearr[CID_COL_TIME])) < 0)) {
	
	cidline++;
      }

      if ((rel > 0 && strcmp(linearr[CID_COL_TIME], cache) > 0)
	  || cidline == session->cid_num) {
	/* only merge new and successive entries */
	
        gtk_clist_insert(GTK_CLIST(session->cid_list), cidline, linearr);
        gtk_clist_set_row_data_full(GTK_CLIST(session->cid_list), cidline,
                                    cid_row_new(), cid_row_destroy);
        gtk_clist_set_pixtext(GTK_CLIST(session->cid_list),
	    cidline, CID_COL_TYPE, type, 50, pixmap, bitmap);
	session->cid_num++;
      }
      free(cache);
      cache = linearr[CID_COL_TIME];
      free(linearr[CID_COL_DURATION]);
    }
    free(cache);

    cid_normalize(session);
  
    if (fclose(f))
      fprintf(stderr, "Error closing %s.\n", calls_filename);
  } else { /* error on fopen() */
    fprintf(stderr,
      "Warning: Couldn't open isdnlog calls logfile. Proceeding without it.\n");
  }
  free(line);
}

/*
 * - marks caller id row as unanswered
 * - sets window title to reflect number of unanswered calls
 *
 * input: session
 *        row     number of specified row
 *        state   1 for mark, 0 for unmark
 */
void cid_mark_row(session_t *session, int row, int state) {
  struct cid_row_t *rowdata = (struct cid_row_t *) gtk_clist_get_row_data(
                                            GTK_CLIST(session->cid_list), row);
  GdkColor color;
  char *title;

  if (rowdata->marked_unanswered != state) {
    if (state) {
      gdk_color_parse("#FF8888", &color);
      session->unanswered++;
      rowdata->marked_unanswered = 1;
    } else {
      gdk_color_parse("#FFFFFF", &color);
      session->unanswered--;
      rowdata->marked_unanswered = 0;
    }
    gtk_clist_set_background(GTK_CLIST(session->cid_list), row, &color);
    if (session->unanswered
	&& 0 < asprintf(&title, _("ANT: %d unanswered"), session->unanswered)) {
      gtk_window_set_title(GTK_WINDOW(session->main_window), title);
      free(title);
    } else {
      gtk_window_set_title(GTK_WINDOW(session->main_window), "ANT " VERSION);
    }
  }
}

/*
 * returns timestring with just digits
 * - caller has got to free() the result
 */
char* cid_purify_timestring(char *s) {
  char* t;
  int length = 0;
  char* result;
  char* result_index;

  for (t=s; *t; t++) {
    if (isdigit (*t))
      length++;
  }
  
  result_index = result = (char*) malloc (length + 1);
  
  for (t=s; *t; t++) {
    if (isdigit (*t)) {
      *(result_index++) = *t;
    }
  }
  *(result_index++) = '\0';
 
  return result;
}

/*
 * returns name of sound filename, if it exists; NULL otherwise
 * - caller has go to free() result
 */
char* cid_get_record_filename(session_t* session, int row) {
  char* timestr;
  char* homedir;
  char* pattern;
  glob_t g;
  char* result; /* found something */
  
  gtk_clist_get_text(GTK_CLIST(session->cid_list), row, CID_COL_TIME, &timestr);
  timestr = cid_purify_timestring(timestr);

  if (!(homedir = get_homedir())) {
    fprintf(stderr, "Warning: Couldn't get home dir.\n");
    return NULL;
  }
  
  if (asprintf(&pattern, "%s/." PACKAGE "/recordings/%s.*",
	homedir, timestr) < 0) {
    fprintf(stderr, "Warning: "
	    "Couldn't allocate memory for filename globbing pattern.\n");
    return NULL;
  }
  
  switch (glob(pattern, 0, NULL, &g)) {
    case 0:
      result = strdup(*(g.gl_pathv));
      break;
    case GLOB_NOMATCH:
      result = NULL;
      break;
    default:
      fprintf(stderr, "Warning: "
	  "globbing error while looking up recorded conversation.\n");
      return NULL;
  }

  globfree(&g);
  free(pattern);
  free(timestr);

  return result;
}

/*
 * marks row with record sign if recording available
 * - deletes mark if recording was removed before
 */
void cid_row_mark_record(session_t* session, int row) {
  char* fn;
  
  if ((fn = cid_get_record_filename(session, row))) {
    gtk_clist_set_pixmap(GTK_CLIST(session->cid_list), row, CID_COL_FLAGS,
        session->symbol_record_pixmap, session->symbol_record_bitmap);
    free(fn);
  } else {
    gtk_clist_set_text(GTK_CLIST(session->cid_list), row, CID_COL_FLAGS, "");
  }
  
}

