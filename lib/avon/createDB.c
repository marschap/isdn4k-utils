/* $Id: createDB.c,v 1.3 1997/05/19 22:58:34 luethje Exp $
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
 * $Log: createDB.c,v $
 * Revision 1.3  1997/05/19 22:58:34  luethje
 * - bugfix: it is possible to install isdnlog now
 * - improved performance for read files for vbox files and mgetty files.
 * - it is possible to decide via config if you want to use avon or
 *   areacode.
 *
 * Revision 1.2  1997/04/17 23:29:50  luethje
 * new structure of isdnrep completed.
 *
 */

/****************************************************************************/

#define  _CREATEDB_C_

/****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "createDB.h"

/****************************************************************************/

#define FILEMODE 0664

/****************************************************************************/

static int      ln = 0;

/****************************************************************************/

static int  (*print_msg)(const char *, ...) = printf;

/****************************************************************************/

void set_print_fct_for_avon(int (*new_print_msg)(const char *, ...))
{
	print_msg = new_print_msg;
}

/****************************************************************************/

int createDB(char *fn, int force)
{
  register int         f;
  auto 	   char        s[BUFSIZ];
  auto 	   struct stat statbuf;


  sprintf(s, "%s.dir", fn);

  if (force || stat(s, &statbuf)) {
    if ((f = open(s, O_CREAT | O_TRUNC | O_RDWR, FILEMODE)) >= 0) {
      close(f);

      sprintf(s, "%s.pag", fn);

      if ((f = open(s, O_CREAT | O_TRUNC | O_RDWR, FILEMODE)) >= 0) {
        close(f);

				if(!openDB(fn, O_RDWR | O_CREAT))
				{
					readAVON(fn);
					closeDB();
					return 0;
				}
      } /* if */
    	else
    		print_msg("Can not open file `%s': %s!\n",s,strerror(errno));
    } /* if */
    else
    	print_msg("Can not open file `%s': %s!\n",s,strerror(errno));

  } /* if */

  return(-1);
} /* createDB */

/******************************************************************************/

int openDB(char *fn, int flag)
{
  dbm = dbm_open(fn, flag, FILEMODE);
  return((dbm == (DBM *)NULL) ? -1 : 0);
} /* dbminit */

/******************************************************************************/

void closeDB()
{
  if (dbm != (DBM *)NULL) {
    dbm_close(dbm);
    dbm = (DBM *)NULL;
  } /* if */
} /* dbmclose */

/******************************************************************************/

void insertDB(char *num, char *area)
{
  key.dsize = strlen(num);
  key.dptr  = num;

  data.dsize = strlen(area) + 1;
  data.dptr = area;

  if (dbm_store(dbm, key, data, DBM_INSERT)) {
    print_msg( "\navon: WARNING: Duplicate Entry ``%s'' in AVON in Line %d, ignored\n", num, ln);

    data = dbm_fetch(dbm, key);

    print_msg( "avon: WARNING: Can't be ``%s'' and ``%s''!\n", area, data.dptr);
    print_msg( "        <- Reading AVON, please wait ...");
  } /* if */
} /* insertDB */

/******************************************************************************/

#ifdef DEVEL
void writeAVON()
{
  auto 	   char  s[BUFSIZ];
  auto 	   FILE *f;


  sprintf(s, "%s%c%s.new", confdir(), C_SLASH, AVON);

  if ((f = fopen(s, "w")) != (FILE *)NULL) {
    for (key = dbm_firstkey(dbm); key.dptr != NULL; key = dbm_nextkey(dbm)) {
      strcpy(s, key.dptr);
      *(s + key.dsize) = 0;
      data = dbm_fetch(dbm, key);
      fprintf(f, "%s:%s\n", s, data.dptr);
    } /* for */

    fclose(f);
  } /* if */
} /* writeAVON */
#endif

/******************************************************************************/

void readAVON(char *fn)
{
  register char *p, *p1;
  auto 	   char  s[BUFSIZ];
  auto 	   FILE *f;


  ln = 0;

  if ((f = fopen(fn, "r")) != (FILE *)NULL) {

    print_msg( "        <- Reading AVON, please wait ...");

    while (fgets(s, BUFSIZ, f) != NULL) {

      ln++;

      if ((*s != '#') && (*s != '\n')) {

        if ((p = strchr(s, '\n'))) {
          *p = 0;

          if (*(p - 1) == '\r') /* I *hate* DOS! */
            *(--p) = 0;

          while (*--p == ' ')
            *p = 0;
        } /* if */

        if ((p = strchr(s, ':'))) { /* "avon.txt" Format */
          *p = 0;

          if (!(ln % 50))
            print_msg( "\r%s", s);

          insertDB(s, p + 1);
        }
        else if (*s == 'D') { /* "avon.uisdn" Format */
          if ((p = p1 = strchr(s, '+'))) {
            p -= 7;
            *p = 0;

            if ((p = strchr(p1, '-')))
              *p = 0;

            if (!(ln % 50))
              print_msg( "\r%s", p1 + 4);

            while ((p = strchr(s + 6, '_')))
              *p = ' ';

  	    insertDB(p1 + 4, s + 6);
          } /* if */
        }
        else
          print_msg( "avons: WARNING: Syntax error in AVON at Line %d, ignored\n", ln);
      } /* if */
    } /* while */

    fclose(f);
    print_msg( "\n");
  } /* if */
} /* readAVON */

/******************************************************************************/

