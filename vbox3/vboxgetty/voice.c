/*
** $Id: voice.c,v 1.3 1998/07/06 09:05:38 michael Exp $
**
** Copyright 1996-1998 Michael 'Ghandi' Herold <michael@abadonna.mayn.de>
**
** $Log: voice.c,v $
** Revision 1.3  1998/07/06 09:05:38  michael
** - New control file code added. The controls are not longer only empty
**   files - they can contain additional informations.
** - Control "vboxctrl-answer" added.
** - Control "vboxctrl-suspend" added.
** - Locking mechanism added.
** - Configuration parsing added.
** - Some code cleanups.
**
** Revision 1.2  1998/06/17 17:01:26  michael
** - First part of the automake/autoconf implementation. Currently vbox will
**   *not* compile!
**
*/

#ifdef HAVE_CONFIG_H
#	include "../config.h"
#endif

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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include "voice.h"
#include "log.h"
#include "modem.h"
#include "stringutils.h"
#include "userrc.h"
#include "vboxrc.h"
#include "vboxgetty.h"
#include "tclscript.h"
#include "control.h"

/** Variables ************************************************************/

static struct vboxuser *voicevboxuser;
static struct vboxcall *voicevboxcall;

static int voicesave = -1;
static int voicevbox =  0;
static int voicehear = -1;
static int voicestat = VBOXVOICE_STAT_OK;

static char voicename_ulaw[PATH_MAX + 1];
static char voicename_vbox[PATH_MAX + 1];

/*************************************************************************/
/** **/
/*************************************************************************/

int voice_init(struct vboxuser *vboxuser, struct vboxcall *vboxcall)
{
	char		msgsavetime[32];
	char		voicestoppl[2];
	char		voicestoprc[2];
	time_t	currenttime;
	char	  *stop;
	int		rc;

	struct vbox_tcl_variable vars[] = 
	{
		{ "vbxv_savetime"		, msgsavetime				},
		{ "vbxv_callerid"		, vboxuser->incomingid	},
		{ "vbxv_callername"	, vboxcall->name 			},
		{ "vbxv_localphone"	, vboxuser->localphone	},
		{ "vbxv_username"		, vboxuser->name			},
		{ "vbxv_userhome"		, vboxuser->home			},
		{ "vbxv_usedscript"	, vboxcall->script		},
		{ "vbxv_saveulaw"		, voicename_ulaw 			},
		{ "vbxv_savevbox"		, voicename_vbox			},
		{ NULL					, NULL						}
	};

	printstring(msgsavetime, "%d", vboxcall->savetime);

		/* Die beiden übergebenen Strukturen global machen, damit alle	*/
		/* Voice Funktionen sie benutzen können.								*/

	voicevboxuser = vboxuser;
	voicevboxcall = vboxcall;

		/* Die Namen der beiden Dateien (*.ulaw und *.vbox) erzeugen.	*/
		/* Eine Datei enthält die Audiodaten, die andere die Inform-	*/
		/* ationen über den Benutzer.												*/

	currenttime = time(NULL);

	printstring(voicename_ulaw, "%s/incoming/%11.11lu-%8.8lu.ulaw", vboxuser->home, (unsigned long)currenttime, (unsigned long)getpid());
	printstring(voicename_vbox, "%s/incoming/%11.11lu-%8.8lu.vbox", vboxuser->home, (unsigned long)currenttime, (unsigned long)getpid());

		/* Variablen für Tcl erzeugen, Kompression setzen, Full Duplex	*/
		/* Audio Modus starten und das Skript aufrufen.						*/

	if (scr_init_variables(vars) == 0)
	{
		log_line(LOG_D, "Setting voice compression to \"ulaw\"...\n");

		if (modem_command(&vboxmodem, "AT+VSM=6+VLS=2", "OK") > 0)
		{
			log_line(LOG_D, "Starting full duplex audio mode...\n");

			if (modem_command(&vboxmodem, "AT+VTX+VRX", "CONNECT") > 0)
			{
				rc = scr_execute(vboxcall->script, vboxuser);

				voice_hear(0);
				voice_save(0);

				printstring(voicestoppl, "%c%c", DLE, ETX);
				printstring(voicestoprc, "%c%c", DLE, DC4);

				log_line(LOG_D, "Sending \"<DLE><ETX>\" to stop playback mode...\n");

				vboxmodem_raw_write(&vboxmodem, voicestoppl, 2);

				log_line(LOG_D, "Sending \"<DLE><DC4>\" to stop record mode...\n");

				vboxmodem_raw_write(&vboxmodem, voicestoprc, 2);

				modem_flush(&vboxmodem, 0);

				if ((stop = ctrl_exists(vboxuser->home, "suspend")))
				{
					ctrl_remove(vboxuser->home, "suspend");

					log_line(LOG_A, "Suspending call to number %s...\n", stop);

					if (modem_command(&vboxmodem, "AT+S1", "OK") > 0) {
						log_line(LOG_D, "Call suspended to number %s.\n", stop);
					} else {
						log_line(LOG_E, "Can't suspend call to number %s.\n", stop);
					}
				}

				return(rc);
			}
		}
	}

	return(-1);
}

/*************************************************************************/
/** voice_wait():	Liest eine angegebene Zeit lang Audiodaten vom Modem.	**/
/*************************************************************************/
/** => timeout		Timeout in Sekunden												**/
/*************************************************************************/
/** <=				 0 wenn der Timeout eingetreten ist.						**/
/**					 1 wenn eine gültige Touchtonesequenz gefunden wurde.	**/
/**					 2 wenn der Anruf suspended werden soll.					**/
/**					-1 bei einem Fehler.												**/
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
		log_line(LOG_D, "Recording \"%s\" (%ds timeout)...\n", voicename_ulaw, timeout);

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

		if (ctrl_exists(voicevboxuser->home, "suspend")) voicestat |= VBOXVOICE_STAT_SUSPEND;

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

	modem_set_timeout(0);

	result = 0;

	if (voicestat & VBOXVOICE_STAT_TOUCHTONE)
	{
		log_line(LOG_D, "Full touchtone sequence found!\n");

		result = 1;
	}

	if (voicestat & VBOXVOICE_STAT_SUSPEND) result = 2;

	if ((voicestat & VBOXVOICE_STAT_HANGUP) || (vboxmodem.nocarrier))
	{
		log_line(LOG_D, "Remote hangup - audio output & saving stopped.\n");

		voice_save(0);
		voice_hear(0);		

		modem_command(&vboxmodem, "", "NO CARRIER");

		result = -1;
	}
	
	return(result);
}

/*************************************************************************/
/** voice_save():	Schaltet das mitspeichern der eingehenden Audiodaten	**/
/**					ein oder aus.														**/
/*************************************************************************/
/** => save			1 wenn die Daten gespeichert werden sollen oder 0		**/
/**					wenn nicht.															**/
/**																							**/
/** <=				0 wenn die Aktion ausgeführt werden konnte oder -1		**/
/**					einem Fehler.														**/
/*************************************************************************/

int voice_save(int save)
{
	struct vboxsave vboxsave;
	int				 desc;

	if (save)
	{
		log_line(LOG_D, "Starting audio recording...\n");

		if (!voicevbox)
		{
				/* Wenn die *.vbox Datei noch nicht existiert wird sie	*/
				/* jetzt erzeugt. Die Datei enthält die Informationen		*/
				/* wer wann wie die Nachricht gesprochen hat.				*/

			log_line(LOG_D, "Opening \"%s\"...\n", voicename_vbox);

			if ((desc = open(voicename_vbox, O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)) != -1)
			{
				vboxsave.time = time(NULL);

				xstrncpy(vboxsave.name	, voicevboxcall->name		, VBOXSAVE_NAME);
				xstrncpy(vboxsave.id		, voicevboxuser->incomingid, VBOXSAVE_ID	);

				if (write(desc, &vboxsave, sizeof(vboxsave)) == sizeof(vboxsave))
				{
					voicevbox = 1;
				}

				close(desc);
			}
			else log_line(LOG_E, "Can't create \"%s\".\n", voicename_vbox);
		}

		if (voicesave == -1)
		{
			log_line(LOG_D, "Opening \"%s\"...\n", voicename_ulaw);

			voicesave = open(voicename_ulaw, O_WRONLY|O_CREAT|O_APPEND, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
		}

		if (voicesave != -1) return(0);

		log_line(LOG_E, "Can't open/append \"%s\".\n", voicename_ulaw);
	}
	else
	{
		log_line(LOG_D, "Stopping audio recording...\n");

		if (voicesave != -1)
		{
			log_line(LOG_D, "Closing \"%s\"...\n", voicename_ulaw);

			close(voicesave);
		}
		
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
