/*
 * ISDN accounting for isdn4linux.
 *
 * Copyright 1996 by Stefan Luethje (luethje@sl-gw.lake.de)
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

#define _LIB_TOOLS_C_

/****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <fnmatch.h>
#include <ctype.h>

#include "libtools.h"

/****************************************************************************/

static int  (*print_msg)(const char *, ...) = printf;

/****************************************************************************/

void set_print_fct_for_libtools(int (*new_print_msg)(const char *, ...))
{
	print_msg = new_print_msg;
}

/****************************************************************************/

char *Not_Space(char *String)
{
	while (isspace(*String)) String++;

	if (*String == '\0')
		String = NULL;

	return String;
}

/****************************************************************************/

char *To_Upper (char *String)
{
	char *Ptr = String;

	while (*Ptr != '\0')
	{
		*Ptr = toupper(*Ptr);
		Ptr++;
	}

	return String;
}

/****************************************************************************/

char *Kill_Blanks(char *String)
{
	int Cnt1 = 0, Cnt2 = 0;


	while (String[Cnt1] != '\0')
	{
		if (!isspace(String[Cnt1]))
			String[Cnt2++] = String[Cnt1];

		Cnt1++;
	}

	String[Cnt2] = '\0';

	return String;
}

/****************************************************************************/

char *FGets(char *String, int StringLen, FILE *fp, int *Line)
{
	int Len;
	char *RetCode = NULL;
	char *Ptr = NULL;
	char *Help = NULL;


	if ((Help = (char*) alloca(StringLen*sizeof(char))) == NULL)
		return NULL;

	*String ='\0';

	while ((RetCode = fgets(Help, StringLen - strlen(String), fp)) != NULL)
	{
		(*Line)++;

		if ((Len = strlen(Help)) > 0 && Help[Len-1]  == '\n' )
			Help[Len-1] = '\0';

		if ((Ptr = Check_Quote(Help,S_COMMENT,QUOTE_DELETE)) != NULL)
			*Ptr = '\0';

		strcat(String,Help);

		if ((Len = strlen(String)) > 0 && String[Len-1]  == '\\' )
			String[Len-1] = '\0';
		else
			break;
	}

	Ptr = String;
	while (isspace(*Ptr)) Ptr++;

	if (*Ptr =='\0' && RetCode != NULL)
		RetCode = FGets(String,StringLen,fp,Line);
	else
	{
		strcpy(Help,Ptr);
		strcpy(String,Help);
	}

	return RetCode;
}

/****************************************************************************/

char *Check_Quote(char *String, char *Quote, int Flag)
{
  char *Ptr = String;
  char *Ptr2;

  if (Ptr == NULL)
    return NULL;

  while ((Ptr = Strpbrk(Ptr,Quote)) != NULL)
  {
  	if (Ptr != String && *(Ptr - 1) == '\\')
  	{
      if (Flag == QUOTE_IGNORE)
      {
      	Ptr++;
      }
      else
      if (Flag == QUOTE_DELETE)
      {
      	Ptr2 = Ptr - 1;

        do
          *Ptr2 = *(Ptr2 + 1);
        while(*Ptr2++ != '\0');
      }
      else
        return NULL;
    }
    else
    	break;
  }

  return Ptr;
}

/****************************************************************************/

const char* Quote_Chars(char *string)
{
	int i;
	int Index = 0;
  static char *Ptr = NULL;
  char *Help = NULL;

	if (Ptr != NULL)
		free(Ptr);

	Ptr = strdup(string);

	if (string == NULL)
	{
		print_msg("%s!\n","Got a NULL string!\n");
		return "";
	}

	while((Help = Strpbrk(Ptr+Index,S_COMMENT)) != NULL)
	{
		Index = Help - Ptr;

		if ((Ptr = (char*) realloc(Ptr,(strlen(Ptr)+2)*sizeof(char))) == NULL)
		{
			print_msg("%s!\n","Can not alllocate memory!\n");
			return "";
		}

		for (i = strlen(Ptr); i >= Index; i--)
			Ptr[i+1] = Ptr[i];

		Ptr[Index] =  '\\';

		Index += 2;
	}

	return Ptr;
}

/****************************************************************************/

/* Als Workaround fuer strpbrk():  seltsame Abstuerze ;-(  */

char *Strpbrk( const char* Str, const char* Accept )
{
	int i;
	int len;
	int first = -1;
	int helpfirst = -1;
	int pos = -1;

	char *RetCode = NULL;


	if (Str == NULL || Accept == NULL)
		return NULL;

	len = strlen(Accept);

	for (i = 0; i < len; i++)
	{
		if ((RetCode = strchr(Str,Accept[i])) != NULL)
		{
			if ((helpfirst = RetCode - Str) < first || first == -1)
			{
				pos = i;
				first = helpfirst;
			}
		}
	}

	if (pos == -1)
		return NULL;
	else
		RetCode = (char*) (Str + first);

	/* Alte Zeile
	while (*Accept != '\0' && (RetCode = strchr(Str,*Accept++)) == NULL);
	*/

	return RetCode;
}

/****************************************************************************/

char **String_to_Array(char* String, char Trenn)
{
	char  *Ptr     = String;
	char **RetCode = NULL;
	char  *Ptr1    = NULL;
	char  *Ptr2    = NULL;
	int Cnt = 2;

	if (*String == '\0')
		return NULL;

	while((Ptr = strchr(Ptr,Trenn)) != NULL)
	{
		Cnt++;
		Ptr++;
	}

	RetCode = (char**) calloc(Cnt,sizeof(char*));
	Ptr     = strdup(String);

	if (RetCode == NULL || Ptr == NULL)
		return NULL;

	Cnt = 0;
	Ptr1 = Ptr;

	do
	{
		Ptr2 = strchr(Ptr1,Trenn);

		if (Ptr2)
			*Ptr2++ = '\0';

		RetCode[Cnt] = (char*) calloc(strlen(Ptr1)+1,sizeof(char));

		if (RetCode == NULL)
			return RetCode;

		strcpy(RetCode[Cnt++],Ptr1);
		Ptr1 = Ptr2;
	}
	while (Ptr1 != NULL);

	free(Ptr);
	return RetCode;
}

/****************************************************************************/

void del_Array(char **Ptr)
{
	int Cnt = 0;

	if (Ptr)
	{
		while (Ptr[Cnt])
			free(Ptr[Cnt++]);

		free (Ptr);
	}
}

/****************************************************************************/

int _append_element(void ***elem, void *ins)
{
	int num = 2;
	void **Ptr = *elem;


	if (Ptr != NULL)
		while(*Ptr != NULL)
			num++,Ptr++;

	if ((*elem = (void**) realloc(*elem,sizeof(void*)*num)) == NULL)
	{
		print_msg("Can not alloc memory!\n");
		return -1;
	}

	(*elem)[num-2] = ins;
	(*elem)[num-1] = NULL;
	return 0;
}

/****************************************************************************/

int _delete_element(void ***elem, int deep)
{
	void **Ptr = *elem;

	if (deep < 0)
		return -1;

	if (Ptr == NULL)
		return 0;

	if (deep != 0)
		while(*Ptr != NULL)
			delete_element(Ptr++,deep-1);

	free(*elem);
	*elem = NULL;
	
	return 0;
}

/****************************************************************************/

int match(register char *p, register char *s, int flags)
{
	char *ptr1;
	char *ptr2;

	if ((flags & F_IGNORE_CASE) || (flags & F_NO_HOLE_WORD))
	{
		if ((flags & F_IGNORE_CASE) && !(flags & F_NO_HOLE_WORD) &&
		    !strcasecmp(p,s)                                       )
			return 0;

		if ((ptr1 = (char*) alloca((strlen(p)+3)*sizeof(char))) == NULL)
			return -1;

		if ((ptr2 = (char*) alloca((strlen(s)+3)*sizeof(char))) == NULL)
			return -1;

		strcpy(ptr1,p);
		strcpy(ptr2,s);

		if (flags & F_IGNORE_CASE)
		{
			To_Upper(ptr1);
			To_Upper(ptr2);
		}

		if (flags & F_NO_HOLE_WORD)
		{
			char string[SHORT_STRING_SIZE];

			sprintf(string,"*%s*",ptr1);
			strcpy(ptr1,string);
		}

		flags &= ~F_IGNORE_CASE;
		flags &= ~F_NO_HOLE_WORD;

		p = ptr1;
		s = ptr2;
	}
	else
		if (!strcmp(p,s))
			return 0;

#ifdef OWN_MATCH
	return _match(p,s);
#else
	return fnmatch(p,s,flags);
#endif
} /* match */

/****************************************************************************/

#ifdef OWN_MATCH
int _match(char* p,char* s)
{
  register int sc, pcc;


  if (!*s || !*p)
    return(-1);

  while ((pcc = *p++ & QCMASK)) {
    sc = *s++ & QMASK;

    switch (pcc) {
      case '[' : if (!(p = cclass(p, sc)))
	           return(-1);
	         break;

      case '?' : if (!sc)
	           return(-1);
	         break;

      case '*' : s--;
	         do {
	           if (!*p || !_match(p, s))
	             return(0);
	         } while (*s++);
	         return(-1);

      default  : if (sc != (pcc &~QUOTE))
	           return(-1);
    }
  }

  return(*s);
} /* _match */
#endif

/****************************************************************************/

int is_double (char *string, double *value)
{
	double dummy2 = 0;
	char* dummy;

	if (*string == '\0')
		return 1;

	dummy = (char*) alloca((strlen(string)+1)*sizeof(char));

	if (value == NULL)
		*value = dummy2;

	return (sscanf(string,"%lf%s",value,dummy) == 1);
}

/****************************************************************************/

int is_integer (char *string, long int *value)
{
	long int dummy2 = 0;
	char* dummy;

	if (*string == '\0')
		return 1;

	dummy = (char*) alloca((strlen(string)+1)*sizeof(char));

	if (value == NULL)
		*value = dummy2;

	return (sscanf(string,"%ld%s",value,dummy) == 1);
}

/****************************************************************************/

