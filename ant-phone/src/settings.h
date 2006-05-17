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
 */

/* own header files */
#include "session.h"

#define SETTINGS_OPTIONS_FILENAME "options"
#define SETTINGS_HISTORY_FILENAME "history"
#define SETTINGS_CALLERID_HISTORY_FILENAME "callerid"


void settings_option_set(session_t *session, char *option, char *value);
void settings_options_read(session_t *session);
void settings_options_write(session_t *session);

void settings_history_read(session_t *session);
void settings_history_write(session_t *session);

void settings_callerid_read(session_t *session);
void settings_callerid_write(session_t *session);
