/* $Id: country.c,v 1.1 1999/05/27 18:19:57 akool Exp $
 *
 * Länderdatenbank
 *
 * Copyright 1995, 1999 by Andreas Kool (akool@isdn4linux.de)
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
 * $Log: country.c,v $
 * Revision 1.1  1999/05/27 18:19:57  akool
 * first release of the new country decoding module
 *
 */

/*
 * Schnittstelle zur Länderdatenbank:
 *
 * void exitCountry(void)
 *   deinitialisiert die Länderdatenbank
 *
 * void initCountry(char *path, char **msg)
 *   initialisiert die Länderdatenbank
 *
 */

#define _COUNTRY_C_

#ifdef STANDALONE
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#if 0
extern const char *basename (const char *name);
#else
#include <errno.h>
#endif
#else
#include "isdnlog.h"
#include "tools.h"
#endif
#include "country.h"

#define LENGTH 10240  /* max length of lines in data file */
#ifdef STANDALONE
#define UNKNOWN -1
#endif


static COUNTRY *Country = NULL;
static int      nCountry = 0;
static int      line=0;


static void warning (char *file, char *fmt, ...)
{
  va_list ap;
  char msg[BUFSIZ];

  va_start (ap, fmt);
  vsnprintf (msg, BUFSIZ, fmt, ap);
  va_end (ap);
#ifdef STANDALONE
  fprintf(stderr, "WARNING: %s line %3d: %s\n", basename(file), line, msg);
#else
  print_msg(PRT_NORMAL, "WARNING: %s line %3d: %s\n", basename(file), line, msg);
#endif
}

static char *strip (char *s)
{
  char *p;

  while (isblank(*s)) s++;
  for (p=s; *p; p++)
    if (*p=='#' || *p=='\n') {
      *p='\0';
      break;
    }
  for (p--; p>s && isblank(*p); p--)
    *p='\0';
  return s;
}

static char* str2set (char **s)
{
  static char buffer[BUFSIZ];
  char *p=buffer;

  while (**s) {
    if (**s==',')
      break;
    else
      *p++=*(*s)++;
  }
  *p = '\0';
  return buffer;
}

static char *xlat (char *s)
{
  static char buffer[BUFSIZ];
  char *p = buffer;

  do {
    *p=tolower(*s);
    switch (*p) {
    case 'ä':
      *p++='a';
      *p  ='e';
      break;
    case 'ö':
      *p++='o';
      *p  ='e';
      break;
    case 'ü':
      *p++='u';
      *p  ='e';
      break;
    case 'ß':
      *p++='s';
      *p  ='s';
      break;
    }
    p++;
  } while (*s++);
  return buffer;
}

static int min3(int x, int y, int z)
{
  if (x < y)
    y = x;
  if (y < z)
    z = y;
  return(z);
}

#define WMAX 64
#define P     1
#define Q     1
#define R     1

static int wld(char *needle, char *haystack) /* weighted Levenshtein distance */
{
  int i, j;
  int l1 = strlen(needle);
  int l2 = strlen(haystack);
  int dw[WMAX+1][WMAX+1];

  dw[0][0]=0;

  for (j=1; j<=WMAX; j++)
    dw[0][j]=dw[0][j-1]+Q;

  for (i=1; i<=WMAX; i++)
    dw[i][0]=dw[i-1][0]+R;

  for (i=1; i<=l1; i++)
    for(j=1; j<=l2; j++)
      dw[i][j]=min3(dw[i-1][j-1]+((needle[i-1]==haystack[j-1])?0:P),dw[i][j-1]+Q,dw[i-1][j]+R);

  return(dw[l1][l2]);
}

static COUNTRY *findCountry (char *name)
{
  char *xname;
  int   i, j;

  if (*name=='+') {
    for (i=0; i<nCountry; i++) {
      for (j=0; j<Country[i].nCode; j++) {
	if (strcmp (name, Country[i].Code[j])==0)
	  return &Country[i];
      }
    }
  } else {
    xname=xlat(name);
    for (i=0; i<nCountry; i++) {
      if (strcmp (name, Country[i].Name)==0)
	return &Country[i];
      for (j=0; j<Country[i].nAlias; j++) {
	if (strcmp (xname, Country[i].Alias[j])==0)
	  return &Country[i];
      }
    }
  }
  return NULL;
}

void exitCountry(void)
{
  int i, j;

  for (i=0; i<nCountry; i++) {
    if (Country[i].Name) free (Country[i].Name);
    for (j=0; j<Country[i].nAlias; j++)
      if (Country[i].Alias[j]) free (Country[i].Alias[j]);
    for (j=0; j<Country[i].nCode; j++)
      if (Country[i].Code[j]) free (Country[i].Code[j]);
  }
  if (Country) free (Country);
  Country=NULL;
  nCountry=0;
}

int initCountry(char *path, char **msg)
{
  FILE    *stream;
  COUNTRY *c;
  int      Aliases=0, Codes=0, Index=-1;
  char    *s, *n, *x;
  char     buffer[LENGTH];
  char     version[LENGTH]="<unknown>";
  static char message[LENGTH];

  if (msg)
    *(*msg=message)='\0';

  exitCountry();

  if (!path || !*path) {
    if (msg) snprintf (message, LENGTH, "Warning: no country database specified!");
    return 0;
  }

  if ((stream=fopen(path,"r"))==NULL) {
    if (msg) snprintf (message, LENGTH, "Error: could not load countries from %s: %s",
		       path, strerror(errno));
    return -1;
  }

  line=0;
  while ((s=fgets(buffer,LENGTH,stream))!=NULL) {
    line++;
    if (*(s=strip(s))=='\0')
      continue;
    if (s[1]!=':') {
      warning (path, "expected ':', got '%s'!", s+1);;
      continue;
    }
    switch (*s) {
    case 'N': /* N:Name */
      s+=2; while (isblank(*s)) s++;
      if ((c=findCountry(s))!=NULL) {
	warning (path, "'%s' is an alias for '%s'", s, c->Name);
      }
      Index++;
      nCountry++;
      Country=realloc(Country, nCountry*sizeof(COUNTRY));
      Country[Index].Name=strdup(s);
      Country[Index].nCode=0;
      Country[Index].Code=NULL;
      Country[Index].nAlias=1; /* translated name ist 1st alias */
      Country[Index].Alias=malloc(sizeof(char*));
      Country[Index].Alias[0]=strdup(xlat(s));
      break;

    case 'A': /* A:Alias[,Alias...] */
      if (Index<0) {
	warning (path, "Unexpected tag '%c'", *s);
	break;
      }
      s+=2;
      while(1) {
	if (*(n=strip(str2set(&s)))) {
	  x=xlat(n);
	  if ((c=findCountry(x))!=NULL) {
	    warning (path, "duplicate entry '%s', ignoring", n);
	  } else {
	    Country[Index].Alias=realloc (Country[Index].Alias, (Country[Index].nAlias+1)*sizeof(char*));
	    Country[Index].Alias[Country[Index].nAlias]=strdup(x);
	    Country[Index].nAlias++;
	    Aliases++;
	  }
	} else {
	  warning (path, "Ignoring empty alias");
	}
	if (*s==',') {
	  s++;
	  continue;
	}
	break;
      }
      break;

    case 'C': /* C: Code[,Code...] */
      if (Index<0) {
	warning (path, "Unexpected tag '%c'", *s);
	break;
      }
      s+=2;
      while(1) {
	if (*(n=strip(str2set(&s)))) {
	  if (*n!='+') {
	    warning (path, "code should start with '+', ignoring", n);
	  } else if ((c=findCountry(n))!=NULL) {
	    warning (path, "duplicate entry '%s', ignoring", n);
	  } else {
	    Country[Index].Code=realloc (Country[Index].Code, (Country[Index].nCode+1)*sizeof(char*));
	    Country[Index].Code[Country[Index].nCode]=strdup(n);
	    Country[Index].nCode++;
	    Codes++;
	  }
	} else {
	  warning (path, "Ignoring empty code");
	}
	if (*s==',') {
	  s++;
	  continue;
	}
	break;
      }
      break;

    case 'V': /* V:xxx Version der Datenbank */
      s+=2; while(isblank(*s)) s++;
      strcpy(version, s);
      break;

    default:
      warning(path, "Unknown tag '%c'", *s);
    }
  }
  fclose(stream);

  if (msg) snprintf (message, LENGTH, "Country Version %s loaded [%d countries, %d aliases, %d codes from %s]",
		     version, nCountry, Aliases, Codes, path);

  return nCountry;
}

#if 0
static int countrymatch(char *name, char *num)
{
  register int 	 i, test = (num == NULL);
  auto	   char	 k[BUFSIZ];


  strcpy(k, name);
  down(k);

  for (i = 0; i < nCountry; i++)
    if ((test || !strncmp(Country[i].prefix, num, strlen(Country[i].prefix))) && !strncmp(Country[i].match, k, strlen(Country[i].match)))
      return(i);

  for (i = 0; i < nCountry; i++)
    if ((test || !strncmp(Country[i].prefix, num, strlen(Country[i].prefix))) && strstr(Country[i].match, k))
      return(i);

  for (i = 0; i < nCountry; i++)
    if ((test || !strncmp(Country[i].prefix, num, strlen(Country[i].prefix))) && strstr(Country[i].hints, k))
      return(i);

  for (i = 0; i < nCountry; i++)
    if ((test || !strncmp(Country[i].prefix, num, strlen(Country[i].prefix))) && (wld(k, Country[i].match) <= DISTANCE))
      return(i);

  return(0);
} /* countymatch */


/*  INPUT: "+372"
   OUTPUT: "Estland"

    INPUT: "Estland"
   OUTPUT: "+372"
*/

int abroad(char *key, char *result)
{
  register int   i;
  auto int mode, match = 0, res = 0;
  auto 	   char  k[BUFSIZ];


  *result = 0;

  if (!memcmp(key, countryprefix, strlen(countryprefix)))  /* +xxx */
    mode = 1;
  else {   	   		  			   /* "Estland" */
    mode = 2;

    strcpy(k, key);
    down(k);
  } /* else */

  if (mode == 1) {
    for (i = 0; i < nCountry; i++) {
      res = strlen(Country[i].prefix);
      match = !strncmp(Country[i].prefix, key, res);

      if (match)
        break;
    } /* for */
  }
  else { /* mode == 2 */
    res = 1;

    for (i = 0; i < nCountry; i++)
      if ((match = !strncmp(Country[i].match, k, strlen(Country[i].match))))
        break;

    if (!match)
      for (i = 0; i < nCountry; i++)
        if ((match = (strstr(Country[i].match, k) != NULL)))
          break;

    if (!match)
      for (i = 0; i < nCountry; i++)
        if ((match = (strstr(Country[i].hints, k) != NULL)))
          break;

#if 0
    if (!match)
      for (i = 0; i < nCountry; i++)
        if ((match = (wld(k, Country[i].match) <= DISTANCE)))
          break;
#endif

  } /* else */

  if (match) {
    if (mode == 1)
      strcpy(result, Country[i].name);
    else
      strcpy(result, Country[i].prefix);

    return(res);
  } /* if */

  return(0);
} /* abroad */

#endif

#ifdef STANDALONE
void main (int argc, char *argv[])
{
  char *msg;

  initCountry ("../prefixes.dat", &msg);
  printf ("%s\n", msg);

}
#endif
