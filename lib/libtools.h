
/* $Id: libtools.h,v 1.1 1997/03/03 04:23:17 fritz Exp $
 *
 * ISDN accounting for isdn4linux.
 *
 * Copyright 1997 by Stefan Luethje (luethje@sl-gw.lake.de)
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
 */

#ifndef _LIB_TOOLS_H_
#define _LIB_TOOLS_H_

#include <linux/limits.h>

/****************************************************************************/

extern char *basename __P((__const char *__name));

/****************************************************************************/

#define CONFDIR_VAR "ISDN"
#define C_SLASH '/'
/* #define C_SLASH '\\' */

#ifndef TMPDIR
# define TMPDIR P_tmpdir
#endif

#ifndef I4LCONFDIR
# define I4LCONFDIR "/etc/isdn"
#endif

#ifndef RUNDIR
# define RUNDIR "/var/run"
#endif

#ifndef CONFFILE
# define CONFFILE "isdn.conf"
#endif

#ifndef USERCONFFILE
#	define USERCONFFILE "~/.isdn"
#endif

/****************************************************************************/

#define SHORT_STRING_SIZE      256
#define LONG_STRING_SIZE      1024
#define BUF_SIZE              4096

/****************************************************************************/

#define C_COMMENT '#'
#define S_COMMENT "#"

#define QUOTE_IGNORE 0
#define QUOTE_DELETE 1

/****************************************************************************/

#define F_IGNORE_CASE		1024
#define F_NO_HOLE_WORD	2048

/****************************************************************************/

#define append_element(a,b) _append_element((void***) a,(void**) b)
#define delete_element(a,b) _delete_element((void***) a, b)

/****************************************************************************/

#ifdef _LIB_TOOLS_C_
#define _EXTERN
#else
#define _EXTERN extern
#endif

_EXTERN void set_print_fkt_for_libtools(int (*new_print_msg)(const char *, ...));
_EXTERN char *Not_Space(char *String);
_EXTERN char *To_Upper (char *String);
_EXTERN char *Kill_Blanks(char *String);
_EXTERN char *FGets(char *String, int StringLen, FILE *fp, int *Line);
_EXTERN char *Check_Quote(char *String, char *Quote, int Flag);
_EXTERN const char* Quote_Chars(char *string);
_EXTERN char *Strpbrk( const char* Str, const char* Accept );
_EXTERN char **String_to_Array(char* String, char Trenn);
_EXTERN void del_Array(char **Ptr);
_EXTERN int _append_element(void ***elem, void *ins);
_EXTERN int _delete_element(void ***elem, int deep);
_EXTERN int match(register char *p, register char *s, int flags);
_EXTERN int is_double (char *string, double *value);
_EXTERN int is_integer (char *string, long int *value);

#undef _EXTERN

/****************************************************************************/

#endif
