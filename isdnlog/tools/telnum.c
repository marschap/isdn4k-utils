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
 *	%Nt .. tld = 2 char isdcode for country
 *	%Na .. area 89 or 089 if no country/Provider
 *	%NA .. areaname
 *	%Nm .. msn
 *      %f .. full +49 89 12356 (Deutschland, Berlin)
 *	%F .. full +49 89/12345, Berlin
 *	%s .. short +48 89 123456
 *	%l .. long +49 89 12356 - Berlin (DE)
 * 	%n .. number 004889123456
 *
 *  N is number of chars to skip in format if part is not present
 *  e.g. "%1c %1a %m"
 *	+4930123 => "+49 39 123"
 *  123 => "123" not "  123"
 *
 *
 * char* prefix2provider(int prefix, char* provider)
 * --------------------------------------------------------------
 *  returns formatted provider for prefix
 *
 * int provider2prefix(char* provider, int*prefix)
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

#ifdef USE_DESTINATION
#include "dest.h"
#else
#include "telnum.h"
#endif

#define DEFAULT (UNKNOWN-1)

/* #define DEBUG 1 */

static TELNUM defnum;

static void error (char *fmt, ...)
{
  va_list ap;
  char msg[BUFSIZ];

  va_start (ap, fmt);
  vsnprintf (msg, BUFSIZ, fmt, ap);
  va_end (ap);
#ifdef STANDALONE
  fprintf(stderr, "%s\n", msg);
#else
  print_msg(PRT_ERR, "%s\n", msg);
#endif
}


static void _init(void);

void initTelNum(void)
{
  if(!vbn)
    vbn="";
  if (!vbnlen || !*vbnlen) {
    error("VBNLEN not defined.\n\tPlease read isdnlog/README\n");
    exit(1);
  }  
  _init();
} /* pre_init */

static inline int Isspace(c) {
  return isspace(c) || c == '_';
}  

static int split_vbn(char **p, TELNUM *num) {
  int l;

  if ((l = provider2prefix(*p, &num->nprovider))) {
	Strncpy(num->provider, *p, l+1);
	*p += l;
	return l;
  }
  return 0;	  
}

static void clearArea(TELNUM *num, int a) {
  strcpy(num->area,a==DEFAULT?defnum.area:"");
  strcpy(num->sarea,a==DEFAULT?defnum.sarea:"?");
  num->narea= a==DEFAULT?defnum.narea:a; 
}

#ifndef USE_DESTINATION
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
  if (country[1] == '1' || country[1] == '7') /* Fixme: quick hack fuer USA/GUS */
    return last;
  Strncpy(c, country, res); /* try shorter because of towns in country.dat */
  l=strlen(c);
  while(l>1) {	
	if ((res = getCountrycode(c, t)) != UNKNOWN) 
	  last=res;
#if DEBUG
  	print_msg(PRT_V, "_getCountrycode(%s)=%d  ", c,res);
#endif
	c[--l] = '\0'; /* try shorter */
  }
  return last;	
}  	
static int split_country(char **p, TELNUM *num) {
  int res=0;
  int len=0;
  char *country=0;
#if DEBUG
  print_msg(PRT_V, "cou: '%s' ", *p);
#endif
  
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
#if DEBUG
    	print_msg(PRT_V, "getCountrycode(%s)= ", country);
#endif
		country[res]='\0';
#if DEBUG
    	print_msg(PRT_V, "\"%s\"  ", country);
#endif
		*p += res;
	  }
	  else {
    	print_msg(PRT_A, "Unknown Country \"%s\"\n", *p);
  	    clearCountry(num, UNKNOWN); 
		res=-10;
	  }	
	}  	
    if (getCountry(country, &num->country) != UNKNOWN) {
#if DEBUG
	  print_msg(PRT_V, "Country \"%s\"\n", num->country->Code[0]);
#endif
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
#if DEBUG
    print_msg(PRT_V, "getArea(%d,%s) => " ,num->nprovider, *p);
#endif
	if((ga=getArea(num->nprovider, *p))) /* sondernummer */
  	  clearCountry(num, 0); 
	else  
  	  clearCountry(num, DEFAULT); 
#if DEBUG
	print_msg(PRT_V, "%d  ", ga);
#endif
  }  
#if DEBUG
  print_msg(PRT_V, "Country %d\n", num->ncountry);
#endif
  return res+len;
}  


static int split_area(char **p, TELNUM *num, int first) {
  int res=0;
  int len=0;
  char *s;
#if DEBUG
  print_msg(PRT_V, "are: '%s' ", *p);
#endif
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
#if DEBUG
	  print_msg(PRT_V,"getAreacode(%d, %s)= '%s'\n",num->ncountry,num->area,num->sarea);
#endif
	}	
	else {
	  clearArea(num, UNKNOWN);
#if DEBUG
	  print_msg(PRT_V,"getAreacode(%d, %s)= 'UNKNOWN'\n",num->ncountry,*p);
#endif
	  return -1;
	}
  }		
  else 
	clearArea(num, DEFAULT);
  if (num->ncountry == DEFAULT)
	setCountry(num);	
  return res+len;
}  

#endif

#ifdef USE_DESTINATION

static inline void clearCountry(TELNUM *num, int c) {
  *num->scountry='\0';
  *num->country='\0';
  num->ncountry=c;
}

int normalizeNumber(char *target, TELNUM *num, int flag) {
  char *origp=strdup(target);
  char *p=origp;
  int res=0;
  char *q;
  
  clearNum(num);
#if DEBUG
  print_msg(PRT_V,"NN %s (Prov %d)=> ",target, num->nprovider);
#endif    
  if (flag & TN_PROVIDER)
    if (!split_vbn(&p, num)) {
      num->nprovider=preselect;
      Strncpy(num->provider, getProvider(preselect),TN_MAX_PROVIDER_LEN);
    }  
  if (flag & TN_COUNTRY) {
    /* subst '00' => '+' */
    if (p[0]=='0' && p[1]=='0')
      *++p='+';
    if(!isdigit(*p)) {
      res=getDest(p, num);
      /* isdnrate is coming with +4319430 but this is a sondernummer */
      if (atoi(mycountry+1) == num->ncountry) {
        q = malloc(strlen(num->area)+strlen(num->msn)+1);
	strcpy(q, num->area);
	strcat(q, num->msn);
        if(getArea(num->nprovider, q)) { /* sondernummer */
  	  clearCountry(num, 0); 
	  *num->sarea='\0';
	  Strncpy(num->area, q, TN_MAX_AREA_LEN);
	  num->narea=atoi(num->area);
	  *num->msn = '\0';
	}
	free(q);
      }	
    }  
    else {  
      if(getArea(num->nprovider, p)) { /* sondernummer */
  	clearCountry(num, 0); 
	*num->sarea='\0';
	Strncpy(num->area, p, TN_MAX_AREA_LEN);
	num->narea=atoi(num->area);
	*num->msn = '\0';
      }		
      else {
        clearArea(num,DEFAULT);
	if (*p == '0') { /* must be distant call in country */
	  q = malloc(strlen(mycountry)+strlen(p));
	  strcpy(q, mycountry);
	  strcat(q, p+1);
	  free(origp);
	  origp=p=q;
          res=getDest(p, num);
	} 
	else 
	  Strncpy(num->msn, p, TN_MAX_MSN_LEN);
      }	
    }	
  }	
  free(origp);      
#if DEBUG
  print_msg(PRT_V,"(%d) %s\n",res,formatNumber("%l Prov %p",num));
#endif    
  return(res);	
}

#else

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
#endif

#define VBN_LEN (*vbnlen-'0')
char *prefix2provider(int prefix, char*s) {
  if (prefix < 100)
    sprintf(s, "%s%0*d", defnum.vbn, VBN_LEN, prefix);
  else
    sprintf(s, "%s%03d", defnum.vbn, prefix - 100);
  return s;
}

int provider2prefix(char *p, int *prefix) {
  char prov[TN_MAX_PROVIDER_LEN];
  int l1, l2=0;
  char *q;
  char *vbns=strdup(vbn);
  q = strtok(vbns, ":");
  while (q) {
	l1=strlen(vbns);
	if (!memcmp(p, q, l1)) {
/*	  Strncpy(num->vbn, q, TN_MAX_VBN_LEN); */
#if DEBUG
      print_msg(PRT_V, "VBN \"%s\"\n", q);
#endif
  	  if (p[l1] == '0' && strchr(vbnlen,':')) /* dreistellige Verbindungsnetzbetreiberkennzahl? */
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
  num->ncountry=defnum.ncountry;
#ifdef USE_DESTINATION  
  strcpy(num->scountry,defnum.scountry);
  strcpy(num->country,defnum.country);
  num->narea=atoi(num->area); /* 1.01 */
  s=malloc(strlen(mycountry)+strlen(num->area)+1);
  strcpy(s,mycountry);
  strcat(s,num->area);
  getDest(s, num);  
  free(s);
#else  
  num->country=defnum.country;
  num->narea=atoi(num->area);
  if (getAreacode(num->ncountry, num->area, &s) != UNKNOWN) {
    Strncpy(num->sarea, s, TN_MAX_SAREA_LEN); 
    free(s);
  }	
  else 
      clearArea(num, UNKNOWN);
#endif      
  strcpy(num->vbn, defnum.vbn);
}

static void _init(void) {
  char *s;
  clearNum(&defnum);
  Strncpy(defnum.area, myarea, TN_MAX_AREA_LEN);
#ifdef USE_DESTINATION
  s=malloc(strlen(mycountry)+strlen(myarea)+1);
  strcpy(s,mycountry);
  strcat(s,myarea);
  getDest(s, &defnum);  
#else
  if (getCountry(mycountry, &defnum.country) != UNKNOWN) {
	defnum.ncountry=atoi(defnum.country->Code[0]+1);
	if (getAreacode(defnum.ncountry, defnum.area, &s) != UNKNOWN) {
	  Strncpy(defnum.sarea, s, TN_MAX_SAREA_LEN); 
	  free(s);
	}	
  }	
#endif  
  Strncpy(defnum.vbn, vbn, TN_MAX_VBN_LEN);
}

void clearNum(TELNUM *num) {  
/*  num->nprovider=UNKNOWN;
  strcpy(num->provider,""); */
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
 *	+4930123 => "+49 30 123"
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
#ifdef USE_DESTINATION
		q=stpcpy(q,num->country); 
#else			
			q=stpcpy(q, num->country->Code[0]); 
#endif			
			first=0;
		  }
		  else 
			SKIP;
		  break;
		case 'C': 
#ifdef USE_DESTINATION
		  if(num->ncountry>0) 
			q=stpcpy(q, num->scountry);
#else			
		  if(num->ncountry>0 && num->country) 
			q=stpcpy(q, num->country->Name);
#endif			
		  else 
			SKIP;
		  break;
		case 't': /* tld */
		  if(*num->tld)
		    q=stpcpy(q, num->tld);  
		  else
		    SKIP;
		  break;    
		case 'a': 
		  if(num->narea>0) {
/*			if(first) *q++ = '0'; / sondernummber - no country */
			q=stpcpy(q,num->area);
		  }	
		  else 
			SKIP;
		  break;
		case 'A': 
		  if(*num->sarea) 
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

                case 'F': if (num->ncountry == defnum.ncountry)
                     	    q = stpcpy(q, formatNumber("%1c %1a/%1m, %A", num));
                	  else
                     	    q = stpcpy(q, formatNumber("%1c %1a/%1m, %2C, %A", num));
                          break;

		case 's': q=stpcpy(q,formatNumber("%1c %1a %m", num)); break;
		case 'l': q=stpcpy(q,formatNumber("%1c %1a %1m - %1A (%t)", num)); break;
		case 'n': 
		  if(num->ncountry>0) {
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

#ifdef TEST_TELNUM
int verbose=0;
static char *myshortname;

static void init()
{
  auto char *version, **message;

  if (readconfig(myshortname) < 0)
    exit(1);

  if (verbose)
    message = &version;
  else
    message = NULL;

  initHoliday(holifile, message);

  if (verbose && *version)
    print_msg(PRT_V, "%s\n", version);
#ifdef USE_DESTINATION
  initDest("/usr/lib/isdn/dest.gdbm", message); /* Fixme: */
#else  
  initCountry(countryfile, message);
#endif
  if (verbose && *version)
    print_msg(PRT_V, "%s\n", version);

  initRate(rateconf, ratefile, zonefile, message);

  if (verbose && *version)
    print_msg(PRT_V, "%s\n", version);

  initTelNum();
}				/* init */

static void deinit(void)
{
  exitRate();
#ifdef USE_DESTINATION
  exitDest();
#else  
  exitCountry();
#endif  
  exitHoliday();
}
int     main(int argc, char *argv[], char *envp[])
{
  register int i;
  TELNUM num;
  myname = argv[0];
  myshortname = basename(myname);

  if(argc>1) {
    init();
    i = argc-1;
    argc++;
    while (i--) {
      normalizeNumber(argv[argc], &num);
      printf("%s => %s\n",argv[argc++], formatNumber("%l", &num);
    }
  }
  return EXIT_SUCCESS;
}      
#endif
