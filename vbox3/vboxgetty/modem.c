/*
** $Id: modem.c,v 1.1 1998/06/17 16:38:48 michael Exp $
**
** Copyright 1997-1998 by Michael Herold <michael@abadonna.mayn.de>
**
** $Log: modem.c,v $
** Revision 1.1  1998/06/17 16:38:48  michael
** - First part of the automake/autoconf implementation. Currently vbox will
**   *not* compile!
**
** Revision 1.2  1998/06/17 12:20:36  michael
** - Changes for automake/autoconf added.
**
** Revision 1.1  1998/06/10 13:31:53  michael
** Source added.
**
*/

#if TIME_WITH_SYS_TIME
#   include <sys/time.h>
#   include <time.h>
#else
#   if HAVE_SYS_TIME_H
#      include <sys/time.h>
#   else
#      include <time.h>
#   endif
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termio.h>
#include <signal.h>

#include "log.h"
#include "modem.h"
#include "libvboxmodem.h"

static struct modemsetup modemsetup =
{
	4,						/* Echo timeout (sec)	*/
	4,						/* Command timeout (sec)	*/
	6,						/* Ring timeout (sec)	*/
	1800,					/* Alive timeout (sec)	*/
	400					/* Toggle DTR (ms)	*/
};

static unsigned char lastmodemresult[VBOXMODEM_BUFFER_SIZE + 1];

static int timeoutstatus = 0;

static void modem_timeout_function(int);
static int modem_write(struct vboxmodem *, char *);
static int modem_get_echo(struct vboxmodem *, char *);
static int modem_get_rawsequence(struct vboxmodem *, char *, int);
static int modem_check_result(char *, char *);

/*************************************************************************/
/** modem_set_timeout():	Sets modem function timeout.						**/
/*************************************************************************/
/** => timeout					Timeout in seconds									**/
/*************************************************************************/

void modem_set_timeout(int timeout)
{
	if (timeout != 0)
	{
		timeoutstatus = 0;

		signal(SIGALRM, modem_timeout_function);
		siginterrupt(SIGALRM, 1);
		alarm(timeout);
	}
	else
	{
		signal(SIGALRM, SIG_IGN);
		alarm(0);
	}
}

/*************************************************************************/
/** modem_get_timeout():	Returns the timeout status.						**/
/*************************************************************************/
/** <=							1 if timeout or 0 if not							**/
/*************************************************************************/

int modem_get_timeout(void)
{
	return(timeoutstatus);
}

/*************************************************************************/
/** modem_get_sequence():	Reads a specified sequence from the modem.	**/
/*************************************************************************/
/** => seq						Sequence to read										**/
/** <=							0 sequence read or -1 not read					**/
/*************************************************************************/

int modem_get_sequence(struct vboxmodem *vbm, char *seq)
{
	return(modem_get_rawsequence(vbm, seq, 0));
}

/*************************************************************************/
/** modem_flush():	Flushs modem input/output.									**/
/*************************************************************************/
/** =>					Pointer to modem structure									**/
/** =>					Flush timeout in seconds									**/
/*************************************************************************/

void modem_flush(struct vboxmodem *vbm, int timeout)
{
	TIO	porttio;
	TIO	savetio;
	long	gotjunk = 0;
	char	onebyte = 0;

	log_line(LOG_D, "Flushing modem%s...\n", (timeout ? " (with timeout)" : ""));

	if (vboxmodem_get_termio(vbm, &porttio) == 0)
	{
		savetio = porttio;

		porttio.c_lflag		&= ~ICANON;
		porttio.c_cc[VMIN]	 = 0;
		porttio.c_cc[VTIME]	 = timeout;

		if (vboxmodem_set_termio(vbm, &porttio) == 0)
		{
			while (vboxmodem_raw_read(vbm, &onebyte, 1) == 1)
			{
				if (gotjunk++ < 20)
				{
					log_line(LOG_D, "Junk: ");
					log_char(LOG_D, onebyte);
					log_text(LOG_D, "\n");
				}
			}

			if (gotjunk > 20)
			{
				log_line(LOG_D, "Flush has junked %d byte(s)...\n", gotjunk);
			}

			vboxmodem_set_termio(vbm, &savetio);
		}
	}

	tcflush(vbm->fd, TCIOFLUSH);
	tcflush(vbm->fd, TCIOFLUSH);
}

/*************************************************************************/
/** modem_hangup(): Toggles the data terminal ready line to hangup the	**/
/**					  modem.															   **/
/*************************************************************************/
/** => vbm				Pointer to modem structure									**/
/** <=					0 on success or -1 on error								**/
/*************************************************************************/

int modem_hangup(struct vboxmodem *vbm)
{
	TIO porttio;
	TIO savetio;

	log_line(LOG_D, "Hangup modem (drop dtr %d ms)...\n", modemsetup.toggle_dtr_time);

	modem_flush(vbm, 1);

	if (vboxmodem_get_termio(vbm, &porttio) == -1) return(-1);

	savetio = porttio;
	
	cfsetospeed(&porttio, B0);
	cfsetispeed(&porttio, B0);

	vboxmodem_set_termio(vbm, &porttio);

	usleep(modemsetup.toggle_dtr_time * 1000);
	      
	return(vboxmodem_set_termio(vbm, &savetio));
}

/*************************************************************************/
/** modem_command():	Sends a command to the modem and waits for one or	**/
/**						more results.													**/
/*************************************************************************/
/** => vbm				Pointer to modem structure									**/
/** => command			Command to send												**/
/** => result			Needed answer(s) (seperater with '|')					**/
/** <=					Number of the found answer, 0 nothing found, -1 on	**/
/**						error																**/
/*************************************************************************/

int modem_command(struct vboxmodem *vbm, char *command, char *result)
{
	char line[VBOXMODEM_BUFFER_SIZE + 1];
	int  back;

	lastmodemresult[0] = '\0';

	if ((command) && (*command))
	{
		modem_flush(vbm, 0);

		log_line(LOG_D, "Sending \"%s\"...\n", command);

		if (strcmp(command, "\r") != 0)
		{
			if ((modem_write(vbm, command) == -1) || (modem_write(vbm, "\r") == -1))
			{
				log_line(LOG_E, "Can't send modem command.\n");

				modem_flush(vbm, 1);

				return(-1);
			}

			if (modem_get_echo(vbm, command) == -1)
			{
				log_line(LOG_E, "Can't read modem command echo.\n");

				modem_flush(vbm, 1);

				return(-1);
			}
		}
		else
		{
			if (vboxmodem_raw_write(vbm, command, strlen(command)) == -1)
			{
				log_line(LOG_E, "Can't send modem command.\n");

				modem_flush(vbm, 1);
				
				return(-1);
			}
		}
	}

	if ((result) && (*result))
	{
		if (modem_read(vbm, line, modemsetup.commandtimeout) == -1)
		{
			if ((command) && (*command))
			{
				log_line(LOG_E, "Can't read modem command result.\n");
			}

			modem_flush(vbm, 1);
			
			return(-1);
		}

		strcpy(lastmodemresult, line);

		if (strcmp(result, "?") == 0) return(0);

		if ((back = modem_check_result(line, result)) < 1)
		{
			log_line(LOG_E, "Modem returns unneeded command \"");
			log_code(LOG_E, line);
			log_text(LOG_E, "\".\n");

			modem_flush(vbm, 1);
			
			return(-1);
		}
		else return(back);
	}

	return(0);
}

/*************************************************************************/
/** modem_read():		Reads a terminated string from the modem.				**/
/*************************************************************************/
/** => vbm				Pointer to modem structure									**/
/** => line				Pointer to write buffer										**/
/** => readtimeout	Timeout in seconds											**/
/*************************************************************************/

int modem_read(struct vboxmodem *vbm, char *line, int readtimeout)
{
	char	c;
	int	r;
	int	timeout;
	int	linelen = 0;
	int	havetxt = 0;

	log_line(LOG_D, "Reading modem answer (%ds timeout)...\n", readtimeout);

	modem_set_timeout(readtimeout);

	while (((r = vboxmodem_raw_read(vbm, &c, 1)) == 1) && (linelen < (VBOXMODEM_BUFFER_SIZE - 1)))
	{
		if (c >= 32) havetxt = 1;

		if (havetxt)
		{
			if (c == '\n') break;
			
			if ((c != '\r') && (c != '\n'))
			{
				*line++ = c;
				
				linelen++;
			}
		}

		if (modem_get_timeout()) break;
	}

	timeout = modem_get_timeout();
	
	modem_set_timeout(0);

	*line = 0;

	if ((r != 1) || (timeout) || (linelen >= (VBOXMODEM_BUFFER_SIZE - 1)))
	{
		log_line(LOG_W, "Can't read from modem [%d]%s.\n", r, (timeout ? " (timeout)" : ""));

		return(-1);
	}

	return(0);
}


/*************************************************************************/
/** **/
/*************************************************************************/

int modem_wait(struct vboxmodem *vbm)
{
	struct timeval  timeout;
	struct timeval *usetimeout;
	      
	fd_set	fd;
	int		back;

	log_line(LOG_D, "Waiting...\n");
	            
	FD_ZERO(&fd);
	FD_SET(vbm->fd, &fd);
      
	if (modemsetup.alivetimeout > 0)
	{
		timeout.tv_sec    = modemsetup.alivetimeout;
		timeout.tv_usec   = modemsetup.alivetimeout * 1000;

		usetimeout = &timeout;
	}
	else usetimeout = NULL;

	back = select(FD_SETSIZE, &fd, NULL, NULL, usetimeout);
   
	if (back <= 0)
	{
		if (back < 0)
		{
			log_line(LOG_E, "Select returns with error (%d)...\n", back);
		}
		else log_line(LOG_D, "Select returns with timeout...\n");

		return(-1);
	}

	return(0);
}

void modem_set_nocarrier(struct vboxmodem *vbm, int carrier)
{
	vbm->nocarrier = carrier;
}

int modem_get_nocarrier(struct vboxmodem *vbm)
{
	return(vbm->nocarrier);
}














/*************************************************************************/
/** modem_timeout_function():	Function called from timeout signal hand-	**/
/**									ler.													**/
/*************************************************************************/
/** => s								Signal number										**/
/*************************************************************************/

static void modem_timeout_function(int s)
{
	alarm(0);
	signal(SIGALRM, SIG_IGN);

	log_line(LOG_D, "Modem timeout function called...\n");

	timeoutstatus = 1;
}

/*************************************************************************/
/** modem_write():	Sends a null terminated string to the modem.			**/
/*************************************************************************/
/** => vbm				Pointer to modem structure									**/
/** => s					Terminated string to write									**/
/*************************************************************************/

static int modem_write(struct vboxmodem *vbm, char *s)
{
	if (vboxmodem_raw_write(vbm, s, strlen(s)) == strlen(s)) return(0);

	return(-1);
}


/*************************************************************************/
/** modem_get_echo():	Reads modem echo.											**/
/*************************************************************************/
/** => vbm					Pointer to modem structure								**/
/** => echo					Command to get echo from								**/
/*************************************************************************/

static int modem_get_echo(struct vboxmodem *vbm, char *echo)
{
	return(modem_get_rawsequence(vbm, echo, 1));
}

/*************************************************************************/
/** modem_get_rawsequence():	Reads a raw sequence from modem. This is	**/
/**									a subroutine for modem_get_sequence() &	**/
/**									modem_get_echo().									**/
/*************************************************************************/

static int modem_get_rawsequence(struct vboxmodem *vbm, char *line, int echo)
{
	char	c;
	int	i;
	int	timeout;
	
	timeout = (echo ? modemsetup.echotimeout : modemsetup.commandtimeout);

	log_line(LOG_D, "Reading modem %s (%ds timeout)...\n", (echo ? "echo" : "sequence"), timeout);

	modem_set_timeout(timeout);

	for (i = 0; i < strlen(line); i++)
	{
		if ((vboxmodem_raw_read(vbm, &c, 1) != 1) || (modem_get_timeout()))
		{
			modem_set_timeout(0);

			return(-1);
		}

		if (line[i] != c)
		{
			modem_set_timeout(0);

			return(-1);
		}
	}

	if (echo)
	{
		if ((vboxmodem_raw_read(vbm, &c, 1) != 1) || (modem_get_timeout()))
		{
			modem_set_timeout(0);

			return(-1);
		}
	}

	modem_set_timeout(0);

	if (echo)
	{
		if (c != '\r') return(-1);
	}
	
	return(0);
}

/*************************************************************************/
/** modem_check_result():	Checks for a string in the modem result.		**/
/*************************************************************************/

static int modem_check_result(char *have, char *need)
{
	char	line[VBOXMODEM_BUFFER_SIZE + 1];
	char *word;
	char *more;
	int	nr;

	log_line(LOG_D, "Waiting for \"");
	log_code(LOG_D, need);
	log_text(LOG_D, "\"... ");

	strncpy(line, need, VBOXMODEM_BUFFER_SIZE);
	line[VBOXMODEM_BUFFER_SIZE] = '\0';

	more	= strchr(line, '|');
	word	= strtok(line, "|");
	nr		= 0;

	while (word)
	{
		nr++;

		if (strncmp(have, word, strlen(word)) == 0)
		{
			if (more)
			{
				log_text(LOG_D, "Got \"");
				log_code(LOG_D, word);
				log_text(LOG_D, "\" (%d).\n", nr);
			}
			else log_text(LOG_D, "Got it.\n");
			
			return(nr);
		}
	
		word = strtok(NULL, "|");
	}

	log_text(LOG_D, "Oops!\n");

	return(0);
}









