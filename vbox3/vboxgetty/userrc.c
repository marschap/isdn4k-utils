/*
** $Id: userrc.c,v 1.3 1998/08/31 15:30:42 michael Exp $
**
** Copyright 1996-1998 Michael 'Ghandi' Herold <michael@abadonna.mayn.de>
**
** $Log: userrc.c,v $
** Revision 1.3  1998/08/31 15:30:42  michael
** - Added touchtone support.
** - Added new tcl command "vbox_breaklist" to clear/set the touchtone
**   breaklist.
** - Removed the audio fragment size setting again. I don't know why this
**   crash my machine. The fragment size setting can be enabled in audio.h
**   with a define.
**
** Revision 1.2  1998/08/31 10:43:14  michael
** - Changed "char" to "unsigned char".
**
** Revision 1.1  1998/07/06 09:05:33  michael
** - New control file code added. The controls are not longer only empty
**   files - they can contain additional informations.
** - Control "vboxctrl-answer" added.
** - Control "vboxctrl-suspend" added.
** - Locking mechanism added.
** - Configuration parsing added.
** - Some code cleanups.
**
*/

#ifdef HAVE_CONFIG_H
#  include "../config.h"
#endif

#include <stdio.h>
#include <string.h>
#include <fnmatch.h>
#include <pwd.h>
#include <grp.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "userrc.h"
#include "rc.h"
#include "stringutils.h"
#include "vboxgetty.h"
#include "log.h"

/*************************************************************************/
/** userrc_parse():	Liest die Datei "vboxgetty.user" ein.					**/
/*************************************************************************/
/** => vboxuser		Zeiger auf die User-Struktur.								**/
/** => home				Spoolverzeichnis des Benutzers.							**/
/** => phone			Lokale Telefonnummer die einem Benutzer zugewiesen	**/
/**						werden soll.													**/
/**																							**/
/** <=					0 wenn die Nummer einem Benutzer zugeordnet werden	**/
/**						konnte oder -1 bei einem Fehler.							**/
/*************************************************************************/

int userrc_parse(struct vboxuser *vboxuser, unsigned char *home, unsigned char *phone)
{
	struct passwd	*pwdent;
	struct group	*grpent;
	unsigned char	 line[USERRC_MAX_RCLINE + 1];
	FILE				*rc;
	unsigned char	*stop;
	unsigned char	*pattern;
	unsigned char	*group;
	unsigned char	*name;
	unsigned char	*space;
	unsigned char	*mask;
	int				 linenr;
	int				 havegroup;

	log_line(LOG_D, "Searching local user for number %s...\n", phone);

	printstring(temppathname, "%s/vboxgetty.user", SYSCONFDIR);

	if ((rc = fopen(temppathname, "r")))
	{
		linenr = 0;

		while (fgets(line, USERRC_MAX_RCLINE, rc))
		{
			linenr++;

			line[strlen(line) - 1] = '\0';
			
			if ((*line == '\0') || (*line == '#')) continue;

			pattern	= strtok(line, ":");
			name		= strtok(NULL, ":");
			group		= strtok(NULL, ":");
			mask		= strtok(NULL, ":");
			space		= strtok(NULL, ":");

			if ((!pattern) || (!name) || (!group) || (!mask) || (!space))
			{
				log_line(LOG_E, "Error in \"%s\" line %d.\n", temppathname, linenr);

				break;
			}

			if (fnmatch(pattern, vboxuser->localphone, 0) == 0)
			{
				log_line(LOG_D, "Found - number matchs pattern \"%s\".\n", pattern);

				if (!*name)
				{
					log_line(LOG_E, "You *must* specify a user name or a user id!\n");

					break;
				}

				if (*name == '#')
					pwdent = getpwuid((uid_t)xstrtol(&name[1], 0));
				else
					pwdent = getpwnam(name);

				if (!pwdent)
				{
					log_line(LOG_E, "Unable to locate \"%s\" in systems passwd list.\n", name);
					
					break;
				}

				vboxuser->uid = pwdent->pw_uid;
				vboxuser->gid = pwdent->pw_gid;

				if ((strlen(home) + strlen(pwdent->pw_name) + 2) < (PATH_MAX - 100))
				{
					xstrncpy(vboxuser->name, pwdent->pw_name, VBOXUSER_USERNAME);

					printstring(vboxuser->home, "%s/%s", home, pwdent->pw_name);
				}
				else
				{
					log_line(LOG_E, "Oops! Spool directory name and user name too long!\n");
					
					break;
				}

				if (*group)
				{
					havegroup = 0;

					setgrent();
					
					while ((grpent = getgrent()))
					{
						if (*group == '#')
						{
							if (grpent->gr_gid == (gid_t)xstrtol(&group[1], 0))
							{
								vboxuser->gid = grpent->gr_gid;
								havegroup	  = 1;
								
								break;
							}
						}
						else
						{
							if (strcmp(grpent->gr_name, group) == 0)
							{
								vboxuser->gid = grpent->gr_gid;
								havegroup	  = 1;
								
								break;
							}
						}
					}
					
					endgrent();

					if (!havegroup)
					{
						log_line(LOG_E, "Unable to locate \"%s\" in systems group list.\n", group);
					
						break;
					}
				}

				vboxuser->space = xstrtol(space, 0);
				vboxuser->umask = (mode_t)strtol(mask, (char **)&stop, 8);

				if (*stop != '\0')
				{
					log_line(LOG_E, "Illegal umask \"%s\" in line %d.\n", mask, linenr);
					
					break;
				}

				fclose(rc);

				log_line(LOG_D, "User \"%s\" (%d.%d) will be used...\n", vboxuser->name, vboxuser->uid, vboxuser->gid);

				return(0);
			}
		}

		fclose(rc);
	}
	else log_line(LOG_W, "Can't open \"%s\".\n", temppathname);

	return(-1);
}
