/* $Id: isdntools.c,v 1.15 1997/06/15 23:50:34 luethje Exp $
 *
 * ISDN accounting for isdn4linux. (Utilities)
 *
 * Copyright 1995, 1997 by Stefan Luethje (luethje@sl-gw.lake.de)
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
 * Revision 1.15  1997/06/15 23:50:34  luethje
 * some bugfixes
 *
 * Revision 1.14  1997/05/19 23:37:05  luethje
 * bugfix for isdnconf
 *
 * Revision 1.13  1997/05/19 22:58:28  luethje
 * - bugfix: it is possible to install isdnlog now
 * - improved performance for read files for vbox files and mgetty files.
 * - it is possible to decide via config if you want to use avon or
 *   areacode.
 *
 * Revision 1.12  1997/05/09 23:31:06  luethje
 * isdnlog: new switch -O
 * isdnrep: new format %S
 * bugfix in handle_runfiles()
 *
 * Revision 1.11  1997/04/15 00:20:17  luethje
 * replace variables: some bugfixes, README comleted
 *
 * Revision 1.10  1997/04/08 21:57:04  luethje
 * Create the file isdn.conf
 * some bug fixes for pid and lock file
 * make the prefix of the code in `isdn.conf' variable
 *
 * Revision 1.9  1997/04/08 00:02:24  luethje
 * Bugfix: isdnlog is running again ;-)
 * isdnlog creates now a file like /var/lock/LCK..isdnctrl0
 * README completed
 * Added some values (countrycode, areacode, lock dir and lock file) to
 * the global menu
 *
 * Revision 1.8  1997/04/03 22:39:13  luethje
 * bug fixes: environ variables are working again, no seg. 11 :-)
 * improved performance for reading the config files.
 *
 * Revision 1.7  1997/03/20 00:19:27  luethje
 * inserted the line #include <errno.h> in avmb1/avmcapictrl.c and imon/imon.c,
 * some bugfixes, new structure in isdnlog/isdnrep/isdnrep.c.
 *
 * Revision 1.6  1997/03/19 00:08:43  luethje
 * README and function expand_number() completed.
 *
 * Revision 1.5  1997/03/18 23:01:50  luethje
 * Function Compare_Sections() completed.
 *
 * Revision 1.4  1997/03/07 23:34:49  luethje
 * README.conffile completed, paranoid_check() used by read_conffiles(),
 * policy.h will be removed by "make distclean".
 *
 * Revision 1.3  1997/03/06 20:36:34  luethje
 * Problem in create_runfie() fixed. New function paranoia_check() implemented.
 *
 * Revision 1.2  1997/03/03 22:05:39  luethje
 * merging of the current version and my tree
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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>

#include "libisdn.h"

/****************************************************************************/

static int (*print_msg)(const char *, ...) = printf;
static char *_get_avon(char *code, int *Len, int flag);
static char *_get_areacode(char *code, int *Len, int flag);
static int create_runfile(const char *file, const char *format);

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

static char *avonlib = NULL;
static char *codelib = NULL;

/****************************************************************************/

void set_print_fct_for_lib(int (*new_print_msg)(const char *, ...))
{
	print_msg = new_print_msg;
	set_print_fct_for_conffile(new_print_msg);
	set_print_fct_for_libtools(new_print_msg);
	set_print_fct_for_avon(new_print_msg);
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
	int all_allowed = 0;
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
		strcpy(Help,countryprefix);
		Ptr++;
	}

	while(*Ptr != '\0')
	{
		if (isdigit(*Ptr) || *Ptr == '?' || *Ptr == '*'|| 
		    *Ptr == '[' ||  *Ptr == ']' || all_allowed   )
		{
			if (*Ptr == '[')
				all_allowed  = 1;

			if (*Ptr == ']')
				all_allowed  = 0;

			Index = strlen(Help);
			Help[Index] = *Ptr;
			Help[Index+1] = '\0';
		}

		Ptr++;
	}

	if (Help[0] == '\0')
		return s;

	if (Help[0] == '*' || !strncmp(Help,countryprefix,strlen(countryprefix)))
	{
		strcpy(Num,Help);
	}
	else
	if (!strncmp(Help,areaprefix,strlen(areaprefix)))
	{
		strcpy(Num,mycountry);
		strcat(Num,Help+strlen(areaprefix));
	}
	else
	{
		strcpy(Num,mycountry);
		strcat(Num,myarea/*+strlen(areaprefix)*/);
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

	return (file[0] == '\0'?s:file);
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

int handle_runfiles(const char *_progname, char **_devices, int flag)
{
	static char   progname[SHORT_STRING_SIZE] = "";
  static char **devices = NULL;
  auto   char   string[PATH_MAX];
  auto   char   string2[SHORT_STRING_SIZE];
  auto   char  *Ptr = NULL;
  auto   int    RetCode = -1;
	auto   FILE  *fp;


	if (progname[0] == '\0' || devices == NULL)
	{
		if (_progname == NULL || _devices == NULL)
			return -1;

		Ptr = strrchr(progname,C_SLASH);
		strcpy(progname,Ptr?Ptr+1:_progname);

		while (*_devices != NULL)
		{
			Ptr = strrchr(*_devices,C_SLASH);
			append_element(&devices,Ptr?Ptr+1:*_devices);
			_devices++;
		}
	}

	if (flag == START_PROG)
	{
		sprintf(string,"%s%c%s.%s.pid",RUNDIR,C_SLASH,progname,devices[0]);

		if ((RetCode = create_runfile(string,"%d\n")) != 0)
		{
			if (RetCode > 0)
			{
				print_msg("Another %s is running with pid %d!\n", progname, RetCode);
				print_msg("If not delete the file `%s' nad try it again!\n", string);
			}

			return RetCode;
		}

		while (*devices != NULL)
		{
			sprintf(string,"%s%c%s%s",LOCKDIR,C_SLASH,LOCKFILE,*devices);

			if ((RetCode = create_runfile(string,"%10d\n")) != 0)
			{
				if (RetCode > 0)
					print_msg("Another process (pid=%d) is running on device %s!\n", RetCode, *devices);

				return RetCode;
			}

			devices++;
		}

		RetCode = 0;
	}

	if (flag == STOP_PROG)
	{
		sprintf(string,"%s%c%s.pid",RUNDIR,C_SLASH,progname);
		unlink(string);

		while (*devices != NULL)
		{
			sprintf(string,"%s%c%s%s",LOCKDIR,C_SLASH,LOCKFILE,*devices);

			if ((fp = fopen(string, "r")) != NULL)
			{
				if (fgets(string2,SHORT_STRING_SIZE,fp) != NULL)
				{
					if (atoi(string2) == (int)getpid())
					{
						if (unlink(string))
							print_msg("Can not remove file %s (%s)!\n", string, strerror(errno));
					}
				}

				fclose(fp);
			}

			devices++;
		}

		RetCode = 0;
	}

	return RetCode;
}

/****************************************************************************/

static int create_runfile(const char *file, const char *format)
{
	auto char  string[SHORT_STRING_SIZE];
	auto int   RetCode = -1;
	auto int   fd      = -1;
	auto FILE *fp;

	if (file == NULL)
		return -1;

	if ((fd = open(file, O_WRONLY|O_CREAT|O_EXCL|O_TRUNC, 0644)) >= 0)
	{
		sprintf(string, format, (int)getpid());

		if (write(fd, string, strlen(string)) != strlen(string) )
		{
			print_msg("Can not write to PID file `%s'!\n", file);
  		RetCode = -1;
		}
  	else
  		RetCode = 0;

		close(fd);
	}
	else
	{
		if ((fp = fopen(file, "r")) == NULL)
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
			if (unlink(file))
				return -1;

			return create_runfile(file,format);
		}
		
		fclose(fp);
	}

	return RetCode;
} /* create_runfile */

/****************************************************************************/

/* Setzt die Laendercodes, die fuer die Lib gebraucht werden. Diese Funktion
   muss von jedem Programm aufgerufen werden!!!
*/

#define _MAX_VARS 7

int Set_Codes(section* Section)
{
	static char *ptr[_MAX_VARS] = {NULL,NULL,NULL,NULL,NULL,NULL,NULL};
	int i;
	int RetCode = 0;
	entry *Entry;
	char *ptr2;
	char s[SHORT_STRING_SIZE];
	section *SPtr;

	for (i=0; i < _MAX_VARS; i++)
		if (ptr[i] != NULL)
		{
			free(ptr[i]);
			ptr[i] = NULL;
		}

	if ((SPtr = Get_Section(Section,CONF_SEC_GLOBAL)) == NULL)
		return -1;

	if ((Entry = Get_Entry(SPtr->entries,CONF_ENT_AREALIB)) != NULL &&
	    Entry->value != NULL                                             )
		ptr[0] = acFileName = strdup(Entry->value);

	if ((Entry = Get_Entry(SPtr->entries,CONF_ENT_AVONLIB)) != NULL &&
	    Entry->value != NULL                                             )
		ptr[1] = avonlib = strdup(Entry->value);
	else
	{
		sprintf(s, "%s%c%s", confdir(), C_SLASH, AVON);
		ptr[1] = avonlib = strdup(s);
	}

	if ((Entry = Get_Entry(SPtr->entries,CONF_ENT_COUNTRY_PREFIX)) != NULL &&
	    Entry->value != NULL                                             )
		ptr[2] = countryprefix = strdup(Entry->value);

	if ((Entry = Get_Entry(SPtr->entries,CONF_ENT_AREA_PREFIX)) != NULL &&
	    Entry->value != NULL                                             )
		ptr[3] = areaprefix = strdup(Entry->value);

	if ((Entry = Get_Entry(SPtr->entries,CONF_ENT_CODELIB)) != NULL &&
	    Entry->value != NULL                                             )
		ptr[4] = codelib = strdup(Entry->value);

	if ((Entry = Get_Entry(SPtr->entries,CONF_ENT_AREA)) != NULL &&
	    Entry->value != NULL                                          )
	{
		ptr2 = Entry->value;

		if (strncmp(Entry->value,areaprefix,strlen(areaprefix)))
			ptr[5] = myarea = strdup(ptr2);
		else
			ptr[5] = myarea = strdup(ptr2+strlen(areaprefix));

		if (ptr[5] != NULL)
			RetCode++;
		else
			print_msg("Error: Variable `%s' are not set!\n",CONF_ENT_AREA);
	}

	if ((Entry = Get_Entry(SPtr->entries,CONF_ENT_COUNTRY)) != NULL &&
	    Entry->value != NULL                                             )
	{
		ptr2 = Entry->value;

		if (strncmp(Entry->value,countryprefix,strlen(countryprefix)))
		{
			sprintf(s,"%s%s",countryprefix,
			                 Entry->value[0]=='+'?(Entry->value)+1:Entry->value);
			ptr2 = s;
		}
			
		if ((ptr[6] = mycountry = strdup(ptr2)) != NULL)
			RetCode++;
		else
			print_msg("Error: Variable `%s' are not set!\n",CONF_ENT_COUNTRY);
	}

	SPtr = Section;

	while ((SPtr = Get_Section(SPtr,CONF_SEC_VAR)) != NULL)
	{
		Entry = SPtr->entries;

		while(Entry != NULL)
		{
			if (setenv(Entry->name, Entry->value, 1) != 0)
				print_msg("Warning: Can not set environment variable `%s=%s'!\n", Entry->name, Entry->value);

			Entry = Entry->next;
		}

		SPtr = SPtr->next;
	}

	return (RetCode==2?0:-1);
}

/****************************************************************************/

char *get_areacode(char *code, int *Len, int flag)
{
	auto     char  *Ptr;
	auto     int    i = 0;
	register int    prefix = strlen(countryprefix);


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

	if (codelib != NULL && !strcasecmp(codelib,"AVON"))
		Ptr = _get_avon(code,Len,flag);
	else
	if (codelib != NULL && !strcasecmp(codelib,"AREACODE"))
		Ptr = _get_areacode(code,Len,flag);
	else
#ifdef LIBAREA
		Ptr = _get_areacode(code,Len,flag);
#else
		Ptr = _get_avon(code,Len,flag);
#endif

	if (Ptr != NULL)
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

static char *_get_avon(char *code, int *Len, int flag)
{
	static int opened = 0;
	static char s[BUFSIZ];
	int prefix = strlen(countryprefix);
	int ll=0;
	int l;


	l = strlen(code) - prefix;

	s[0] = '\0';

	if (Len != NULL)
		*Len = -1;

	if (!opened)
	{
		if (avonlib == NULL && !(flag & C_NO_ERROR))
				print_msg("No path for AVON library defined!\n");

		if (!access(avonlib, R_OK))
		{
			createDB(avonlib,0);
			openDB(avonlib,O_WRONLY);
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

/****************************************************************************/

static char *_get_areacode(char *code, int *Len, int flag)
{
	auto     int    cc = 0;
	auto     char  *err;
	static   acInfo ac;
	static   int    warned = 0;
	int prefix = strlen(countryprefix);

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

/****************************************************************************/

int read_conffiles(section **Section, char *groupfile)
{
	static section *conf_dat = NULL;
	static int read_again = 0;
	auto char    s[6][BUFSIZ];
	auto char **vars  = NULL;
	auto char **files = NULL;
	auto int    fileflag[6];
	auto int    i = 0;
	auto int      RetCode = -1;

	*Section = NULL;

	if (!read_again)
	{
	  sprintf(s[0], "%s%c%s", confdir(), C_SLASH, CONFFILE);
	  sprintf(s[1], "%s%c%s", confdir(), C_SLASH, CALLERIDFILE);

		if (paranoia_check(s[0]))
			return -1;

		if (paranoia_check(s[1]))
			return -1;

	  append_element(&files,s[0]);
	  fileflag[i++] = MERGE_FILE;
	  append_element(&files,s[1]);
	  fileflag[i++] = APPEND_FILE;

	  if (groupfile != NULL)
	  {
		  strcpy(s[2],groupfile);
			append_element(&files,s[2]);
	  	fileflag[i++] = MERGE_FILE;
		}

	  strcpy(s[3],expand_file(USERCONFFILE));
		append_element(&files,s[3]);
	  fileflag[i++] = MERGE_FILE;
	}

	sprintf(s[4],"%s|%s/%s|!%s",CONF_SEC_MSN,CONF_SEC_NUM,CONF_ENT_NUM,CONF_ENT_SI);
	append_element(&vars,s[4]);

/*
	sprintf(s[5],"%s|%s/%s",CONF_SEC_MSN,CONF_SEC_NUM,CONF_ENT_ALIAS);
	append_element(&vars,s[5]);
*/

	if ((RetCode = read_files(&conf_dat, files, fileflag, vars, C_OVERWRITE|C_NOT_UNIQUE|C_NO_WARN_FILE)) > 0)
	{
		*Section = conf_dat;

		if (Set_Codes(conf_dat) != 0)
			return -1;
	}
	else
		*Section = conf_dat;

	if (!read_again)
		delete_element(&files,0);

	delete_element(&vars,0);

	read_again = 1;
	return RetCode;
}

/****************************************************************************/

int paranoia_check(char *cmd)
{
	struct stat stbuf;


	if (getuid() == 0)
	{
		if (stat(cmd, &stbuf))
		{
			if (errno == ENOENT)
				return 0;

			print_msg("stat() failed for file `%s', stay on the safe side!\n", cmd);
			return -1;
		}

		if (stbuf.st_uid != 0)
		{
			print_msg("Owner of file `%s' is not root!\n", cmd);
			return -1;
		}

		if ((stbuf.st_gid != 0 && (stbuf.st_mode & S_IWGRP)) ||
		    (stbuf.st_mode & S_IWOTH)                          )
		{
			print_msg("File `%s' is writable by group or world!\n", cmd);
			return -1;
		}
	}

	return 0;
}

/****************************************************************************/

