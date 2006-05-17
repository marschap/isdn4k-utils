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

/* GTK */
#include <gtk/gtk.h>

/* own header files */
#include "session.h"

enum call_type_t {
  CALL_IN,
  CALL_OUT
};

/*
 * (Sequential) positions of columns (order)
 */
enum cid_col_t {
  CID_COL_FLAGS,
  CID_COL_TIME,
  CID_COL_TYPE,
  CID_COL_FROM,
  CID_COL_TO,
  CID_COL_DURATION,
  /*XXX, "Zone" */

  CID_COL_NUMBER      /* for calculating size */
};

/*
 * _additional_ (invisible) data associated with a row
 */
struct cid_row_t {
  int marked_unanswered;
};

void cid_toggle_cb(GtkWidget *widget, gpointer data, guint action);
GtkWidget *cid_new(session_t *session);
void cid_add_line(session_t *session,
		  enum call_type_t ct, gchar *from, gchar *to);
void cid_set_date(session_t *session, time_t date);
void cid_set_from(session_t *session, gchar *from);
void cid_set_duration(session_t *session, gchar *message);
void cid_jump_to_end(session_t *session);
void cid_normalize(session_t *session);
void cid_add_saved_line(session_t *session, char *date, char *type,
			char *from, char *to, char *duration);
void cid_calls_merge(session_t *session);
void cid_mark_row(session_t *session, int row, int state);
char* cid_get_record_filename(session_t* session, int row);
void cid_row_mark_record(session_t* session, int row);

