
/* $Id: conffile.h,v 1.2 1997/03/03 22:05:39 luethje Exp $
 *
 * ISDN accounting for isdn4linux.
 *
 * Copyright 1995, 1996 by Andreas Kool (akool@Kool.f.EUnet.de)
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

#ifndef _CONFFILE_H_
#define _CONFFILE_H_

/****************************************************************************/

#include "libtools.h"

/****************************************************************************/

#define C_OVERWRITE    1
#define C_WARN         2
#define C_NOT_UNIQUE   4
#define C_APPEND       8
#define C_NO_WARN_FILE 16

/****************************************************************************/

#define C_EQUAL            '='
#define C_BEGIN_SECTION    '['
#define C_END_SECTION      ']'
#define C_BEGIN_SUBSECTION '{'
#define C_END_SUBSECTION   '}'
#define C_BEGIN_INCLUDE		 '('
#define C_END_INCLUDE  		 ')'

#define S_KEY_INCLUDE      "INCLUDE"

/****************************************************************************/

#define S_ALL_QUOTE "*?|&/"

/****************************************************************************/

#define C_OR  '|'
#define C_AND '&'

/****************************************************************************/

#define R_TYPE_INT		0
#define R_TYPE_LONG		1
#define R_TYPE_DOUBLE	2
#define R_TYPE_CHAR		3
#define R_TYPE_STRING	4

/****************************************************************************/

#define F_NOT_TAGGED 0
#define F_TAGGED     1

/****************************************************************************/

#define Get_Type_Match(a,b,c,d,e,f) _Get_Type_Match(a,b,c,d,e,(void**)f)
#define Get_Type_Value(a,b,c,d)     _Get_Type_Value(a,b,c,(void**)d)

/****************************************************************************/

struct _section;

typedef struct _entry {
	char    *name;
	char    *value;
	struct _section *subsection;
	struct _entry *next;
	char   flag;
} entry;

typedef struct _section {
	char  *name;
	entry *entries;
	struct _section *next;
	char   flag;
} section;

typedef struct _cfiles {
	char* name;
	int   modtime;
} cfile;

/****************************************************************************/

#ifdef _BASE_CONFFILE_C_
#	define _EXTERN
#else
#	define _EXTERN extern
#endif

_EXTERN section *read_file(section *Section, const char *FileName, int Flags);
_EXTERN section *write_file(section *Section, const char *FileName, char *Program, char* Version);
_EXTERN section* Get_SubSection(section* Section, char *Variable);
_EXTERN section* Get_Section(section* Section, char *Sectionname);
_EXTERN entry* Get_Entry(entry* Entry, char *Variable);
_EXTERN entry *Set_Entry(section *Section, char *Sectionname, char *Variable, char *Value, int Flag);
_EXTERN section *Set_Section(section **Section, char *Sectionname, int Flag);
_EXTERN section *Del_Section(section **Section, char *Sectionname);
_EXTERN section *Set_SubSection(section *Section, char *Variable, section *SubSection, int Flag);
_EXTERN void set_print_fct_for_conffile(int (*new_print_msg)(const char *, ...));
_EXTERN void free_section(section *Ptr);
_EXTERN int read_files(section **main_sec, char** files, char ***variables, int flags);
_EXTERN int Filter_Sections(section **sec, char** path);
_EXTERN section* Get_Section_Match(section* Section, char *Path, char* Value, int  (*_match)(char*, char*), entry **RetEntry);
_EXTERN char *Get_Value(section *Section, char *Path);
_EXTERN int _Get_Type_Match(section *Section, char *Path, char* Pattern, int  (*_match)(char*, char*), int Type, void **Pointer);
_EXTERN int _Get_Type_Value(section *Section, char *Path, int Type, void **Pointer);

#undef _EXTERN

/****************************************************************************/

#endif /* _CONFFILE_H_ */
