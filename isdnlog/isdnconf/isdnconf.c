/* $Id: isdnconf.c,v 1.9 1997/05/25 19:40:53 luethje Exp $
 *
 * ISDN accounting for isdn4linux. (Report-module)
 *
 * Copyright 1996, 1997 by Stefan Luethje (luethje@sl-gw.lake.de)
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

			ptr = (CEPtr = Get_Entry(conf_dat->entries,CONF_ENT_ZONE))?(CEPtr->value?CEPtr->value:""):"";
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

int main(int argc, char *argv[], char *envp[])
{
	int c;
	int Cnt = 0;
	section *conf_dat = NULL;
	char *myname = basename(argv[0]);
	FILE *fp;

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
		int len;
		
		if ((ptr = get_areacode(areacode,&len,quiet?C_NO_ERROR|C_NO_WARN:0)) != NULL)
		{
			if (!isdnmon)
			{
				print_msg(PRT_NORMAL,"%s\n",ptr);
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

