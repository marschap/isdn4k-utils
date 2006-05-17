/*
 * server functionality:
 *   * let other processes issue commands (dial)
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

#define SERVER_LOCAL_SOCKET_NAME ".socket"
#define SERVER_CONNECTIONS_MAX_PENDING 5
#define SERVER_INBUF_SIZE 1024

enum local_msg_t {
  LOCAL_MSG_CALL
};

char *server_local_socket_name(void);
int server_init(session_t *session);
void server_handle_local_input(gpointer data, gint fd _U_,
			       GdkInputCondition condition _U_);
int server_deinit(session_t *session);
