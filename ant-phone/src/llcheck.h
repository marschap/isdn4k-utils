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

GtkWidget *llcheck_bar_new(gint width, gint height,
			   unsigned char r, unsigned char g, unsigned char b);
void llcheck_bar_set(GtkWidget *bar, double max);
void llcheck_bar_reset(GtkWidget *bar);
void llcheck_bar_deinit(GtkWidget *bar);
void llcheck(GtkWidget *widget, gpointer data, guint action);
void llcheck_toggle_cb(GtkWidget *widget, gpointer data, guint action);
