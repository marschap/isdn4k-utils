/* $Id: isdntools.c,v 1.1 1997/03/03 04:23:15 fritz Exp $
 *
 * ISDN accounting for isdn4linux. (Utilities)
 *
 * Copyright 1995, 1997 by Andreas Kool (akool@Kool.f.EUnet.de)
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
 * $Log: isdntools.c,v $
 * Revision 1.1  1997/03/03 04:23:15  fritz
 * Added files in lib
 *
 * Revision 2.6.26  1997/01/19  22:23:43  akool
 * Weitere well-known number's hinzugefuegt
 *
 * Revision 2.6.24  1997/01/15  19:13:43  akool
 * neue AreaCode Lib 0.99 integriert
 *
 * Revision 2.6.20  1997/01/05  20:06:43  akool
 * atom() erkennt nun "non isdnlog" "/tmp/isdnctrl0" Output's
 *
 * Revision 2.6.19  1997/01/05  19:39:43  akool
 * AREACODE Support added
 *
 * Revision 2.40    1996/06/16  10:06:43  akool
 * double2byte(), time2str() added
 *
 * Revision 2.3.26  1996/05/05  12:09:16  akool
 * known.interface added
 *
 * Revision 2.3.15  1996/04/22  21:10:16  akool
 *
 * Revision 2.3.4  1996/04/05  11:12:16  sl
 * confdir()
 *
 * Revision 2.2.5  1996/03/25  19:41:16  akool
 * 1TR6 causes implemented
 *
 * Revision 2.23  1996/03/14  20:29:16  akool
 * Neue Routine i2a()
 *
 * Revision 2.17  1996/02/25  19:14:16  akool
 * Soft-Error in atom() abgefangen
 *
 * Revision 2.06  1996/02/07  18:49:16  akool
 * AVON-Handling implementiert
 *
 * Revision 2.01  1996/01/20  12:11:16  akool
 * Um Einlesen der neuen isdnlog.conf Felder erweitert
 * discardconfig() implementiert
 *
 * Revision 2.00  1996/01/10  20:11:16  akool
 *
 */

/****************************************************************************/


#define  PUBLIC /**/
#define  _ISDNTOOLS_C_

/****************************************************************************/

#include <fnmatch.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>

#include "libisdn.h"

/****************************************************************************/

static int (*print_msg)(const char *, ...) = printf;
static char *_get_areacode(char *code, int *Len, int flag);

/****************************************************************************/

static char areacodes[][2][30] = {
	{"+49130", "Toll-free"},
	{"+491802", "Service 180-2 (0,12/Anruf)"},
	{"+491803", "Service 180-3 (DM 0,24/Anruf)"},
	{"+491805", "Service 180-5 (DM 0,48/Anruf)"},
	{"+491901", "Service 190 (DM 1,20/Minute)"},
	{"+491902", "Service 190 (DM 1,20/Minute)"},
	{"+491903", "Service 190 (DM 1,20/Minute)"},
	{"+491904", "Service 190 (DM 0,80/Minute)"},
	{"+491905", "Service 190 (DM 1,20/Minute)"},
	{"+491906", "Service 190 (DM 0,80/Minute)"},
	{"+491907", "Service 190 (DM 2,40/Minute)"},
	{"+491908", "Service 190 (DM 3,60/Minute)"},
	{"+491909", "Service 190 (DM 2,40/Minute)"},
	{"+49161", "Mobilfunknetz C"},
	{"+49171", "Mobilfunknetz D1"},
	{"+49172", "Mobilfunknetz D2"},
	{"+49177", "Mobilfunknetz E-Plus"},
	{"+491188", "Auskunft Inland"},
	{"+491910", "T-Online"},
	{"", ""},
};

static char countrycodes[][2][30] = {
	{"+30", "Greek"},
	{"+31", "Nethland"},
	{"+32", "Belgium"},
	{"+33", "France"},
	{"+34", "Spain"},
	{"+39", "Italy"},
	{"+41", "Swiss"},
	{"+43", "Austria"},
	{"+44", "Great Britain"},
	{"+45", "Danmark"},
	{"+46", "Sweden"},
	{"+47", "Norway"},
	{"+49", "Germany"},
	{"+352", "Luxembourg"},
	{"+1", "United States"},
	{"", ""},
};

char *avonlib;

/****************************************************************************/

void set_print_fkt_for_lib(int (*new_print_msg)(const char *, ...))
{
	print_msg = new_print_msg;
	set_print_fkt_for_conffile(new_print_msg);
	set_print_fkt_for_libtools(new_print_msg);
#ifndef LIBAREA
	set_print_fkt_for_avon(new_print_msg);
#endif
}

/****************************************************************************/

int num_match(char* Pattern, char *number)
{
	int RetCode = -1;
	char **Ptr;
	char **Array;

	if (!strcmp(Pattern, number))
		RetCode = 0;
	else
	if (strchr(Pattern,C_NUM_DELIM))
	{
		Ptr = Array = String_to_Array(Pattern,C_NUM_DELIM);

		while (*Ptr != NULL && RetCode != 0)
		{
			RetCode = match(expand_number(*Ptr), number, 0);
			Ptr++;
		}

		del_Array(Array);
	}
	else
		RetCode = match(expand_number(Pattern), number, 0);

	return RetCode;
}

/****************************************************************************/

char *expand_number(char *s)
{
	char *Ptr;
	int   Index;
	char Help[SHORT_STRING_SIZE];
	static char Num[SHORT_STRING_SIZE];


	Help[0] = '\0';
	Ptr = s;

	if (Ptr == NULL || Ptr[0] == '\0')
		return "";

	while(isblank(*Ptr))
		Ptr++;

	if (*Ptr  == '+')
	{
		strcpy(Help,S_COUNTRY_PREFIX);
		Ptr++;
	}

	while(*Ptr != '\0')
	{
		switch (*Ptr)
		{
			case '\t':
			case ' ':
			case '-':
			case '/':
			          break;
			default : Index = strlen(Help);
			          Help[Index] = *Ptr;
			          Help[Index+1] = '\0';
			          break;
		}

		Ptr++;
	}

	if (Help[0] == '*' || !strncmp(Help,S_COUNTRY_PREFIX,strlen(S_COUNTRY_PREFIX)))
	{
		strcpy(Num,Help);
	}
	else
	if (!strncmp(Help,S_AREA_PREFIX,strlen(S_AREA_PREFIX)))
	{
		strcpy(Num,mycountry);
		strcat(Num,Help+strlen(S_AREA_PREFIX));
	}
	else
	{
		strcpy(Num,mycountry);
		strcat(Num,myarea/*+strlen(S_AREA_PREFIX)*/);
		strcat(Num,Help);
	}

	return Num;
}

/****************************************************************************/

char *expand_file(char *s)
{
	char *Ptr;
	uid_t id = -1;
	char  Help[PATH_MAX];
	static char file[PATH_MAX];
	struct passwd *password;


	Help[0] = '\0';
	file[0] = '\0';

	if (s == NULL)
		return NULL;

	if (s[0] == '~')
	{
		if (s[1] == C_SLASH)
		{
			/* Ghandi, vielleicht kommt hier auch getuid() hin */
			id = geteuid();
		}
		else
		{
			strcpy(Help,s+1);
			if ((Ptr = strchr(Help,C_SLASH)) != NULL)
				*Ptr = '\0';
			else
				return NULL;
		}

		setpwent();
		while((password = getpwent()) != NULL &&
		      strcmp(password->pw_name,Help)  &&
		      password->pw_uid != id            );

		if (password == NULL)
			return NULL;

		strcpy(file,password->pw_dir);
		strcat(file,strchr(s,C_SLASH));
	}

	return file;
}

/****************************************************************************/

char *confdir(void)
{
  static char *confdirvar = NULL;

	if (confdirvar == NULL && (confdirvar = getenv(CONFDIR_VAR)) == NULL)
    confdirvar = I4LCONFDIR;

  return(confdirvar);
} /* confdir */

/****************************************************************************/

int delete_runfile(const char *progname)
{
  char *Ptr = NULL;
  char runfile[PATH_MAX];

	if (progname != NULL)
		return -1;

	Ptr = strrchr(progname,C_SLASH);
	sprintf(runfile,"%s%c%s.pid",RUNDIR,C_SLASH,Ptr?Ptr+1:progname);

	return unlink(runfile);
} /* delete_runfile */

/****************************************************************************/

int create_runfile(const char *progname)
{
  char *Ptr = NULL;
  char runfile[PATH_MAX];
  char string[SHORT_STRING_SIZE];
  int RetCode = -1;
  FILE *fp;

	if (progname == NULL)
		return -1;

	Ptr = strrchr(progname,C_SLASH);
	sprintf(runfile,"%s%c%s.pid",RUNDIR,C_SLASH,Ptr?Ptr+1:progname);

	if (access(runfile,W_OK) != 0 && errno == ENOENT)
	{
		if ((fp = fopen(runfile, "w")) == NULL)
			return -1;

		fprintf(fp, "%d\n", (int)getpid());
		fclose(fp);
		chmod(runfile, 0644);
  
  	RetCode = 0;
	}
	else
	{
		if ((fp = fopen(runfile, "r")) == NULL)
			return -1;

		if (fgets(string,SHORT_STRING_SIZE,fp) != NULL)
			RetCode = atoi(string);
		else
			/* Datei ist leer. */
			RetCode = -1;

		if ( RetCode == -1 || (int)getpid() == RetCode                           ||
		    ((int) getpid() != RetCode && kill(RetCode,0) != 0 && errno == ESRCH)  )
		    /* Wenn der alte Prozess nicht mehr existiert! */
		{

			fclose(fp);
			if (unlink(runfile))
				return -1;

			return create_runfile(progname);
		}
		
		fclose(fp);
	}

	return RetCode;
} /* create_runfile */

/****************************************************************************/

/* Setzt die Laendercodes, die fuer die Lib gebraucht werden. Diese Funktion
   muss von jedem Programm aufgerufen werden!!!
*/

#define _MAX_VARS 3

int Set_Codes(section* Section)
{
	static char *ptr[_MAX_VARS] = {NULL,NULL,NULL};
	int i;
	int RetCode = 0;
	entry *Entry;
	char *ptr2;
	char s[SHORT_STRING_SIZE];

	for (i=0; i < _MAX_VARS; i++)
		if (ptr[i] != NULL)
			free(ptr[i]);

	if ((Section = Get_Section(Section,CONF_SEC_GLOBAL)) == NULL)
		return -1;

#ifdef LIBAREA
	if ((Entry = Get_Entry(Section->entries,CONF_ENT_AREALIB)) != NULL &&
	    Entry->value != NULL                                             )
		ptr[0] = acFileName = strdup(Entry->value);
#else
	if ((Entry = Get_Entry(Section->entries,CONF_ENT_AVONLIB)) != NULL &&
	    Entry->value != NULL                                             )
		ptr[0] = avonlib = strdup(Entry->value);
	else
	{
		sprintf(s, "%s%c%s", CONFIG_AVON_DATA, C_SLASH, AVON);
		ptr[0] = avonlib = strdup(s);
	}
#endif

	if ((Entry = Get_Entry(Section->entries,CONF_ENT_AREA)) != NULL &&
	    Entry->value != NULL                                          )
	{
		ptr2 = Entry->value;

		if (strncmp(Entry->value,S_AREA_PREFIX,strlen(S_AREA_PREFIX)))
			ptr[1] = myarea = strdup(ptr2);
		else
			ptr[1] = myarea = strdup(ptr2+strlen(S_AREA_PREFIX));

		if (ptr[1] != NULL)
			RetCode++;
	}

	if ((Entry = Get_Entry(Section->entries,CONF_ENT_COUNTRY)) != NULL &&
	    Entry->value != NULL                                             )
	{
		ptr2 = Entry->value;

		if (strncmp(Entry->value,S_COUNTRY_PREFIX,strlen(S_COUNTRY_PREFIX)))
		{
			sprintf(s,"%s%s",S_COUNTRY_PREFIX,
			                 Entry->value[0]=='+'?(Entry->value)+1:Entry->value);
			ptr2 = s;
		}
			
		if ((ptr[2] = mycountry = strdup(ptr2)) != NULL)
			RetCode++;
	}

	return (RetCode==2?0:-1);
}

/****************************************************************************/

char *get_areacode(char *code, int *Len, int flag)
{
	auto     char  *Ptr;
	auto     int    i = 0;
	register int    prefix = strlen(S_COUNTRY_PREFIX);


	if (Len != NULL)
		*Len = -1;

	if (code == NULL || code[0] =='\0')
		return NULL;

	if (!(flag & C_NO_EXPAND))
	{
		char *ptr = expand_number(code);

		if ((code = alloca(strlen(ptr))) == NULL)
			print_msg("Can not allocate memory!\n");

		strcpy(code,ptr);
	}

	while (areacodes[i][0][0] != '\0')
	{
		if (!memcmp(areacodes[i][0]+1,code+prefix,strlen(areacodes[i][0]+1)))
		{
			if (Len != NULL)
				*Len = strlen(areacodes[i][0]) - 1 /* das "+" */ + prefix;

			return areacodes[i][1];
		}

		i++;
	}

	if ((Ptr = _get_areacode(code,Len,flag)) != NULL)
		return Ptr;

	i=0;

	while (countrycodes[i][0][0] != '\0')
	{
		if (!memcmp(countrycodes[i][0]+1,code+prefix,strlen(countrycodes[i][0]+1)))
		{
			if (Len != NULL)
				*Len = strlen(countrycodes[i][0]) - 1 /* das "+" */ + prefix;

			return countrycodes[i][1];
		}

		i++;
	}

	if (!(flag & C_NO_WARN))
		print_msg("Unknown areacode `%s'!\n", code);

	return NULL;
}

/****************************************************************************/

#ifndef LIBAREA
static char *_get_areacode(char *code, int *Len, int flag)
{
	static int opened = 0;
	static char s[BUFSIZ];
	int prefix = strlen(S_COUNTRY_PREFIX);
	int ll=0;
	int l;


	l = strlen(code) - prefix;

	s[0] = '\0';

	if (Len != NULL)
		*Len = -1;

	if (!opened)
	{
		auto     char  s[BUFSIZ];


		if (avonlib == NULL && !(flag & C_NO_ERROR))
				print_msg("No path for AVON library defined!\n");

		if (!access(avonlib, R_OK))
		{
			if (createDB(avonlib, 0) && !openDB(avonlib))
				readAVON(avonlib);
			else
				(void)openDB(avonlib);
		}

		opened = 1;

		if (dbm == NULL && !(flag & C_NO_ERROR))
			print_msg("!!! Problem with AVON database! - disabling AVON support!\n");
	}

	if (dbm != NULL && l > 3)
	{
		key.dptr = code+prefix;

		do
		{
			ll++;
			key.dsize = l--;
			data = dbm_fetch(dbm, key);

			if (data.dptr != NULL)
			{
				if (Len != NULL)
					*Len = ll;

				strcpy(s,data.dptr);
			}
		}
		while(l > 1);
	}

	return (s[0]?s:NULL);
}
#endif

/****************************************************************************/

#ifdef LIBAREA
static char *_get_areacode(char *code, int *Len, int flag)
{
	auto     int    cc = 0;
	auto     char  *err;
	static   acInfo ac;
	static   int    warned = 0;
	int prefix = strlen(S_COUNTRY_PREFIX);

	if (!warned && (cc = GetAreaCodeInfo(&ac, code + prefix)) == acOk)
	{
		if (ac.AreaCodeLen > 0)
		{
			if (Len != NULL)
				*Len = ac.AreaCodeLen + prefix;

			return ac.Info;
		}
	}
	else
	{
		switch (cc) {
			case acFileError    : err = "Cannot open/read file";
			                      break;
			case acInvalidFile  : err = "The file exists but is no area code data file";
			                      break;
			case acWrongVersion : err = "Wrong version of data file";
			                      break;
			default             : err = "Unknown AreaCode error";
			                      break;
    } /* switch */

		if (!(flag & C_NO_ERROR))
			print_msg("!!! Problem with AreaCode: %s - disabling AreaCode support!\n", err);

		warned = 1;
	}

	return NULL;
}
#endif

/****************************************************************************/

