/* $Id: isdnrate.c,v 1.4 1999/06/30 20:53:28 akool Exp $
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

#define WIDTH   13
#define MAXLAST	 5

static char *myname, *myshortname;
static char  options[] = "Vvd:hb:s:t";
static char  usage[]   = "%s: usage: %s [ -%s ] Destination ...\n";

static int    verbose = 0, header = 0, best = MAXPROVIDER, table = 0;
static int    duration = LCR_DURATION;
static time_t start;
static int    day, month, year, hour, min;
static char   country[BUFSIZ], area[BUFSIZ], msn[BUFSIZ];


typedef struct {
  int    prefix;
  double rate;
  char  *explain;
} SORT;

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
  /* myicountry = atoi(mycountry + strlen(countryprefix)); */
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

      case 'd' : duration = strtol(optarg, NIL, 0);
      	       	 break;

      case 'h' : header++;
      	       	 break;

      case 'b' : best = strtol(optarg, NIL, 0);
      	       	 break;

      case 's' : day = atoi(optarg);
      	       	 if ((p = strchr(optarg, '/'))) {
                   month = atoi(p + 1);

                   if ((p = strchr(p + 1, '/'))) {
                     year = atoi(p + 1);

           	     if (year < 100)
                       year += 1900; /* JA, JA!! */

                     if ((p = strchr(p + 1, '/'))) {
                       hour = atoi(p + 1);

                       if ((p = strchr(p + 1, '/')))
                         min = atoi(p + 1);
                     }
                   }
      	       	 }
                 break;

      case 't' : table++;
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


static char *printrate(RATE Rate)
{
  static char message[BUFSIZ];


  if (Rate.Basic > 0)
    sprintf(message, "%s + %s/%3.1fs = %s + %s/Min (%s)",
      printRate(Rate.Basic),
      printRate(Rate.Price),
      Rate.Duration,
      printRate(Rate.Basic),
      printRate(60 * Rate.Price / Rate.Duration),
      explainRate(&Rate));
  else
    sprintf(message, "%s/%3.1fs = %s/Min (%s)",
      printRate(Rate.Price),
      Rate.Duration,
      printRate(60 * Rate.Price / Rate.Duration),
      explainRate(&Rate));

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


static void numsplit(char *num, char *country, char *area, char *msn)
{
  register int   l1;
  auto	   int	 l2;
  register char *p;
  auto	   char *s;


  *country = *area = *msn = 0;

  if ((l1 = getCountrycode(num, &s)) != UNKNOWN) {
    Strncpy(country, num, l1 + 1);

    if (verbose)
      print_msg(PRT_NORMAL, " Country %s : %s\n", country, s);

    if ((p = get_areacode(num, &l2, C_NO_WARN | C_NO_EXPAND | C_NO_ERROR))) {
      Strncpy(area, num + l1, l2 + 1 - l1);

      if (verbose)
        print_msg(PRT_NORMAL, " Area %s : %s\n", area, p);

      strcpy(msn, num + l2);

      if (verbose)
        print_msg(PRT_NORMAL, " Number %s\n", msn);
    } /* if */

#if 0
    num += i;

    p = country;

    while (*p && !isdigit(*p))
      p++;

    icountry = atoi(p);
    l = getAreacode(icountry, num, s);

    print_msg(PRT_NORMAL, "l=%d, icountry=%d, num=``%s'', s=``%s''\n", l, icountry, num, s);
#endif
  } /* if */

} /* numsplit */


static int normalizeNumber(char *target)
{
  auto COUNTRY *Country;
  auto char     num[BUFSIZ];


  if (isalpha(*target)) {
    if (getCountry(target, &Country) != UNKNOWN)
      strcpy(num, Country->Code[0]);
    else {
      print_msg(PRT_NORMAL, "Unknown country \"%s\"\n", target);
      return(0);
    } /* else */
  }
  else
    strcpy(num, target);

  numsplit(num, country, area, msn);
  return(1);
} /* normalizeNumber */


static int compute()
{
  register int  i, n = 0;
  auto 	   RATE Rate;


  for (i = 0; i < MAXPROVIDER; i++) {
    clearRate(&Rate);
    Rate.src[0] = mycountry;
    Rate.src[1] = myarea;
    Rate.src[2] = "";

    Rate.dst[0] = country;
    Rate.dst[1] = area;
    Rate.dst[2] = msn;

    Rate.prefix = i;

    buildtime();

    Rate.start = start;
    Rate.now   = start + duration;

    if (!getRate(&Rate, NULL)) {
      sort[n].prefix = Rate.prefix;
      sort[n].rate = Rate.Charge;
      sort[n].explain = strdup(printrate(Rate));

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
    print_msg(PRT_NORMAL, "%s%02d %s %8.3f (%s)\n", vbn, sort[i].prefix, currency, sort[i].rate, sort[i].explain);
} /* result */


static void purge(int n)
{
  register int i;


  for (i = 0; i < n; i++)
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
  register int        n, d, i, lasthour;
  register char      *px;
  auto 	   struct tm *tm;
  auto	   SORT	      last[MAXLAST];
  auto 	   int        used[MAXPROVIDER];
  auto 	   int        hours[MAXPROVIDER];


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
      print_msg(PRT_NORMAL, "\n%s:\n", d ? "Werktag" : "Wochende");

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
          if (last[i].prefix == UNKNOWN)
            px = "";
          else
            px = getProvider(last[i].prefix);

          if (!i)
            print_msg(PRT_NORMAL, "    %02d:00 .. %02d:59 %s%02d:%s%*s = %s %s (%s)\n",
              lasthour, hour - 1, vbn, last[i].prefix, px,
              max(WIDTH, strlen(px)) - strlen(px), "", currency,
              double2str(last[i].rate, 5, 3, DEB),
              last[i].explain);
          else
            print_msg(PRT_NORMAL, "                   %s%02d:%s%*s = %s %s (%s)\n",
              vbn, last[i].prefix, px,
              max(WIDTH, strlen(px)) - strlen(px), "", currency,
              double2str(last[i].rate, 5, 3, DEB),
              last[i].explain);
        } /* for */

        used[last[0].prefix] = 1;

        if (lasthour >= hour)
          hours[last[0].prefix] += ((24 - lasthour) + hour);
        else
          hours[last[0].prefix] += hour - lasthour;

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
      if (last[i].prefix == UNKNOWN)
        px = "";
      else
        px = getProvider(last[i].prefix);

      if (!i)
        print_msg(PRT_NORMAL, "    %02d:00 .. %02d:59 %s%02d:%s%*s = %s %s (%s)\n",
          lasthour, hour - 1, vbn, last[i].prefix, px,
          max(WIDTH, strlen(px)) - strlen(px), "", currency,
          double2str(last[i].rate, 5, 3, DEB),
          last[i].explain);
      else
        print_msg(PRT_NORMAL, "                   %s%02d:%s%*s = %s %s (%s)\n",
          vbn, last[i].prefix, px,
          max(WIDTH, strlen(px)) - strlen(px), "", currency,
          double2str(last[i].rate, 5, 3, DEB),
          last[i].explain);
#if 0
    if ((lasthour == 7) && (hour == 7))
      print_msg(PRT_NORMAL, "    immer          %s%02d:%s%*s = %s %s (%s)\n",
        vbn, last[0].prefix, px, max(WIDTH, strlen(px)) - strlen(px), "",
        currency, double2str(last[0].rate, 5, 3, DEB), last[0].explain);
    else
      print_msg(PRT_NORMAL, "    %02d:00 .. %02d:59 %s%02d:%s%*s = %s %s (%s)\n",
        lasthour, hour - 1, vbn, last[0].prefix, px, max(WIDTH, strlen(px)) - strlen(px), "",
        currency, double2str(last[0].rate, 5, 3, DEB), last[0].explain);
#endif
    } /* for */

    used[last[0].prefix] = 1;

    if (lasthour >= hour)
      hours[last[0].prefix] += ((24 - lasthour) + hour);
    else
      hours[last[0].prefix] += hour - lasthour;
  } /* for */
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

#if 0
    print_msg(PRT_NORMAL, "currency=%s\n", currency);
    print_msg(PRT_NORMAL, "vbn=%s\n", vbn);
    print_msg(PRT_NORMAL, "mycountry=%s\n", mycountry);
    print_msg(PRT_NORMAL, "myarea=%s\n", myarea);
    print_msg(PRT_NORMAL, "countryprefix=%s\n", countryprefix);
    print_msg(PRT_NORMAL, "mynum=%s\n", mynum);
    print_msg(PRT_NORMAL, "myicountry=%d\n", myicountry);
#endif

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
    print_msg(PRT_NORMAL, "\t-d duration\t duration of call in seconds (default %d seconds)\n", LCR_DURATION);
    print_msg(PRT_NORMAL, "\t-h\t\tshow a header\n");
    print_msg(PRT_NORMAL, "\t-t\t\tshow a table\n");
    print_msg(PRT_NORMAL, "\t-b best\tshow only the first <best> provider(s) (default %d)\n", MAXPROVIDER);
    print_msg(PRT_NORMAL, "\t-s d/m/y/h/m\tstart of call (default now)\n");
  } /* else */

  return(0);
} /* isdnrate */
