/* $Id: isdnrate.c,v 1.11 1999/07/15 16:42:04 akool Exp $
 *
 * ISDN accounting for isdn4linux. (rate evaluation)
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
 * $Log: isdnrate.c,v $
 * Revision 1.11  1999/07/15 16:42:04  akool
 * small enhancement's and fixes
 *
 * Revision 1.10  1999/07/07 19:44:07  akool
 * patches from Michael and Leo
 *
 * Revision 1.9  1999/07/04 20:47:05  akool
 * rate-de.dat V:1.02-Germany [04-Jul-1999 22:56:37]
 *
 * Revision 1.8  1999/07/03 10:24:14  akool
 * fixed Makefile
 *
 * Revision 1.7  1999/07/02 19:18:00  akool
 * rate-de.dat V:1.02-Germany [02-Jul-1999 21:27:20]
 *
 * Revision 1.6  1999/07/02 18:20:50  akool
 * rate-de.dat V:1.02-Germany [02-Jul-1999 20:29:21]
 * country-de.dat V:1.02-Germany [02-Jul-1999 19:13:54]
 *
 * Revision 1.5  1999/07/01 20:40:07  akool
 * isdnrate optimized
 *
 * Revision 1.4  1999/06/30 20:53:28  akool
 * added "-t" option to "isdnrate"
 *
 * Revision 1.3  1999/06/30 17:17:37  akool
 * isdnlog Version 3.39
 *
 * Revision 1.2  1999/06/29 20:11:25  akool
 * now compiles with ndbm
 * (many thanks to Nima <nima_ghasseminejad@public.uni-hamburg.de>)
 *
 * Revision 1.1  1999/06/28 19:16:33  akool
 * isdnlog Version 3.38
 *   - new utility "isdnrate" started
 *
 */

#include "isdnlog.h"
#include "tools/zone.h"

#define WIDTH   19
#define MAXLAST	 5
#define ZAUNPFAHL  1 /* FIXME: Michi: Offset */

static char *myname, *myshortname;
static char  options[] = "Vvd:h:l:Hb:s:tx::u";
static char  usage[]   = "%s: usage: %s [ -%s ] Destination ...\n";

static int    verbose = 0, header = 0, best = MAXPROVIDER, table = 0, explain = 0;
static int    usestat = 0, provider = UNKNOWN;
static int    duration = LCR_DURATION;
static time_t start;
static int    day, month, year, hour, min;
static char   country[BUFSIZ], area[BUFSIZ], msn[BUFSIZ];
static char   ignore[MAXPROVIDER];


typedef struct {
  int    prefix;
  double rate;
  char  *explain;
} SORT;

typedef struct {
  int   weight;
  int	index;
} SORT2;

static SORT sort[MAXPROVIDER];

int print_msg(int Level, const char *fmt, ...)
{
  auto va_list ap;
  auto char    String[BUFSIZ * 3];


  va_start(ap, fmt);
  (void)vsnprintf(String, BUFSIZ * 3, fmt, ap);
  va_end(ap);

  if (Level == PRT_ERR)
    return(1);

  fprintf(Level == PRT_ERR ? stderr : stdout, "%s", String);

  return(0);
} /* print_msg */


static void pre_init()
{
  preselect = DTAG;      /* Telekomik */
  vbn = strdup("010"); 	 /* Germany */
} /* pre_init */


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

  if (*version && verbose)
    print_msg(PRT_NORMAL, "%s\n", version);

  initCountry(countryfile, message);

  if (*version && verbose)
    print_msg(PRT_NORMAL, "%s\n", version);

  initRate(rateconf, ratefile, zonefile, message);

  if (*version && verbose)
    print_msg(PRT_NORMAL, "%s\n", version);
} /* init */


static void post_init()
{
  auto char s[BUFSIZ];


  sprintf(s, "%s%s", mycountry, myarea);
  mynum = strdup(s);
} /* post_init */


static int opts(int argc, char *argv[])
{
  register int   c;
  register char *p;


  while ((c = getopt(argc, argv, options)) != EOF) {
    switch (c) {
      case 'V' : print_version(myshortname);
      	       	 exit(0);

      case 'v' : verbose++;
      	       	 break;

      case 'l' : duration = strtol(optarg, NIL, 0); /* l wie lt */
      	       	 break;

      case 'H' : header++;
      	       	 break;

      case 'b' : best = strtol(optarg, NIL, 0);
      	       	 break;

      case 'd' : day = atoi(optarg);
      	       	 if ((p = strchr(optarg, '/'))) {
                   month = atoi(p + 1);

                   if ((p = strchr(p + 1, '/'))) {
                     year = atoi(p + 1);

           	     if (year < 50)
                       year += 2000;
		     else if (year < 100)  
                       year += 1900;
                   }
      	       	 }
                 break;
      case 'h': hour = atoi(optarg);		  
                   if ((p = strchr(p + 1, ':')))
                         min = atoi(p + 1);
                 break;

      case 't' : table++;
      	       	 break;

      case 'x' : explain++;
                 {
		   int x;
                   if (optarg && (x=atoi(optarg)))
		     explain=x;
		 }     
      	       	 break;

      case 'u' : usestat++;
      	       	 break;

      case '?' : print_msg(PRT_ERR, usage, myshortname, myshortname, options);
      	       	 break;
    } /* switch */
  } /* while */

  if (argc > optind)
  return(optind);
  else
    return(0);
} /* opts */


static int compare(const void *s1, const void *s2)
{
  return(((SORT *)s1)->rate > ((SORT *)s2)->rate);
} /* compare */


static int compare2(const void *s1, const void *s2)
{
  return(((SORT2 *)s1)->weight < ((SORT2 *)s2)->weight);
} /* compare2 */

char *short_explainRate (RATE *Rate)
{
  static char buffer[BUFSIZ];
  char       *p=buffer;

  if (Rate->Zone && *Rate->Zone)
    p+=sprintf (p, "%s", Rate->Zone);
  else
    p+=sprintf (p, "Zone %d", Rate->zone);

  if (!Rate->domestic && Rate->Country && *Rate->Country)
    p+=sprintf (p, " (%s)", Rate->Country);

  if (Rate->Day && *Rate->Day)
    p+=sprintf (p, ", %s", Rate->Day);

  if (Rate->Hour && *Rate->Hour)
    p+=sprintf (p, ", %s", Rate->Hour);

  return buffer;
}

static char *printrate(RATE *Rate)
{
  static char message[BUFSIZ];


  if (Rate->Basic > 0)
    sprintf(message, "%s + %s/%.4fs = %s + %s/Min (%s)",
      printRate(Rate->Basic),
      printRate(Rate->Price),
      Rate->Duration,
      printRate(Rate->Basic),
      printRate(60 * Rate->Price / Rate->Duration),
      short_explainRate(Rate));
  else
    sprintf(message, "%s/%.4fs = %s/Min (%s)",
      printRate(Rate->Price),
      Rate->Duration,
      printRate(60 * Rate->Price / Rate->Duration),
      short_explainRate(Rate));

  return(message);
} /* printrate */


static void buildtime()
{
  auto struct tm tm;


  tm.tm_sec = 0;
  tm.tm_min = min;
  tm.tm_hour = hour;
  tm.tm_mday = day;
  tm.tm_mon = month - 1;
  tm.tm_year = year - 1900; /* ja, ja, ich weiá ;-) */
  tm.tm_isdst = -1;

  start = mktime(&tm);
} /* buildtime */


static void splittime()
{
  auto struct tm *tm;

  tm = localtime(&start);


  min = tm->tm_min;
  hour = tm->tm_hour;
  day = tm->tm_mday;
  month = tm->tm_mon + 1;
  year = tm->tm_year + 1900; /* ja, ja, ich weiá ;-) */
} /* splittime */


static char *Provider(int prefix)
{
  register char *p;
  register int   l;
  static   char  s[BUFSIZ];


  if (prefix == UNKNOWN)
    return("?");

  p = getProvider(prefix);

  l = max(WIDTH, strlen(p)) - strlen(p);

  if (prefix < 100)
    sprintf(s, "%s%02d:%s%*s", vbn, prefix, p, l, "");
  else
    sprintf(s, "%s%03d:%s%*s", vbn, prefix - 100, p, l - 1, "");

  return(s);
} /* Provider */


static void numsplit(char *num)
{
  register int   l1, l3, zone;
  auto	   int	 l2;
  register char *p;
  auto	   char *s;


  print_msg(PRT_NORMAL, "NUMSPLIT(%s)\n", num);

  *country = *area = *msn = 0;

  if (verbose && (provider != UNKNOWN))
    print_msg(PRT_NORMAL, " Provider %s\n", Provider(provider));

  if ((l1 = getCountrycode(num, &s)) != UNKNOWN) {
    Strncpy(country, num, l1 + 1);

    if (verbose)
      print_msg(PRT_NORMAL, " Country %s : %s\n", country, s);

    if ((p = get_areacode(num, &l2, C_NO_WARN | C_NO_EXPAND | C_NO_ERROR))) {
      Strncpy(area, num + l1, l2 + 1 - l1);

      if (verbose)
        print_msg(PRT_NORMAL, " Area %s : %s\n", area, p);

      strcpy(msn, num + l2);

      if (verbose && *msn)
        print_msg(PRT_NORMAL, " Number %s\n", msn);
    } /* if */

    p = country;

    while (*p && !isdigit(*p))
      p++;

    l3 = getAreacode(atoi(p), num + l1, &s);

    if (1 /* l3 != UNKNOWN */) {
      print_msg(PRT_NORMAL, "getAreacode(%d, %s, %s)=%d\n", atoi(p), num + l1, s, l3);
      if (l3 != UNKNOWN)
      free(s);
      zone = getZone(DTAG, myarea, num + l1);
      print_msg(PRT_NORMAL, "getZone(%d,%s,%s)=%d\n", DTAG, myarea, num + l1, zone);

      switch (zone) {
         case 1 : print_msg(PRT_NORMAL, "Ortszone\n");     break;
         case 2 : print_msg(PRT_NORMAL, "Cityzone\n");     break;
         case 3 : print_msg(PRT_NORMAL, "Regionalzone\n"); break;
         case 4 : print_msg(PRT_NORMAL, "Fernzone\n");    break;
        default : print_msg(PRT_NORMAL, "*** BUG ***\n");  break;
      } /* switch */
    } /* if */
  }
  else {
    l3 = getAreacode(49, num, &s);

    if (l3 != UNKNOWN) {
      print_msg(PRT_NORMAL, "getAreacode(%d, %s, %s)=%d\n", atoi(p), num + l1, s, l3);
      free(s);

      zone = getZone(DTAG, mynum, num);
      print_msg(PRT_NORMAL, "getZone=%d\n", zone);
    } /* if */
  } /* else */
} /* numsplit */


static int normalizeNumber(char *target)
{
  register int      l1, l2;
  register char	    c;
  auto 	   COUNTRY *Country;
  auto 	   char     num[BUFSIZ];


  if (isalpha(*target)) {
    if (getCountry(target, &Country) != UNKNOWN)
      strcpy(num, Country->Code[0]);
    else {
      print_msg(PRT_NORMAL, "Unknown country \"%s\"\n", target);
      return(0);
    } /* else */
  }
  else {
    l1 = strlen(vbn);

    if (!memcmp(target, vbn, l1)) {
      if (target[l1] == '0') /* dreistellige Verbindungsnetzbetreiberkennzahl? */
        l2 = l1 + 3;
      else
        l2 = l1 + 2;

      c = target[l2];
      target[l2] = 0;
      provider = atoi(target + l1);

      if (l2 == 6)
        provider += 100;

      target[l2] = c;
      memmove(target, target + l2, strlen(target) - l2);
    } /* if */

    if (*target == '+')
      strcpy(num, target);
    else if (!memcmp(target, "00", 2))
      sprintf(num, "+%s", target + 2);
    else if (*target == '0') {
#if 0
      if (!strchr("18", target[1])) /* FIXME: "18" ist Deutsche-Sonderrufnummernerkennung! */
#endif
        sprintf(num, "%s%s", mycountry, target + 1);
#if 0
      else
        sprintf(num, "%s%s", mycountry, target);
#endif
    }
    else
      sprintf(num, "%s%s%s", mycountry, myarea, target);
  } /* else */

  numsplit(num);
  return(1);
} /* normalizeNumber */


static int compute()
{
  register int  i, n = 0;
  register int	low = 0, high = MAXPROVIDER - 1;
  auto 	   RATE Rate;
  auto	   char s[BUFSIZ];


  if (provider != UNKNOWN) {
    low = high = provider;
  } /* if */

  for (i = low; i <= high; i++) {

    if (ignore[i])
      continue;

    clearRate(&Rate);
    Rate.src[0] = mycountry;
    Rate.src[1] = myarea;
    Rate.src[2] = "";

    Rate.dst[0] = country;
    Rate.dst[1] = area;
    Rate.dst[2] = msn;

    /* Rate.Service = "Internet by call"; */

    Rate.prefix = i;

    buildtime();

    Rate.start = start;
    Rate.now   = start + duration - ZAUNPFAHL;

    /* kludge to suppress "impossible" Rates */

    if (!getRate(&Rate, NULL) && (Rate.Price != 99.99)) {
      sort[n].prefix = Rate.prefix;
      sort[n].rate = Rate.Charge;

      if (explain == 2) {
        sprintf(s, " (%s)", printrate(&Rate));
        sort[n].explain = strdup(s);
      }
      else if (explain == 1) {
        sprintf(s, " (%s)", Rate.Zone);
        sort[n].explain = strdup(s);
      }	
      else
        sort[n].explain = strdup("");

      n++;
    } /* if */
  } /* for */

  qsort((void *)sort, n, sizeof(SORT), compare);

  return(n);
} /* compute */


static void result(char *target, int n)
{

  register int  i;
  auto 	   char num[BUFSIZ];


  *num = 0; /* FIXME */

  if (header)
    print_msg(PRT_NORMAL, "Eine %d Sekunden lange Verbindung von %s %s nach %s %s %s kostet am %s\n",
      duration, mycountry, myarea, country, area, msn, ctime(&start));

  if (n > best)
    n = best;

  for (i = 0; i < n; i++)
    print_msg(PRT_NORMAL, "%s %s %8.3f%s\n",
      Provider(sort[i].prefix), currency, sort[i].rate, sort[i].explain);
} /* result */


static void purge(int n)
{
  register int i;


  for (i = 0; i < n; i++)
    if (sort[i].explain)
      free(sort[i].explain);
} /* purge */


/*
    Werktag
    Wochenende
      Ortszone
      Regionalzone
      Fernzone
      Handy
      Internet
        0..23 Uhr
*/

static void printTable()
{
  register int        n, d, i, h, lasthour;
  auto 	   struct tm *tm;
  auto	   SORT	      last[MAXLAST];
  auto 	   int        used[MAXPROVIDER];
  auto 	   int        hours[MAXPROVIDER];
  auto 	   int        weight[MAXPROVIDER];
  auto 	   int        useds = 0, maxhour;
  auto	   SORT2      wsort[MAXPROVIDER];
  static   int	      firsttime = 1;


  memset(used, 0, sizeof(used));
  memset(hours, 0, sizeof(hours));
  memset(weight, 0, sizeof(weight));

  if (header)
    print_msg(PRT_NORMAL, "Eine %d Sekunden lange Verbindung von %s %s nach %s %s %s kostet\n",
      duration, mycountry, myarea, country, area, msn);

  for (d = 0; d < 2; d++) {
    last[0].prefix = UNKNOWN;
    lasthour = UNKNOWN;

    buildtime();
    tm = localtime(&start);

    if (!d) { 	     	         /* Wochenende */
      while (tm->tm_wday) {      /* find next sunday */
        start += (60 * 60 * 24);
      	tm = localtime(&start);
      } /* while */
    }
    else   	                 /* Werktag (Montag) */
      start += (60 * 60 * 24);

    splittime();
    buildtime();

    hour = 7;
    min = 0;

    if (header)
      print_msg(PRT_NORMAL, "\n%s:\n", d ? "Werktag" : "Wochenende");

    while (1) {

      n = compute();

      if (last[0].prefix == UNKNOWN) {
        for (i = 0; i < MAXLAST; i++) {
          if (sort[i].prefix) {
            last[i].prefix = sort[i].prefix;
  	    last[i].rate = sort[i].rate;
  	    last[i].explain = strdup(sort[i].explain);
          } /* if */
        } /* for */
      } /* if */

      if (lasthour == UNKNOWN)
        lasthour = hour;

      if (sort[0].prefix != last[0].prefix) {
        for (i = 0; i < MAXLAST; i++) {

          if (!i)
            print_msg(PRT_NORMAL, "    %02d:00 .. %02d:59 %s = %s %s%s\n",
              lasthour, hour - 1, Provider(last[i].prefix),
              currency,
              double2str(last[i].rate, 5, 3, DEB),
              last[i].explain);
          else
            print_msg(PRT_NORMAL, "                   %s = %s %s%s\n",
              Provider(last[i].prefix),
              currency,
              double2str(last[i].rate, 5, 3, DEB),
              last[i].explain);
        } /* for */

        used[last[0].prefix]++;

        if (lasthour >= hour)
          h = ((24 - lasthour) + hour);
        else
          h = hour - lasthour;

        hours[last[0].prefix] += h;

        if ((lasthour > 8) && (lasthour < 21))
          h *= 2;

	weight[last[0].prefix] += h * (d ? 5 : 2);

        for (i = 0; i < MAXLAST; i++) {
          last[i].prefix = sort[i].prefix;
  	  last[i].rate = sort[i].rate;
  	  last[i].explain = strdup(sort[i].explain);
        } /* for */

        lasthour = hour;
      } /* if */

      purge(n);

      hour++;

      if (hour == 24)
        hour = 0;
      else if (hour == 7)
        break;
    } /* while */

    for (i = 0; i < MAXLAST; i++) {

      if (!i) {
        if ((lasthour == 7) && (hour == 7))
          print_msg(PRT_NORMAL, "    immer          %s = %s %s%s\n",
            Provider(last[i].prefix),
            currency,
            double2str(last[i].rate, 5, 3, DEB),
            last[i].explain);
        else
          print_msg(PRT_NORMAL, "    %02d:00 .. %02d:59 %s = %s %s%s\n",
            lasthour, hour - 1, Provider(last[i].prefix),
            currency,
            double2str(last[i].rate, 5, 3, DEB),
            last[i].explain);
      }
      else
        print_msg(PRT_NORMAL, "                   %s = %s %s%s\n",
          Provider(last[i].prefix),
          currency,
          double2str(last[i].rate, 5, 3, DEB),
          last[i].explain);
    } /* for */

    used[last[0].prefix]++;

    if (lasthour >= hour)
      h = ((24 - lasthour) + hour);
    else
      h = hour - lasthour;

    hours[last[0].prefix] += h;

    if ((lasthour > 8) && (lasthour < 21))
      h *= 2;

    weight[last[0].prefix] += h * (d ? 5 : 2);

  } /* for */

  if (usestat) {
    print_msg(PRT_NORMAL, "\nProvider(s) used:\n");

    maxhour = 9999999;
    useds = 0;

    for (i = 0; i < MAXPROVIDER; i++)
      if (used[i]) {
        print_msg(PRT_NORMAL, "%s %d times, %d hours, weight = %d\n",
          Provider(i), used[i], hours[i], weight[i]);

        wsort[useds].weight = weight[i];
        wsort[useds].index = i;

      	useds++;

      	if (hours[i] < maxhour)
          maxhour = hours[i];
      } /* if */

    if ((best < MAXPROVIDER) && (best < useds)) {
      print_msg(PRT_NORMAL, "Retrying with only %d provider(s), eliminating %d provider(s)\n", best, useds - best);

      qsort((void *)wsort, useds, sizeof(SORT2), compare2);

      for (i = 0; i < useds; i++) {
        print_msg(PRT_NORMAL, "%s %d times, %d hours, weight = %d\n",
          Provider(wsort[i].index), used[wsort[i].index], hours[wsort[i].index], weight[wsort[i].index]);

        if (i == best - 1)
          print_msg(PRT_NORMAL, "\n");

        if (i >= best - 1)
          ignore[wsort[i].index]++;
      } /* for */

      if (firsttime)
        printTable();

      firsttime = 0;

    } /* if */
  } /* if */
} /* printTable */


int main(int argc, char *argv[], char *envp[])
{
  register int i, n;


  myname = argv[0];
  myshortname = basename(myname);

  time(&start);
  splittime();

  if ((i = opts(argc, argv))) {
    pre_init();
    init();
    post_init();

    memset(ignore, 0, sizeof(ignore));

    while (i < argc) {
      if (normalizeNumber(argv[i])) {
        if (table)
          printTable();
        else {
          n = compute();
	  result(argv[i], n);
	  purge(n);
        } /* else */
      } /* if */
      i++;
    } /* while */
  }
  else {
    print_msg(PRT_NORMAL, usage, myshortname, myshortname, options);
    print_msg(PRT_NORMAL, "\n");
    print_msg(PRT_NORMAL, "\t-V\t\tshow version infos\n");
    print_msg(PRT_NORMAL, "\t-v\t\tverbose\n");
    print_msg(PRT_NORMAL, "\t-l duration\t duration of call in seconds (default %d seconds)\n", LCR_DURATION);
    print_msg(PRT_NORMAL, "\t-H\t\tshow a header\n");
    print_msg(PRT_NORMAL, "\t-t\t\tshow a table\n");
    print_msg(PRT_NORMAL, "\t-b best\tshow only the first <best> provider(s) (default %d)\n", MAXPROVIDER);
    print_msg(PRT_NORMAL, "\t-d d/m/y\tstart date of call (default now)\n");
    print_msg(PRT_NORMAL, "\t-h h:/m\tstart time of call (default now)\n");
    print_msg(PRT_NORMAL, "\t-x\texplain each rate\n");
    print_msg(PRT_NORMAL, "\t-x2\texplain more\n");
    print_msg(PRT_NORMAL, "\t-u\tshow usage stats\n");
  } /* else */

  return(0);
} /* isdnrate */
