#define TESTat

/* telnum.c
 * (c) 1999 by Leopold Toetsch <lt@toetsch.at>
 *
 * telefon number utils
 *
 * Copyright 1999 by Leopold Toetsch <lt@toetsch.at>
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
 *
 * Interface
 * 
 * void initTelNum(void)
 * ---------------------
 * init the package, call this once on startup
 *
 * int normalizeNumber(char *number, TELNUM *num, int flag)
 * --------------------------------------------------------
 *	number is "[provider] [country] [area] [msn]"
 *	country may be name of country
 *	spaces or '_' are allowd to separate parts of number
 *	fills struct TELNUM *num with parts of number
 *  for flag look at telnum.h
 * 	ret 0 .. ok
 *	ret 1 .. area not found
 *	ret UNKNOWN .. country not found
 *
 * 	Input number e.g.
 * 	10055 0049 89 1234567
 * 	100550049891234567
 *  1033 Deutschland 89 1234567
 *  +49 89 1234567
 *	089 1234567
 *  1234567
 *
 * char * formatNumber(char* format, TELNUM* num)
 * ----------------------------------------------
 *	takes a format string
 *	%Np .. Provider
 *	%Nc .. country +49
 *	%NC .. countryname
 *	%Na .. area 89 or 089 if no country/Provider
 *	%NA .. areaname
 *	%Nm .. msn
 *  %f .. full +49 89 12356 (Deutschland, Berlin)
 *	%s .. short +48 89 123456
 * 	%n .. number 004889123456
 *
 *  N is number of chars to skip in format if part is not present
 *  e.g. "%1c %1a %m"
 *	+4930123 => "+49 39 123"
 *  123 => "123" not "  123"
 *
 *
 * char* prefix2provider(int prefix, char* provider, TELNUM *num)
 * --------------------------------------------------------------
 *  returns formatted provider for prefix
 *
 * int provider2prefix(char* provider, int*prefix, TELNUM *num)
 * ------------------------------------------------------------
 *	ret len of provider
 *
 * void clearNum(TELNUM *num)
 * --------------------------
 *	call this if you have a number on stack or want to reuse it
 *	normalizeNumber calls this for you
 *
 * void initNum(TELNUM *num)
 *	inits a number with myarea, mycountry
 *	you may set the area yourself prior to calling this
 *
 */

#include "telnum.h"

#define DEFAULT (UNKNOWN-1)

static TELNUM defnum;

static void _init(void);

#ifdef TESTde
#undef ISDN_AT
#define ISDN_DE
#endif
#ifdef TESTnl
#undef ISDN_AT
#define ISDN_NL
#endif

/* --------------------- ADJUST here ----------------*/
#ifdef ISDN_AT
void initTelNum(void)
{
  vbn = strdup("10");  /* Fixme: VBN is defined in isdn.conf but not read yet */
  preselect=1;
#define VBN_LEN 2  /* len of digits following vbn */
#define VBN_GERMANY 0
  _init();
} /* pre_init */
#endif

#ifdef ISDN_DE
void initTelNum(void)
{
  vbn = strdup("010"); 	/* Fixme: VBN is defined in isdn.conf but not read yet */
  preselect=33;
#define VBN_LEN 2  
#define VBN_GERMANY 1 /* vbn[3] == '0' =>  3 stellig folgt */
  _init();
} /* pre_init */
#endif

#ifdef ISDN_NL
void initTelNum()
{
  vbn = strdup("16:17"); /* Fixme: VBN is defined in isdn.conf but not read yet */
  preselect=999;
#define VBN_LEN 2  /* Paul: ? is this correct */
#define VBN_GERMANY 0
  _init();
} /* pre_init */
#endif

/* ----------------- end ADJUST here ----------------*/

static inline int Isspace(c) {
  return isspace(c) || c == '_';
}  

static int split_vbn(char **p, TELNUM *num) {
  int l;

  print_msg(PRT_V, "vbn: p(%d) '%s' ", num->nprovider,*p);
  if ((l = provider2prefix(*p, &num->nprovider, num))) {
	Strncpy(num->provider, *p, l+1);
	*p += l;
    print_msg(PRT_V, "Provider \"%s\" %d\n", num->provider, num->nprovider);
	return l;
  }
  print_msg(PRT_V, "No Provider\n");
  return 0;	  
}
static inline void clearCountry(TELNUM *num, int c) {
  num->country=0;
  num->ncountry=c;
}
static inline void setCountry(TELNUM *num) {
  num->country=defnum.country;
  num->ncountry=defnum.ncountry;
}
static int _getCountrycode(char *country, char ** t) {
  char c[TN_MAX_COUNTRY_LEN];
  int res, l, last;
  if ((last=res = getCountrycode(country, t)) == UNKNOWN)
	return UNKNOWN;
  if(res>=TN_MAX_COUNTRY_LEN) {
    print_msg(PRT_A, "Problem getCountrycode \"%s\" returned\n", country,res);
	return UNKNOWN;
  }	
  Strncpy(c, country, res); /* try shorter because of towns in country.dat */
  l=strlen(c);
  while(l>1) {	
	if ((res = getCountrycode(c, t)) != UNKNOWN) 
	  last=res;
  	print_msg(PRT_V, "_getCountrycode(%s)=%d  ", c,res);
	c[--l] = '\0'; /* try shorter */
  }
  return last;	
}  	
static int split_country(char **p, TELNUM *num) {
  int res=0;
  int len=0;
  char *country=0;
  print_msg(PRT_V, "cou: '%s' ", *p);
  
  if (!memcmp(*p,"00", 2) || !isdigit(**p)) {
	res = 0; /* len of country known ? */
	if (!isdigit(**p)) { /* alnum or + */
	  if (**p == '+')	
		country = strdup(*p);
	  else {	/* alpha */  
		while (!isdigit(**p) && **p)
		  (*p)++, res++;
	    while (Isspace(**p))
		  (*p)--,res--;
		country = malloc(res+1);
		Strncpy(country, (*p)-res, res+1);	
	  }	
	}
	else {
	  country = malloc(strlen(*p)+2);
	  sprintf(country, "+%s", (*p)+2);
	  (*p)++;len++;
	}	  
	if (res == 0) {
	  if((res = _getCountrycode(country, 0)) != UNKNOWN) {
    	print_msg(PRT_V, "getCountrycode(%s)= ", country);
		country[res]='\0';
    	print_msg(PRT_V, "\"%s\"  ", country);
		*p += res;
	  }
	  else {
    	print_msg(PRT_A, "Unknown Country \"%s\"\n", *p);
  	    clearCountry(num, UNKNOWN); 
		res=-10;
	  }	
	}  	
    if (getCountry(country, &num->country) != UNKNOWN) {
	  print_msg(PRT_V, "Country \"%s\"\n", num->country->Code[0]);
	  num->ncountry=atoi(num->country->Code[0]+1);
	}  
  	else {
  	  clearCountry(num, UNKNOWN); 
      print_msg(PRT_A, "Unknown Country \"%s\"\n", *p);
  	  res=-10;
	}	
	free(country);
  }
  else {
	int ga;
    print_msg(PRT_V, "getArea(%d,%s) => " ,num->nprovider, *p);
	if((ga=getArea(num->nprovider, *p))) /* sondernummer */
  	  clearCountry(num, 0); 
	else  
  	  clearCountry(num, DEFAULT); 
	print_msg(PRT_V, "%d  ", ga);
  }  
  print_msg(PRT_V, "Country %d\n", num->ncountry);
  return res+len;
}  

static void clearArea(TELNUM *num, int a) {
  strcpy(num->area,a==DEFAULT?defnum.area:"");
  strcpy(num->sarea,a==DEFAULT?defnum.sarea:"?");
  num->narea= a==DEFAULT?defnum.narea:a; 
}

static int split_area(char **p, TELNUM *num, int first) {
  int res=0;
  int len=0;
  char *s;
  print_msg(PRT_V, "are: '%s' ", *p);
  if (num->ncountry == 0) {	/* sondernummer */
	clearArea(num, 0);
	return 0; 
  }	
  if (**p == '0' && first)
	(*p)++, len++;
  if (len || num->ncountry>0) {
	if (num->ncountry == DEFAULT)
	  setCountry(num);	
	if((res=getAreacode(num->ncountry, *p, &s)) != UNKNOWN) {
	  Strncpy(num->sarea, s, TN_MAX_SAREA_LEN);	 
  	  Strncpy(num->area, *p, min(res+1, TN_MAX_AREA_LEN));	 
	  (*p) += res;
	  num->narea=atoi(num->area);
	  print_msg(PRT_V,"getAreacode(%d, %s)= '%s'\n",num->ncountry,num->area,num->sarea);
	}	
	else {
	  clearArea(num, UNKNOWN);
	  print_msg(PRT_V,"getAreacode(%d, %s)= 'UNKNOWN'\n",num->ncountry,*p);
	  return -1;
	}
  }		
  else 
	clearArea(num, DEFAULT);
  if (num->ncountry == DEFAULT)
	setCountry(num);	
  return res+len;
}  

int normalizeNumber(char *target, TELNUM *num, int flag) {
  int res=0;
  int first=0;
  char n[TN_MAX_NUM_LEN];
  char *p, *q;
  
  for (p=target, q=n; *p; p++)
	if (!Isspace(*p))
	  *q++ = *p;
  *q = '\0';
  p = n;
  clearNum(num);
  if (flag & TN_PROVIDER)
	split_vbn(&p, num);
  if (flag & TN_COUNTRY) {	
	res = split_country(&p, num);
	if (res<0) 
	  return UNKNOWN;
	else if(res==0)
	  first=1;		
  }	  
  if (flag & TN_AREA)	
	res = split_area(&p, num, first);
  Strncpy(num->msn, p, TN_MAX_MSN_LEN);
  if (res<0)
	return 1;
  return(0);	
}

char *prefix2provider(int prefix, char*s, TELNUM *num) {
  if (prefix < 100)
    sprintf(s, "%s%0*d", num->vbn, VBN_LEN, prefix);
  else
    sprintf(s, "%s%03d", num->vbn, prefix - 100);
  return s;
}

int provider2prefix(char *p, int *prefix, TELNUM *num) {
  char prov[TN_MAX_PROVIDER_LEN];
  int l1, l2=0;
  char *q;
  char *vbns=strdup(vbn);
  q = strtok(vbns, ":");
  while (q) {
	l1=strlen(vbns);
	if (!memcmp(p, q, l1)) {
	  Strncpy(num->vbn, q, TN_MAX_VBN_LEN);
      print_msg(PRT_V, "VBN \"%s\"\n", q);
  	  if (p[l1] == '0' && VBN_GERMANY) /* dreistellige Verbindungsnetzbetreiberkennzahl? */
    	l2 = l1 + 3;	/* 1002 is provider UTA in AT */
  	  else
    	l2 = l1 + VBN_LEN;
	  Strncpy(prov, p+l1, l2+1-l1);
	  *prefix = (l2==6) ? 100+atoi(prov) : atoi(prov);
	  break;
	}  
	q = strtok(0, ":");
  }
  free(vbns);
  return l2;
}  

void initNum(TELNUM *num) {
  char *s;
  if(!*num->area)
	Strncpy(num->area, myarea, TN_MAX_AREA_LEN);
  num->country=defnum.country;
  num->ncountry=defnum.ncountry;
  num->narea=atoi(num->area);
  if (getAreacode(num->ncountry, num->area, &s) != UNKNOWN) {
    Strncpy(num->sarea, s, TN_MAX_SAREA_LEN); 
    free(s);
  }	
  else 
      clearArea(num, UNKNOWN);
  strcpy(num->vbn, defnum.vbn);
}

static void _init(void) {
  char *s;
  clearNum(&defnum);
  Strncpy(defnum.area, myarea, TN_MAX_AREA_LEN);
  if (getCountry(mycountry, &defnum.country) != UNKNOWN) {
	defnum.ncountry=atoi(defnum.country->Code[0]+1);
	if (getAreacode(defnum.ncountry, defnum.area, &s) != UNKNOWN) {
	  Strncpy(defnum.sarea, s, TN_MAX_SAREA_LEN); 
	  free(s);
	}	
  }	
  Strncpy(defnum.vbn, vbn, TN_MAX_VBN_LEN);
}

void clearNum(TELNUM *num) {  
  strcpy(num->provider,"");
/*  num->nprovider=preselect; */
  strcpy(num->area,"");
  initNum(num);
  strcpy(num->msn,"");
  Strncpy(num->vbn, vbn, TN_MAX_VBN_LEN);
}
/*	%Np .. Provider
 *	%Nc .. country +49
 *	%NC .. countryname
 *	%Na .. area 89 or 089 if no country/Provider
 *	%NA .. areaname
 *	%Nm .. msn
 *  %f .. full +49 89 12356 (Deutschland, Berlin)
 *	%s .. short +48 89 123456
 * 	%n .. number 004889123456
 *  N is number of chars to skip in format if part is not present
 *  e.g. "%1c %1a %m"
 *	+4930123 => "+49 39 123"
 *  123 => "123" not "  123"
 */
#define SKIP if(skip>0)	\
			  while(skip-- && *p && p[1]) \
				p++
 
char * formatNumber(char* format, TELNUM* num) {
  char *s=retstr[++retnum];
  char *p, *q, *r;
  int first=1;
  int skip;
  
  retnum %= MAXRET;
  *s = '\0';
  for (p=format, q=s; *p; p++) {
	if (*p =='%') {
	  skip = 0;
again:	  
	  switch (*++p) {
		case 'p':
		  if (num->nprovider>0) {
			q=stpcpy(q, num->provider); 
			first=0; 
		  }
		  else 
			SKIP;
		  break;
		case 'c': 
		  if(num->ncountry>0 && num->country) {
			q=stpcpy(q, num->country->Code[0]); 
			first=0;
		  }
		  else 
			SKIP;
		  break;
		case 'C': 
		  if(num->ncountry>0 && num->country) 
			q=stpcpy(q, num->country->Name);
		  else 
			SKIP;
		  break;
		case 'a': 
		  if(num->narea>0) {
			if(first) *q++ = '0'; /* areaprefix ?? */
			q=stpcpy(q,num->area);
		  }	
		  else 
			SKIP;
		  break;
		case 'A': 
		  if(num->narea>0) 
			q=stpcpy(q,num->sarea);
		  else 
			SKIP;
		  break;
		case 'm': 
		  if(*num->msn)
			q=stpcpy(q,num->msn);
		  else 
			SKIP;
		  break;
		case 'f': q=stpcpy(q,formatNumber("%1c %1a %1m (%2C, %A)", num)); break;
		case 's': q=stpcpy(q,formatNumber("%1c %1a %m", num)); break;
		case 'n': 
		  if(num->ncountry>0 && num->country) {
			q=stpcpy(r=++q,formatNumber("%c%a%m", num));
			r[0]=r[1]='0'; 
		  }	
		  else	
			q=stpcpy(q,formatNumber("%a%m", num)); 
		  break;
		default:
		  if (isdigit(*p)) {
			skip = strtol(p, &p, 10);
			p--;
			goto again;
		  }	
	  }	
	}  
	else	
	  *q++ = *p;
  }
  *q = '\0';	  
  return s;
}

