/* $Id: isdnrep.h,v 1.8 1997/05/15 23:24:56 luethje Exp $
 *
 * ISDN accounting for isdn4linux.
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
 * $Log: isdnrep.h,v $
 * Revision 1.8  1997/05/15 23:24:56  luethje
 * added new links on HTML
 *
 * Revision 1.7  1997/05/15 22:21:40  luethje
 * New feature: isdnrep can transmit via HTTP fax files and vbox files.
 *
 * Revision 1.6  1997/04/20 22:52:28  luethje
 * isdnrep has new features:
 *   -variable format string
 *   -can create html output (option -w1 or ln -s isdnrep isdnrep.cgi)
 *    idea and design from Dirk Staneker (dirk.staneker@student.uni-tuebingen.de)
 * bugfix of processor.c from akool
 *
 * Revision 1.5  1997/04/16 22:23:00  luethje
 * some bugfixes, README completed
 *
 * Revision 1.4  1997/04/03 22:30:03  luethje
 * improved performance
 *
 * Revision 1.3  1997/03/24 22:52:14  luethje
 * isdnrep completed.
 *
 */

#ifndef _ISDNREP_H_
#define _ISDNREP_H_

#define PUBLIC extern
#include <tools.h>

/*****************************************************************************/

#ifdef  MAXUNKNOWN
#undef  MAXUNKNOWN
#endif
#define MAXUNKNOWN   500

#ifdef  MAXCONNECTS
#undef  MAXCONNECTS
#endif
#define MAXCONNECTS  500

/*****************************************************************************/

#define H_PRINT_HTML   1
#define H_PRINT_HEADER 2

/*****************************************************************************/

#ifdef _CHEAP_C_
#define _EXTERN
#else
#define _EXTERN extern
#endif

_EXTERN double cheap96(time_t when, int zone, int *zeit);
_EXTERN double cheap(time_t when, int zone);

#undef _EXTERN

/*****************************************************************************/

#ifdef _REP_FUNC_C_
#define _EXTERN
#define _SET_NULL   = NULL
#define _SET_0      = 0
#define _SET_1      = 1
#define _SET_EMPTY  = ""
#else
#define _EXTERN extern
#define _SET_NULL
#define _SET_0
#define _SET_1
#define _SET_EMPTY
#define _SET_FILE
#endif

_EXTERN int read_logfile(char *myname);
_EXTERN void set_print_fct_for_isdnrep(int (*new_print_msg)(int Level, const char *, ...));
_EXTERN int get_term (char *String, time_t *Begin, time_t *End,int delentries);
_EXTERN int set_msnlist(char *String);
_EXTERN int send_html_request(char *myname, char *option);

_EXTERN int     (*print_msg)(int Level, const char *, ...) _SET_NULL;
_EXTERN int     incomingonly    _SET_0;
_EXTERN int     outgoingonly    _SET_0;
_EXTERN int     verbose         _SET_0;
_EXTERN int     timearea        _SET_0;
_EXTERN int     phonenumberonly _SET_0;
_EXTERN int     compute         _SET_0;
_EXTERN int     delentries      _SET_0;
_EXTERN int     numbers         _SET_0;
_EXTERN int     html         		_SET_0;
_EXTERN int     seeunknowns  		_SET_0;
_EXTERN int     header          _SET_1;
_EXTERN char	  timestring[256] _SET_EMPTY;
_EXTERN char	  *lineformat     _SET_NULL;
_EXTERN time_t  begintime       _SET_0;
_EXTERN time_t  endtime         _SET_0;

#undef _SET_NULL
#undef _SET_0
#undef _SET_1
#undef _SET_EMPTY
#undef _EXTERN

/*****************************************************************************/

#define LOG_VERSION_1 "1.0"
#define LOG_VERSION_2 "2.0"
#define LOG_VERSION_3 LOG_VERSION

/*****************************************************************************/

#define C_DELIM '|'

/*****************************************************************************/


/*****************************************************************************/

typedef struct {
  char   num[NUMSIZE];
  int	 called;
  int	 connects;
  time_t connect[MAXCONNECTS];
} UNKNOWN;

/*****************************************************************************/

#endif /* _ISDNREP_H_ */
