/*
 * line level checker functions
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
#ifdef HAVE_UNISTD_H
  #include <unistd.h>
#endif
#include <errno.h>

/* GTK */
#include <gtk/gtk.h>

/* own header files */
#include "globals.h"
#include "llcheck.h"
#include "session.h"
#include "gtk.h"

/* constants */

/* number of milliseconds for history */
#define HISTORY_TIME 1500

/*
 * the chain link type for the maximum history
 */
struct history_t {
  double value;
  struct history_t *next;
};

/*
 * return an empty (all zero) history of fixed size (10)
 */
struct history_t *history_new(void) {
  struct history_t *root = NULL;
  struct history_t *temp;
  int i;

  for (i = 0; i < 10; i++) {
    temp = root;
    root = (struct history_t *)malloc(sizeof(struct history_t));
    root->value = 0.0;
    root->next = temp;
  }

  return root;
}

/*
 * free memory allocated for history
 */
void history_free(struct history_t *history) {
  while (history != NULL) {
    struct history_t *next = history->next;
    free(history);
    history = next;
  }
}

/*
 * appends x to history (rotating) and returns maximum value in whole history
 *
 * NOTE: history is assumed to be non-empty
 */
double history_append(struct history_t **history, double x) {
  double result = x;
  struct history_t **hist = &(*history)->next;

  /* get new maximum */
  while (*hist != NULL) {
    if ((*hist)->value > result)
      result = (*hist)->value;
    hist = &(*hist)->next;
  }

  /* append */
  *hist = *history; /* hist is ** to last element -> append old first one */
  *history = (*history)->next;
  (*hist)->next = NULL;
  (*hist)->value = x;

  return result;
}

/*
 * sets history to all zero
 */
void history_reset(struct history_t *history) {
  while (history) {
    history->value = 0.0;
    history = history->next;
  }
}

/*
 * called when OK button is clicked
 */
static void llcheck_quit(GtkObject *window) {
  session_t *session = gtk_object_get_data(window, "session");
  GtkWidget *bar = gtk_object_get_data(window, "bar");

  /* remove own and restore old handlers */
  gtk_input_remove(session->gtk_audio_input_tag);
  session_io_handlers_start(session);

  /* stop effect */
  session_effect_stop(session);

  llcheck_bar_deinit(bar);

  gtk_widget_destroy(GTK_WIDGET(window));
  session_set_state(session, STATE_READY);
}

/*
 * gdk callback on audio input
 *
 * input: widget: the llcheck_bar
 */
static void llcheck_handle_audio_input(gpointer widget, gint fd _U_,
				       GdkInputCondition condition _U_) {
  session_t *session = gtk_object_get_data(widget, "session");
  int i, got;
  int max = 0;
  unsigned char sample;

  got = read(session->audio_fd_in, session->audio_inbuf,
	     session->fragment_size_in);

  if (got != -1) {
    for (i = 0; i < got;
	 i += session->audio_sample_size_in) {
      
      if (session->audio_sample_size_in == 1) {
	sample = session->audio_LUT_analyze[
		   session->audio_LUT_out[(int)(session->audio_inbuf[i])]];
      } else { /* audio_sample_size == 2 */
	/* multiple byte samples are used "little endian" in int
	   to look up in LUT (see mediation_makeLUT) */
	sample = session->audio_LUT_analyze[
	  session->audio_LUT_out[(int)(session->audio_inbuf[i]) |
				 (int)(session->audio_inbuf[i+1]) << 8]];
      }

      if (abs((int)sample - 128) > max)
	max = abs((int)sample - 128);
    }
    
    llcheck_bar_set(widget, (double)max / 128);

  } else {
    switch (errno) {
    case EAGAIN:
      fprintf(stderr,
	      "llcheck_handle_audio_input: "
	      "EAGAIN - no data immediately available.\n");
      break;
    case EBADF:
      fprintf(stderr,
	      "llcheck_handle_audio_input: "
	      "EBADF - invalid file descriptor.\n");
      break;
    case EINTR:
      fprintf(stderr,
	      "llcheck_handle_audio_input: EINTR - interrupted by signal.\n");
      break;
    case EIO:
      fprintf(stderr,
	      "llcheck_handle_audio_input: EIO - hardware error.\n");
      break;
    }
  }
}

/*
 * called when sound is requested in level check window
 *
 * input: widget: (play) button
 *        data:   session pointer
 */
static void llcheck_play(GtkWidget *widget, gpointer data) {
  session_t *session = (session_t *)data;
  GtkWidget *bar = gtk_object_get_data(GTK_OBJECT(widget), "bar");

  if (session->effect != EFFECT_NONE) { /* already playing -> stop feeding */
    gtk_input_remove(session->effect_tag);
  }

  /* carefully reset audio */
  gtk_input_remove(session->gtk_audio_input_tag);
  session_reset_audio(session);
  session->gtk_audio_input_tag = gtk_input_add_full(session->audio_fd_in,
					            GDK_INPUT_READ,
					            llcheck_handle_audio_input,
					            NULL,
					            (gpointer) bar,
					            NULL);

  /* start recording (again) */
  read(session->audio_fd_in, session->audio_inbuf,
       session->fragment_size_in);

  /* finally, start playing */
  session_effect_start(session, EFFECT_TEST);
}

/*
 * Returns a line level check bar
 * input: width, height: size of the whole widget
 *        r, g, b:       color of bright bar, dark bar will be some derivative
 *                       of it
 *
 */
GtkWidget *llcheck_bar_new(gint width, gint height,
			   unsigned char r, unsigned char g, unsigned char b) {
  GtkWidget *progress_bar;
  GtkWidget *bar;
  GtkRcStyle *style;
  GdkColor color;

  /* the result to prepare */
  bar = gtk_hbox_new(FALSE, 0);
  gtk_widget_set_size_request(bar, width, height);
  gtk_object_set_data(GTK_OBJECT(bar), "width", GINT_TO_POINTER(width));

  /* progress bar: fast/slow relation progressbar */
  progress_bar = gtk_progress_bar_new();
  style = gtk_rc_style_new();

  style->color_flags[GTK_STATE_PRELIGHT] = GTK_RC_BG;
  color.red = r * 257; color.green = g * 257; color.blue = b * 257;
  gdk_colormap_alloc_color(gtk_widget_get_colormap(bar), &color,
                           TRUE, TRUE);
  style->bg[GTK_STATE_PRELIGHT] = color;

  style->color_flags[GTK_STATE_NORMAL] = GTK_RC_BG;
  color.red   = r * 257 * 2 / 3;
  color.green = g * 257 * 2 / 3;
  color.blue  = b * 257 * 2 / 3;
  gdk_colormap_alloc_color(gtk_widget_get_colormap(bar), &color,
			   TRUE, TRUE);
  style->bg[GTK_STATE_NORMAL] = color;

  gtk_widget_modify_style(progress_bar, style);
  gtk_rc_style_unref(style);
  gtk_progress_set_percentage(GTK_PROGRESS(progress_bar), 0.0); /* 0..1 */
  gtk_object_set_data(GTK_OBJECT(bar), "pbar1", (gpointer) progress_bar);
  gtk_widget_set_size_request(progress_bar, 1, -1);  /* 1..width-1 */
  gtk_box_pack_start(GTK_BOX(bar), progress_bar, TRUE, TRUE, 0);
  gtk_widget_show(progress_bar);

  /* progress bar: rest */
  progress_bar = gtk_progress_bar_new();
  style = gtk_rc_style_new();
  style->color_flags[GTK_STATE_NORMAL] = GTK_RC_BG;
  color.red   = r * 257 / 3;
  color.green = g * 257 / 3;
  color.blue  = b * 257 / 3;
  gdk_colormap_alloc_color(gtk_widget_get_colormap(bar), &color,
			   TRUE, TRUE);
  style->bg[GTK_STATE_NORMAL] = color;
  gtk_widget_modify_style(progress_bar, style);
  gtk_rc_style_unref(style);
  gtk_progress_set_percentage(GTK_PROGRESS(progress_bar), 0.0);
  gtk_widget_set_size_request(progress_bar, width - 1, -1);  /* 1..width-1 */
  gtk_object_set_data(GTK_OBJECT(bar), "pbar2", (gpointer) progress_bar);
  gtk_object_set_data(GTK_OBJECT(bar), "history", (gpointer) history_new());
  gtk_box_pack_start(GTK_BOX(bar), progress_bar, TRUE, TRUE, 0);
  gtk_widget_show(progress_bar);

  return bar;
}

/*
 * set state of line level check bar and update maximum history
 *
 * input: bar:    llcheck_bar
 *        max:    the current maximum (0.0 <= max <= 1.0)
 */
void llcheck_bar_set(GtkWidget *bar, double max) {
  /* the maximum area */
  GtkWidget *pbar1 =
    (GtkWidget *)gtk_object_get_data(GTK_OBJECT(bar), "pbar1");
  /* the dark area */
  GtkWidget *pbar2 =
    (GtkWidget *)gtk_object_get_data(GTK_OBJECT(bar), "pbar2");
  struct history_t *history =
    (struct history_t *)gtk_object_get_data(GTK_OBJECT(bar), "history");
  double maxmax = history_append(&history, max);
  int width = GPOINTER_TO_INT(gtk_object_get_data(GTK_OBJECT(bar), "width"));
      /* saving additional width
	 (bar->allocation.width is valid only with shown widgets) */
  int pbar1size;
  double percentage;

  gtk_object_set_data(GTK_OBJECT(bar), "history", history);

  pbar1size = maxmax * (width - 2) + 1;
  gtk_widget_set_size_request(pbar1, pbar1size, -1);
  gtk_widget_set_size_request(pbar2, width - pbar1size, -1);
  percentage = maxmax > 0.0 ? max / maxmax : 0.0;
  gtk_progress_set_percentage(GTK_PROGRESS(pbar1), percentage);
}

/*
 * set line level check bar to all zero (history included);
 */
void llcheck_bar_reset(GtkWidget *bar) {
  struct history_t *history =
    (struct history_t *)gtk_object_get_data(GTK_OBJECT(bar), "history");

  history_reset(history);
  llcheck_bar_set(bar, 0.0);
}

/*
 * deinit bar (free history mem ...)
 */
void llcheck_bar_deinit(GtkWidget *bar) {
  struct history_t *history = gtk_object_get_data(GTK_OBJECT(bar), "history");
  history_free(history);
}

/*
 * The Line level check callback
 * -> displays dialog window with level check bar and play button
 * -> modal
 */
void llcheck(GtkWidget *widget _U_, gpointer data, guint action _U_) {
  session_t *session = (session_t *) data;

  GtkWidget *window;
  GtkWidget *vbox;
  GtkWidget *button_box;
  GtkWidget *button;
  GtkWidget *label;
  GtkWidget *bar;

  /* set to service state -> open audio device */
  if (!session_set_state(session, STATE_SERVICE)) {

  /* the window itself */
  window = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(window), _("Line Level Check"));

  gtk_object_set(GTK_OBJECT(window), "allow_shrink", FALSE, NULL);
  gtk_object_set(GTK_OBJECT(window), "allow_grow", FALSE, NULL);

  /* new big vbox */
  vbox = gtk_vbox_new(FALSE, 10);
  gtk_container_add(GTK_CONTAINER(GTK_DIALOG(window)->vbox), vbox);
  gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);
  gtk_widget_show(vbox);
  
  /* message label */
  label = gtk_label_new(_("Please check the line input level\n"
			  "and adjust it using your favorite\n"
			  "mixer application.\n"
			  "You can also play a sound\n"
			  "to test the sound output."));
  gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_CENTER);
  gtk_box_pack_start(GTK_BOX(vbox), label,
		     FALSE, FALSE, 0);
  gtk_widget_show(label);

  /* the line level check bar */
  bar = llcheck_bar_new(300, 20, 200, 0, 0);
  gtk_box_pack_start(GTK_BOX(vbox), bar, FALSE, FALSE, 0);
  gtk_widget_show(bar);

  /* action area */
  button_box = gtk_hbutton_box_new();
  gtk_container_add(GTK_CONTAINER(GTK_DIALOG(window)->action_area),
		    button_box);
  gtk_widget_show(button_box);

  /* the play button */
  button = gtk_button_new_with_label(_("Play sound"));
  gtk_box_pack_start_defaults(GTK_BOX(button_box), button);
  gtk_signal_connect(GTK_OBJECT(button), "clicked",
		     GTK_SIGNAL_FUNC(llcheck_play),
		     (gpointer) session );
  gtk_widget_show(button);

  /* save session in window (quit(): audio handling) */
  gtk_object_set_data(GTK_OBJECT(window), "session", (gpointer) session);
  /* save bar in window (quit(): destroy) */
  gtk_object_set_data(GTK_OBJECT(window), "bar", (gpointer) bar);
  /* save session in bar widget (input handling) */
  gtk_object_set_data(GTK_OBJECT(bar), "session", (gpointer) session);
  /* save bar in button (in case play button is pressed) */
  gtk_object_set_data(GTK_OBJECT(button), "bar", (gpointer) bar);

  /* the OK button */
  button = gtk_button_new_with_label(_("OK"));
  gtk_box_pack_start_defaults(GTK_BOX(button_box), button);

  gtk_signal_connect_object(GTK_OBJECT(button), "clicked",
			    GTK_SIGNAL_FUNC(llcheck_quit), GTK_OBJECT(window));
  /* handler for kill function of window */
  gtk_signal_connect_object(GTK_OBJECT(window), "delete_event",
			    GTK_SIGNAL_FUNC(llcheck_quit), GTK_OBJECT(window));

  gtk_widget_show(button);
  
  gtk_window_set_modal(GTK_WINDOW(window), TRUE);

  /* remove old and set up own audio input handler */
  session_io_handlers_stop(session);
  session->gtk_audio_input_tag = gtk_input_add_full(session->audio_fd_in,
					            GDK_INPUT_READ,
					            llcheck_handle_audio_input,
						    NULL,
					            (gpointer) bar,
						    NULL);

  read(session->audio_fd_in, session->audio_inbuf, /* start recording */
       session->fragment_size_in);

  /* show everything */
  gtk_widget_show(window);
  } else {
    show_audio_error_dialog();
  }
}

/* called when line levels view check button is toggled */
void llcheck_toggle_cb(GtkWidget *widget _U_, gpointer data, guint action _U_) {
  session_t *session = (session_t *) data;

  if (GTK_CHECK_MENU_ITEM (session->llcheck_check_menu_item)->active) {
    gtk_widget_show(session->llcheck);
    session->option_show_llcheck = 1;
  } else {
    gtk_widget_hide(session->llcheck);
    session->option_show_llcheck = 0;
    /* shrink if no growing callerid monitor present */
    if (!session->option_show_callerid)
      gtk_window_resize(GTK_WINDOW(session->main_window), 1, 1);
  }
}
