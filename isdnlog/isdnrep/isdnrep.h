/* $Id: isdnrep.h,v 1.15 1999/07/12 11:37:37 calle Exp $
 *
 * ISDN accounting for isdn4linux.
 *
 * Copyright 1995, 1999 by Andreas Kool (akool@isdn4linux.de)
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
 * Revision 1.15  1999/07/12 11:37:37  calle
 * Bugfix: isdnrep defined print_msg as function pointer, the object files
 *         in tools directory, declare it as external function.
 * 	compiler and linker did not detect the problem.
 * 	Now print_msg is a function in rep_main.c and I copied
 * 	print_in_modules from isdnconf. Also set_print_fct_for_isdnrep
 * 	is removed from isdnrep.c. isdnrep didn´t crash now, but throw
 * 	out warning messages about rate.dat and did´t generate output.
 *
 * Revision 1.14  1999/05/04 19:33:19  akool
 * isdnlog Version 3.24
 *
 *  - fully removed "sondernummern.c"
 *  - removed "gcc -Wall" warnings in ASN.1 Parser
 *  - many new entries for "rate-de.dat"
 *  - better "isdnconf" utility
 *
 * Revision 1.13  1999/03/24 19:38:41  akool
 * - isdnlog Version 3.10
 * - moved "sondernnummern.c" from isdnlog/ to tools/
 * - "holiday.c" and "rate.c" integrated
 * - NetCologne rates from Oliver Flimm <flimm@ph-cip.uni-koeln.de>
 * - corrected UUnet and T-Online rates
 *
 * Revision 1.12  1999/01/24 19:02:25  akool
 *  - second version of the new chargeint database
 *  - isdnrep reanimated
 *
 * Revision 1.11  1998/11/24 20:52:46  akool
 *  - changed my email-adress
 *  - new Option "-R" to supply the preselected provider (-R24 -> Telepassport)
 *  - made Provider-Prefix 6 digits long
 *  - full support for internal S0-bus implemented (-A, -i Options)
 *  - isdnlog now ignores unknown frames
 *  - added 36 allocated, but up to now unused "Auskunft" Numbers
 *  - added _all_ 122 Providers
 *  - Patch from Jochen Erwied <mack@Joker.E.Ruhr.DE> for Quante-TK-Anlagen
 *    (first dialed digit comes with SETUP-Frame)
 *
 * Revision 1.10  1998/03/29 19:54:17  luethje
 * idnrep: added html feature (incoming/outgoing calls)
 *
 * Revision 1.9  1998/03/08 11:43:08  luethje
 * I4L-Meeting Wuerzburg final Edition, golden code - Service Pack number One
 *
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
#include <holiday.h>
#include <rate.h>

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

#ifdef _REP_FUNC_C_
#define _EXTERN
#define _SET_NULL   = NULL
#define _SET_0      = 0
#define _SET_1      = 1
#define _SET_33	    = 33
#define _SET_EMPTY  = ""
#else
#define _EXTERN extern
#define _SET_NULL
#define _SET_0
#define _SET_1
#define _SET_33
#define _SET_EMPTY
#define _SET_FILE
#endif

_EXTERN int read_logfile(char *myname);
_EXTERN int get_term (char *String, time_t *Begin, time_t *End,int delentries);
_EXTERN int set_msnlist(char *String);
_EXTERN int send_html_request(char *myname, char *option);
_EXTERN int new_args(int *nargc, char ***nargv);

_EXTERN int     print_msg(int Level, const char *, ...);
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
_EXTERN int     preselect	_SET_33;


#undef _SET_NULL
#undef _SET_0
#undef _SET_1
#undef _SET_EMPTY
#undef _EXTERN

/*****************************************************************************/

#define LOG_VERSION_1 "1.0"
#define LOG_VERSION_2 "2.0"
#define LOG_VERSION_3 "3.0"
#define LOG_VERSION_4 "3.1"

/*****************************************************************************/

#define C_DELIM '|'

/*****************************************************************************/


/*****************************************************************************/

typedef struct {
  char   num[NUMSIZE];
  char   mynum[NUMSIZE];
  int   si1;
  int	 called;
  int	 connects;
  time_t connect[MAXCONNECTS];
  int    cause;
} UNKNOWNS;

/*****************************************************************************/

#endif /* _ISDNREP_H_ */
