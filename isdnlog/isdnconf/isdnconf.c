/* $Id: isdnconf.c,v 1.32 1999/06/15 20:03:46 akool Exp $
 *
 * ISDN accounting for isdn4linux. (Report-module)
 *
 * Copyright 1996, 1999 by Stefan Luethje (luethje@sl-gw.lake.de)
 *                     and Andreas Kool (akool@isdn4linux.de)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * $Log: isdnconf.c,v $
 * Revision 1.32  1999/06/15 20:03:46  akool
 * isdnlog Version 3.33
 *   - big step in using the new zone files
 *   - *This*is*not*a*production*ready*isdnlog*!!
 *   - Maybe the last release before the I4L meeting in Nuernberg
 *
 * Revision 1.31  1999/06/13 14:07:28  akool
 * isdnlog Version 3.32
 *
 *  - new option "-U1" (or "ignoreCOLP=1") to ignore CLIP/COLP Frames
 *  - TEI management decoded
 *
 * Revision 1.30  1999/06/09 19:58:12  akool
 * isdnlog Version 3.31
 *  - Release 0.91 of zone-Database (aka "Verzonungstabelle")
 *  - "rate-de.dat" V:1.02-Germany [09-Jun-1999 21:45:26]
 *
 * Revision 1.29  1999/06/03 18:50:10  akool
 * isdnlog Version 3.30
 *  - rate-de.dat V:1.02-Germany [03-Jun-1999 19:49:22]
 *  - small fixes
 *
 * Revision 1.28  1999/06/01 19:33:27  akool
 * rate-de.dat V:1.02-Germany [01-Jun-1999 20:52:32]
 *
 * Revision 1.27  1999/05/22 10:18:18  akool
 * isdnlog Version 3.29
 *
 *  - processing of "sonderrufnummern" much more faster
 *  - detection for sonderrufnummern of other provider's implemented
 *    (like 01929:FreeNet)
 *  - Patch from Oliver Lauer <Oliver.Lauer@coburg.baynet.de>
 *  - Patch from Markus Schoepflin <schoepflin@ginit.de>
 *  - easter computing corrected
 *  - rate-de.dat 1.02-Germany [22-May-1999 11:37:33] (from rate-CVS)
 *  - countries-de.dat 1.02-Germany [22-May-1999 11:37:47] (from rate-CVS)
 *  - new option "-B" added (see README)
 *    (using "isdnlog -B16 ..." isdnlog now works in the Netherlands!)
 *
 * Revision 1.26  1999/05/13 11:39:09  akool
 * isdnlog Version 3.28
 *
 *  - "-u" Option corrected
 *  - "ausland.dat" removed
 *  - "countries-de.dat" fully integrated
 *      you should add the entry
 *      "COUNTRYFILE = /usr/lib/isdn/countries-de.dat"
 *      into section "[ISDNLOG]" of your config file!
 *  - rate-de.dat V:1.02-Germany [13-May-1999 12:26:24]
 *  - countries-de.dat V:1.02-Germany [13-May-1999 12:26:26]
 *
 * Revision 1.25  1999/05/09 18:24:10  akool
 * isdnlog Version 3.25
 *
 *  - README: isdnconf: new features explained
 *  - rate-de.dat: many new rates from the I4L-Tarifdatenbank-Crew
 *  - added the ability to directly enter a country-name into "rate-xx.dat"
 *
 * Revision 1.24  1999/05/04 19:32:23  akool
 * isdnlog Version 3.24
 *
 *  - fully removed "sondernummern.c"
 *  - removed "gcc -Wall" warnings in ASN.1 Parser
 *  - many new entries for "rate-de.dat"
 *  - better "isdnconf" utility
 *
 * Revision 1.23  1999/04/30 19:07:46  akool
 * isdnlog Version 3.23
 *
 *  - changed LCR probing duration from 181 seconds to 153 seconds
 *  - "rate-de.dat" filled with May, 1. rates
 *
 * Revision 1.22  1999/04/19 19:24:02  akool
 * isdnlog Version 3.18
 *
 * - countries-at.dat added
 * - spelling corrections in "countries-de.dat" and "countries-us.dat"
 * - LCR-function of isdnconf now accepts a duration (isdnconf -c .,duration)
 * - "rate-at.dat" and "rate-de.dat" extended/fixed
 * - holiday.c and rate.c fixed (many thanks to reinelt@eunet.at)
 *
 * Revision 1.21  1999/04/17 14:10:56  akool
 * isdnlog Version 3.17
 *
 * - LCR functions of "isdnconf" fixed
 * - HINT's fixed
 * - rate-de.dat: replaced "1-5" with "W" and "6-7" with "E"
 *
 * Revision 1.20  1999/04/15 19:14:29  akool
 * isdnlog Version 3.15
 *
 * - reenable the least-cost-router functions of "isdnconf"
 *   try "isdnconf -c <areacode>" or even "isdnconf -c ."
 * - README: "rate-xx.dat" documented
 * - small fixes in processor.c and rate.c
 * - "rate-de.dat" optimized
 * - splitted countries.dat into countries-de.dat and countries-us.dat
 *
 * Revision 1.19  1999/04/14 13:16:18  akool
 * isdnlog Version 3.14
 *
 * - "make install" now install's "rate-xx.dat", "rate.conf" and "ausland.dat"
 * - "holiday-xx.dat" Version 1.1
 * - many rate fixes (Thanks again to Michael Reinelt <reinelt@eunet.at>)
 *
 * Revision 1.18  1999/04/10 16:35:14  akool
 * isdnlog Version 3.13
 *
 * WARNING: This is pre-ALPHA-dont-ever-use-Code!
 * 	 "tarif.dat" (aka "rate-xx.dat"): the next generation!
 *
 * You have to do the following to test this version:
 *   cp /usr/src/isdn4k-utils/isdnlog/holiday-de.dat /etc/isdn
 *   cp /usr/src/isdn4k-utils/isdnlog/rate-de.dat /usr/lib/isdn
 *   cp /usr/src/isdn4k-utils/isdnlog/samples/rate.conf.de /etc/isdn/rate.conf
 *
 * After that, add the following entries to your "/etc/isdn/isdn.conf" or
 * "/etc/isdn/callerid.conf" file:
 *
 * [ISDNLOG]
 * SPECIALNUMBERS = /usr/lib/isdn/sonderrufnummern.dat
 * HOLIDAYS       = /usr/lib/isdn/holiday-de.dat
 * RATEFILE       = /usr/lib/isdn/rate-de.dat
 * RATECONF       = /etc/isdn/rate.conf
 *
 * Please replace any "de" with your country code ("at", "ch", "nl")
 *
 * Good luck (Andreas Kool and Michael Reinelt)
 *
 * Revision 1.17  1999/04/03 12:46:54  akool
 * - isdnlog Version 3.12
 * - "%B" tag in ILABEL/OLABEL corrected
 * - isdnlog now register's incoming calls when there are no free B-channels
 *   (idea from sergio@webmedia.es)
 * - better "samples/rate.conf.de" (suppress provider without true call-by-call)
 * - "tarif.dat" V:1.17 [03-Apr-99]
 * - Added EWE-Tel rates from Reiner Klaproth <rk1@msjohan.dd.sn.schule.de>
 * - isdnconf can now be used to generate a Least-cost-router table
 *   (try "isdnconf -c .")
 * - isdnlog now simulate a RELEASE COMPLETE if nothing happpens after a SETUP
 * - CHARGEMAX Patches from Oliver Lauer <Oliver.Lauer@coburg.baynet.de>
 *
 * Revision 1.16  1999/03/24 19:37:38  akool
 * - isdnlog Version 3.10
 * - moved "sondernnummern.c" from isdnlog/ to tools/
 * - "holiday.c" and "rate.c" integrated
 * - NetCologne rates from Oliver Flimm <flimm@ph-cip.uni-koeln.de>
 * - corrected UUnet and T-Online rates
 *
 * Revision 1.15  1999/03/20 14:32:56  akool
 * - isdnlog Version 3.08
 * - more tesion)) Tarife from Michael Graw <Michael.Graw@bartlmae.de>
 * - use "bunzip -f" from Franz Elsner <Elsner@zrz.TU-Berlin.DE>
 * - show another "cheapest" hint if provider is overloaded ("OVERLOAD")
 * - "make install" now makes the required entry
 *     [GLOBAL]
 *     AREADIFF = /usr/lib/isdn/vorwahl.dat
 * - README: Syntax description of the new "rate-at.dat"
 * - better integration of "sondernummern.c" from mario.joussen@post.rwth-aachen.de
 * - server.c: buffer overrun fix from Michael.Weber@Post.RWTH-Aachen.DE (Michael Weber)
 *
 * Revision 1.14  1999/03/15 21:27:48  akool
 * - isdnlog Version 3.06
 * - README: explain some terms about LCR, corrected "-c" Option of "isdnconf"
 * - isdnconf: added a small LCR-feature - simply try "isdnconf -c 069"
 * - isdnlog: dont change CHARGEINT, if rate is't known!
 * - sonderrufnummern 1.02 [15-Mar-99] :: added WorldCom
 * - tarif.dat 1.09 [15-Mar-99] :: added WorldCom
 * - isdnlog now correctly handles the new "Ortstarif-Zugang" of UUnet
 *
 * Revision 1.13  1999/03/07 18:18:42  akool
 * - new 01805 tarif of DTAG
 * - new March 1999 tarife
 * - added new provider "01051 Telecom"
 * - fixed a buffer overrun from Michael Weber <Michael.Weber@Post.RWTH-Aachen.DE>
 * - fixed a bug using "sondernnummern.c"
 * - fixed chargeint change over the time
 * - "make install" now install's "sonderrufnummern.dat", "tarif.dat",
 *   "vorwahl.dat" and "tarif.conf"! Many thanks to
 *   Mario Joussen <mario.joussen@post.rwth-aachen.de>
 * - Euracom Frames would now be ignored
 * - fixed warnings in "sondernnummern.c"
 * - "10plus" messages no longer send to syslog
 *
 * Revision 1.12  1998/09/22 20:59:08  luethje
 * isdnrep:  -fixed wrong provider report
 *           -fixed wrong html output for provider report
 *           -fixed strange html output
 * kisdnlog: -fixed "1001 message window" bug ;-)
 *
 * Revision 1.11  1998/05/11 19:43:43  luethje
 * Some changes for "vorwahlen.dat"
 *
 * Revision 1.10  1998/05/10 22:11:52  luethje
 * Added support for VORWAHLEN2.EXE
 *
 * Revision 1.9  1997/05/25 19:40:53  luethje
 * isdnlog:  close all files and open again after kill -HUP
 * isdnrep:  support vbox version 2.0
 * isdnconf: changes by Roderich Schupp <roderich@syntec.m.EUnet.de>
 * conffile: ignore spaces at the end of a line
 *
 * Revision 1.8  1997/05/05 21:21:42  luethje
 * bugfix for option -M
 *
 * Revision 1.6  1997/04/15 00:20:01  luethje
 * replace variables: some bugfixes, README comleted
 *
 * Revision 1.5  1997/04/10 23:32:15  luethje
 * Added the feature, that environment variables are allowed in the config files.
 *
 * Revision 1.4  1997/04/06 21:06:08  luethje
 * problem with empty/not existing file resolved.
 *
 * Revision 1.3  1997/04/03 22:36:23  luethje
 * splitt the file isdn.conf into callerid.conf and ~/.isdn (for editing).
 *
 * Revision 1.2  1997/03/23 20:58:31  luethje
 * some bugfixes
 *
 */

#include "isdnconf.h"
#include "tools/zone.h"

/*****************************************************************************/
#define ZAUNPFAHL  1 /* FIXME: Michi: Offset */
/*****************************************************************************/

int print_in_modules(const char *fmt, ...);
int print_msg(int Level, const char *fmt, ...);
int add_data(section **conf_dat);
int delete_data(char *, char *, section **conf_dat);
int look_data(section **conf_dat);
int find_data(char *_alias, char *_number, section *conf_dat);
const char* make_word(const char *in);
char* tmp_dup(const char *in);
int add_line(section **Ptr, const char *Name);

/*****************************************************************************/

static int and    = 0;
static int add    = 0;
static int del    = 0;
static int msn    = 0;
static int quiet  = 0;
static int short_out= 0;
static int long_out= 0;
static int oneentry= 0;
static int isdnmon = 0;

static int match_flags = F_NO_HOLE_WORD;

static char areacode[SHORT_STRING_SIZE] = "";
static char si[SHORT_STRING_SIZE];
static char number[BUFSIZ] = "";
static char alias[BUFSIZ] = "";
static char conffile[BUFSIZ];
static char callerfile[BUFSIZ];

/*****************************************************************************/

void info(int chan, int reason, int state, char *msg)
{
  /* DUMMY - dont needed here! */
} /* info */

/*****************************************************************************/

int add_line(section **Ptr, const char *Name)
{
	char buf[BUFSIZ] = "";


	if (fgets(buf,BUFSIZ,stdin) != NULL)
	{
		if (strlen(buf) > 1)
		{
			buf[strlen(buf)-1] = '\0';

			if (Set_Entry(*Ptr,NULL,tmp_dup(Name),buf,C_WARN) == NULL)
			{
				print_msg(PRT_ERR,"%s\n","Can not allocate memory!");
				return -1;
			}
		}
	}
	else
	{
		free_section(*Ptr);
		*Ptr = NULL;
		return -1;
	}

	return 0;
}

/*****************************************************************************/

int add_data(section **conf_dat)
{
	int Cnt = 0;
	section *NewPtr = NULL;
	section *SubPtr = NULL;


	while(1)
	{
		NewPtr = NULL;

		if (Set_Section(&NewPtr,tmp_dup(msn?CONF_SEC_MSN:CONF_SEC_NUM),C_NOT_UNIQUE) == NULL)
		{
			print_msg(PRT_ERR,"%s\n","Can not allocate memory!");
			return -1;
		}

		if (!quiet)
			print_msg(PRT_NORMAL,"%s:\t\t",make_word(CONF_ENT_ALIAS));
		if (add_line(&NewPtr,CONF_ENT_ALIAS))
			break;

		if (!quiet)
			print_msg(PRT_NORMAL,"%s:\t\t",make_word(CONF_ENT_NUM));
		if (add_line(&NewPtr,CONF_ENT_NUM))
			break;

		if (!quiet)
			print_msg(PRT_NORMAL,"%s:\t\t",CONF_ENT_SI);
		if (add_line(&NewPtr,CONF_ENT_SI))
			break;

		if (!quiet)
			print_msg(PRT_NORMAL,"%s:\t\t",make_word(CONF_ENT_ZONE));
		if (add_line(&NewPtr,CONF_ENT_ZONE))
			break;

		if (!quiet)
			print_msg(PRT_NORMAL,"%s:\t",make_word(CONF_ENT_INTFAC));
		if (add_line(&NewPtr,CONF_ENT_INTFAC))
			break;

		while(1)
		{
			SubPtr = NULL;

			if (Set_Section(&SubPtr,tmp_dup(CONF_SEC_FLAG),C_NOT_UNIQUE) == NULL)
			{
				print_msg(PRT_ERR,"%s\n","Can not allocate memory!");
				return -1;
			}

			if (!quiet)
				print_msg(PRT_NORMAL,"%s:\t\t",make_word(CONF_ENT_FLAGS));
			if (add_line(&SubPtr,CONF_ENT_FLAGS))
				break;

			if (!quiet)
				print_msg(PRT_NORMAL,"%s:\t",make_word(CONF_ENT_PROG));
			if (add_line(&SubPtr,CONF_ENT_PROG))
				break;

			if (!quiet)
				print_msg(PRT_NORMAL,"%s:\t\t",make_word(CONF_ENT_USER));
			if (add_line(&SubPtr,CONF_ENT_USER))
				break;

			if (!quiet)
				print_msg(PRT_NORMAL,"%s:\t\t",make_word(CONF_ENT_GROUP));
			if (add_line(&SubPtr,CONF_ENT_GROUP))
				break;

			if (!quiet)
				print_msg(PRT_NORMAL,"%s:\t",make_word(CONF_ENT_INTVAL));
			if (add_line(&SubPtr,CONF_ENT_INTVAL))
				break;

			if (!quiet)
				print_msg(PRT_NORMAL,"%s:\t\t",make_word(CONF_ENT_TIME));
			if (add_line(&SubPtr,CONF_ENT_TIME))
				break;

			Set_SubSection(NewPtr,tmp_dup(CONF_ENT_START),SubPtr,C_APPEND | C_WARN);
		}

		if (!quiet)
			print_msg(PRT_NORMAL,"\n\n");

		while(*conf_dat != NULL)
			conf_dat = &((*conf_dat)->next);

		*conf_dat = NewPtr;
		Cnt++;
	}

	if (!quiet)
		print_msg(PRT_NORMAL,"\n");

	return Cnt;
}

/*****************************************************************************/

int delete_data(char * _alias , char * _number, section **conf_dat)
{
	short_out = 1;

	if (!quiet)
		print_msg(PRT_NORMAL,"Following entry deleted!\n");

	if (!quiet)
		find_data(_alias,_number,*conf_dat);

	Del_Section(conf_dat,NULL);
	return 0;
}

/*****************************************************************************/

int find_data(char *_alias, char *_number, section *conf_dat)
{
	auto char *ptr;
	auto entry *CEPtr;
	auto section *SPtr;
	const char *area;

	if (quiet)
	{
		print_msg(PRT_NORMAL,"%s",_alias?_alias:(number[0]?expand_number(number):alias));
	}
	else
	{
		print_msg(PRT_NORMAL,"%s:\t\t%s\n",make_word(CONF_ENT_ALIAS),_alias?_alias:S_UNKNOWN);
		print_msg(PRT_NORMAL,"%s:\t\t%s\n",make_word(CONF_ENT_NUM),_number?_number:S_UNKNOWN);

		if (_number != NULL && (ptr = get_areacode(_number,NULL,C_NO_ERROR)) != NULL)
			print_msg(PRT_NORMAL,"Location:\t%s\n",ptr);

		if (!short_out)
		{
			ptr = (CEPtr = Get_Entry(conf_dat->entries,CONF_ENT_SI))?(CEPtr->value?CEPtr->value:"0"):"0";
			print_msg(PRT_NORMAL,"%s:\t\t%s\n",CONF_ENT_SI,ptr);

			area = area_diff_string(NULL,_number);
			ptr = (char*)(const char*) (area[0] != '\0'?area:(CEPtr = Get_Entry(conf_dat->entries,CONF_ENT_ZONE))?(CEPtr->value?CEPtr->value:""):"");
			print_msg(PRT_NORMAL,"%s:\t\t%s\n",make_word(CONF_ENT_ZONE),ptr);

			ptr = (CEPtr = Get_Entry(conf_dat->entries,CONF_ENT_INTFAC))?(CEPtr->value?CEPtr->value:""):"";
			print_msg(PRT_NORMAL,"%s:\t%s\n",make_word(CONF_ENT_INTFAC),ptr);
		}

		if (long_out)
		{
			SPtr = Get_SubSection(conf_dat,CONF_ENT_START);

			while (SPtr != NULL)
			{
				ptr = (CEPtr = Get_Entry(SPtr->entries,CONF_ENT_FLAGS))?(CEPtr->value?CEPtr->value:""):"";
				print_msg(PRT_NORMAL,"%s:\t\t%s\n",make_word(CONF_ENT_FLAGS),ptr);

				ptr = (CEPtr = Get_Entry(SPtr->entries,CONF_ENT_PROG))?(CEPtr->value?CEPtr->value:""):"";
				print_msg(PRT_NORMAL,"%s:\t%s\n",make_word(CONF_ENT_PROG),ptr);

				ptr = (CEPtr = Get_Entry(SPtr->entries,CONF_ENT_USER))?(CEPtr->value?CEPtr->value:""):"";
				print_msg(PRT_NORMAL,"%s:\t%s\n",make_word(CONF_ENT_USER),ptr);

				ptr = (CEPtr = Get_Entry(SPtr->entries,CONF_ENT_GROUP))?(CEPtr->value?CEPtr->value:""):"";
				print_msg(PRT_NORMAL,"%s:\t%s\n",make_word(CONF_ENT_GROUP),ptr);

				ptr = (CEPtr = Get_Entry(SPtr->entries,CONF_ENT_INTVAL))?(CEPtr->value?CEPtr->value:""):"";
				print_msg(PRT_NORMAL,"%s:\t%s\n",make_word(CONF_ENT_INTVAL),ptr);

				ptr = (CEPtr = Get_Entry(SPtr->entries,CONF_ENT_TIME))?(CEPtr->value?CEPtr->value:""):"";
				print_msg(PRT_NORMAL,"%s:\t\t%s\n",make_word(CONF_ENT_TIME),ptr);

				SPtr = SPtr->next;
			}
		}
	}

	print_msg(PRT_NORMAL,"\n");

	return 0;
}

/*****************************************************************************/

int look_data(section **conf_dat)
{
	int Cnt = 0;
	auto entry *CEPtr;
	char *_number = NULL, *_alias = NULL;
	char _si[SHORT_STRING_SIZE];
	section *old_conf_dat = NULL;


	while (*conf_dat != NULL)
	{
		old_conf_dat = *conf_dat;

		if (!strcmp((*conf_dat)->name,CONF_SEC_NUM) || !strcmp((*conf_dat)->name,CONF_SEC_MSN))
		{
			int Ret = 0;

			free(_number);
			free(_alias);
			_number = _alias = NULL;
			_si[0] = '\0';

			if ((CEPtr = Get_Entry((*conf_dat)->entries,CONF_ENT_NUM)) != NULL)
				if (del)
					_number = strdup(Replace_Variable(CEPtr->value));
				else
					_number = strdup(CEPtr->value);

			if ((CEPtr = Get_Entry((*conf_dat)->entries,CONF_ENT_ALIAS)) != NULL)
				if (del)
					_alias = strdup(Replace_Variable(CEPtr->value));
				else
					_alias = strdup(CEPtr->value);

			if ((CEPtr = Get_Entry((*conf_dat)->entries,CONF_ENT_SI)) != NULL &&
			    CEPtr->value != NULL)
				if (del)
					sprintf(_si,"%ld",strtol(Replace_Variable(CEPtr->value), NIL, 0));
				else
					sprintf(_si,"%ld",strtol(CEPtr->value, NIL, 0));

			if (and)
				Ret = 1;
			else
				Ret = 0;

			if (number[0] && _number != NULL)
			{
				if (!num_match(_number,number))
					Ret = 1;
				else
					Ret = 0;
			}

			if (alias[0] && _alias != NULL)
			{
				int Ret2 = 0;

				if (!match(alias,_alias,match_flags))
					Ret2 = 1;

				if (and)
					Ret &= Ret2;
				else
					Ret |= Ret2;
			}

			if (si[0])
			{
				int Ret2 = 0;

				if (*_si == '\0' || !strcmp(_si,si))
					Ret2 = 1;

				if (and)
					Ret &= Ret2;
				else
					Ret |= Ret2;
			}

			if (Ret == 1)
			{
				if (del)
				{
					delete_data(_alias,_number,conf_dat);
					Cnt++;
				}
				else
				{
					find_data(_alias,_number,*conf_dat);
					Cnt++;
				}

				if (oneentry)
					break;
			}
		}

		if (*conf_dat != NULL && old_conf_dat == *conf_dat)
			conf_dat = &((*conf_dat)->next);
	}

	if (Cnt == 0 && quiet && !del)
		find_data(NULL,_number,*conf_dat);

	free(_number);
	free(_alias);
	return Cnt;
}

/*****************************************************************************/

char* tmp_dup(const char *in)
{
	static char out[SHORT_STRING_SIZE];

	strcpy(out,in);
	return out;
}

/*****************************************************************************/

const char* make_word(const char *in)
{
	int i = 0;
	static char out[SHORT_STRING_SIZE];

	out[0] ='\0';

	while(in[i] != '\0')
		out[i] = tolower(in[i]),i++;

	out[i] ='\0';
	out[0] = toupper(out[0]);

	return out;
}

/*****************************************************************************/

int print_msg(int Level, const char *fmt, ...)
{
	auto va_list ap;
	auto char    String[LONG_STRING_SIZE];


	va_start(ap, fmt);
	vsnprintf(String, LONG_STRING_SIZE, fmt, ap);
	va_end(ap);

	if (Level & PRT_ERR)
		fprintf(stderr, "%s", String);
	else
		fprintf(stdout, "%s", String);

	return 0;
}

/*****************************************************************************/

int print_in_modules(const char *fmt, ...)
{
	auto va_list ap;
	auto char    String[LONG_STRING_SIZE];


	va_start(ap, fmt);
	(void)vsnprintf(String, LONG_STRING_SIZE, fmt, ap);
	va_end(ap);

	return print_msg(PRT_ERR, "%s", String);
}

/*****************************************************************************/

static char *printrate(RATE Rate)
{
  static char message[BUFSIZ];


  if (Rate.Basic > 0)
    sprintf(message, "%s %s + %s/%3.1fs = %s %s + %s/Min (%s)",
      currency, double2str(Rate.Basic, 5, 3, DEB),
      double2str(Rate.Price, 5, 3, DEB),
      Rate.Duration,
      currency, double2str(Rate.Basic, 5, 3, DEB),
      double2str(60 * Rate.Price / Rate.Duration, 5, 3, DEB),
      explainRate(&Rate));
  else
    sprintf(message, "%s %s/%3.1fs = %s %s/Min (%s)",
      currency, double2str(Rate.Price, 5, 3, DEB),
      Rate.Duration,
      currency, double2str(60 * Rate.Price / Rate.Duration, 5, 3, DEB),
      explainRate(&Rate));

  return(message);
} /* printrate */


#define MAXPROVIDER 1000

typedef struct {
  int    prefix;
  double rate;
  char  *explain;
  char  *msg;
} SORT;

static SORT sort[MAXPROVIDER];


static int compare(const void *s1, const void *s2)
{
  return(((SORT *)s1)->rate > ((SORT *)s2)->rate);
} /* compare */


static void showRates(RATE Rate, char *message)
{
  if (Rate.Basic > 0)
    sprintf(message, "%s %s + %s/%3.1fs = %s %s + %s/Min (%s)",
      currency, double2str(Rate.Basic, 5, 3, DEB),
      double2str(Rate.Price, 5, 3, DEB),
      Rate.Duration,
      currency, double2str(Rate.Basic, 5, 3, DEB),
      double2str(60 * Rate.Price / Rate.Duration, 5, 3, DEB),
      explainRate(&Rate));
  else
    sprintf(message, "%s %s/%3.1fs = %s %s/Min (%s)",
      currency, double2str(Rate.Price, 5, 3, DEB),
      Rate.Duration,
      currency, double2str(60 * Rate.Price / Rate.Duration, 5, 3, DEB),
      explainRate(&Rate));
} /* showRates */


#define WIDTH   13

static void showLCR(int duration)
{
  auto int    tz, hour, provider, lastprovider = UNKNOWN, lasthour = UNKNOWN, *p;
  auto int    useds = 0, maxhour, leastprovider = UNKNOWN;
  auto int    ignoreprovider = UNKNOWN;
  auto int    z, z1, z2, z3;
  auto double price, bestRate, lastRate;
  auto double lastDTAG, DTAGRate;
  auto RATE   Rate;
#if 0
  auto int    probe[] = { REGIOCALL, GERMANCALL, C_NETZ, D1_NETZ, D2_NETZ, E_PLUS_NETZ, E2_NETZ, INTERNET, AUKUNFT_IN, AUSKUNFT_AUS, 0 };
#else
  auto int    probe[] = { REGIOCALL, GERMANCALL, D2_NETZ, INTERNET, AUKUNFT_IN, 0 };
#endif
  auto int    used[MAXPROVIDER];
  auto int    hours[MAXPROVIDER];
  auto char   lastmessage[BUFSIZ], message[BUFSIZ], *px;
  auto struct tm *tm;
  auto time_t werktag, wochenende;
  auto char  *msg;


  time(&werktag);

  print_msg(PRT_NORMAL, "Least-Cost-Routing-Table [Verbindungsdauer:%d Sekunden], Stand: %s (C)Copyright 1999 AKsoftware - Andreas Kool\n", duration, ctime(&werktag) + 4);

  tm = localtime(&werktag);

  if ((tm->tm_wday == 6) || (tm->tm_wday == 0)) {
    wochenende = werktag;
    werktag += (2 * 24 * 60 * 60);
  }
  else
    wochenende = werktag + ((6 - tm->tm_wday) * 24 * 60 * 60);

#if 0
retry:
#endif

  memset(used, 0, sizeof(used));
  memset(hours, 0, sizeof(hours));

  for (tz = 0; tz < 2; tz++) { /* Werktag .. Wochenende */

    switch (tz) {
      case 0 : print_msg(PRT_NORMAL, "\nWerktag:\n");
      	       tm = localtime(&werktag);
      	       break;

      case 1 : print_msg(PRT_NORMAL, "\nWochenende/Feiertag:\n");
      	       tm = localtime(&wochenende);
      	       break;
    } /* switch */

    p = probe;

    while (*p) {

      switch (*p) {
        case REGIOCALL    : print_msg(PRT_NORMAL, "  RegioCall (im Umkreis von 50 km):\n");        break;
        case GERMANCALL   : print_msg(PRT_NORMAL, "  GermanCall (Deutschlandweit):\n");       break;
        case D2_NETZ	  : print_msg(PRT_NORMAL, "  Mobilfunk (alle Handy's):\n"); break;
#if 0
	case C_NETZ       : print_msg(PRT_NORMAL, "  C Mobilfunk:\n");      break;
	case D1_NETZ      : print_msg(PRT_NORMAL, "  D1 Mobilfunk:\n");     break;
	case D2_NETZ      : print_msg(PRT_NORMAL, "  D2 Mobilfunk:\n");     break;
	case E_PLUS_NETZ  : print_msg(PRT_NORMAL, "  Eplus Mobilfunk:\n");  break;
	case E2_NETZ      : print_msg(PRT_NORMAL, "  E2 Mobilfunk:\n");     break;
#endif
	case INTERNET     : print_msg(PRT_NORMAL, "  Internet:\n");         break;
	case AUKUNFT_IN   : print_msg(PRT_NORMAL, "  Auskunft Inland:\n");  break;
	case AUSKUNFT_AUS : print_msg(PRT_NORMAL, "  Auskunft Ausland:\n"); break;
      } /* switch */

      lastprovider = UNKNOWN;
      lasthour = UNKNOWN;

      hour = 7;

      while (1) {

        tm->tm_hour = hour;
        tm->tm_sec = tm->tm_min = 0;

        z1 = *p;

        if (z1 == D2_NETZ)
          z2 = 30;
        else if (z1 == INTERNET)
          z2 = 159;
        else if (z1 == AUKUNFT_IN)
          z2 = 59;
        else
          z2 = z1;

        z3 = UNKNOWN;

        price = 999999.9;

        for (z = z1; z <= z2; z++) {
      	  memset(&Rate, 0, sizeof(Rate));
      	  Rate.zone = z;
      	  Rate.start = mktime(tm);
      	  Rate.now  = Rate.start + duration - ZAUNPFAHL;

          provider = getLeastCost(&Rate, ignoreprovider);

          /* print_msg(PRT_NORMAL, "z=%d, provider=%d, Rate.Price=%g\n", z, provider, Rate.Price); */

          if (provider == UNKNOWN) /* Bitte keine Lcken !! */
            break;

	  if (Rate.Price < price) {
            price = Rate.Price;
            z3 = z;
	  } /* if */
        } /* for */

        if (z3 != UNKNOWN) {
      	  memset(&Rate, 0, sizeof(Rate));
      	  Rate.zone = z3;
      	  Rate.start = mktime(tm);
      	  Rate.now  = Rate.start + duration - ZAUNPFAHL;

          provider = getLeastCost(&Rate, ignoreprovider);
        }
        else
          print_msg(PRT_NORMAL, "OOPS: cannot find cheapest provider?\n");


        memset(&Rate, 0, sizeof(Rate));
        Rate.prefix = provider;
      	Rate.zone = z3;
      	Rate.start = mktime(tm);
      	Rate.now  = Rate.start + duration - ZAUNPFAHL;

        (void)getRate(&Rate, &msg);
        bestRate = Rate.Charge;
	showRates(Rate, message);

        memset(&Rate, 0, sizeof(Rate));
        Rate.prefix = DTAG;
      	Rate.zone = z3;
      	Rate.start = mktime(tm);
      	Rate.now  = Rate.start + duration - ZAUNPFAHL;

        (void)getRate(&Rate, &msg);
        DTAGRate = Rate.Charge;

#if 0
        print_msg(PRT_NORMAL, "DEBUG::tz=%d, zone=%d, Hour=%02d, P=%d, %s  lasthour=%d, lastprovider=%d, now=%s", tz, *p, hour, provider, getProvider(provider), lasthour, lastprovider, ctime(&Rate.start));
#endif

        if (lastprovider == UNKNOWN) {
          lastRate = bestRate;
          lastDTAG = DTAGRate;
          lastprovider = provider;
	  strcpy(lastmessage, message);
        } /* if */

        if (lasthour == UNKNOWN)
          lasthour = hour;

        if (provider != lastprovider) {
          if (lastprovider == UNKNOWN)
            px = "";
          else
            px = getProvider(lastprovider);

          print_msg(PRT_NORMAL, "    %02d:00 .. %02d:59 %s%02d:%s%*s = %s %s (%s)   [DTAG: %s %s]\n",
            lasthour, hour - 1, vbn, lastprovider, px,
            max(WIDTH, strlen(px)) - strlen(px), "", currency,
            double2str(lastRate, 5, 3, DEB),
            lastmessage, currency, double2str(lastDTAG, 5, 3, DEB), lastDTAG / lastRate * 100.0);

          used[lastprovider] = 1;

          if (lasthour >= hour)
            hours[lastprovider] += ((24 - lasthour) + hour);
          else
            hours[lastprovider] += hour - lasthour;

          lastprovider = provider;
          lasthour = hour;
          lastRate = bestRate;
          lastDTAG = DTAGRate;
	  strcpy(lastmessage, message);
        } /* if */

	hour++;
      	if (hour == 24)
          hour = 0;
      	else if (hour == 7)
          break;
      } /* for */

      if (lastprovider == UNKNOWN)
        px = "";
      else
      px = getProvider(lastprovider);

      if ((lasthour == 7) && (hour == 7))
        print_msg(PRT_NORMAL, "    immer          %s%02d:%s%*s = %s %s (%s)   [DTAG: %s %s]\n",
          vbn, lastprovider, px, max(WIDTH, strlen(px)) - strlen(px), "",
          currency, double2str(lastRate, 5, 3, DEB), lastmessage, currency, double2str(lastDTAG, 5, 3, DEB));
      else
        print_msg(PRT_NORMAL, "    %02d:00 .. %02d:59 %s%02d:%s%*s = %s %s (%s)   [DTAG: %s %s]\n",
          lasthour, hour - 1, vbn, lastprovider, px, max(WIDTH, strlen(px)) - strlen(px), "",
            currency, double2str(lastRate, 5, 3, DEB), lastmessage, currency, double2str(lastDTAG, 5, 3, DEB));

      used[lastprovider] = 1;

      if (lasthour >= hour)
        hours[lastprovider] += ((24 - lasthour) + hour);
      else
        hours[lastprovider] += hour - lasthour;

      p++;
    } /* while */
  } /* for */

  print_msg(PRT_NORMAL, "\nProvider(s) used:\n");

  maxhour = 9999999;
  useds = 0;

  for (provider = 0; provider < MAXPROVIDER; provider++)
    if (used[provider]) {
      print_msg(PRT_NORMAL, "%s%02d:%s%*s(%d hours)\n", vbn, provider, getProvider(provider), max(WIDTH, strlen(getProvider(provider))) - strlen(getProvider(provider)), "", hours[provider]);
      useds++;

      if (hours[provider] < maxhour) {
        maxhour = hours[provider];
        leastprovider = provider;
      } /* if */
    } /* if */

#if 0
  if (useds == 6) {
    if (ignoreprovider != leastprovider) {

      print_msg(PRT_NORMAL, "OOOPS: More than 5 providers used. Retry with %s%02d:%s ignored\n",
        vbn, leastprovider, getProvider(leastprovider));

      ignoreprovider = leastprovider;
      goto retry;
    } /* if */
  } /* if */
#endif
} /* showLCR */


static void showWorld(int duration)
{
  auto     FILE *f = fopen("/usr/lib/isdn/countries.dat", "r");
  auto     char  s[BUFSIZ], areacode[BUFSIZ];
#if 0
  auto	   int	 provider;
#endif
  register int   n;
  register char *p;
  auto	   RATE  Rate;
  auto	   char *msg;


  if (f != (FILE *)NULL) {
    while (fgets(s, BUFSIZ, f)) {

      if ((p = strchr(s, '\n')))
        *p = 0;

      /* Fixme */
#define abroad(x,y) 1
      if (abroad(s, areacode)) {

        memset(&Rate, 0, sizeof(Rate));

  	time(&Rate.start);
      	Rate.now    = Rate.start + duration - ZAUNPFAHL;

        n = 0;

      	for (Rate.prefix = 0; Rate.prefix < MAXPROVIDER; Rate.prefix++) {
      	  Rate.zone = getZone(Rate.prefix, myarea, areacode);

          if (Rate.zone != UNKNOWN) {

  	    time(&Rate.start);
      	    Rate.now    = Rate.start + duration - ZAUNPFAHL;


            if (getRate(&Rate, &msg) != UNKNOWN) {
      	      sort[n].prefix = Rate.prefix;
      	      sort[n].rate = Rate.Charge;
              sort[n].msg = strdup(msg);
	      sort[n].explain = strdup(printrate(Rate));
      	      n++;
            } /* if */
#if 0
            if (provider = getLeastCost(&Rate, UNKNOWN)) {
              print_msg(PRT_NORMAL, "%-50s  %s %s%d:%s (%s)\n",
                s, areacode, vbn, provider, getProvider(provider), explainRate(&Rate));
              break;
            } /* if */
#endif
          } /* if */
      	} /* for */

        if (n > 0) {
          qsort((void *)sort, n, sizeof(SORT), compare);

          print_msg(PRT_NORMAL, "%-46s  %-9s %s%d:%s (%s)\n",
            s, areacode, vbn, sort[0].prefix,
            getProvider(sort[0].prefix), sort[0].explain);
        } /* if */

      }
      else
        print_msg(PRT_NORMAL, "%-50s  UNKNOWN\n", s);
    } /* while */

    fclose(f);
  } /* if */
} /* showWorld */


int main(int argc, char *argv[], char *envp[])
{
	int c, n = 0;
	int Cnt = 0;
	section *conf_dat = NULL;
	char *myname = basename(argv[0]);
	FILE *fp;
	COUNTRY *Country;
  	RATE Rate;
	char *msg;
	char *country;


	static char usage[]   = "%s: usage: %s [ -%s ]\n";
	static char options[] = "ADdn:a:t:f:c:wslimqgV1M:";


	set_print_fct_for_tools(print_in_modules);

	alias[0] = '\0';
	number[0] = '\0';
	sprintf(conffile,"%s%c%s",confdir(),C_SLASH,CONFFILE);
	strcpy(callerfile,USERCONFFILE);

	while ((c = getopt(argc, argv, options)) != EOF)
		switch (c) {
			case 'A' : add++;
			           break;

			case 'D' : del++;
			           match_flags &= ~F_NO_HOLE_WORD;
			           break;

 			case 'V' : print_version(myname);
			           exit(0);

			case 'd' : and++;
			           break;

			case 'm' : msn++;
			           break;

			case 'i' : match_flags |= F_IGNORE_CASE;
			           break;

			case 'n' : strcpy(number, optarg);
			           break;

			case 'a' : strcpy(alias, optarg);
			           break;

			case 't' : sprintf(si,"%ld",strtol(optarg, NIL, 0));
			           break;

			case 's' : short_out++;
			           break;

			case 'l' : long_out++;
			           break;

			case 'w' : match_flags &= ~F_NO_HOLE_WORD;
			           break;

			case 'M' : isdnmon++;
			           oneentry++;
			           and++;
			           quiet++;
			           strcpy(areacode, optarg);
			           strcpy(number, optarg);
			           break;

			case '1' : oneentry++;
			           break;

			case 'q' : quiet++;
			           break;

			case 'f' : strcpy(callerfile, optarg);
			           break;

			case 'c' : strcpy(areacode, optarg);
			           break;

			case 'g' : sprintf(callerfile,"%s%c%s",confdir(),C_SLASH,CALLERIDFILE);
			           break;

			case '?' : print_msg(PRT_ERR, usage, myname, myname, options);
			           return(1);
    }

	if (isdnmon && (add || del || short_out || long_out))
	{
		print_msg(PRT_ERR,"Error: Can not do isdnmon output and delete or add or short or long!\n",conffile);
		exit(7);
	}

	if (add || del)
	{
		if ((conf_dat = read_file(NULL, conffile, C_NOT_UNIQUE)) == NULL)
		{
			print_msg(PRT_ERR,"Error while reading file `%s': no rights on file, syntax error\nor emtpy (delete it first)!\n",conffile);
			exit(2);
		}

		if (Set_Codes(conf_dat) != 0)
		{
			print_msg(PRT_ERR,"Error: Variables `%s' and `%s' are not set!\n",CONF_ENT_AREA,CONF_ENT_COUNTRY);
			exit(5);
		}

		free_section(conf_dat);
		conf_dat = NULL;

		if (access(expand_file(callerfile),W_OK))
		{
			if (errno != ENOENT)
			{
				print_msg(PRT_ERR,"Error: Can not open file `%s' (%s)!\n",expand_file(callerfile),strerror(errno));
				exit(6);
			}
			else
			{
				if (add)
				{
					if ((fp = fopen(expand_file(callerfile),"w")) == NULL)
					{
						print_msg(PRT_ERR,"Error: Can not open file `%s' (%s)!\n",expand_file(callerfile),strerror(errno));
						exit(6);
					}
					else
						print_msg(PRT_ERR,"Create file `%s'!\n",expand_file(callerfile),strerror(errno));

					fclose(fp);
				}
				else
				{
					print_msg(PRT_ERR,"File `%s' doesn't exist!\n",expand_file(callerfile));
					exit(6);
				}
			}
		}
		else
		{
			if ((conf_dat = read_file(NULL, expand_file(callerfile), C_NOT_UNIQUE)) == NULL)
			{
				print_msg(PRT_ERR,"Error while reading file `%s': no rights on file, syntax error\nor emtpy (delete it first)!\n",expand_file(callerfile));
				exit(2);
			}
		}
	}
	else
	{
		if (read_isdnconf(&conf_dat) == NULL)
			exit(2);
	}

	if (number[0] != '\0')
	{
		strcpy(number, expand_number(number));
		if (isdnmon)
			print_msg(PRT_NORMAL,"%s\t",number);
	}

	if (areacode[0] != '\0')
	{
		char *ptr;
		int len, i, zone = UNKNOWN, zone2 = UNKNOWN, duration = TESTDURATION;


 		initHoliday(holifile, NULL);
 		initRate("/etc/isdn/rate.conf", "/usr/lib/isdn/rate-de.dat", "/usr/lib/isdn/zone-de-%s.gdbm", NULL);
 		/* initRate(NULL, "/usr/lib/isdn/rate-de.dat", "/usr/lib/isdn/countries-de.dat", NULL, NULL); */
		currency = strdup("DM");
		vbn = strdup("010");

		if (1 /* FIXME: (*areacode == '.') || (ptr = get_areacode(areacode,&len,quiet?C_NO_ERROR|C_NO_WARN:0)) != NULL */ )
		{
			if (!isdnmon)
			{
                                register char *p;


                                if ((p = strchr(areacode, ','))) {
                                  duration = atoi(p + 1);
                                  *p = 0;
                                } /* if */

                                if (*areacode == '.') {
                                  if (strlen(areacode) > 1) {
                                    zone = atoi(areacode + 1);
                                    ptr = "";
				  }
                                  else {
                                    showLCR(duration);
               			    exit(0);
                                  } /* else */
                                }
                                else if (*areacode == '*') {
                                  showWorld(duration);
                                  exit(0);
                                }
				else {
                                  if (isalpha(*areacode)) {
				    if (getCountry(areacode, &Country))
				      strcpy(areacode, Country->Code[0]);
                                  } /* if */

    				  zone = area_diff(NULL, areacode);

      				  switch (zone) {
      				    case AREA_ERROR    :
				    case AREA_UNKNOWN  : zone = UNKNOWN;    break;
				    case AREA_LOCAL    : zone = CITYCALL;   break;
				    case AREA_R50      : zone = REGIOCALL;  break;
				    case AREA_FAR      : zone = GERMANCALL; break;
				    case AREA_ABROAD   : zone = UNKNOWN;    break;
      				  } /* switch */
                                } /* else */

                                memset(&Rate, 0, sizeof(Rate));

  				time(&Rate.start);
      				Rate.now    = Rate.start + duration - ZAUNPFAHL;

                                if (zone != UNKNOWN)
                                  zone2 = zone;
#if 0
                                else
                                  zone2 = getZone(DTAG, myarea, areacode);
#endif

                                if (zone2 == UNKNOWN) {
				  if (getCountrycode(areacode, &country))
                                    print_msg(PRT_NORMAL, "Ein %d Sekunden langes Gespraech nach %s (%s) kostet am %s",
                                      duration, country, areacode, ctime(&Rate.start));
                                  else
                                    print_msg(PRT_NORMAL, "Ein %d Sekunden langes Gespraech in Zone Welt (%s) kostet am %s",
                                      duration, areacode, ctime(&Rate.start));
                                }
                                else {
                                  auto int   ll;
                                  auto char *p;

				  if ((p = get_areacode(areacode, &ll, C_NO_WARN | C_NO_EXPAND | C_NO_ERROR)) != 0)
                                    print_msg(PRT_NORMAL, "Ein %d Sekunden langes Gespraech nach %s (Zone %d) kostet am %s",
                                      duration, p, zone2, ctime(&Rate.start));
                                  else
                                    print_msg(PRT_NORMAL, "Ein %d Sekunden langes Gespraech in Zone %d kostet am %s",
                                      duration, zone2, ctime(&Rate.start));
                                } /* else */

                                for (Rate.prefix = 0; Rate.prefix < MAXPROVIDER; Rate.prefix++) {
                                  if (zone != UNKNOWN)
                                    Rate.zone = zone;
                                  else
				    Rate.zone = getZone(Rate.prefix, myarea, areacode);

                                  if (Rate.zone != UNKNOWN) {

  				    time(&Rate.start);
      				    Rate.now    = Rate.start + duration - ZAUNPFAHL;

                                    if (getRate(&Rate, &msg) != UNKNOWN) {
      				      sort[n].prefix = Rate.prefix;
      				      sort[n].rate = Rate.Charge;
                                      sort[n].msg = strdup(msg);
				      sort[n].explain = strdup(printrate(Rate));
      				      n++;
                                    } /* if */
                                  } /* if */
                                } /* for */

                                qsort((void *)sort, n, sizeof(SORT), compare);

                                for (i = 0; i < n; i++)
                                  print_msg(PRT_NORMAL, "%s%02d %s %8.3f (%s)\n", vbn, sort[i].prefix, currency, sort[i].rate, sort[i].explain);
#if 0
                                print_msg(PRT_NORMAL, "getLeastCost=%d\n", getLeastCost(&Rate, UNKNOWN));
#endif
				exit(0);
			}

			print_msg(PRT_NORMAL,"%s\t%d\t",ptr,len);
		}
		else
		{
			if (!isdnmon)
				exit(3);

			print_msg(PRT_NORMAL,"\t0\t");
		}
	}

	if (optind < argc && !add)
	{
		if (!alias[0] && optind + 1 == argc)
			strcpy(alias, argv[optind]);
		else
		{
			print_msg(PRT_ERR,"Error: Can not set two strings for alias!\n");
			exit(3);
		}
	}

	if (!add)
	{
		if (!alias[0] && !number[0] && !si[0])
		{
			print_msg(PRT_ERR, usage, argv[0], argv[0], options);
			exit(4);
		}
	}
	else
	{
		if (alias[0] || number[0] || si[0])
		{
			print_msg(PRT_ERR, usage, argv[0], argv[0], options);
			exit(5);
		}
	}

	if (add && del)
	{
		print_msg(PRT_ERR,"Error: Can not do add and delete together!\n");
		exit(1);
	}

	if (short_out && long_out)
	{
		print_msg(PRT_ERR,"Error: Can not do long and short output together!\n");
		exit(1);
	}

	if (!add && !del && Replace_Variables(conf_dat))
		exit(8);

	if (add)
		Cnt = add_data(&conf_dat);
	else
		Cnt = look_data(&conf_dat);

	if ((add || del) && Cnt > 0)
	{
		if (conf_dat == NULL)
		{
			if (unlink(expand_file(callerfile)))
				print_msg(PRT_ERR,"Error: Can not delete file `%s' (%s)!\n",expand_file(callerfile),strerror(errno));
			else
				print_msg(PRT_ERR,"Warning: File `%s' is empty. It is deleted now!\n", expand_file(callerfile));
		}
		else
		if (write_file(conf_dat,expand_file(callerfile),myname,VERSION) == NULL)
			exit(5);
	}

	if (del && !Cnt)
		print_msg(PRT_ERR, "Warning: No entry deleted!\n");

	free_section(conf_dat);

	return 0;
}

