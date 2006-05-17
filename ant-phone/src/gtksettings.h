/*
 * gtk GUI functions
 *
 * This file is part of ANT (Ant is Not a Telephone)
 *
 * Copyright 2002, 2003 Roland Stigge
 *
 */

/* GTK */
#include <gtk/gtk.h>

/* own header files */
#include "session.h"

void gtksettings_cb_ok(GtkWidget *widget, gpointer data);
void gtksettings_cb(GtkWidget *widget _U_, gpointer data, guint action _U_);
