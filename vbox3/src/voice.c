/*
** $Id: voice.c,v 1.1 1998/06/10 13:31:59 michael Exp $
**
** Copyright 1997-1998 by Michael Herold <michael@abadonna.mayn.de>
**
** $Log: voice.c,v $
** Revision 1.1  1998/06/10 13:31:59  michael
** Source added.
**
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include "log.h"
#include "vboxgetty.h"
#include "modem.h"
#include "voice.h"

static int voicesave = -1;
static int voicehear = -1;
static int voicestat = VBOXVOICE_STAT_OK;

static char voicename[1000];

int voice_init(void)
{
	log_line(LOG_D, "Setting voice compression to \"ulaw\"...\n");

	if (modem_command(&vboxmodem, "AT+VSM=6+VLS=2", "OK") > 0)
	{
		log_line(LOG_D, "Starting full duplex audio mode...\n");

		if (modem_command(&vboxmodem, "AT+VTX+VRX", "CONNECT") > 0)
		{
			/* start answer.tcl */

			printstring(voicename, "/tmp/vbox-1.ulaw");

			voice_hear(1);
			voice_save(1);

			voice_wait(30);
			
			voice_save(0);
			voice_hear(0);

			return(0);
		}
	}

	return(-1);
}

/*************************************************************************/
/** voice_wait():	Reads audio datas.												**/
/*************************************************************************/
/** => timeout		Timeout in seconds												**/
/*************************************************************************/

int voice_wait(int timeout)
{
	char line_i[1];
	char line_o[VBOXVOICE_BUFFER_SIZE + 1];
	int  byte_i;
	int  byte_o;
	int  result;
	int  gotdle;

	voicestat	= VBOXVOICE_STAT_OK;
	gotdle		= 0;

	if (voicesave == -1)
		log_line(LOG_D, "Reading audio datas (%ds timeout)...\n", timeout);
	else
		log_line(LOG_D, "Recording \"%s\" (%ds timeout)...\n", voicename, timeout);

	modem_set_timeout(timeout);
	
	while (voicestat == VBOXVOICE_STAT_OK)
	{
		byte_i = 0;
		byte_o = 0;
		result = 0;

		while ((byte_o < VBOXVOICE_BUFFER_SIZE) && (voicestat == VBOXVOICE_STAT_OK))
		{
			if ((result = vboxmodem_raw_read(&vboxmodem, line_i, 1)) == 1)
			{
				byte_i++;

				if (gotdle)
				{
					switch (*line_i)
					{
						case ETX:
							voicestat |= VBOXVOICE_STAT_HANGUP;
							break;
							
						default:
							/*if (voice_check_touchtone(*line_i) == 0) voicestat |= VBOXVOICE_STAT_TOUCHTONE;*/
							break;
					}
					
					gotdle = 0;
				}
				else
				{
					if (*line_i != DLE)
					{
						line_o[byte_o++] = *line_i;
					}
					else gotdle = 1;
				}
			}
			else break;
		}

		if (byte_o > 0)
		{
			if (voicesave != -1) write(voicesave, line_o, byte_o);
			if (voicehear != -1) write(voicehear, line_o, byte_o);
		}

		if ((result != 1) || (modem_get_timeout()))
		{
			if (!modem_get_timeout())
			{
				log_line(LOG_W, "Can't read voice data: %s!\n", strerror(errno));

				voicestat |= VBOXVOICE_STAT_TIMEOUT;
			}
			else voicestat |= VBOXVOICE_STAT_TIMEOUT;
		}
	}	

	if (voicestat & VBOXVOICE_STAT_TOUCHTONE)
	{
		log_line(LOG_D, "Full touchtone sequence found!\n");
	}

	if ((voicestat & VBOXVOICE_STAT_HANGUP) || (vboxmodem.nocarrier))
	{
		log_line(LOG_D, "Remote hangup - audio output & saving stopped.\n");

		voice_save(0);
		voice_hear(0);		

		modem_command(&vboxmodem, "", "NO CARRIER");
	}









	
	modem_set_timeout(0);

	return(0);
}



/*************************************************************************/
/** **/
/*************************************************************************/

int voice_save(int save)
{
	if (save)
	{
		if (voicesave == -1)
		{
			voicesave = open(voicename, O_WRONLY|O_CREAT|O_APPEND);
		}

		if (voicesave != -1) return(0);
	}
	else
	{
		if (voicesave != -1) close(voicesave);
		
		voicesave = -1;

		return(0);
	}

	return(-1);
}

int voice_hear(int hear)
{
	if (hear)
	{
		if (voicehear == -1)
		{
			voicehear = open("/dev/audio", O_WRONLY);
		}

		if (voicehear != -1) return(0);
	}
	else
	{
		if (voicehear != -1) close(voicehear);
		
		voicehear = -1;

		return(0);
	}

	return(-1);
}
