/*
 * client functionality:
 *   * let a process issue commands (e.g. dial)
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
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>

/* own header files */
#include "globals.h"
#include "client.h"
#include "server.h"
#include "util.h"

/*
 * (try to) connect to server and issue call command
 *
 * returns 0 on success, -1 otherwise
 */
int client_make_call(char *number) {
  int sock;
  struct sockaddr_un local_name;
  size_t size;
  char *filename;
  int bytes;
  char *msg;

  sock = socket(PF_LOCAL, SOCK_STREAM, 0);
  if (sock < 0) {
    perror("client: local socket()");
    return -1;
  }

  filename = server_local_socket_name();

  local_name.sun_family = AF_LOCAL;
  strncpy(local_name.sun_path, filename, sizeof(local_name.sun_path));
  size = SUN_LEN(&local_name);

  if (connect(sock, (struct sockaddr *) &local_name, sizeof(local_name)) < 0) {
    perror("client: local connect()");
    return -1;
  }
  
  if (asprintf(&msg, "%c%s", LOCAL_MSG_CALL, number) < 0) {
    fprintf(stderr, "asprintf error");
    return -1;
  }
  bytes = write(sock, msg, 1 + strlen(number) + 1);
  if (bytes < 0) {
    perror("socket write");
    return -1;
  }
  free(msg);

  close(sock);
  free(filename);
  return 0;
}
