/* $Id: isdnrep.c,v 1.27 1997/05/25 19:41:06 luethje Exp $
 *
 * ISDN accounting for isdn4linux. (Report-module)
 *
 * Copyright 1995, 1997 by Andreas Kool (akool@Kool.f.EUnet.de)
 *                     and Stefan Luethje (luethje@sl-gw.lake.de)
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
 * $Log: isdnrep.c,v $
 * Revision 1.27  1997/05/25 19:41:06  luethje
 * isdnlog:  close all files and open again after kill -HUP
 * isdnrep:  support vbox version 2.0
 * isdnconf: changes by Roderich Schupp <roderich@syntec.m.EUnet.de>
 * conffile: ignore spaces at the end of a line
 *
 * Revision 1.26  1997/05/19 22:58:18  luethje
 * - bugfix: it is possible to install isdnlog now
 * - improved performance for read files for vbox files and mgetty files.
 * - it is possible to decide via config if you want to use avon or
 *   areacode.
 *
 * Revision 1.25  1997/05/17 14:58:28  luethje
 * bug fix in HTML-Link
 *
 * Revision 1.24  1997/05/17 01:08:21  luethje
 * some bugfixes
 *
 * Revision 1.23  1997/05/15 23:24:54  luethje
 * added new links on HTML
 *
 * Revision 1.22  1997/05/15 22:21:35  luethje
 * New feature: isdnrep can transmit via HTML fax files and vbox files.
 *
 * Revision 1.21  1997/05/11 22:59:19  luethje
 * new version
 *
 * Revision 1.20  1997/05/10 12:57:00  luethje
 * some changes
 *
 * Revision 1.19  1997/05/10 01:21:06  luethje
 * some primitive changes
 *
 * Revision 1.18  1997/05/09 23:30:55  luethje
 * isdnlog: new switch -O
 * isdnrep: new format %S
 * bugfix in handle_runfiles()
 *
 * Revision 1.17  1997/05/06 22:13:31  luethje
 * bugfixes in HTML-Code of the isdnrep
 *
 * Revision 1.16  1997/05/05 21:04:21  luethje
 * README completed
 * some changes for the isdnmon
 *
 * Revision 1.15  1997/05/04 22:23:15  luethje
 * README completed
 * new features of the format string
 *
 * Revision 1.14  1997/05/04 20:19:54  luethje
 * README completed
 * isdnrep finished
 * interval-bug fixed
 *
 * Revision 1.13  1997/04/20 23:44:49  luethje
 * some primitve changes
 *
 * Revision 1.12  1997/04/20 22:52:25  luethje
 * isdnrep has new features:
 *   -variable format string
 *   -can create html output (option -w1 or ln -s isdnrep isdnrep.cgi)
 *    idea and design by Dirk Staneker (dirk.staneker@student.uni-tuebingen.de)
 * bugfix of processor.c from akool
 *
 * Revision 1.11  1997/04/17 23:29:45  luethje
 * new structure of isdnrep completed.
 *
 * Revision 1.9  1997/04/08 21:56:53  luethje
 * Create the file isdn.conf
 * some bug fixes for pid and lock file
 * make the prefix of the code in `isdn.conf' variable
 *
 * Revision 1.8  1997/04/06 21:03:37  luethje
 * switch -f is working again
 * currency_factor is float again ;-)
 * renamed files isdnrep.c to rep_main.c and function.c isdnrep.c
 *
 * Revision 1.3  1997/04/03 22:30:00  luethje
 * improved performance
 *
 * Revision 1.2  1997/03/31 22:43:11  luethje
 * Improved performance of the isdnrep, made some changes of README
 *
 * Revision 1.1  1997/03/24 22:52:09  luethje
 * isdnrep completed.
 *
 * Revision 1.4  1997/03/23 23:11:59  luethje
 * improved performance
 *
 * Revision 1.3  1997/03/23 20:25:23  luethje
 * tmporary (running) result of the new structure of isdnrep
 *
 * Revision 1.2  1997/03/20 00:19:13  luethje
 * inserted the line #include <errno.h> in avmb1/avmcapictrl.c and imon/imon.c,
 * some bugfixes, new structure in isdnlog/isdnrep/isdnrep.c.
 *
 * Revision 1.1  1997/03/16 20:59:05  luethje
 * Added the source code isdnlog. isdnlog is not working yet.
 * A workaround for that problem:
 * copy lib/policy.h into the root directory of isdn4k-utils.
 *
 * Revision 2.6.19  1997/01/04  13:00:35  akool
 * Korrektur bzgl. ISDN_CH
 * Danke an Markus Maeder (mmaeder@cyberlink.ch)
 *
 * Revision 2.6.15  1997/01/02  11:32:35  akool
 * -V Option added
 *
 * Revision 2.6.11  1996/12/30  10:17:35  akool
 * Dutch accounting (Ad Aerts <ad@aasup.nl>)
 *
 * Revision 2.50  1996/08/25  11:48:35  akool
 * Auswertung bei non-AOCE-Anschluessen verbessert
 *
 * Revision 2.13  1996/03/16  16:11:35  akool
 * Andruck der "ibytes" implementiert
 *
 * Revision 2.10  1996/01/16  01:04:35  sl
 * Ausgabe von bestimmten MSN's ergaenzt, Nur ankommende dor ausgehende
 * Gespraeche anzeigen, Ueberarbeitung der kompletten Sourcen.
 * Funktion fuer fremde Waehrungen ergaenzt
 *
 * Revision 2.03  1996/01/07  03:35:17  sl
 * Optionen -c und -6 entfernt, Tab-Bug behoben, Bug in automatischer
 * Gebuehrenberechnung beim alten Logformat behoben,
 * Zeitangabe beim Summary,
 *
 * Revision 2.02  1996/01/07  03:35:17  sl
 * Zeitraumausgabe angepasst, loeschen von isdn.log-Eintraegen
 *
 * Revision 2.01  1996/01/06  18:15:20  sl
 * Zeitraumausgabe, nur Calls-Anzeige, optionales Logfile
 *
 * Revision 2.00  1996/01/01  17:50:19  akool
 * Auswertung der neuen Eintraege (I/O, Cause) implementiert
 * Vollstaendiges Redesign
 *
 * Revision 1.25  1995/11/19  09:56:19  akool
 * Neue Option "-c" aktiviert die eigene Berechnung der Einheiten bei
 *   fehlender Gebuehreninfo
 *
 * Revision 1.24  1995/11/12  11:09:19  akool
 * Formatfelder vergroessert
 * Fremdverursachte Einheiten werden ge-()
 * Alle Floating-Point-Zahlen werden deutsch (mit Komma) ausgegeben
 *
 * Revision 1.23  1995/11/06  18:04:19  akool
 * Denkfehler in der "-6" Berechnung :-(
 *
 * Revision 1.22  1995/10/22  15:31:19  akool
 * Aufschluesselung bach Tarifzonen implementiert
 *
 * Revision 1.21  1995/10/17  19:53:19  akool
 * Auf echte Charging-Info umgestellt
 *
 * Revision 1.20  1995/10/15  17:58:19  akool
 * Vergleich errechnete zu uebermittelten Einheiten implementiert
 *
 * Revision 1.16  1995/10/12  13:50:19  akool
 * Neue Option "MYMSNS=x" in der "isdnlog.conf" zum definieren der Anzahl
 * eigener MSN's
 *
 * Revision 1.15  1995/10/09  18:42:18  akool
 * Offensichtlich sinnlose Daten i.d. "isdn.log" werden nun ignoriert
 *
 * Revision 1.14  1995/10/08  10:25:16  akool
 * Ueberschreitung verschiedener Zeittakte bei einer Verbindung implementiert.
 *
 * Revision 1.6  1995/09/30  19:55:17  akool
 * First public release
 *
 * Revision 1.1  1995/09/23  16:44:19  akool
 * Initial revision
 *
 */


#define _REP_FUNC_C_

#include <dirent.h>
#include <search.h>
#include <linux/limits.h>

#include "isdnrep.h"
#include "../../vbox/src/libvbox.h"

/*****************************************************************************/

#define END_TIME    1

#define SET_TIME    1
#define GET_TIME    2
#define GET_DATE    4
#define GET_DATE2   8
#define GET_YEAR   16

/*****************************************************************************/

#define GET_OUT     0
#define GET_IN      1

#define GET_BYTES   0
#define GET_BPS     2

/*****************************************************************************/

#define C_BEGIN_FMT '%'

#define FMT_FMT 1
#define FMT_STR 2

/*****************************************************************************/

#define C_VBOX  'v'
#define C_FAX   'f'

#define F_VBOX  1
#define F_FAX   2

/*****************************************************************************/

#define F_BEGIN 1
#define F_END   2

/*****************************************************************************/

#define DEF_FMT "  %X %D %15.15H %T %-15.15F %7u %U %I %O"
#define WWW_FMT "%X %D %17.17H %T %-17.17F %-20.20l SI: %S %9u %U %I %O"

/*****************************************************************************/

#define F_1ST_LINE      1
#define F_BODY_HEADER   2
#define F_BODY_HEADERL  4
#define F_BODY_LINE     8
#define F_BODY_BOTTOM1 16
#define F_BODY_BOTTOM2 32
#define F_COUNT_ONLY   64

/*****************************************************************************/

#define H_BG_COLOR     "#FFFFFF"
#define H_TABLE_COLOR1 "#CCCCFF"
#define H_TABLE_COLOR2 "#FFCCCC"
#define H_TABLE_COLOR3 "#CCFFCC"

#define H_1ST_LINE     "<CENTER><FONT size=+1><B>%s</B></FONT><P>\n"
#define H_BODY_LINE    "<TR>%s</TR>\n"
#define H_BODY_HEADER1 "<TABLE width=%g%% bgcolor=%s border=0 cellspacing=0 cellpadding=0>\n"
#define H_BODY_HEADER2 "<COL width=%d*>\n"
//#define H_BODY_HEADER2 "<COLGROUP span=1 width=%d*>\n"
#define H_BODY_HEADER3 "<TH colspan=%d>%s</TH>\n"
#define H_BODY_BOTTOM1 "<TD align=left colspan=%d>%s</TD>%s\n"
#define H_BODY_BOTTOM2 "</TABLE><P>\n"

#define H_LINE "<TR><TD colspan=%d><HR size=%d noshade width=100%%></TD></TR>\n"

#define H_LEFT         "<TD align=left><TT>%s</TT></TD>"
#define H_CENTER       "<TD align=center><TT>%s</TT></TD>"
#define H_RIGHT        "<TD align=right><TT>%s</TT></TD>"
#define H_LINK         "<A HREF=\"%s?-M+%c%d%s\">%s</A>"
#define H_LINK_DAY     "<A HREF=\"%s?%s\">%s</A> "

#define H_EMPTY        "&nbsp;"

/*****************************************************************************/

typedef struct {
	int   type;
	char *string;
	char  s_type;
	char *range;
} prt_fmt;

typedef struct {
	int    version;
	int    compression;
	int    used;
	time_t time;
	char   type;
	char  *name;
} file_list;

/*****************************************************************************/

static time_t get_month(char *String, int TimeStatus);
static time_t get_time(char *String, int TimeStatus);
static int show_msn(one_call *cur_call);
static int add_sum_calls(sum_calls *s1, sum_calls *s2);
static int print_sum_calls(sum_calls *s, int computed);
static int add_one_call(sum_calls *s1, one_call *s2, double units);
static int clear_sum(sum_calls *s1);
static char *print_currency(double money, int computed);
static void strich(int type);
static int n_match(char *Pattern, char* Number, char* version);
static int set_caller_infos(one_call *cur_call, char *string, time_t from);
static int set_alias(one_call *cur_call, int *nx, char *myname);
static int print_header(int lday);
static int print_entries(one_call *cur_call, double unit, int *nx, char *myname);
static int print_bottom(double unit, char *start, char *stop);
static char *get_time_value(time_t t, int *day, int flag);
static char **string_to_array(char *string);
static prt_fmt** get_format(const char *format);
static char *set_byte_string(int flag, double Bytes);
static int print_line(int status, one_call *cur_call, int computed, char *overlap);
static int print_line2(int status, const char *, ...);
static int print_line3(const char *fmt, ...);
static int append_string(char **string, prt_fmt *fmt_ptr, char* value);
static char *html_conv(char *string);
static int get_format_size(void);
static int set_col_size(void);
static char *overlap_string(char *s1, char *s2);
static char* fill_spaces(char *string);
static void free_format(prt_fmt** ptr);
static int html_bottom(char *progname, char *start, char *stop);
static int html_header(void);
static char *print_diff_date(char *start, char *stop);
static int get_file_list(void);
static int set_dir_entries(char *directory, int (*set_fct)(const char *, const char *));
static int set_vbox_entry(const char *path, const char *file);
static int set_mgetty_entry(const char *path, const char *file);
static int set_element_list(file_list* elem);
static int Compare_files(const void *e1, const void *e2);
static file_list *get_file_to_call(time_t filetime, char type);
static char *get_links(time_t filetime, char type);
static char *append_fax(char **string, char *file, char type, int version);
static int time_in_interval(time_t t1, time_t t2, char type);
static char *nam2html(char *file);
static char *get_a_day(time_t t, int d_diff, int m_diff, int flag);
static char *get_time_string(time_t begin, time_t end, int d_diff, int m_diff);
static char *create_vbox_file(char *file, int *compression);

/*****************************************************************************/

static int      invertnumbers = 0;
static int      unknowns = 0;
static UNKNOWN  unknown[MAXUNKNOWN];
static int      zones[MAXZONES];
static int      zones_usage[MAXZONES];
static double  	zones_dm[MAXZONES];
static double  	zones_dur[MAXZONES];
static char**   ShowMSN = NULL;
static int*     colsize = NULL;
static double   h_percent = 100.0;
static char*    h_table_color = H_TABLE_COLOR1;
static file_list **file_root = NULL;
static int      file_root_size = 0;
static int      file_root_member = 0;
static char    *_myname;
static time_t   _begintime;
static int      read_path = 0;

/*****************************************************************************/

static char msg1[]    = "%s: Can't open %s (%s)\n";
static char wrongdate2[]   = "wrong date for delete: %s\n";
static char nomemory[]   = "Out of memory!\n";

static char htmlconv[][2][10] = {
	{">", "&gt;"},
	{"<", "&lt;"},
	{"" , ""},
};

/*****************************************************************************/

static int Tarif96 = 0;
static int Tarif962 = 0;

static sum_calls day_sum;
static sum_calls day_com_sum;
static sum_calls all_sum;
static sum_calls all_com_sum;
  
/*****************************************************************************/

void set_print_fct_for_isdnrep(int (*new_print_msg)(int Level, const char *, ...))
{
  print_msg = new_print_msg;
}

/*****************************************************************************/

int send_html_request(char *myname, char *option)
{
	char file[PATH_MAX];
	char commandline[PATH_MAX];
	char *filetype = NULL;
	char *command  = NULL;
	char *vboxfile = NULL;
	int  compression;


	if (*option == C_VBOX)
	{
		sprintf(file,"%s%c%s",vboxpath,C_SLASH,option+2);

		if (option[1] == '1')
		{
			if (vboxcommand1)
				command = vboxcommand1;
			else
				filetype = "application/x-zyxel4";
		}
		else
		if (option[1] == '2')
		{
			if (vboxcommand2)
				command = vboxcommand2;
			else
			{
				vboxfile = strcpy(file,create_vbox_file(file,&compression));

				switch(compression)
				{
					case 2 : filetype = "application/x-zyxel2";
					         break;
					case 3 : filetype = "application/x-zyxel3";
					         break;
					case 4 : filetype = "application/x-zyxel4";
					         break;
					case 6 : filetype = "application/x-ulaw";
					         break;
					default: print_msg(PRT_NORMAL, "Content-Type: text/plain\n\n");
				           print_msg(PRT_NORMAL, "%s: unsupported compression type of vbox file :`%d'\n",myname,compression);
				           return -1;
					         break;
				}
			}
		}
		else
		{
			print_msg(PRT_NORMAL, "Content-Type: text/plain\n\n");
			print_msg(PRT_NORMAL, "%s: unsupported version of vbox `%c'\n",myname,option[0]);
			return -1;
		}
	}
	else
	if (*option == C_FAX)
	{
		sprintf(file,"%s%c%s",mgettypath,C_SLASH,option+2);

		if (option[1] == '3')
		{
			if (mgettycommand)
				command = mgettycommand;
			else
				filetype = "application/x-faxg3";
		}
		else
		{
			print_msg(PRT_NORMAL, "Content-Type: text/plain\n\n");
			print_msg(PRT_NORMAL, "%s:unsupported version of fax `%c%c'\n",myname,option[0]);
			return -1;
		}
	}
	else
	{
		print_msg(PRT_NORMAL, "Content-Type: text/plain\n\n");
		print_msg(PRT_NORMAL, "%s:invalid option string `%c%c'\n",myname,option[0],option[1]);
		return -1;
	}

	if (command == NULL)
	{
		char precmd[SHORT_STRING_SIZE];

		sprintf(precmd, "echo \"Content-Type: %s\"",filetype);
		system(precmd);
		sprintf(precmd, "echo");
		system(precmd);
	//	print_msg(PRT_NORMAL, "Content-Type: %s\n\n",filetype);
	}

	sprintf(commandline,"%s %s",command?command:"cat",file);
	system(commandline);

	if (vboxfile)
	{
		if (unlink(vboxfile))
		{
			print_msg(PRT_ERR,"Can not delete file `%s': %s!\n",file, strerror(errno));
			return -1;
		}
	}

	return 0;
}

/*****************************************************************************/

int read_logfile(char *myname)
{
  auto	   double     einheit = 0.23;
  auto	   int	      nx[2];
  auto	   time_t     now, from = 0;
  auto 	   struct tm *tm;
  auto	   int	      lday = -1;
  auto	   char	      start[20] = "", stop[20];
  auto	   char*      tmpfile = NULL;
  auto     FILE      *fi, *ftmp = NULL;
  auto     char       string[BUFSIZ], s[BUFSIZ];
  one_call            cur_call;


	_myname = myname;
	_begintime = begintime;

	if (html & H_PRINT_HEADER)
		html_header();

	if (lineformat == NULL)
	{
		if (html)
			lineformat = WWW_FMT;
		else
			lineformat = DEF_FMT;
	}
			
	if (get_format(lineformat) == NULL)
		return -1;

	/* following two lines must be after get_format()! */
	if (html)
		get_file_list();

  clear_sum(&day_sum);
  clear_sum(&day_com_sum);
  clear_sum(&all_sum);
  clear_sum(&all_com_sum);

	if (knowns == 0 || strcmp(known[knowns-1]->who,S_UNKNOWN))
	{
		if ((known = (KNOWN**) realloc(known, sizeof(KNOWN *) * (knowns+1))) == NULL)
    {
      print_msg(PRT_ERR, nomemory);
      return -1;
    }

		if ((known[knowns] = (KNOWN*) calloc(1,sizeof(KNOWN))) == NULL)
    {
      print_msg(PRT_ERR, nomemory);
      return -1;
    }

    known[knowns]->who = S_UNKNOWN;
    known[knowns++]->num = "0000";
	}

  if (delentries)
  {
    if(begintime)
    {
      print_msg(PRT_ERR, wrongdate2, timestring);
      return -1;
    }
    else
    {
      if ((tmpfile = tmpnam(NULL)) == NULL)
      	return -1;

  		if ((ftmp = fopen(tmpfile, "w")) == (FILE *)NULL)
      {
        print_msg(PRT_ERR, msg1, tmpfile, strerror(errno));
        return -1;
      }
    }
  }

  if (!timearea) { /* from darf nicht gesetzt werden, wenn alle Telefonate angezeigt werden sollen */
    time(&now);
    	/* aktuelle Zeit wird gesetzt */
    tm = localtime(&now);
    	/* Zeit von 1970 nach Struktur */
    tm->tm_sec = tm->tm_min = tm->tm_hour = 0;
    from = mktime(tm);
    	/* Struktur nach Zeit von 1970 */
    	/* from hat den aktuellen Tag 0.00 Uhr */
  } /* if */

  if ((fi = fopen(logfile, "r")) == (FILE *)NULL){
  	print_msg(PRT_ERR,"Can not open file `%s': %s!\n",logfile,strerror(errno));
  	return -1;
  }

  memset(zones, 0, sizeof(zones));

  while (fgets(s, BUFSIZ, fi)) {
    strcpy(string,s);

		if (*s == '#')
			continue;

		if (set_caller_infos(&cur_call,s,from) != 0)
			continue;

		if (!begintime)
			begintime = cur_call.t;

		if (timearea)
		{
			if (delentries)
				if (cur_call.t > endtime)
					fputs(string,ftmp);
				else
					continue;
		}

		if (!verbose && cur_call.duration == 0)
			continue;

		if (phonenumberonly)
			if (!show_msn(&cur_call))
				continue;

		if (incomingonly && cur_call.dir == DIALOUT)
			continue;

		if (outgoingonly && cur_call.dir == DIALIN)
			continue;

		get_time_value(cur_call.t,&day,SET_TIME);

		/* Andreas, warum ist diese Abfrage hier drin, warum soll das nicht moeglich
		   sein? */
		if (cur_call.duration > 172800.0) /* laenger als 2 Tage? Das KANN nicht sein! */
			cur_call.duration = 0;

		set_alias(&cur_call,nx,myname);

		if (day != lday) {
			if (header)
				print_header(lday);

			lday = day;

			if (!*start)
			{
				sprintf(start, "%s %s", get_time_value(0,NULL,GET_DATE),
				                        get_time_value(0,NULL,GET_YEAR));
				sprintf(stop, "%s %s", get_time_value(0,NULL,GET_DATE),
				                       get_time_value(0,NULL,GET_YEAR));
			}
			else
				sprintf(stop, "%s %s", get_time_value(0,NULL,GET_DATE),
				                       get_time_value(0,NULL,GET_YEAR));
		} /* if */

		if (!currency_factor)
#ifdef ISDN_NL
			einheit = 0.0011; /* cost of one second local tariff during office hours */
#elif defined(ISDN_CH)
			einheit = 0.01;
#else
			einheit = Tarif96 ? 0.12 : 0.23;
#endif
			else
				einheit = currency_factor;

		print_entries(&cur_call,einheit,nx,myname);

	} /* while */

	fclose(fi);

	if (delentries) /* Erzeugt neue verkuerzte Datei */
		lday = -1;

	if (lday != -1 && header)
		print_bottom(einheit, start, stop);

	if (delentries) /* Erzeugt neue verkuerzte Datei */
	{
		fclose(ftmp);

		if ((ftmp = fopen(tmpfile, "r")) == (FILE *)NULL)
		{
			print_msg(PRT_ERR, msg1, tmpfile, strerror(errno));
			return -1;
		}
		if ((fi = fopen(logfile, "w")) == (FILE *)NULL)
		{
			print_msg(PRT_ERR, msg1, logfile, strerror(errno));
			return -1;
		}

		while (fgets(s, BUFSIZ, ftmp))
			fputs(s,fi);

		fclose(fi);
		fclose(ftmp);

		unlink(tmpfile);
	}

	if (html & H_PRINT_HEADER)
		html_bottom(myname,start,stop);

	return 0;
} /* read_logfile */

/*****************************************************************************/

static int print_bottom(double unit, char *start, char *stop)
{
  auto     char       string[BUFSIZ];
  register int	      i, j, k;
  register char      *p = NULL;
  sum_calls           tmp_sum;


	if (timearea) {
		strich(1);
		print_sum_calls(&day_sum,0);

		if (day_com_sum.eh)
		{
			print_sum_calls(&day_com_sum,1);

			clear_sum(&tmp_sum);
			add_sum_calls(&tmp_sum,&day_sum);
			add_sum_calls(&tmp_sum,&day_com_sum);
			strich(1);
			print_sum_calls(&tmp_sum,0);
		}
		else
			print_msg(PRT_NORMAL,"\n");
	} /* if */

	add_sum_calls(&all_sum,&day_sum);
	add_sum_calls(&all_com_sum,&day_com_sum);

	strich(2);
	print_sum_calls(&all_sum,0);

	if (all_com_sum.eh)
	{
		print_sum_calls(&all_com_sum,1);

		clear_sum(&tmp_sum);
		add_sum_calls(&tmp_sum,&all_sum);
		add_sum_calls(&tmp_sum,&all_com_sum);
		strich(2);
		print_sum_calls(&tmp_sum,0);
	}
	else
		print_msg(PRT_NORMAL,"\n");

	print_line2(F_BODY_BOTTOM2,"");

	get_format("%-14.14s %4d call(s) %10.10s  %12s %-12s %-12s");

	for (j = 0; j < 2; j++)
	{
		if ((j == DIALOUT && !incomingonly) || (!outgoingonly && j == DIALIN))
		{
			sprintf(string,"\n%s Summary for %s",j==DIALOUT?"DIALOUT":"DIALIN",
			              print_diff_date(start,stop));

			h_percent = 80.0;
			h_table_color = H_TABLE_COLOR2;
			print_line2(F_BODY_HEADER,"");
			print_line2(F_BODY_HEADERL,"%s",string);
			strich(3);

			for (i = mymsns; i < knowns; i++) {
				if (known[i]->usage[j]) {
					print_line3(NULL,
					          /*!numbers?*/known[i]->who/*:known[i]->num*/,
					          known[i]->usage[j],
					          double2clock(known[i]->dur[j]),
					          j==DIALOUT?print_currency(known[i]->dm,0):
					                     fill_spaces(print_currency(known[i]->dm,0)),
					          set_byte_string(GET_IN|GET_BYTES,known[i]->ibytes[j]),
					          set_byte_string(GET_OUT|GET_BYTES,known[i]->obytes[j]));
				} /* if */
			} /* for */

			print_line2(F_BODY_BOTTOM2,"");
		}
	}

	h_percent = 60.0;
	h_table_color = H_TABLE_COLOR3;
	get_format("Zone %c : %-10.10s %4d call(s) %10.10s  %s");
	print_line2(F_BODY_HEADER,"");

	for (i = 1; i < MAXZONES; i++)
		if (zones[i]) {

			p = "";

			switch (i) {
				case 1 : p = "City";       break;
				case 2 : p = "Region 50";  break;
				case 3 : p = "Region 200"; break;
				case 4 : p = "Fernzone";   break;
			} /* switch */

			print_line3(NULL,
			          (char) i+48, p, zones_usage[i], double2clock(zones_dur[i]),
			          print_currency(zones_dm[i],0));
		} /* if */

	if (known[knowns-1]->eh)
	{
		print_line3(NULL,
		          'x', S_UNKNOWN,
		          known[knowns-1]->usage[DIALOUT], double2clock(known[knowns-1]->dur[DIALOUT]),
#ifdef ISDN_NL
		          print_currency(known[knowns-1]->eh * unit + known[knowns-1]->usage * 0.0825,0));
#else
		          print_currency(known[knowns-1]->eh * unit, 0));
#endif
	}

	print_line2(F_BODY_BOTTOM2,"");

	if (seeunknowns && unknowns) {
		print_msg(PRT_NORMAL,"\n\nUnknown caller(s)\n");
		strich(3);

		for (i = 0; i < unknowns; i++) {

			print_msg(PRT_NORMAL,"%s %-14s ", unknown[i].called ? "called by" : "  calling", unknown[i].num);

			for (k = 0; k < unknown[i].connects; k++) {
				strcpy(string, ctime(&unknown[i].connect[k]));

				if ((p = strchr(string, '\n')))
					*p = 0;

				*(string + 19) = 0;

				if (k && (k + 1) % 2) {
					print_msg(PRT_NORMAL,"\n\t\t\t ");
				} /* if */

				print_msg(PRT_NORMAL,"%s%s", k & 1 ? ", " : "", string + 4);
			} /* for */

			print_msg(PRT_NORMAL,"\n");
		} /* for */
	} /* if */

	return 0;
}

/*****************************************************************************/

static int print_line3(const char *fmt, ...)
{
	char *string = NULL;
	char  tmpstr[BUFSIZ*3];
	auto  va_list ap;
	prt_fmt** fmtstring;


	if (fmt == NULL)
		fmtstring = get_format(NULL);
	else
		fmtstring = get_format(fmt);

	if (fmtstring == NULL)
		return -1;

	va_start(ap, fmt);

	while(*fmtstring != NULL)
	{
		if ((*fmtstring)->type == FMT_FMT)
		{
			switch((*fmtstring)->s_type)
			{
				case 's' : append_string(&string,*fmtstring,va_arg(ap,char*));
				           break;
				case 'c' : sprintf(tmpstr,"%c",va_arg(ap,char));
				           append_string(&string,*fmtstring,tmpstr);
				           break;
				case 'd' : sprintf(tmpstr,"%d",va_arg(ap,int));
				           append_string(&string,*fmtstring,tmpstr);
				           break;
				case 'f' : sprintf(tmpstr,"%f",va_arg(ap,double));
				           append_string(&string,*fmtstring,tmpstr);
				           break;
				default  : print_msg(PRT_ERR, "Internal Error: unknown format `%c'!\n",(*fmtstring)->s_type);
				           break;
			}
		}
		else
		if ((*fmtstring)->type == FMT_STR)
		{
			append_string(&string,NULL,(*fmtstring)->string);
		}
		else
			print_msg(PRT_ERR, "Internal Error: unknown format type `%d'!\n",(*fmtstring)->type);

		fmtstring++;
	}

	va_end(ap);

	print_line2(F_BODY_LINE,"%s",string);
	free(string);
	return 0;
}

/*****************************************************************************/

static int print_line2(int status, const char *fmt, ...)
{
	char string[BUFSIZ*3];
	auto va_list ap;


	va_start(ap, fmt);
	vsnprintf(string, BUFSIZ*3, fmt, ap);
	va_end(ap);

	if (!html)
		status = 0;

	switch (status)
	{
		case F_COUNT_ONLY  : break;
		case F_1ST_LINE    : print_msg(PRT_NORMAL,H_1ST_LINE,string);
		                     break;
		case F_BODY_BOTTOM1:
		case F_BODY_LINE   : print_msg(PRT_NORMAL,H_BODY_LINE,string);
		                     break;
		case F_BODY_HEADER : print_msg(PRT_NORMAL,H_BODY_HEADER1,h_percent,h_table_color,get_format_size());
		                     set_col_size();
		                     break;
		case F_BODY_HEADERL: print_msg(PRT_NORMAL,H_BODY_HEADER3,get_format_size(),string);
		                     break;
		case F_BODY_BOTTOM2: print_msg(PRT_NORMAL,H_BODY_BOTTOM2,string);
		                     break;
		default            : print_msg(PRT_NORMAL,"%s\n",string);
		                     break;
	}

	return 0;
}

/*****************************************************************************/

static int print_line(int status, one_call *cur_call, int computed, char *overlap)
{
	char *string = NULL;
	char *Ptr;
	char  help[32];
	prt_fmt **fmtstring = get_format(NULL);
	int dir;
	int i = 0;
	int free_col;
	int last_free_col = -1;

	if (colsize == NULL || status == F_COUNT_ONLY)
	{
		free(colsize);

		if ((colsize = (int*) calloc(get_format_size()+1,sizeof(int))) == NULL)
		{
			print_msg(PRT_ERR, nomemory);
			return -1;
		}
	}

	while (*fmtstring != NULL)
	{
		free_col = 0;

		if ((*fmtstring)->type == FMT_FMT)
		{
			switch((*fmtstring)->s_type)
			{
				/* time: */
				case 'X': if (status == F_BODY_LINE)
				            colsize[i] = append_string(&string,*fmtstring, get_time_value(0,NULL,GET_TIME));
				          else
				          {
				          	free_col = 1;
				          	if (!html || status == F_COUNT_ONLY)
				            	colsize[i] = append_string(&string,*fmtstring, fill_spaces(get_time_value(0,NULL,GET_TIME)));
				          }
				          break;
				/* date (normal format with year): */
				case 'x': if (status == F_BODY_LINE)
				            colsize[i] = append_string(&string,*fmtstring, get_time_value(0,NULL,GET_DATE2));
				          else
				          {
				          	free_col = 1;
				          	if (!html || status == F_COUNT_ONLY)
				            	colsize[i] = append_string(&string,*fmtstring, fill_spaces(get_time_value(0,NULL,GET_DATE2)));
				          }
				          break;
				/* date (without year): */
				case 'y': if (status == F_BODY_LINE)
				            colsize[i] = append_string(&string,*fmtstring, get_time_value(0,NULL,GET_DATE));
				          else
				          {
				          	free_col = 1;
				          	if (!html || status == F_COUNT_ONLY)
				            	colsize[i] = append_string(&string,*fmtstring, fill_spaces(get_time_value(0,NULL,GET_DATE)));
				          }
				          break;
				/* year: */
				case 'Y': if (status == F_BODY_LINE)
				            colsize[i] = append_string(&string,*fmtstring, get_time_value(0,NULL,GET_YEAR));
				          else
				          {
				          	free_col = 1;
				          	if (!html || status == F_COUNT_ONLY)
				            	colsize[i] = append_string(&string,*fmtstring, fill_spaces(get_time_value(0,NULL,GET_YEAR)));
				          }
				          break;
				/* duration: */
				case 'D': if (status == F_BODY_LINE)
				            colsize[i] = append_string(&string,*fmtstring, double2clock(cur_call->duration));
				          else
				          {
				          	free_col = 1;
				          	if (!html || status == F_COUNT_ONLY)
				            	colsize[i] = append_string(&string,*fmtstring, fill_spaces(double2clock(cur_call->duration)));
				          }
				          break;
				/* Home (if possible the name): */
				/* Benoetigt Range! */
				case 'H': if (status == F_BODY_LINE)
				          {
				          	dir = cur_call->dir?CALLED:CALLING;
				            if (!numbers)
				          	{
				          		colsize[i] = append_string(&string,*fmtstring,
				           	             cur_call->who[dir][0]?cur_call->who[dir]:cur_call->num[dir]);
				          		break;
				          	}
				          }
				/* Home (number): */
				/* Benoetigt Range! */
				case 'h': if (status == F_BODY_LINE)
				            colsize[i] = append_string(&string,*fmtstring,
				                        cur_call->num[cur_call->dir?CALLED:CALLING]);
				          else
				          {
				          	free_col = 1;
				          	if (!html || status == F_COUNT_ONLY)
				          		colsize[i] = append_string(&string,*fmtstring, "");
				          }
				          break;
				/* The other (if possible the name): */
				/* Benoetigt Range! */
				case 'F': if (status == F_BODY_LINE)
				          {
				            dir = cur_call->dir?CALLING:CALLED;
				          	if (!numbers)
				          	{
				          		colsize[i] = append_string(&string,*fmtstring,
				           	             cur_call->who[dir][0]?cur_call->who[dir]:cur_call->num[dir]);
				          		break;
				          	}
				          }
				/* The other (number): */
				/* Benoetigt Range! */
				case 'f': if (status == F_BODY_LINE)
				            colsize[i] = append_string(&string,*fmtstring,
				                        cur_call->num[cur_call->dir?CALLING:CALLED]);
				          else
				          {
				          	free_col = 1;
				          	if (!html || status == F_COUNT_ONLY)
				          		colsize[i] = append_string(&string,*fmtstring, "");
				          }
				          break;
				/* The home location: */
				/* Benoetigt Range! */
				case 'L': if (status == F_BODY_LINE)
				          {
				            dir = cur_call->dir?CALLED:CALLING;
				          	if (cur_call->num[dir][0] != C_UNKNOWN &&
				          	    cur_call->num[dir][0] != '\0'      &&
				          	    (Ptr = get_areacode(cur_call->num[dir],NULL,C_NO_ERROR)) != NULL)
				          		colsize[i] = append_string(&string,*fmtstring, Ptr);
				          	else
				          		colsize[i] = append_string(&string,*fmtstring, "");
				          }
				          else
				          {
				          	free_col = 1;
				          	if (!html || status == F_COUNT_ONLY)
				          		colsize[i] = append_string(&string,*fmtstring, "");
				          }
				          break;
				/* The home location: */
				/* Benoetigt Range! */
				case 'l': if (status == F_BODY_LINE)
				          {
				            dir = cur_call->dir?CALLING:CALLED;
				          	if (cur_call->num[dir][0] != C_UNKNOWN &&
				          	    cur_call->num[dir][0] != '\0'      &&
				          	    (Ptr = get_areacode(cur_call->num[dir],NULL,C_NO_ERROR)) != NULL)
				          		colsize[i] = append_string(&string,*fmtstring, Ptr);
				          	else
				          		colsize[i] = append_string(&string,*fmtstring, "");
				          }
				          else
				          {
				          	free_col = 1;
				          	if (!html || status == F_COUNT_ONLY)
				          		colsize[i] = append_string(&string,*fmtstring, "");
				          }
				          break;
				/* The "To"-sign (from home to other) (-> or <-): */
				case 'T': if (status == F_BODY_LINE)
				            colsize[i] = append_string(&string,*fmtstring, cur_call->dir?"<-":"->");
				          else
				          {
				          	free_col = 1;
				          	if (!html || status == F_COUNT_ONLY)
				          		colsize[i] = append_string(&string,*fmtstring, "  ");
				          }
				          break;
				/* The "To"-sign (from other to home) (-> or <-): */
				case 't': if (status == F_BODY_LINE)
				            colsize[i] = append_string(&string,*fmtstring, cur_call->dir?"->":"<-");
				          else
				          {
				          	free_col = 1;
				          	if (!html || status == F_COUNT_ONLY)
				          		colsize[i] = append_string(&string,*fmtstring, "  ");
				          }
				          break;
				/* The units (if exists): */
				/* Benoetigt Range! */
				case 'u': if (cur_call->eh > 0)
				          {
				          	sprintf(help,"%d EH",cur_call->eh);
				          	colsize[i] = append_string(&string,*fmtstring, help);
				          }
				          else
				          	colsize[i] = append_string(&string,*fmtstring, "");
				          break;
				/* The money or/and a message: */
				case 'U': if (cur_call->duration || cur_call->eh > 0)
				          {
				          	if (cur_call->dir)
				          		colsize[i] = append_string(&string,NULL,"            ");
				          	else
				          		colsize[i] = append_string(&string,*fmtstring,print_currency(cur_call->dm,computed));
				          }
				          else
				          if (status == F_BODY_LINE && cur_call->cause != -1)
				          	colsize[i] = append_string(&string,*fmtstring,qmsg(TYPE_CAUSE, VERSION_EDSS1, cur_call->cause));
				          else
				          	colsize[i] = append_string(&string,NULL,"            ");
				          break;
				/* In-Bytes: */
				/* Benoetigt Range! */
				case 'I': colsize[i] = append_string(&string,*fmtstring,set_byte_string(GET_IN|GET_BYTES,(double)cur_call->ibytes));
				          break;
				/* Out-Bytes: */
				/* Benoetigt Range! */
				case 'O': colsize[i] = append_string(&string,*fmtstring,set_byte_string(GET_OUT|GET_BYTES,(double)cur_call->obytes));
				          break;
				/* In-Bytes per second: */
				/* Benoetigt Range! */
				case 'P': colsize[i] = append_string(&string,*fmtstring,set_byte_string(GET_IN|GET_BPS,cur_call->duration?cur_call->ibytes/(double)cur_call->duration:0.0));
				          break;
				/* Out-Bytes per second: */
				/* Benoetigt Range! */
				case 'p': colsize[i] = append_string(&string,*fmtstring,set_byte_string(GET_OUT|GET_BPS,cur_call->duration?cur_call->obytes/(double)cur_call->duration:0.0));
				          break;
				/* SI: */
				case 'S': if (status == F_BODY_LINE)
				          	colsize[i] = append_string(&string,*fmtstring,int2str(cur_call->si,2));
				          else
				          	colsize[i] = append_string(&string,*fmtstring,"  ");
				          break;
				/* Link for answering machine! */
				case 'C': if (html)
				          {
				          	if (status == F_BODY_LINE)
				          		colsize[i] = append_string(&string,*fmtstring,get_links(cur_call->t,C_VBOX));
				          	else
				          		colsize[i] = append_string(&string,*fmtstring,"     ");
				          }
				          else
				          	print_msg(PRT_ERR, "Unknown format %%C!\n");
				          break;
				/* Link for fax! */
				case 'G': if (html)
				          {
				          	if (status == F_BODY_LINE)
				          		colsize[i] = append_string(&string,*fmtstring,get_links(cur_call->t,C_FAX));
				          	else
				          		colsize[i] = append_string(&string,*fmtstring,"        ");
				          }
				          else
				          	print_msg(PRT_ERR, "Unknown format %%G!\n");
				          break;
				/* there are dummy entries */
				case 'c': 
				case 'd': 
				case 's': if (status != F_BODY_LINE)
				          	free_col = 1;
				          	
				          colsize[i] = append_string(&string,*fmtstring, " ");
				          break;
				default : print_msg(PRT_ERR, "Internal Error: unknown format `%c'!\n",(*fmtstring)->s_type);
				          break;
			}
		}
		else
		if ((*fmtstring)->type == FMT_STR)
		{
			if (!html || status == F_COUNT_ONLY || status == F_BODY_LINE || last_free_col != i-1)
				colsize[i] = append_string(&string,NULL,(*fmtstring)->string);
			else
				free_col = 1;
		}
		else
			print_msg(PRT_ERR, "Internal Error: unknown format type `%d'!\n",(*fmtstring)->type);

		if (html && status == F_BODY_BOTTOM1 && free_col && last_free_col == i-1)
			last_free_col = i;

		fmtstring++;
		i++;
	}

	if (last_free_col != -1)
	{
		char *help2 = NULL;


		if ((help2 = (char*) calloc(strlen(H_BODY_BOTTOM1)+(string?strlen(string):0)+strlen(overlap)+1,sizeof(char))) == NULL)
		{
			print_msg(PRT_ERR, nomemory);
			return -1;
		}

		sprintf(help2,H_BODY_BOTTOM1,last_free_col+1,overlap,string?string:"");

		free(string);
		string = help2;
		overlap = NULL;
	}

	colsize[i] = -1;

	if (status == F_COUNT_ONLY)
		return strlen(string);
	else
		print_line2(status,"%s",overlap_string(string,overlap));

	free(string);

	return 0;
}

/*****************************************************************************/

static int append_string(char **string, prt_fmt *fmt_ptr, char* value)
{
	char  tmpstr[BUFSIZ*3];
	char  tmpfmt2[20];
	char *tmpfmt;
	char *htmlfmt;
	int   condition = html && (*value == ' ' || fmt_ptr == NULL || (fmt_ptr->s_type!='C' && fmt_ptr->s_type!='G'));


	if (fmt_ptr != NULL)
		sprintf(tmpfmt2,"%%%ss",fmt_ptr->range);
	else
		strcpy(tmpfmt2,"%s");

	if (html)
	{
		switch (fmt_ptr==NULL?'\0':fmt_ptr->range[0])
		{
			case '-' : htmlfmt = H_LEFT;
			           break;
			case '\0': htmlfmt = H_CENTER;
			           break;
			default  : htmlfmt = H_RIGHT;
			           break;
		}
			
		if ((tmpfmt = (char*) alloca(sizeof(char)*(strlen(htmlfmt)+strlen(tmpfmt2)+1))) == NULL)
		{
			print_msg(PRT_ERR, nomemory);
			return -1;
		}

		sprintf(tmpfmt,htmlfmt,tmpfmt2);
	}
	else
		tmpfmt = tmpfmt2;

	sprintf(tmpstr,tmpfmt,condition?html_conv(value):value);
	
	if (*string == NULL)
		*string = (char*) calloc(strlen(tmpstr)+1,sizeof(char));
	else
		*string = (char*) realloc(*string,sizeof(char)*(strlen(*string)+strlen(tmpstr)+1));

	if (*string == NULL)
	{
		print_msg(PRT_ERR, nomemory);
		return -1;
	}

	strcat(*string,tmpstr);

	return strlen(tmpstr);
}

/*****************************************************************************/

static char *html_conv(char *string)
{
	static char  RetCode[BUFSIZ];
	char *ptr = RetCode;
	int   i;
	int empty = 1;

	if (string == NULL)
		return NULL;

	do
	{
		for(i = 0; htmlconv[i][0][0] != '\0'; i++)
		{
			if (htmlconv[i][0][0] == *string)
			{
				strcpy(ptr,htmlconv[i][1]);
				while (*ptr != '\0') ptr++;
				break;
			}
		}

		if (htmlconv[i][0][0] == '\0')
			*ptr++ = *string;

		if (*string != '\0' && !isspace(*string))
			empty = 0;
	}
	while(*string++ != '\0');

	if (empty)
		return H_EMPTY;

	return RetCode;
}

/*****************************************************************************/

static char *set_byte_string(int flag, double Bytes)
{
	static char string[4][20];
	static int num = 0;
	int  factor = 1;
	char prefix = ' ';


	num = (num+1)%4;

	if (!Bytes)
	{
		if (flag & GET_BPS)
			strcpy(string[num],"              ");
		else
			strcpy(string[num],"            ");
	}
	else
	{
		if (Bytes >= 9999999999.0)
		{
			factor = 1073741824;
			prefix = 'G';
		}
		else
		if (Bytes >= 9999999)
		{
			factor = 1048576;
			prefix = 'M';
		}
		else
		if (Bytes >= 9999)
		{
			factor = 1024;
			prefix = 'k';
		}

		sprintf(string[num],"%c=%s %cB%s",flag&GET_IN?'I':'O',double2str(Bytes/factor,7,2,0),prefix,flag&GET_BPS?"/s":"");
	}

	return string[num];
}

/*****************************************************************************/

static int set_col_size(void)
{
	one_call *tmp_call;
	int size = 0;
	int i = 0;

	if ((tmp_call = (one_call*) calloc(1,sizeof(one_call))) == NULL)
	{
		print_msg(PRT_ERR, nomemory);
		return -1;
	}

	print_line(F_COUNT_ONLY,tmp_call,0,NULL);

	while(colsize[i] != -1)
		if (html)
			print_msg(PRT_NORMAL,H_BODY_HEADER2,colsize[i++]);
		else
			size += colsize[i++];

	free(tmp_call);
	return size;
}

/*****************************************************************************/

static int get_format_size(void)
{
	int i = 0;
	prt_fmt** fmt = get_format(NULL);

	if (fmt == NULL)
		return 0;

	while(*fmt++ != NULL) i++;

	return i;
}

/*****************************************************************************/

static char *overlap_string(char *s1, char *s2)
{
	int i = 0;

	if (s1 == NULL || s2 == NULL)
		return s1;

	while(s1[i] != '\0' && s2[i] != '\0')
	{
		if (isspace(s1[i]))
			s1[i] = s2[i];

		i++;
	}

	return s1;
}

/*****************************************************************************/

static char* fill_spaces(char *string)
{
	static char RetCode[256];
	char *Ptr = RetCode;

	while (*string != '\0')
	{
		*Ptr++ = ' ';
		string++;
	}

	*Ptr = '\0';

	return RetCode;
}

/*****************************************************************************/

static void free_format(prt_fmt** ptr)
{
	prt_fmt** ptr2 = ptr;


	if (ptr == NULL)
		return;

	while(*ptr != NULL)
	{
		free((*ptr)->string);
		free((*ptr)->range);
		free((*ptr++));
	}

	free(ptr2);
	free(colsize);
	colsize = NULL;
}

/*****************************************************************************/

static prt_fmt** get_format(const char *format)
{
	static prt_fmt **RetCode = NULL;
	prt_fmt *fmt = NULL;
	char    *Ptr = NULL;
	char    *string = NULL;
	char    *start = NULL;
	char    *End = NULL;
	char     Range[20] = "";
	int      num;
	char     Type;


	if (format == NULL)
		return RetCode;

	free_format(RetCode);
	RetCode = NULL;

	if ((End    = (char*) alloca(sizeof(char)*(strlen(format)+1))) == NULL ||
	    (string = (char*) alloca(sizeof(char)*(strlen(format)+1))) == NULL   )
	{
		print_msg(PRT_ERR, nomemory);
		return NULL;
	}

	Ptr = start = strcpy(string,format);

	do
	{
		if (*Ptr == C_BEGIN_FMT)
		{
			if (Ptr[1] != C_BEGIN_FMT)
			{
				if (Ptr != start)
				{
					if ((fmt = (prt_fmt*) calloc(1,sizeof(prt_fmt))) == NULL)
					{
						print_msg(PRT_ERR, nomemory);
						delete_element(&RetCode,0);
						return NULL;
					}

					*Ptr = '\0';
					fmt->string= strdup(start);
					fmt->type  = FMT_STR;
					append_element(&RetCode,fmt);
				}

				*Range = *End = '\0';
				if ((num = sscanf(Ptr+1,"%[^a-zA-Z]%c%[^\n]",Range,&Type,End)) > 1 ||
				    (num = sscanf(Ptr+1,"%c%[^\n]",&Type,End))                 > 0   )
				{
					if (!isalpha(Type))
    				print_msg(PRT_ERR, "Warning: Invalid token in format type `%c'!\n",Type);

					if ((fmt = (prt_fmt*) calloc(1,sizeof(prt_fmt))) == NULL)
					{
						print_msg(PRT_ERR, nomemory);
						delete_element(&RetCode,0);
						return NULL;
					}

					switch (Type)
					{
						case 'C': read_path |= F_VBOX;
						          break;
						case 'G': read_path |= F_FAX;
						          break;
						default : break;
					}

					fmt->s_type= Type;
					fmt->range = strdup(Range);
					fmt->type  = FMT_FMT;

					append_element(&RetCode,fmt);
					*Range = '\0';

					if (*End != '\0')
						Ptr = start = strcpy(string,End);
					else
						Ptr = start = "";
				}
				else
				{
    			print_msg(PRT_ERR, "Error: Invalid token in format string `%s'!\n",format);
    			return NULL;
    		}
			}
			else
			{
				memmove(Ptr,Ptr+1,strlen(Ptr));
				Ptr++;
			}
		}
		else
			Ptr++;
	}
	while(*Ptr != '\0');

	if (Ptr != start)
	{
		if ((fmt = (prt_fmt*) calloc(1,sizeof(prt_fmt))) == NULL)
		{
			print_msg(PRT_ERR, nomemory);
			delete_element(&RetCode,0);
			return NULL;
		}

		fmt->string= strdup(start);
		fmt->type  = FMT_STR;
		append_element(&RetCode,fmt);
	}

	return RetCode;
}

/*****************************************************************************/

static int print_entries(one_call *cur_call, double unit, int *nx, char *myname)
{
	auto time_t  t1, t2;
	auto double  takt;
  auto int     computed = 0, go, zone = 1, zeit = -1;


	if (cur_call->pay && !cur_call->eh)
	/* Falls Betrag vorhanden und Einheiten nicht, Einheiten berechnen */
		cur_call->eh = cur_call->pay/unit;
	else if (cur_call->currency_factor                          &&
		       cur_call->currency_factor != unit && cur_call->eh>0  )
		/* Falls Einheiten sich auf anderen Einheiten-Faktor beziehen, Einheiten korrigieren */
		cur_call->eh /= unit / cur_call->currency_factor;


#if 0
	if (compute && !currency_factor &&
	    !cur_call->dir && ((cur_call->eh == -1) ||
	    (!cur_call->eh && cur_call->duration && cur_call->cause == -1))) { /* DIALOUT, keine AOCE Meldung */
#else
	go = 0;

	if ((cur_call->eh == -1) && !cur_call->dir) { /* Rauswahl, Gebuehr unbekannt */
		if (nx[1] == -1) {       		      	/* Gegner unbekannt! */
			if (compute) {
				zone = compute;                       /* in "-c x" Zone vermuten */
				go = 1;
			}
			else {                                  /* mit 0 DM berechnen */
				cur_call->eh = 0;
				go = 0;
			} /* else */
		}
		else {
			go = 1;
			if (!(zone = known[nx[1]]->zone))
				go = cur_call->eh = 0;
		} /* else */
	} /* if */

	if (go) {
#endif
		t1 = cur_call->t;
		t2 = cur_call->t + cur_call->duration;

		cur_call->eh = takt = 0;
		computed = 1;

		while (t1 < t2) {
			if (Tarif96)
				takt = cheap96(t1, zone, &zeit);
			else
				takt = cheap(t1, zone);

			if (!takt) {
				print_msg(PRT_ERR, "%s: OOPS! Abbruch: Zeittakt==0 ???\n", myname);
				break;
			} /* if */

			cur_call->eh++;
			t1 += takt;

		} /* while */
	} /* if */

	if (cur_call->duration || (cur_call->eh > 0))
	{
		add_one_call(computed?&day_com_sum:&day_sum,cur_call,unit);

		if (cur_call->dir) {
			if (nx[CALLING] == -1) {
				known[knowns-1]->usage[DIALIN]++;
				known[knowns-1]->ibytes[DIALIN] += cur_call->ibytes;
				known[knowns-1]->obytes[DIALIN] += cur_call->obytes;

				if (cur_call->duration > 0)
					known[knowns-1]->dur[DIALIN] += cur_call->duration;
			} /* if */
			else
			{
				known[nx[CALLING]]->usage[cur_call->dir]++;
				known[nx[CALLING]]->dur[cur_call->dir] += cur_call->duration;
				known[nx[CALLING]]->ibytes[DIALIN] += cur_call->ibytes;
				known[nx[CALLING]]->obytes[DIALIN] += cur_call->obytes;
			}
		}
		else {

			if (nx[CALLED] == -1) {
				known[knowns-1]->eh += cur_call->eh;
				known[knowns-1]->dm += cur_call->dm;
				known[knowns-1]->ibytes[DIALOUT] += cur_call->ibytes;
				known[knowns-1]->obytes[DIALOUT] += cur_call->obytes;
				known[knowns-1]->usage[DIALOUT]++;

				if (cur_call->duration > 0)
					known[knowns-1]->dur[DIALOUT] += cur_call->duration;
			}
			else {
				known[nx[CALLED]]->eh += cur_call->eh;
				known[nx[CALLED]]->dm += cur_call->dm;
				known[nx[CALLED]]->usage[cur_call->dir]++;
				known[nx[CALLED]]->dur[cur_call->dir] += cur_call->duration;
				known[nx[CALLED]]->ibytes[DIALOUT] += cur_call->ibytes;
				known[nx[CALLED]]->obytes[DIALOUT] += cur_call->obytes;

				zones[known[nx[CALLED]]->zone] += cur_call->eh;
				zones_dm[known[nx[CALLED]]->zone] += cur_call->dm;

				if (cur_call->duration > 0)
					zones_dur[known[nx[CALLED]]->zone] += cur_call->duration;

				zones_usage[known[nx[CALLED]]->zone]++;
			} /* if */
		} /* else */
	}
	else {
		if (cur_call->cause != -1)
			day_sum.err++;
	} /* else */

	print_line(F_BODY_LINE,cur_call,computed,NULL);
	return 0;
}

/*****************************************************************************/

static int print_header(int lday) 
{
  sum_calls tmp_sum;
	time_t now;


	if (lday == -1) {
		time(&now);
		print_line2(F_1ST_LINE,"I S D N  Connection Report  -  %s", ctime(&now));
	}
	else
	{
		strich(1);
		print_sum_calls(&day_sum,0);

		if (day_com_sum.eh)
		{
			print_sum_calls(&day_com_sum,1);

			clear_sum(&tmp_sum);
			add_sum_calls(&tmp_sum,&day_sum);
			add_sum_calls(&tmp_sum,&day_com_sum);
			strich(1);
			print_sum_calls(&tmp_sum,0);
		}
		else
			print_msg(PRT_NORMAL,"\n\n");

		add_sum_calls(&all_sum,&day_sum);
		clear_sum(&day_sum);

		add_sum_calls(&all_com_sum,&day_com_sum);
		clear_sum(&day_com_sum);

		print_line2(F_BODY_BOTTOM2,"");
	} /* if */

	h_percent = 100.0;
	h_table_color = H_TABLE_COLOR1;
	print_line2(F_BODY_HEADER,"");
	print_line2(F_BODY_HEADERL,"%s %s", get_time_value(0,NULL,GET_DATE),
	                                       get_time_value(0,NULL,GET_YEAR));

	return 0;
}

/*****************************************************************************/

static char *get_time_value(time_t t, int *day, int flag)
{
	static char time_string[4][18] = {"","",""};
	static int  oldday = -1;
	static int  oldyear = -1;
	struct tm *tm;
	

	if (flag & SET_TIME)
	{
  	tm = localtime(&t);

		/*sprintf(time_string[0],"%d:%d:%d",tm->tm_hour,tm->tm_min,tm->tm_sec);*/
		strftime(time_string[0],17,"%X",tm);

		if (oldday != tm->tm_yday || oldyear != tm->tm_year)
		{
	 		*day = tm->tm_mday;

			Tarif96 = tm->tm_year > 95;

		/* Ab Juli '96 gibt's noch mal eine Senkung in den Zonen 3 und 4
		   genaue Preise sind jedoch noch nicht bekannt. FIX-ME */

			Tarif962 = (Tarif96 && (tm->tm_mon > 5)) || (tm->tm_year > 96);

			strftime(time_string[1],17,"%a %b %d",tm);
			strftime(time_string[2],17,"%x",tm);
			oldday  = tm->tm_yday;

			if (oldyear != tm->tm_year)
			{
				sprintf(time_string[3],"%d",1900+tm->tm_year);
				oldyear = tm->tm_year;
			}
		}
	}

	if (flag & GET_TIME)
		return time_string[0];
	else
	if (flag & GET_DATE)
		return time_string[1];
	else
	if (flag & GET_DATE2)
		return time_string[2];
	else
	if (flag & GET_YEAR)
		return time_string[3];

	return NULL;

}

/*****************************************************************************/

static int set_alias(one_call *cur_call, int *nx, char *myname)
{
	auto int n, cc, i, j;
	auto int hit;


	for (n = CALLING; n <= CALLED; n++) {
		nx[n] = -1;
		hit = 0;

		if (!*(cur_call->num[n])) {
//			if (!numbers)
			{
				cur_call->num[n][0] = C_UNKNOWN;
				cur_call->num[n][1] = '\0';
			}
			/* Wenn keine Nummer, dann Name "UNKNOWN" */

			hit++;
		} else {
			/* In der folg. Schleife werden Nummern durch Namen ersetzt */
			cc = 0;

			for (j = 0; ((j < 2) && !cc); j++) {
				for (i = 0; i < knowns; i++) {

					if (cur_call->version[0] != '\0')
					{
						if (!strcmp(cur_call->version,LOG_VERSION_2) ||
						    !strcmp(cur_call->version,LOG_VERSION_3)   )
							cc = ((known[i]->si == cur_call->si) || j) &&
							     !n_match(known[i]->num, cur_call->num[n], cur_call->version);
					}
					else
					{
						if (*cur_call->num[n] != '0') {

							/* Alte Syntax der "isdn.log" : Ohne vorlaufene "0" */
							cc = ((known[i]->si == cur_call->si) || j) &&
							     !match(known[i]->num+1, cur_call->num[n], 0);

							if (!cc) {
								/* Ganz alte Syntax der "isdn.log" : Ohne Vorwahl */
								cc = ((known[i]->si == cur_call->si) || j) &&
							     !n_match(known[i]->num, cur_call->num[n], LOG_VERSION_1);
							} /* if */
						}
					}

					if (cc) {

						strncpy(cur_call->who[n], known[i]->who,NUMSIZE);

 						nx[n] = i;
						hit++;
						break;
					} /* if */
				} /* for */
			} /* for */

			/* In der naechsten Schleife werden die unbekannten Nummern
			   registriert */
			if (!hit) {
				for (i = 0; i < unknowns; i++)
					if (!strcmp(unknown[i].num, cur_call->num[n])) {
					hit++;
					break;
				} /* if */

				strcpy(unknown[i].num, cur_call->num[n]);
				unknown[i].called = !n;
				unknown[i].connect[unknown[i].connects] = cur_call->t;

				/* ACHTUNG: MAXCONNECTS und MAXUNKNOWN sollten unbedingt groesser sein ! */
				if (unknown[i].connects + 1 < MAXCONNECTS)
					unknown[i].connects++;
				else
					print_msg(PRT_ERR, "%s: WARNING: Too many unknown connection's from %s\n", myname, unknown[i].num);

				if (!hit) {
					if (unknowns < MAXUNKNOWN)
						unknowns++;
					else
						print_msg(PRT_ERR, "%s: WARNING: Too many unknown number's\n", myname);
				} /* if */
			} /* if */
		} /* else */
	} /* for */

	return 0;
}

/*****************************************************************************/

static int set_caller_infos(one_call *cur_call, char *string, time_t from)
{
  register int    i = 0;
  auto     char **array;


	array = string_to_array(string);

	if (array[5] == NULL)
		return -1;

	cur_call->t = atol(array[5]);

	if (timearea && (cur_call->t < begintime || cur_call->t > endtime))
		return -1;
	else
		if (cur_call->t < from)
			return -1;

	cur_call->eh = 0;
	cur_call->dir = -1;
	cur_call->cause = -1;
	cur_call->ibytes = cur_call->obytes = 0L;
	cur_call->dm    = 0.0;
	cur_call->version[0] = '\0';
	cur_call->pay = 0.0;
	cur_call->si = cur_call->si1 = 0;
	cur_call->dir = DIALOUT;
	cur_call->who[0][0] = '\0';
	cur_call->who[1][0] = '\0';

	for (i = 1; array[i] != NULL; i++)
	{
		switch (i)
		{
			case  0 : cur_call->t = atom(array[i]);
			          break;
			case  1 : strcpy(cur_call->num[0], Kill_Blanks(array[i]));
			          break;
			case  2 : strcpy(cur_call->num[1], Kill_Blanks(array[i]));
			          break;
			case  3 : cur_call->duration = (double)atoi(array[i]);
			          break;
			case  4 : cur_call->duration = atoi(array[i]) / 100.0;
			          break;
			case  5 : /*cur_call->t = atol(array[i]);*/
			          break;
			case  6 : cur_call->eh = atoi(array[i]);
			          break;
			case  7 : cur_call->dir = (*array[i] == 'I') ? DIALIN : DIALOUT;
			          break;
			case  8 : cur_call->cause = atoi(array[i]);
			          break;
			case  9 : cur_call->ibytes = atol(array[i]);
			          break;
			case  10: cur_call->obytes = atol(array[i]);
			          break;
			case  11: strcpy(cur_call->version,array[i]);
			          break;
			case  12: cur_call->si = atoi(array[i]);
			          break;
			case  13: cur_call->si1 = atoi(array[i]);
			          break;
			case  14: cur_call->currency_factor = atof(array[i]);
			          break;
			case  15: strncpy(cur_call->currency, array[i], 3);
			          break;
			case  16: cur_call->pay = atof(array[i]);
			          break;
			default : print_msg(PRT_ERR,"Unknown element found `%s'!\n",array[i]); 
			          break;
		}
	}

	if (i < 3)
		return -1;

	return 0;
}

/*****************************************************************************/

static char **string_to_array(char *string)
{
	static char *array[30];
	auto   char *ptr;
	auto   int   i = 0;

	if ((ptr = strrchr(string,C_DELIM)) != NULL)
		*ptr = '\0';

	array[5] = NULL;
	array[0] = string;

	while((string = strchr(string,C_DELIM)) != NULL)
	{
		*string++ = '\0';
		array[++i] = string;
	}

	array[++i] = NULL;
	return array;
}

/*****************************************************************************/

int get_term (char *String, time_t *Begin, time_t *End,int delentries)
{
  time_t now;
  time_t  Date[2];
  int Cnt;
  char DateStr[2][256] = {"",""};


  time(&now);

  if (String[0] == '-')
    strcpy(DateStr[1],String+1);
  else
  if (String[strlen(String)-1] == '-')
  {
    strcpy(DateStr[0],String);
    DateStr[0][strlen(String)-1] ='\0';
  }
  else
  if (strchr(String,'-'))
  {
    if (sscanf(String,"%[/.0-9]-%[/.0-9]",DateStr[0],DateStr[1]) != 2)
      return 0;
  }
  else
  {
    strcpy(DateStr[0],String);
    strcpy(DateStr[1],String);
  }

  for (Cnt = 0; Cnt < 2; Cnt++)
  {
    if (strchr(DateStr[Cnt],'/'))
      Date[Cnt] = get_month(DateStr[Cnt],delentries?0:Cnt);
    else
      Date[Cnt] = get_time(DateStr[Cnt],delentries?0:Cnt);
  }

  *Begin = DateStr[0][0] == '\0' ? 0 : Date[0];
  *End = DateStr[1][0] == '\0' ? now : Date[1];

  return 1;
}

/*****************************************************************************/

static time_t get_month(char *String, int TimeStatus)
{
  time_t now;
  int Cnt = 0;
  struct tm *TimeStruct;
  int Args[3];
  int ArgCnt;


  time(&now);
  TimeStruct = localtime(&now);
  TimeStruct->tm_sec = 0;
  TimeStruct->tm_min = 0;
  TimeStruct->tm_hour= 0;
  TimeStruct->tm_mday= 1;
  TimeStruct->tm_isdst= -1;

  ArgCnt = sscanf(String,"%d/%d/%d",&(Args[0]),&(Args[1]),&(Args[2]));

  switch (ArgCnt)
  {
    case 3:
      TimeStruct->tm_mday = Args[0];
      Cnt++;
    case 2:
      if (Args[Cnt+1] > 99)
        TimeStruct->tm_year = ((Args[Cnt+1] / 100) - 19) * 100 + (Args[Cnt+1]%100);
      else
        TimeStruct->tm_year = Args[Cnt+1];
    case 1:
      TimeStruct->tm_mon = Args[Cnt];
      break;
    default:
      return 0;
  }

  if (TimeStatus == END_TIME)
  {
    if (ArgCnt == 3)	/* Wenn Tag angegeben ist */
    {
      TimeStruct->tm_mday++;
      TimeStruct->tm_mon--;
    }
  }
  else
    TimeStruct->tm_mon--;

  return mktime(TimeStruct);
}

/*****************************************************************************/

static time_t get_time(char *String, int TimeStatus)
{
  time_t now;
  int Len = 0;
  int Year;
  char *Ptr;
  struct tm *TimeStruct;


  time(&now);
  TimeStruct = localtime(&now);
  TimeStruct->tm_sec = 0;
  TimeStruct->tm_min = 0;
  TimeStruct->tm_hour= 0;
  TimeStruct->tm_isdst= -1;

  switch (strlen(String))
  {
    case 0:
            return 0;
    case 1:
    case 2:
            TimeStruct->tm_mday = atoi(String);
            break;
    default:
            Len = strlen(String);

      if ((Ptr = strchr(String,'.')) != NULL)
      {
        TimeStruct->tm_sec = atoi(Ptr+1);
        Len -= strlen(Ptr);
      }

      if (Len % 2)
        return 0;

      if (sscanf(String,"%2d%2d%2d%2d%d",
          &(TimeStruct->tm_mon),
          &(TimeStruct->tm_mday),
          &(TimeStruct->tm_hour),
          &(TimeStruct->tm_min),
          &Year		) 		> 4)
        if (Year > 99)
          TimeStruct->tm_year = ((Year / 100) - 19) * 100 + (Year%100);
        else
          TimeStruct->tm_year = Year;

      TimeStruct->tm_mon--;
      break;
  }

  if (TimeStatus == END_TIME)
    if (TimeStruct->tm_sec == 0 &&
        TimeStruct->tm_min == 0 &&
        TimeStruct->tm_hour== 0   )
      TimeStruct->tm_mday++;
    else
    if (TimeStruct->tm_sec == 0 &&
        TimeStruct->tm_min == 0   )
      TimeStruct->tm_hour++;
    else
    if (TimeStruct->tm_sec == 0   )
      TimeStruct->tm_min++;
    else
      TimeStruct->tm_sec++;

  return mktime(TimeStruct);
}

/*****************************************************************************/

int set_msnlist(char *String)
{
  int Cnt;
  int Value = 1;
  char *Ptr = String;


  if (ShowMSN)
    return 0;

  while((Ptr = strchr(Ptr,',')) != NULL)
  {
    Ptr++;
    Value++;
  }

  ShowMSN = (char**) calloc(Value+1,sizeof(char*));
  for(Cnt = 0; Cnt < Value; Cnt++)
    ShowMSN[Cnt] = (char*) calloc(NUMSIZE,sizeof(char));

  if (!ShowMSN)
  {
    print_msg(PRT_ERR, nomemory);
    exit(1);
  }

  if (*String == 'n')
  {
    ++String;
    invertnumbers++;
  }

  Cnt = 0;

  while(String)
  {

    if (*String == 'm')
    {
      Value = atoi(++String);

      if (Value > 0 && Value <= mymsns)
      {
        strcpy(ShowMSN[Cnt],known[Value-1]->num);
        Cnt++;
      }
      else
      {
        print_msg(PRT_ERR,"Invalid MSN %d!\n",Cnt);
      }
    }
    else
      sscanf(String,"%[^,],",ShowMSN[Cnt++]);

    String = strchr(String,',');
    if (String) String++;
  }

  return 0;
}

/*****************************************************************************/

static int show_msn(one_call *cur_call)
{
  int Cnt;

  for(Cnt = 0; ShowMSN[Cnt] != NULL; Cnt++)
    if (!num_match(ShowMSN[Cnt], cur_call->num[0]) ||
        !num_match(ShowMSN[Cnt]   , cur_call->num[1]))
      return !invertnumbers;
    else
    if (!strcmp(ShowMSN[Cnt],"0"))
      return !invertnumbers;

  return invertnumbers;
}

/*****************************************************************************/

static char *print_currency(double money, int computed)
{
  static char RetCode[256];


	sprintf(RetCode,"%s %s%c", double2str(money,8,2,0),currency,computed?'*':' ');
  return RetCode;
}

/*****************************************************************************/

static int print_sum_calls(sum_calls *s, int computed)
{
  static char String[256];
  one_call *tmp_call;
  int RetCode;

	if ((tmp_call = (one_call*) calloc(1,sizeof(one_call))) == NULL)
	{
		print_msg(PRT_ERR, nomemory);
		return -1;
	}

	tmp_call->eh = s->eh;
	tmp_call->dm = s->dm;
	tmp_call->obytes = s->obytes;
	tmp_call->ibytes = s->ibytes;

  sprintf(String,"%3d IN=%s, %3d OUT=%s, %3d failed",
    s->in,
    double2clock(s->din),
    s->out,
    double2clock(s->dout),
    s->err);

  RetCode = print_line(F_BODY_BOTTOM1,tmp_call,computed,String);
	free(tmp_call);
  return RetCode;
}

/*****************************************************************************/

static int add_one_call(sum_calls *s1, one_call *s2, double units)
{
  if (s2->dir == DIALIN)
  {
    s1->in++;
    s1->din += s2->duration > 0?s2->duration:0;
  }
  else
  {
    s2->dm = s2->eh * units;

    s1->out++;
    s1->dout   += s2->duration > 0?s2->duration:0;
    s1->eh     += s2->eh;
    s1->dm     += s2->dm;
  }

  s1->ibytes += s2->ibytes;
  s1->obytes += s2->obytes;
#ifdef ISDN_NL
  s1->dm       += 0.0825; /* add call setup charge */
  s2->dm       += 0.0825;
#endif
  return 0;
}

/*****************************************************************************/

static int clear_sum(sum_calls *s1)
{
  s1->in     = 0;
  s1->out    = 0;
  s1->eh     = 0;
  s1->err    = 0;
  s1->din    = 0;
  s1->dout   = 0;
  s1->dm     = 0;
  s1->ibytes = 0L;
  s1->obytes = 0L;

  return 0;
}

/*****************************************************************************/

static int add_sum_calls(sum_calls *s1, sum_calls *s2)
{
  s1->in     += s2->in;
  s1->out    += s2->out;
  s1->eh     += s2->eh;
  s1->err    += s2->err;
  s1->din    += s2->din;
  s1->dout   += s2->dout;
  s1->dm     += s2->dm;
  s1->ibytes += s2->ibytes;
  s1->obytes += s2->obytes;
  return 0;
}

/*****************************************************************************/

static void strich(int type)
{
	if (html)
	{
		switch (type) {
			case 3 : 
			case 1 : print_msg(PRT_NORMAL,H_LINE,get_format_size(),1);
			         break;
			case 2 : print_msg(PRT_NORMAL,H_LINE,get_format_size(),3);
			         break;
			default: print_msg(PRT_ERR,"Internal error: Invalid line flag!\n");
			         break;
		} /* switch */
  }
  else
	{
		char *string;
		int size = set_col_size();

		if ((string = (char*) calloc(size+1,sizeof(char))) == NULL)
    {
      print_msg(PRT_ERR, nomemory);
      return;
    }

		while (size>0)
			string[--size] = type==2?'=':'-';

		print_msg(PRT_NORMAL,"%s\n",string);
		
		free(string);
  }
} /* strich */

/*****************************************************************************/

static int n_match(char *Pattern, char* Number, char* version)
{
	int RetCode = -1;
	char s[SHORT_STRING_SIZE];

	if (!strcmp(version,LOG_VERSION_3))
	{
		RetCode = num_match(Pattern,Number);
	}
	else
	if (!strcmp(version,LOG_VERSION_2))
	{
		strcpy(s,expand_number(Number));
		RetCode = num_match(Pattern,s);
	}
	else
	if (!strcmp(version,LOG_VERSION_1))
	{
		if ((RetCode = match(Pattern, Number,0)) != 0            &&
		    !strncmp(Pattern,areaprefix,strlen(areaprefix))  )
		{
			sprintf(s,"*%s%s",myarea/*+strlen(areaprefix)*/,Pattern);
			RetCode = match(s,Number,0);
		}
	}
	else
		print_msg(PRT_ERR,"Unknown Version of logfile entries!\n");

	return RetCode;
}

/*****************************************************************************/

static int html_header(void)
{
	print_msg(PRT_NORMAL,"Content-Type: text/html\n\n");
	print_msg(PRT_NORMAL,"<HTML>\n");
	print_msg(PRT_NORMAL,"<BODY bgcolor=%s>\n",H_BG_COLOR);

	return 0;
}

/*****************************************************************************/

static int html_bottom(char *_progname, char *start, char *stop)
{
	char *progname = strdup(_progname);
	char *ptr      = strrchr(progname,'.');


	if (ptr)
		*ptr = '\0';

	if ((ptr = get_time_string(_begintime,endtime,0,-1)) != NULL)
		print_msg(PRT_NORMAL,H_LINK_DAY,_myname,ptr,"previous month");

	if ((ptr = get_time_string(_begintime,endtime,-1,0)) != NULL)
		print_msg(PRT_NORMAL,H_LINK_DAY,_myname,ptr,"previous day");

	if ((ptr = get_time_string(_begintime,endtime,1,0)) != NULL)
		print_msg(PRT_NORMAL,H_LINK_DAY,_myname,ptr,"next day");

	if ((ptr = get_time_string(_begintime,endtime,0,1)) != NULL)
		print_msg(PRT_NORMAL,H_LINK_DAY,_myname,ptr,"next month");

	print_msg(PRT_NORMAL,"\n</BODY>\n");
	print_msg(PRT_NORMAL,"<HEAD><TITLE>%s %s\n",progname,print_diff_date(start,stop));
	print_msg(PRT_NORMAL,"</TITLE>\n");
	print_msg(PRT_NORMAL,"</HTML>\n");

	free(progname);
	return 0;
}

/*****************************************************************************/

static char *print_diff_date(char *start, char *stop)
{
	static char RetCode[64];

	if (strcmp(start,stop))
		sprintf(RetCode,"%s .. %s",start,stop);
	else
		sprintf(RetCode,"%s",start);

	return RetCode;
}

/*****************************************************************************/

static int get_file_list(void)
{
	if (read_path & F_VBOX)
		set_dir_entries(vboxpath,set_vbox_entry);

	if (read_path & F_FAX)
		set_dir_entries(mgettypath,set_mgetty_entry);

	qsort(file_root,file_root_member,sizeof(file_list*),Compare_files);
	return 0;
}

/*****************************************************************************/

static int set_dir_entries(char *directory, int (*set_fct)(const char *, const char *))
{
	struct dirent *eptr;
	DIR *dptr;

	if (directory != NULL)
	{
		if ((dptr = opendir(directory)) != NULL)
		{
			while ((eptr = readdir(dptr)) != NULL)
			{
				if (eptr->d_name[0] != '.')
				{
					if (set_fct(directory,eptr->d_name))
						return -1;
				}
			}

			closedir(dptr);
		}
	}

	return 0;
}

/*****************************************************************************/

static int set_vbox_entry(const char *path, const char *file)
{
	struct tm tm;
	file_list *lptr = NULL;
	char string[PATH_MAX];
	int cnt;
	FILE *fp;
	vaheader_t ptr;



	sprintf(string,"%s%c%s",path,C_SLASH,file);

	if ((fp = fopen(string,"r")) == NULL)
		return -1;

	fread(&ptr,sizeof(vaheader_t),1,fp);
	fclose(fp);

	if (strncmp(ptr.magic,"VBOX",4))
	{
		/* Version 0.x and 1.x of vbox! */

		if ((cnt = sscanf(file,"%2d%2d%2d%2d%2d%2d",
	 		            &(tm.tm_year),
	 		            &(tm.tm_mon),
	 		            &(tm.tm_mday),
	 		            &(tm.tm_hour),
	 		            &(tm.tm_min),
	 		            &(tm.tm_sec))) != 6)
		{
			print_msg(PRT_ERR,"invalid file name `%s'!\n",file);
			return -1;
		}

		if ((lptr = (file_list*) calloc(1,sizeof(file_list))) == NULL)
		{
			print_msg(PRT_ERR, nomemory);
			return -1;
		}

	 	if (tm.tm_mday > 31)
	 	{
	 		int help = tm.tm_mday;
	 		tm.tm_mday = tm.tm_year;
	 		tm.tm_year = help;
	 	}

		tm.tm_mon--;
		tm.tm_isdst = -1;

		lptr->name = strdup(file);
	 	lptr->time = mktime(&tm);
		lptr->type = C_VBOX;
		lptr->used = 0;
		lptr->version = 1;
		lptr->compression = 0;
	}
	else
	{
		/* Version 2.x of vbox! */

		if ((lptr = (file_list*) calloc(1,sizeof(file_list))) == NULL)
		{
			print_msg(PRT_ERR, nomemory);
			return -1;
		}

		lptr->name = strdup(file);
	 	lptr->time = ntohl(ptr.time);
		lptr->type = C_VBOX;
		lptr->used = 0;
		lptr->version = 2;
		lptr->compression = ntohl(ptr.compression);
	}
/*
	else
	{
		print_msg(PRT_ERR, "Version %d of vbox is not implemented yet!\n",vboxversion);
		print_msg(PRT_ERR, "Invalid version %d of vbox!\n",vboxversion);
		return -1;
	}
*/

	return set_element_list(lptr);
}

/*****************************************************************************/

static int set_mgetty_entry(const char *path, const char *file)
{
	file_list *lptr = NULL;
	char string[PATH_MAX];

	sprintf(string,"%s%c%s",path,C_SLASH,file);

	if (access(string,R_OK))
		return -1;

	if ((lptr = (file_list*) calloc(1,sizeof(file_list))) == NULL)
	{
		print_msg(PRT_ERR, nomemory);
		return -1;
	}

	lptr->name = strdup(file);

	strcpy(string,"0x3");
	strncpy(string+3,file+2,7);
	lptr->time = strtol(string,NIL,0);

	lptr->type = C_FAX;
	lptr->used = 0;
	lptr->version = 3;
	lptr->compression = 0;

	return set_element_list(lptr);
}

/*****************************************************************************/

static int set_element_list(file_list* elem)
{
	if (file_root_size == file_root_member)
	{
		file_root_size += 10;
		if ((file_root = (file_list**) realloc(file_root,sizeof(file_list*)*file_root_size)) == NULL)
		{
			print_msg(PRT_ERR, nomemory);
			return -1;
		}
	}

	file_root[file_root_member++] = elem;

	return 0;
}

/*****************************************************************************/

static int Compare_files(const void *e1, const void *e2)
{
	if ((*(file_list**) e1)->time > (*(file_list**) e2)->time)
		return 1;
	else
	if ((*(file_list**) e1)->time < (*(file_list**) e2)->time)
		return -1;
	
	return 0;
}

/*****************************************************************************/

static file_list *get_file_to_call(time_t filetime, char type)
{
	static file_list **ptr = NULL;
	static int         cnt;
	static time_t      _filetime;
	int interval;

	if (filetime == 0)
	{
		if (ptr != NULL)
		{
			ptr++;
			cnt--;
		}
	}
	else
	{
		ptr = file_root;
		cnt = file_root_member;
		_filetime = filetime;
	}

	while (cnt > 0 && (interval = time_in_interval((*ptr)->time,_filetime,type)) < 1)
	{
		if (interval == 0 && (*ptr)->used == 0 && (*ptr)->type == type)
		{
			(*ptr)->used = 1;
			return (*ptr);
		}

		ptr++;
		cnt--;
	}

	return NULL;
}

/*****************************************************************************/

static int time_in_interval(time_t t1, time_t t2, char type)
{
	int begin;
	int end;

	switch (type)
	{
		case C_VBOX : begin = 0;
		              end   = 10;
		              break;
		case C_FAX  : begin = -2;
		              end   = 1;
		              break;
		default     : begin = 0;
		              end   = 0;
		              break;
	}

	if ((int) t1 < ((int) t2) + begin)
		return -1;

	if ((int) t1 > ((int) t2) + end)
		return 1;

	return 0;
}

/*****************************************************************************/

static char *get_links(time_t filetime, char type)
{
	static char *string[5] = {NULL,NULL,NULL,NULL,NULL};
	static int   cnt = -1;
	file_list   *ptr;


	cnt = (cnt+1) % 5;
	free(string[cnt]);
	string[cnt] = NULL;

	if (type == C_VBOX)
	{
		if ((ptr = get_file_to_call(filetime,type)) != NULL)
		{
			if ((string[cnt] = (char*) calloc(SHORT_STRING_SIZE,sizeof(char))) == NULL)
			{
				print_msg(PRT_ERR, nomemory);
				return NULL;
			}

			sprintf(string[cnt],H_LINK,_myname,type,ptr->version,nam2html(ptr->name),"phone");
		}
	}
	else
	if (type == C_FAX)
	{
		while((ptr = get_file_to_call(filetime,type)) != NULL)
		{
			append_fax(&(string[cnt]),ptr->name,type,ptr->version);
			filetime = 0;
		}
	}
	else
	{
		print_msg(PRT_ERR, "Internal error: Invalid type %d of file!\n",(int)type);
		return NULL;
	}

	return string[cnt]?string[cnt]:"        ";
}

/*****************************************************************************/

static char *append_fax(char **string, char *file, char type, int version)
{
	static int cnt = 0;
	char help[BUFSIZ];
	char help2[20];

	if (*string == NULL)
		cnt = 1;
	else
		cnt++;

	sprintf(help2,"&lt;%d&gt;",cnt);
	sprintf(help,H_LINK,_myname,type,version,nam2html(file),help2);

	if (*string == NULL)
		*string = strdup("Fax: ");

	if ((*string = (char*) realloc(*string,sizeof(char)*(strlen(*string)+strlen(help)+2))) == NULL)
	{
		print_msg(PRT_ERR, nomemory);
		return NULL;
	}

	strcat(*string,help);
	strcat(*string," ");

	return *string;
}

/*****************************************************************************/

static char *nam2html(char *file)
{
	static char RetCode[SHORT_STRING_SIZE];
	char *ptr = RetCode;

	while (*file != '\0')
	{
		switch (*file)
		{
			case '+': strcpy(ptr,"%2b");
			          ptr += 3;
			          file++;
			          break;
			default : *ptr++ = *file++; 
			          break;
		}
	}

	*ptr = '\0';

	return RetCode;
}

/*****************************************************************************/

static char *get_a_day(time_t t, int d_diff, int m_diff, int flag)
{
	static char string[16];
	struct tm *tm;
	time_t cur_time;
	char   flag2 = 0;


	if (t == 0)
	{
		if (flag & F_END)
			return NULL;

		time(&t);
		flag2 = 1;
	}

	if (flag & F_END)
		t++;

	tm = localtime(&t);

	tm->tm_mday += d_diff;
	tm->tm_mon  += m_diff;
	tm->tm_isdst = -1;

	if (flag2 == 1)
	{
		tm->tm_hour  = 0;
		tm->tm_min   = 0;
		tm->tm_sec   = 0;
	}

	t = mktime(tm);

	if (flag & F_END)
		t-=2;

	time(&cur_time);

	if (cur_time < t)
	{
		if (!(flag & F_END))
			return NULL;
	/*
		if (flag & F_END)
			t = cur_time;
		else
			return NULL;
	*/
	}

	tm = localtime(&t);
	sprintf(string,"%02d%02d%02d%02d%04d.%02d",tm->tm_mon+1,tm->tm_mday,tm->tm_hour,tm->tm_min,tm->tm_year+1900,tm->tm_sec);

	return string;
}

/*****************************************************************************/

static char *get_time_string(time_t begin, time_t end, int d_diff, int m_diff)
{
	static char string[40];
	char *ptr = NULL;


	sprintf(string,"-w%d+",html-1);
	/*                         ^^---sehr gefaehrlich, da eine UND-Verknuepfung!!! */

	if ((ptr = get_a_day(begin,d_diff,m_diff,F_BEGIN)) != NULL)
	{
		strcat(string,"-t");
		strcat(string,ptr);

		if ((ptr = get_a_day(end,d_diff,m_diff,F_END)) != NULL)
		{
			strcat(string,"-");
			strcat(string,ptr);
		}

		return string;
	}

	return NULL;
}

/*****************************************************************************/

static char *create_vbox_file(char *file, int *compression)
{
	int fdin, fdout, len;
	char string[BUFSIZ];
	char *fileout = NULL;
	vaheader_t header;

	if ((fdin = open(file,O_RDONLY)) == -1)
	{
		print_msg(PRT_ERR,"Can not open file `%s': %s!\n",file, strerror(errno));
		return NULL;
	}

	if (read(fdin,&header,sizeof(vaheader_t)) == sizeof(vaheader_t))
	{
		if (compression != NULL)
			*compression = ntohl(header.compression);

		if ((fileout = tmpnam(NULL)) == NULL || (fdout = open(fileout,O_WRONLY | O_CREAT,0444)) == -1)
		{
			print_msg(PRT_ERR,"Can not open file `%s': %s!\n",fileout, strerror(errno));
			close(fdin);
			return NULL;
		}

		while((len = read(fdin,string,BUFSIZ)) > 0)
		{
			if (write(fdout,string,len) != len)
			{
				print_msg(PRT_ERR,"Can not write to file `%s': %s!\n",fileout, strerror(errno));
				close(fdout);
				close(fdin);
				unlink(fileout);
				return NULL;
			}
		}

		close(fdout);
	}
	
	close(fdin);
	return fileout;
}

/*****************************************************************************/

