/* $Id: read_line.c,v 1.1 1999/06/30 17:19:12 he Exp $ */
/*
  Copyright 1998 by Michael Mauch <michael.mauch@gmx.de>

    This code is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This code is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/ 

/*
   Interface to the readline library for line editing and history.

   Most parts are snippets from the man pages / info files for readline.
   It works on the console and in rxvt if linked with readline-2.2.1 and 
   ncurses-4.2-5; I don't know about other versions.
*/


#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <termios.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "read_line.h"

#if 0
/* old stuff removed from eftp.c */
#include "read_line.h"
void eft_read_line_prompt(char* s)
{
  read_line_prompt_string = s;
  eft_prompt(s);
}

#define eft_prompt eft_read_line_prompt
#endif


char* read_line_prompt_string = NULL;


/* 
   Set the `O_NONBLOCK' flag of FD if VALUE is nonzero,
   or clear the flag if VALUE is 0.
   Return 0 on success, or -1 on error with `errno' set. 
*/
static int set_nonblock_flag (int fd, int value)
{
  int oldflags = fcntl (fd, F_GETFL, 0);
  /* If reading the flags failed, return error indication now. */
  if (oldflags == -1)
    return -1;
  /* Set just the flag we want to set. */
  if (value != 0)
    oldflags |= O_NONBLOCK;
  else
    oldflags &= ~O_NONBLOCK;
  /* Store modified flag word in the descriptor. */
  return fcntl (fd, F_SETFL, oldflags);
}


static void set_echo_flag (int fd, int value)
{
  struct termios tattr;
  tcgetattr (fd, &tattr);
  if(value)
    tattr.c_lflag |= ECHO;
  else
    tattr.c_lflag &= ~ECHO;
  tcsetattr (fd, TCSANOW, &tattr);
}


static struct termios* saved_attributes;

static void reset_input_mode (void)
{
  if(saved_attributes)
  {
    tcsetattr (STDIN_FILENO, TCSADRAIN, saved_attributes);
    set_nonblock_flag(STDIN_FILENO,1);
  }
}

void read_line_exit (void)
{
  reset_input_mode();
}


static void set_input_mode (void)
{
  struct termios tattr;

  /* If stdin is not a terminal, read_line() will call read(),
     so don't do anything to the terminal here. */
  if(!isatty (STDIN_FILENO))
    return;

  if(!saved_attributes)
  {
    saved_attributes = (struct termios*)malloc(sizeof(struct termios));
    if(!saved_attributes)
    {
      fprintf(stderr,"\nout of memory in read_line.c::set_input_mode()\n");
      return;
    }  
    /* Save the terminal attributes so we can restore them later. */
    tcgetattr (STDIN_FILENO, saved_attributes);
    atexit (reset_input_mode);
  }    

  /* Set the funny terminal modes. */
  tcgetattr (STDIN_FILENO, &tattr);
  tattr.c_lflag &= ~(ICANON);
  tattr.c_cc[VMIN] = 1;        /* read() only one character */
  tattr.c_cc[VTIME] = 0;       /* return immediately from read() */ 
  tcsetattr (STDIN_FILENO, TCSADRAIN, &tattr);
}


static int read_line_init_done = 0;


void read_line_init (void)
{
  if( !read_line_init_done ) {
    set_input_mode();
    read_line_init_done = 1;
  }    
} 

/* 
 * A drop-in replacement for the read function,
 * with history and command line editing.
 */
ssize_t read_line (int fd, void *buf, size_t count)
{  
  /* A static variable for holding the line. */
  static char* line_read = NULL;
  static char* prompt_string = NULL;
  
  if( (fd != STDIN_FILENO) || !isatty(STDIN_FILENO) )
    return read(fd, buf, count);

  if( !read_line_init_done )
    read_line_init();

  /* If the buffer has already been allocated, return the memory
     to the free pool. */
  if( line_read ) {
      free(line_read);
      line_read = (char *)NULL;
  }

  set_echo_flag(STDIN_FILENO,1);

  if(read_line_prompt_string)
  {
    if(prompt_string)
      free(prompt_string);
    prompt_string = (char*)malloc(strlen(read_line_prompt_string)+2);
    if(!prompt_string)
    {
      fprintf(stderr,"\nout of memory in read_line.c::read_line()\n");
      return 0;
    }
    sprintf(prompt_string,"\r%s",read_line_prompt_string);
  }

  /* Get a line from the user. */
  line_read = readline(prompt_string?:"");
  set_echo_flag(STDIN_FILENO,0);

  /* If the line has any text in it, save it on the history. */
  if( line_read && *line_read ) {
    add_history(line_read);
    strncpy(buf, line_read, count);
    return strlen(line_read);
  }
  return 0;
}

