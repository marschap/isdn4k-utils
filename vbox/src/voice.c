
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <limits.h>
#include <errno.h>
#include <time.h>

#include "voice.h"
#include "init.h"
#include "script.h"
#include "modem.h"
#include "log.h"
#include "perms.h"
#include "rcvbox.h"
#include "libvbox.h"

/** Variables ************************************************************/

static int	voicestatus		= VOICE_ACTION_OK;
static int	sequencestatus	= ST_NO_INPUT;

char touchtones[TOUCHTONE_BUFFER_LEN + 1];

/** Prototypes ***********************************************************/

static int		voice_set_compression(short);
static void		voice_set_header(vaheader_t *);
static void		voice_handle_dle(char);
static void		voice_handle_touchtone(char);
static void		voice_close_or_unlink(int, char *);

/*************************************************************************
 ** voice_init_section():	Set the voice setup defaults.						**
 *************************************************************************/

void voice_init_section(void)
{
	xstrncpy(setup.voice.standardmsg		, setup.spool					, VOICE_MAX_MESSAGE );
	xstrncat(setup.voice.standardmsg		, "/messages/standard.msg"	, VOICE_MAX_MESSAGE );
	xstrncpy(setup.voice.beepmsg			, setup.spool					, VOICE_MAX_MESSAGE );
	xstrncat(setup.voice.beepmsg			, "/messages/beep.msg"		, VOICE_MAX_MESSAGE );
	xstrncpy(setup.voice.timeoutmsg		, setup.spool					, VOICE_MAX_MESSAGE );
	xstrncat(setup.voice.timeoutmsg		, "/messages/timeout.msg"	, VOICE_MAX_MESSAGE );
	xstrncpy(setup.voice.tclscriptname	, setup.spool					, VOICE_MAX_SCRIPT  );
	xstrncat(setup.voice.tclscriptname	, "/standard.tcl"				, VOICE_MAX_SCRIPT  );
	xstrncpy(setup.voice.checknewpath	, setup.spool					, VOICE_MAX_CHECKNEW);
	xstrncat(setup.voice.checknewpath	, "/incoming"					, VOICE_MAX_CHECKNEW);
	xstrncpy(setup.voice.callerid			, "*** Unknown ***"			, VOICE_MAX_CALLERID);
	xstrncpy(setup.voice.phone				, "*** Unknown ***"			, VOICE_MAX_PHONE	  );
	xstrncpy(setup.voice.name				, "*** Unknown ***"			, VOICE_MAX_NAME	  );
	xstrncpy(setup.voice.section			, "STANDARD"					, VOICE_MAX_SECTION );

	setup.voice.rings			= -1;
	setup.voice.ringsonnew	= -1;
	setup.voice.doanswer		= TRUE;
	setup.voice.dorecord		= TRUE;
	setup.voice.dobeep		= TRUE;
	setup.voice.domessage	= TRUE;
	setup.voice.dotimeout	= TRUE;
	setup.voice.recordtime	= TRUE;
}

/*************************************************************************
 ** voice_user_section():	Loads a user section.								**
 *************************************************************************/

void voice_user_section(char *id)
{
	vboxrc_find_user_from_id(id);
	vboxrc_find_user_section(setup.voice.section);
}

/*************************************************************************
 ** voice_set_compression():	Sets voice compression mode.					**
 *************************************************************************/

static int voice_set_compression(short c)
{
	char command[64];

	printstring(command, "AT+VSM=%d+VLS=2", c);

	log(L_DEBUG, "Setting voice compression \"%s\"...\n", compressions[c]);

	return(modem_command(command, "OK|VCON"));
}

/*************************************************************************
 ** 
 *************************************************************************/

static void voice_close_or_unlink(int fd, char *name)
{
	if (fd != -1) close(fd);
	
	if (name) unlink(name);
	if (name) unlink(name);
}


/*************************************************************************
 ** voice_put_message():	Plays voice message.									**
 *************************************************************************/

int voice_put_message(char *message)
{
/*
	vaheader_t header;

	char	linein[MODEM_BUFFER_LEN + 1];
	char	lineout[MODEM_BUFFER_LEN + MODEM_BUFFER_LEN + 1];
	int	fd;
	short	compression;
	int	i;
	int	bytesin;
	int	bytesout;
	int	written;

	Log(L_INFO, "Playing \"%s\"...\n", message);

	if (access(message, R_OK) != 0)
	{
		log(L_ERROR, "Message \"%s\" is not readable.\n", message);

		return(VOICE_ACTION_ERROR);
	}

	if ((fd = open(message, O_RDONLY)) == -1)
	{
		log(L_ERROR, "Can't open \"%s\".\n", message);

		return(VOICE_ACTION_ERROR);
	}

	if (read(fd, &header, sizeof(vaheader_t)) != sizeof(vaheader_t))
	{
		log(L_ERROR, "Can't read voice header from message.\n");

		close(fd);

		return(VOICE_ACTION_ERROR);
	}

	if (strncmp(Header.Magic, "RMD1", 4) != 0)
	{
		Log(L_ERROR, "No raw modem data header found.\n");

		Close(FD);
		SetModemTermIO(&SaveIO);

		Return(VOICE_ACTION_ERROR);
	}	

	if ((strcmp(Header.Modem, VOICE_MODEM) != 0) && (strcmp(Header.Modem, VOICE_LISDN) != 0))
	{
		Log(L_ERROR, "Wrong modem type found.\n");
		
		Close(FD);
		SetModemTermIO(&SaveIO);

		Return(VOICE_ACTION_ERROR);
	}

	compression = 4;

	if (!voice_set_compression(compression))
	{
		log(L_ERROR, "Can't set voice audio compression or line mode.\n");
		
		close(fd);

		return(VOICE_ACTION_ERROR);
	}

	if (modem_get_nocarrier_state())
	{
		close(FD);

		return(VOICE_ACTION_HANGUP);
	}
*/
		/* Ansonsten Modem in den Voice-Play-Modus versetzen und die	*/
		/* Nachricht spielen. AT+VTX liefert ein NO ANSWER wenn kein	*/
		/* Carrier mehr anliegt (gibt es beim ZyXEL nicht)!				*/
/*
	if (modem_command("AT+VTX", "CONNECT") == 0)
	{
		log(L_ERROR, "Can't start voice play mode.\n");
		
		close(fd);

		return(VOICE_ACTION_ERROR);
	}

	voicestatus	= VOICE_ACTION_OK;
	sequencestatus	= ST_NO_INPUT;

	while (voicestatus == VOICE_ACTION_OK)
	{
*/
			/* Daten vom Modem in den Buffer lesen. Wenn keine Daten mehr	*/
			/* vorhanden sind, Schleife abbrechen.									*/
/*
		if ((bytesin = read(fd, linein, MODEM_BUFFER_LEN)) <= 0) break;
*/
			/* Eingelesene Daten durchlaufen und alle <DLE> in <DLE><DLE>	*/
			/* umwandeln?!																	*/
/*
		bytesout = 0;

		for (i = 0; i < bytesin; i++)
		{
			lineout[bytesout] = linein[i];

			if (lineOot[bytesout++] == DLE) lineout[bytesout++] = DLE;
		}
*/                         
			/* Daten so lange schreiben, bis sie zu Ende sind oder ein	*/
			/* Fehler aufgetreten ist.												*/
/*
		Log(L_JUNK, "Play: <DATA %d incoming; ", bytesin);

		if (modem_get_nocarrier_state())
		{
			LogT(L_JUNK , "nothing outgoing>\n");
			LogL(L_DEBUG, "Modem returns \"NO CARRIER\" - Stop playing...\n");
               
			voicestatus = VOICE_ACTION_HANGUP;
		}
		else
		{
			LogT(L_JUNK, "%d outgoing>\n", BytesOut);

			Written	= 0;
			errno		= 0;

			while (((Written += RawWriteToModem(&LineOut[Written], BytesOut - Written)) != BytesOut) && (errno == 0));

			if ((Written != BytesOut) || (errno != 0))
			{
				Log(L_ERROR, "Could only write %d of %d bytes (%s).\n", Written, BytesOut, strerror(errno));

				VoiceStatus = VOICE_ACTION_ERROR;
			}
		}
*/
			/* Eingegende Daten vom Modem prüfen. Wenn ein <DLE><DC4> er-	*/
			/* kannt wird, wird die Message nicht mehr weiter gespielt.		*/

/*
		while ((TestModemInput()) && (VoiceStatus == VOICE_ACTION_OK))
		{
			Log(L_DEBUG, "Searching for touchtone sequences...\n");

			if ((BytesIn = RawReadFromModem(LineIn, MODEM_BUFFER_LEN)) > 0)
			{
				if (GetNoCarrierFlag())
				{
					Log(L_DEBUG, "Modem returns \"NO CARRIER\" - Stop playing...\n");

					VoiceStatus = VOICE_ACTION_HANGUP;
				}
				else
				{
					for (i = 0; i < BytesIn; i++) HandleDLE(LineIn[i]);
*/
						/* Wenn eine gültige Touchtonesequenze eingegeben	*/
						/* wurde und diese in der Breaklist steht, wird		*/
						/* das Spielen abgebrochen.								*/
/*
					if ((index(Touchtones, '#')) && (index(Touchtones, '*')))
					{
						Log(L_DEBUG, "Touchtone sequence \"%s\" found.\n", Touchtones);

						if (ListFindNode(&Breaklist, Touchtones))
						{
							Log(L_INFO, "Sequence \"%s\" found in breaklist...\n", Touchtones);

							VoiceStatus = VOICE_ACTION_TOUCHTONES;
						}
					}
				}
			}
			else Log(L_ERROR, "Can't read input from modem.\n");
		}
	}

	Close(FD);

	Pause(500);
*/
		/* Wenn der Carrier noch besteht, ein <DLE><ETX> ans Modem	*/
		/* schicken. Der Emulator antwortet dann mit einem "VCON".	*/
/*
	if (GetNoCarrierFlag() == FALSE)
	{
		SPrintF(LineOut, "%c%c%c%c", DLE, DC4, DLE, ETX);

		Log(L_JUNK, "Play: <DLE><DC4>\n");
		Log(L_JUNK, "Play: <DLE><ETX>\n");
                    
		RawWriteToModem(LineOut, strlen(LineOut));

		SPrintF(LineOut, "VCON|%c%c", DLE, ETX);

		ModemCommand("\r",  LineOut);
	}

	SetModemTermIO(&SaveIO);

	ModemCommand("AT", "OK|VCON");
	ModemCommand("AT", "OK");

	if (GetNoCarrierFlag()) VoiceStatus = VOICE_ACTION_HANGUP;

	Return(VoiceStatus);
*/

	return(VOICE_ACTION_OK);
}

/*************************************************************************
 ** voice_get_message():	Record a message.										**
 *************************************************************************
 ** If save is true the recorded data are not written.						**
 *************************************************************************/

int voice_get_message(char *name, char *timestr, int save)
{
	vaheader_t	header;
	char			line_i[MODEM_BUFFER_LEN + 1];
	char			line_o[MODEM_BUFFER_LEN + 1];
	int			byte_i;
	int			byte_o;
	int			result;
	int			havedle;
	int			savetimeout;
	int			fd;

	savetimeout = xstrtol(timestr, 90);

	if (save)
		log(L_DEBUG, "Recording \"%s\" (%d secs)...\n", name, savetimeout);
	else
		log(L_DEBUG, "Waiting %d secs for input...\n", savetimeout);

	if (!voice_set_compression(setup.modem.compression))
	{
		log(L_ERROR, "Can't set voice audio compressen.\n");

		return(VOICE_ACTION_ERROR);
	}

	if (save)
	{
		if ((fd = open(name, O_WRONLY|O_CREAT|O_TRUNC, S_IWUSR|S_IWGRP|S_IWOTH)) == -1)
	   {
	   	log(L_ERROR, "Can't create \"%s\".\n", name);

			return(VOICE_ACTION_ERROR);
		}

		truncate(name, 0);

		voice_set_header(&header);

		if (!header_put(fd, &header))
		{
	      log(L_ERROR, "Can't write raw voice audio header.\n");
      
			voice_close_or_unlink(fd, name);

			return(VOICE_ACTION_ERROR);
		}
	}
	else fd = -1;

	if (modem_get_nocarrier_state())
	{
		if (save) voice_close_or_unlink(fd, name);

		return(VOICE_ACTION_LOCALHANGUP);
	}

	if (modem_command("AT+VRX", "CONNECT") == 0)
	{
		log(L_ERROR, "Can't start record mode.\n");

		if (save) voice_close_or_unlink(fd, name);

		return(VOICE_ACTION_ERROR);
	}

		/* Solange Daten aufzeichnen, bis der Timeout abgelaufen ist oder	*/
		/* der Anrufer aufgelegt hat.													*/

	sequencestatus	= ST_NO_INPUT;
	voicestatus		= VOICE_ACTION_OK;
	havedle			= FALSE;

	modem_set_timeout(savetimeout);

	while (voicestatus == VOICE_ACTION_OK)
	{
		byte_o = 0;
		byte_i = 0;
		result = 0;

		while ((byte_o < MODEM_BUFFER_LEN) && (voicestatus == VOICE_ACTION_OK))
		{
			if ((result = modem_raw_read(line_i, 1)) == 1)
			{
				byte_i++;

				if (havedle)
				{
					havedle = FALSE;
                              
					switch (line_i[0])
					{
						case DLE:
							line_o[byte_o++] = DLE;
							break;

						case ETX:
							voicestatus = VOICE_ACTION_REMOTEHANGUP;
							break;

						default:
							voice_handle_dle(DLE);
							voice_handle_dle(line_i[0]);
							break;
					}
				}
				else
				{
					if (line_i[0] != DLE)
					{
						line_o[byte_o++] = line_i[0];
					}
					else havedle = TRUE;
				}
			}
			else break;
		}

		if (byte_o > 0)
		{
			if (save)
			{
				log(L_JUNK, "Record: <DATA %d incoming; %d outgoing>\n", byte_i, byte_o);
                         
				write(fd, line_o, byte_o);
			}
			else log(L_JUNK, "Wait: <DATA %d incoming>\n", byte_i);
		}

		if ((result != 1) || (modem_get_timeout()))
		{
			if (!modem_get_timeout())
			{
				log(L_ERROR, "Can't read incoming data (%s).\n", strerror(errno));
				
				voicestatus = VOICE_ACTION_ERROR;
			}
			else voicestatus = VOICE_ACTION_TIMEOUT;
		}

		if ((voicestatus == VOICE_ACTION_OK) || (voicestatus == VOICE_ACTION_TIMEOUT))
		{
			if ((index(touchtones, '#')) && (index(touchtones, '*')))
			{
				log(L_DEBUG, "Touchtone sequence \"%s\" found.\n", touchtones);

				if (list_find_node(&breaklist, touchtones))
				{
					log(L_INFO, "Sequence \"%s\" found in breaklist...\n", touchtones);

					voicestatus = VOICE_ACTION_TOUCHTONES;
				}
				else
				{
					log(L_DEBUG, "Sequence \"%s\" not in breaklist (ignored)...\n", touchtones);

					*touchtones = '\0';
				}
			}
		}
	}

	modem_set_timeout(0);

	if (save)
	{
		close(fd);

		permissions_set(name, setup.users.uid, setup.users.gid, S_IRUSR|S_IRGRP|S_IROTH|S_IWUSR|S_IWGRP|S_IWOTH, setup.users.umask);
	}

	if ((voicestatus == VOICE_ACTION_REMOTEHANGUP) || (modem_get_nocarrier_state()))
	{
			/* Remote hangup: Modem should response with the sequence	*/
			/* VCON or NO CARRIER.													*/

		printstring(line_i, "VCON|NO CARRIER|%c%c", DLE, ETX);

		log(L_JUNK, "%s: <DLE><ETX>\n", (save ? "Record" : "Wait"));

		modem_command("", line_i);	/* NO CARRIER */
		modem_command("", line_i);	/* <DLE><ETX> */
	}
	else
	{
			/* Local hangup: send <DLE><DC4> to the modem and read the	*/
			/* response <DLE><ETX>.													*/

		printstring(line_i, "VCON|NO CARRIER|%c%c", DLE, ETX);
		printstring(line_o, "%c%c", DLE, DC4);

		log(L_JUNK, "Sending \"<DLE><DC4>\"...\n");

		modem_raw_write(line_o, strlen(line_o));

		modem_command("", line_i);	/* VCON */
	}

	if (modem_get_nocarrier_state()) voicestatus = VOICE_ACTION_REMOTEHANGUP;

	return(voicestatus);
}









/*************************************************************************
 ** voice_set_header():	Fills the vbox audio header.							**
 *************************************************************************/

static void voice_set_header(vaheader_t *header)
{
	memset(header, 0, sizeof(vaheader_t));

	xstrncpy(header->magic	 , VAH_MAGIC				, VAH_MAX_MAGIC	);
	xstrncpy(header->name	 , setup.voice.name		, VAH_MAX_NAME		);
	xstrncpy(header->callerid, setup.voice.callerid	, VAH_MAX_CALLERID);
	xstrncpy(header->phone	 , setup.voice.phone		, VAH_MAX_PHONE	);
	xstrncpy(header->location, "<not supported>"		, VAH_MAX_LOCATION);

	header->time			= htonl(time(NULL));
	header->compression	= htonl(setup.modem.compression);
}


static void voice_handle_dle(char Byte)
{
	log_line(L_JUNK, "Sequence: Found \"");
	log_char(L_JUNK, Byte);
	log_text(L_JUNK, "\"%s.\n", ((sequencestatus == ST_NO_INPUT) ? "" : " (<DLE>)"));

	switch (sequencestatus)
	{
		case ST_NO_INPUT:
               
			switch (Byte)
			{
				case DLE:
					sequencestatus = ST_GOT_DLE;
					break;

				case XON:
					log(L_WARN, "Sequence: Found \"<XON>\".\n");
					break;

				case XOFF:
					log(L_WARN, "Sequence: Found \"<XOFF>\".\n");
					break;

				case NL:
				case CR:
					break;

				default:
					log_line(L_ERROR, "Sequence: Illegal \"");
					log_char(L_ERROR, Byte);
					log_text(L_ERROR, "\".\n");
					break;
			}
                                   
			return;

		case ST_GOT_DLE:
          
			switch (Byte)
			{
				case DC4:
					log(L_DEBUG, "Sequence: Found \"<DLE><DC4>\" - Stop current action...\n");

					voicestatus = VOICE_ACTION_LOCALHANGUP;

					break;

				case ETX:
					log(L_DEBUG, "Sequence: Found \"<DLE><ETX>\" - Cancel current action...\n");

					voicestatus = VOICE_ACTION_LOCALHANGUP;

					break;

					/* MFV-Ton 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, * oder # er-	*/
					/* kannt (Touchtone).											*/
	
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
				case '*':
				case '#':
					voice_handle_touchtone(Byte);
					break;

					/* Besetztzeichen erkannt */

				case 'b':
					log(L_DEBUG, "Sequence: Found busytone - Hangup...\n");

					voicestatus = VOICE_ACTION_LOCALHANGUP;

					break;
					
					/* T.30 Faxrufton erkannt */

				case 'c':
					log(L_DEBUG, "Sequence: Found T.30 faxtone - Cancel current action...\n");

					voicestatus = VOICE_ACTION_LOCALHANGUP;

					break;
					
					/* Datenträger eines Modems erkannt */
					
				case 'e':
					break;

					/* Wählton erkannt */

				case 'd':
					break;

					/* Quiet (Ruhe) erkannt. Das Modem hat erkannt, daß zu-	*/
					/* nächst Sprachdaten angekommen sind und eine Stille		*/
					/* folgte, die länger war als durch AT+VSD definiert.		*/

				case 'q':
					log(L_DEBUG, "Sequence: Found quiet - Cancel current action...\n");

					voicestatus = VOICE_ACTION_LOCALHANGUP;

					break;

					/* Silence (Stille) erkannt. Das Modem hat bei Beginn	*/
					/* des Sprachmodus keine Sprachdaten erkannt, aber es	*/
					/* wurde eine Stille erkannt, die länger war als dur-	*/
					/* ch AT+VSD definiert.											*/

				case 's':
					log(L_DEBUG, "Sequence: Found silence - Cancel current action...\n");

					voicestatus = VOICE_ACTION_LOCALHANGUP;

					break;

				default:
					log_line(L_ERROR, "Sequence: Illeagal <DLE> shielded code \"");
					log_char(L_ERROR, Byte);
					log_text(L_ERROR, "\".\n");
					break;
			}

			sequencestatus = ST_NO_INPUT;

			return;
	}
}


static void voice_handle_touchtone(char Tone)
{
	char Temp[2];

	if (Tone == '*')
	{
		printstring(touchtones, "*");
		
		return;
	}

	if (*touchtones != '*')
	{
		if ((Tone != '#') && (Tone != '*'))
		{
			printstring(touchtones, "*%c#", Tone);
		}
		else
		{
			if (Tone == '#') printstring(touchtones, "*#");
		}

		return;
	}

	if (index(touchtones, '#'))
	{
		if ((Tone != '#') && (Tone != '*'))
		{
			printstring(touchtones, "*%c#", Tone);
		}
		else
		{
			if (Tone == '#') printstring(touchtones, "*#");
		}

		return;
	}

	if (strlen(touchtones) < TOUCHTONE_BUFFER_LEN)
	{
		Temp[0] = Tone;
		Temp[1] = 0;
		
		strcat(touchtones, Temp);
	}
}

