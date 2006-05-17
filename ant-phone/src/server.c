/*
 * server functionality:
 *   * let other processes issue commands (e.g. dial)
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
#include <stddef.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

/* own header files */
#include "globals.h"
#include "server.h"
#include "session.h"
#include "util.h"

/*
 * returns the name of the local socket file
 *
 * NOTE: caller has to free himself the allocated memory
 */
char *server_local_socket_name(void) {
  char *filename;

  if (asprintf(&filename, "%s/." PACKAGE "/%s",
	       get_homedir(), SERVER_LOCAL_SOCKET_NAME) < 0) {
    fprintf(stderr,
	    "Warning: Couldn't allocate memory for history filename.\n");
    return NULL;
  }
  return filename;
}

/*
 * init server setup for session
 */
int server_init(session_t *session) {
  struct sockaddr_un local_name;
  int local_sock;
  size_t size;
  char *filename;

  filename = server_local_socket_name();

  local_name.sun_family = AF_LOCAL;
  strncpy(local_name.sun_path, filename, sizeof(local_name.sun_path));
  size = SUN_LEN(&local_name);

  free(filename);

  local_sock = socket(PF_LOCAL, SOCK_STREAM, 0); /* open socket */
  if (local_sock < 0) {
    perror("local socket");
    return -1;
  }

  if (touch_dotdir())
    return -1;

  /* bind (file) name to socket */
  if (bind(local_sock, (struct sockaddr *) &local_name, size)) {
    perror("local bind");
    fprintf(stderr,
	    "This program was possibly killed by accident on last run.\n"
	    "In this case, try running it with option -r.\n");
    return -1;
  }

  if (listen(local_sock, SERVER_CONNECTIONS_MAX_PENDING) < 0) {
    perror("listen");
    return -1;
  }

  session->local_sock = local_sock;

  return 0;
}

/*
 * callback for local input on session->local_socket
 */
void server_handle_local_input(gpointer data, gint fd _U_,
			       GdkInputCondition condition _U_) {
  session_t *session = (session_t *) data;
  int sock; /* the actual socket */
  struct sockaddr_un clientname; /* data about connecting client */
  socklen_t size;
  char buffer[SERVER_INBUF_SIZE];
  int bytes;

  sock = accept(session->local_sock, (struct sockaddr *) &clientname, &size);
  if (sock < 0) {
    perror("local accept");
  }

  bytes = read(sock, buffer, SERVER_INBUF_SIZE);
  if (bytes > 0) { /* got message */
    if (buffer[0] == LOCAL_MSG_CALL) {
      if (debug) fprintf(stderr, "Request (%d bytes): call %s.\n",
			 bytes, &buffer[1]);
      session_make_call(session, &buffer[1]);
    }
  } else if (bytes < 0) { /* error */
    perror("local read");
  } else { /* EOF */
    fprintf(stderr, "local read: EOF");
  }

  close(sock);
}


/*
 * deinit server setup for session
 */
int server_deinit(session_t *session) {
  char *filename;

  close(session->local_sock);
  if (-1 == unlink(filename = server_local_socket_name())) {
    perror("unlink socket file");
  }
  free(filename);

  return 0;
}
