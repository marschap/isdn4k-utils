/* $Id: isdnrate.c,v 1.1 1999/06/28 19:16:33 akool Exp $
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
 * Revision 1.1  1999/06/28 19:16:33  akool
 * isdnlog Version 3.38
 *   - new utility "isdnrate" started
 *

#include "isdnlog.h"


static char *myname, *myshortname;
static char  options[] = "Vvd:hb:";
static char  usage[]   = "%s: usage: %s [ -%s ] Target\n";

static int verbose = 0, header = 0, best = MAXPROVIDER;
static int duration = TESTDURATION;

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
  myshortname = basename(myname);

  preselect = DTAG;      /* Telekomik */
  vbn = strdup("010"); 	 /* Germany */
} /* pre_init */


static void init()
{
  auto char *version;


  if (readconfig(myshortname) < 0)
    exit(1);

  initHoliday(holifile, &version);

  if (*version && verbose)
    print_msg(PRT_NORMAL, "%s\n", version);

  initCountry(countryfile, &version);

  if (*version && verbose)
    print_msg(PRT_NORMAL, "%s\n", version);

  initRate(rateconf, ratefile, zonefile, &version);

  if (*version && verbose)
    print_msg(PRT_NORMAL, "%s\n", version);
} /* init */


static void post_init()
{
  auto char s[BUFSIZ];


  sprintf(s, "%s%s", mycountry, myarea);
  mynum = strdup(s);
  myicountry = atoi(mycountry + strlen(countryprefix));
} /* post_init */


static char *opts(int argc, char *argv[])
{
  register int  c;


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

      case '?' : print_msg(PRT_ERR, usage, myshortname, myshortname, options);
      	       	 break;
    } /* switch */
  } /* while */

  if (optind < argc)
    return(argv[optind]);
  else
    return(NULL);
} /* opts */


static int compare(const void *s1, const void *s2)
{
  return(((SORT *)s1)->rate > ((SORT *)s2)->rate);
} /* compare */


static char *printrate(RATE Rate)
{
  static char message[BUFSIZ];


  if (Rate.Basic > 0)
    sprintf(message, "%s %s + %s/%3.1fs = %s %s + %s/Min (%s)",
      currency, double2str(Rate.Basic, 5, 3, DEB),
      double2str(Rate.Price, 5, 3, DEB),
      Rate.Duration,
      currency, double2str(Rate.Basic, 5, 3, DEB),
      double2str(60 * Rate.Price / Rate.Duration, 5, 3, DEB),
      explainRate(&Rate));
  else
    sprintf(message, "%s %s/%3.1fs = %s %s/Min (%s)",
      currency, double2str(Rate.Price, 5, 3, DEB),
      Rate.Duration,
      currency, double2str(60 * Rate.Price / Rate.Duration, 5, 3, DEB),
      explainRate(&Rate));

  return(message);
} /* printrate */


static void compute(char *target)
{
  register int i, n = 0;
  auto char     areacode[BUFSIZ], *s;
  auto COUNTRY *Country;
  auto RATE     Rate;
  auto time_t   now;


  if (isalpha(*target)) {
    if (getCountry(target, &Country) != UNKNOWN)
      strcpy(areacode, Country->Code[0]);
    else
      print_msg(PRT_NORMAL, "Unknown country \"%s\"\n", target);
  }
  else {
    strcpy(areacode, target);
#if 0
    if (getCountrycode(areacode, &s) != UNKNOWN)
      target = *s;
#endif
  } /* else */

  time(&now);
  n = 0;

  for (i = 0; i < MAXPROVIDER; i++) {
    clearRate(&Rate);
    Rate.src = mynum;
    Rate.dst = areacode;
    Rate.prefix = i;
    Rate.start = now;
    Rate.now   = now + duration;

    if (!getRate(&Rate, NULL)) {
      sort[n].prefix = Rate.prefix;
      sort[n].rate = Rate.Charge;
      sort[n].explain = strdup(printrate(Rate));

      n++;
    } /* if */
  } /* for */

  qsort((void *)sort, n, sizeof(SORT), compare);

  if (header)
    print_msg(PRT_NORMAL, "Ein %d Sekunden langes Gespraech nach %s (%s) kostet am %s\n",
      duration, target, areacode, ctime(&now));

  if (n > best)
    n = best;

  for (i = 0; i < n; i++)
    print_msg(PRT_NORMAL, "%s%02d %s %8.3f (%s)\n", vbn, sort[i].prefix, currency, sort[i].rate, sort[i].explain);
} /* compute */


int main(int argc, char *argv[], char *envp[])
{
  register char *p;


  myname = argv[0];

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

  if ((p = opts(argc, argv))) {
    compute(p);
  }
  else {
    print_msg(PRT_NORMAL, usage, myshortname, myshortname, options);
    print_msg(PRT_NORMAL, "\n");
    print_msg(PRT_NORMAL, "\t-V\t\tshow version infos\n");
    print_msg(PRT_NORMAL, "\t-v\t\tverbose\n");
    print_msg(PRT_NORMAL, "\t-d duration\t duration of call in seconds (default %d seconds)\n", TESTDURATION);
    print_msg(PRT_NORMAL, "\t-h\t\tshow a header\n");
    print_msg(PRT_NORMAL, "\t-b best\tshow only the first <best> provider(s) (default %d)\n", MAXPROVIDER);
  } /* else */

  return(0);
} /* isdnrate */
