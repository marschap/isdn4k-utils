/* $Id: isdnbill.c,v 1.6 1999/12/02 19:28:02 akool Exp $
 *
 * ISDN accounting for isdn4linux. (Billing-module)
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
 * Revision 1.1  1995/09/23  16:44:19  akool
 * Initial revision
 *
 */

#include "isdnlog.h"
#include "tools/zone.h"
#include <unistd.h>
#include <asm/param.h>
#include <math.h>
#include "dest.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

#define DEBUG

#define CALLING          0
#define CALLED           1

#define MAXMYMSN        20
#define MAXSI           10

#define FREECALL         0
#define ORTSZONE         1
#define CITYCALL         2
#define REGIOCALL        3
#define GERMANCALL       4
#define SONDERRUFNUMMERN 5
#define AUSLAND          6
#define ELSEWHERE        7
#define MAXZONE          8

#define SUBTOTAL         0
#define TOTAL            1

#define COUNTRYLEN      32
#define PROVLEN         20
#define MSNLEN           6

#undef  ME
#undef  OTHER

#define ME     (c.dialout ? CALLING : CALLED)
#define OTHER  (c.dialout ? CALLED : CALLING)

typedef struct {
  char   num[2][64];
  double duration;
  time_t connect;
  int    units;
  int    dialout;
  int    cause;
  int    ibytes;
  int    obytes;
  char   version[64];
  int    si1;
  int    si2;
  double currency_factor;
  char   currency[64];
  double pay;
  int    provider;

  double compute;
  int    computed;
  double aktiv;
  char   country[BUFSIZ];
  char   sprovider[BUFSIZ];
  char   error[BUFSIZ];
  int    zone;
  int    ihome;
  int    known[2];
} CALLER;

typedef struct {
  char    msn[10];
  int     ncalls;
  double  pay;
  double  duration;
  double  compute;
  double  aktiv;
  char   *alias;
} MSNSUM;

typedef struct {
  int    ncalls;
  double pay;
  double duration;
  int    failed;
  double compute;
  double aktiv;
} PROVSUM;

typedef struct {
  int    ncalls;
  double pay;
  double duration;
  double compute;
  double aktiv;
} ZONESUM;

typedef struct {
  char   msn[32];
  int    ncalls;
  double pay;
  double duration;
  double compute;
} PARTNER;

static char  options[]   = "nv:V";
static char  usage[]     = "%s: usage: %s [ -%s ]\n";


static CALLER   c;
static MSNSUM   msnsum[2][MAXSI][MAXMYMSN];
static PROVSUM  provsum[2][MAXPROVIDER];
static ZONESUM  zonesum[2][MAXZONE];
static PARTNER *partner[2];
static PARTNER *unknown[2];
static TELNUM   number[2];
static char     rstr[BUFSIZ];
static char     home[BUFSIZ];
static int      nhome = 0;
static int	nunknown[2] = { 0, 0 };

int verbose = 0;

static int      onlynumbers = 0;


int print_msg(int Level, const char *fmt, ...)
{
  auto va_list ap;
  auto char    String[BUFSIZ * 3];


  if ((Level > 1 && !verbose) || ((Level > 2) && (verbose < 2)))
    return(1);

  va_start(ap, fmt);
  (void)vsnprintf(String, BUFSIZ * 3, fmt, ap);
  va_end(ap);

  fprintf((Level == PRT_NORMAL) ? stdout : stderr, "%s", String);

  return(0);
} /* print_msg */


int print_in_modules(const char *fmt, ...)
{
  auto va_list ap;
  auto char    String[LONG_STRING_SIZE];


  va_start(ap, fmt);
  (void)vsnprintf(String, LONG_STRING_SIZE, fmt, ap);
  va_end(ap);

  return print_msg(PRT_ERR, "%s", String);
} /* print_in_modules */


static void when(char *s, int *day, int *month)
{
  auto struct tm *tm = localtime(&c.connect);


  *day = tm->tm_mday;
  *month = tm->tm_mon + 1;

  sprintf(s, "%02d.%02d.%04d %02d:%02d:%02d",
    *day, *month, tm->tm_year + 1900, tm->tm_hour, tm->tm_min, tm->tm_sec);
} /* when */


static void deb(char *s)
{
  register char *p = strchr(s, 0);


  while (*--p == ' ')
    *p = 0;
} /* deb */


char *timestr(double n)
{
  auto int x, h, m, s;
#if 0
  auto int d = 0;
#endif


  if (n <= 0.0)
    sprintf(rstr, "         ");
  else {
    x = (int)n;

    h = (int)(x / 60 / 60);
    x %= 60 * 60;
    m = (int)(x / 60);
    s = (int)(x % 60);

#if 0
    if (h > 99) {
      while (h > 99) {
        h -= 24;
        d++;
      } /* while */

      sprintf(rstr, "%d day(s), %2d:%02d:%02d", d, h, m, s);
    }
    else
#endif
      sprintf(rstr, "%3d:%02d:%02d", h, m, s);
  } /* else */

  return(rstr);
} /* timestr */


static void strich(char c, int len)
{
  auto char s[BUFSIZ];


  memset(s, c, len);
  s[len] = 0;

  printf(s);
  printf("\n");
} /* strich */


static void total(int w)
{
  register int i, j;
  auto     int ncalls = 0, failed = 0;
  auto     double duration = 0.0, pay = 0.0, compute = 0.0, aktiv = 0.0;


  printf("\n\nMSN                   calls  Duration  Charge       Computed\n");
  strich('-', 63);

  for (i = 0; i < nhome; i++) {
    for (j = 0; j < MAXSI; j++) {
      if (msnsum[w][j][i].ncalls) {
        printf("%6s,%d %-12s %5d %s  DM %8.3f  DM %8.3f\n",
          msnsum[w][j][i].msn,
          j,
          msnsum[w][j][i].alias,
          msnsum[w][j][i].ncalls,
          timestr(msnsum[w][j][i].duration),
          msnsum[w][j][i].pay,
          msnsum[w][j][i].compute);

        ncalls   += msnsum[w][j][i].ncalls;
        duration += msnsum[w][j][i].duration;
        pay      += msnsum[w][j][i].pay;
        compute  += msnsum[w][j][i].compute;
      } /* if */

      if (w == SUBTOTAL) {
        strcpy(msnsum[TOTAL][j][i].msn, msnsum[SUBTOTAL][j][i].msn);
        msnsum[TOTAL][j][i].alias = msnsum[SUBTOTAL][j][i].alias;
        msnsum[TOTAL][j][i].ncalls   += msnsum[SUBTOTAL][j][i].ncalls;
        msnsum[TOTAL][j][i].duration += msnsum[SUBTOTAL][j][i].duration;
        msnsum[TOTAL][j][i].pay      += msnsum[SUBTOTAL][j][i].pay;
        msnsum[TOTAL][j][i].compute  += msnsum[SUBTOTAL][j][i].compute;

        msnsum[SUBTOTAL][j][i].ncalls = 0;
        msnsum[SUBTOTAL][j][i].duration = 0;
        msnsum[SUBTOTAL][j][i].pay = 0;
        msnsum[SUBTOTAL][j][i].compute = 0;
      } /* if */
    } /* for */
  } /* for */

  strich('=', 63);

  printf("                      %5d %s  DM %8.3f  DM %8.3f\n",
    ncalls,
    timestr(duration),
    pay,
    compute);


  ncalls = 0;
  failed = 0;
  duration = 0.0;
  pay = 0.0;
  compute = 0.0;

  printf("\n\nProvider                       calls  Duration  Charge      Computed    failures  avail\n");
  strich('-', 87);

  for (i = 0; i < MAXPROVIDER; i++) {
    if (provsum[w][i].ncalls) {
      if (i < 100)
        printf(" %s%02d", vbn, i);
      else
        printf("%s%03d", vbn, i - 100);

      printf(":%-24s", getProvider(pnum2prefix(i, c.connect)));

      printf("%5d %s  DM %8.3f DM %8.3f %8d %5.1f%%\n",
        provsum[w][i].ncalls,
        timestr(provsum[w][i].duration),
        provsum[w][i].pay,
        provsum[w][i].compute,
        provsum[w][i].failed,
        100.0 * (provsum[w][i].ncalls - provsum[w][i].failed) / provsum[w][i].ncalls);

      ncalls   += provsum[w][i].ncalls;
      duration += provsum[w][i].duration;
      pay      += provsum[w][i].pay;
      compute  += provsum[w][i].compute;
      failed   += provsum[w][i].failed;

      if (w == SUBTOTAL) {
        provsum[TOTAL][i].ncalls   += provsum[SUBTOTAL][i].ncalls;
        provsum[TOTAL][i].duration += provsum[SUBTOTAL][i].duration;
        provsum[TOTAL][i].pay      += provsum[SUBTOTAL][i].pay;
        provsum[TOTAL][i].failed   += provsum[SUBTOTAL][i].failed;
        provsum[TOTAL][i].compute  += provsum[SUBTOTAL][i].compute;

        provsum[SUBTOTAL][i].ncalls = 0;
        provsum[SUBTOTAL][i].duration = 0;
        provsum[SUBTOTAL][i].pay = 0;
        provsum[SUBTOTAL][i].failed = 0;
        provsum[SUBTOTAL][i].compute = 0;
      } /* if */
    } /* if */
  } /* for */

  strich('=', 87);

  printf("%*s%5d %s  DM %8.3f DM %8.3f %8d %5.1f%%\n",
    31, "",
    ncalls,
    timestr(duration),
    pay,
    compute,
    failed,
    100.0 * (ncalls - failed) / ncalls);


  ncalls = 0;
  duration = 0.0;
  pay = 0.0;
  compute = 0.0;
  aktiv = 0;

  printf("\n\nZone           calls  Duration   Charge       Computed    Einsparung AktivPlus\n");
  strich('-', 78);

  for (i = 0; i < MAXZONE; i++) {
    if (zonesum[w][i].ncalls) {
      switch (i) {
        case 0 : printf("FreeCall        "); break;
        case 1 : printf("Ortszone        "); break;
        case 2 : printf("CityCall (R20)  "); break;
        case 3 : printf("RegioCall (R50) "); break;
        case 4 : printf("GermanCall      "); break;
        case 5 : printf("Sonderrufnummern"); break;
        case 6 : printf("Ausland         "); break;
        case 7 : printf("elsewhere       "); break;
      } /* switch */

      printf("%5d %s  DM %8.3f  DM %8.3f  DM %8.3f\n",
        zonesum[w][i].ncalls,
        timestr(zonesum[w][i].duration),
        zonesum[w][i].pay,
        zonesum[w][i].compute,
        zonesum[w][i].aktiv);

      ncalls   += zonesum[w][i].ncalls;
      duration += zonesum[w][i].duration;
      pay      += zonesum[w][i].pay;
      compute  += zonesum[w][i].compute;
      aktiv    += zonesum[w][i].aktiv;
    } /* if */

    if (w == SUBTOTAL) {
      zonesum[TOTAL][i].ncalls   += zonesum[SUBTOTAL][i].ncalls;
      zonesum[TOTAL][i].duration += zonesum[SUBTOTAL][i].duration;
      zonesum[TOTAL][i].pay      += zonesum[SUBTOTAL][i].pay;
      zonesum[TOTAL][i].compute  += zonesum[SUBTOTAL][i].compute;
      zonesum[TOTAL][i].aktiv    += zonesum[SUBTOTAL][i].aktiv;

      zonesum[SUBTOTAL][i].ncalls = 0;
      zonesum[SUBTOTAL][i].duration = 0;
      zonesum[SUBTOTAL][i].pay = 0;
      zonesum[SUBTOTAL][i].compute = 0;
      zonesum[SUBTOTAL][i].aktiv = 0;
    } /* if */
  } /* for */

  strich('=', 78);

  printf("%*s%5d %s  DM %8.3f  DM %8.3f  DM %8.3f\n",
    16, "",
    ncalls,
    timestr(duration),
    pay,
    compute,
    aktiv);

  printf("\n\n");
} /* total */


static void showpartner()
{
  register int i, k;


  for (k = 0; k < 2; k++) {
    switch (k) {
      case 0 : printf("\nAnrufer (DIALIN):\n");             break;
      case 1 : printf("\nAngerufene Partner (DIALOUT):\n"); break;
    } /* switch */

    for (i = 0; i < knowns; i++)
      if (partner[k][i].ncalls)
        printf("%-16s %-25s %5d %10.3f %s %10.3f\n",
          partner[k][i].msn,
          known[i]->who,
          partner[k][i].ncalls,
          partner[k][i].pay,
          timestr(partner[k][i].duration),
          partner[k][i].compute);

    printf("\n");

    for (i = 0; i < nunknown[k]; i++)
      if (unknown[k][i].ncalls)
        printf("%-16s %-25s %5d %10.3f %s %10.3f\n",
          unknown[k][i].msn,
          "",
          unknown[k][i].ncalls,
          unknown[k][i].pay,
          timestr(unknown[k][i].duration),
          unknown[k][i].compute);
  } /* for */
} /* showpartner */


static char *numtonam(int n)
{
  register int i, j = -1;


  if (onlynumbers)
    return(NULL);

  for (i = 0; i < knowns; i++) {
    if (!num_match(known[i]->num, c.num[n])) {
      j = i;

      if (known[i]->si == c.si1) {
        c.known[n] = i;
        return(known[i]->who);
      } /* if */
    } /* if */
  } /* for */

  c.known[n] = j;

  return((j == -1) ? NULL : known[j]->who);
} /* numtonam */


static void justify(char *fromnum, char *tonum, TELNUM number)
{
  register char *p1, *p2;
  auto     char  s[BUFSIZ], sx[BUFSIZ], sy[BUFSIZ];


  p1 = numtonam(c.dialout ? CALLED : CALLING);

  if (*number.msn)
    sprintf(sx, "%s%s%s", number.area, (*number.area ? "/" : ""), number.msn);
  else
    strcpy(sx, number.area);

  if (*number.country && strcmp(number.country, mycountry))
    sprintf(s, "%s %s", number.country, sx);
  else
    sprintf(s, "%s%s", (*number.country ? "0" : ""), sx);

  p2 = msnsum[SUBTOTAL][c.si1][c.ihome].alias;

  printf("%12s %s %-21s",
    p2 ? p2 : fromnum,
    (c.dialout ? "->" : "<-"),
    p1 ? p1 : s);

  *s = 0;

  if (getSpecial(tonum))
    sprintf(s, "%s", getSpecialName(tonum));
  else {
    if (*number.country && strcmp(number.country, mycountry))
      sprintf(s, "%s", number.scountry);
    else if (*number.scountry && strcmp(number.country, mycountry))
      sprintf(s, "%s", number.scountry);

    if (*number.sarea) {
      if (*s)
        strcat(s, ", ");

      strcat(s, number.sarea);

      if (c.dialout) {
        sprintf(sy, ",%d", c.zone);
        strcat(s, sy);
      } /* if */
    } /* if */

    if (!*s)
      sprintf(s, "??? TARGET? (%s,%s,%s)", number.country, number.scountry, number.sarea);
  } /* else */

  s[COUNTRYLEN] = 0; /* clipping */

  sprintf(c.country, "%-*s", COUNTRYLEN, s);
} /* justify */


static void findme()
{
  register int   i;
  register char *p;
  auto     char  s[BUFSIZ];


  sprintf(s, "%*s", -MSNLEN, number[ME].msn);
  s[MSNLEN] = 0;

  p = strstr(home, s);

  if (p == NULL) {
    strcat(home, s);
    strcat(home, "|");

    p = strstr(home, s);
    c.ihome = (int)(p - home) / 7;

    i = c.si1;

    for (c.si1 = 0; c.si1 < MAXSI; c.si1++) {
      strcpy(msnsum[SUBTOTAL][c.si1][c.ihome].msn, number[ME].msn);
      msnsum[SUBTOTAL][c.si1][c.ihome].alias = numtonam(ME);
    } /* for */

    c.si1 = i;

    nhome++;
  } /* if */

  c.ihome = (int)(p - home) / 7;
} /* findme */


static void findrate()
{
  auto RATE  Rate;
  auto char *version;


  clearRate(&Rate);
  Rate.start  = c.connect;
  Rate.now    = c.connect + c.duration;
  Rate.prefix = pnum2prefix(c.provider, c.connect);

  Rate.src[0] = number[CALLING].country;
  Rate.src[1] = number[CALLING].area;
  Rate.src[2] = number[CALLING].msn;

  Rate.dst[0] = number[CALLED].country;
  Rate.dst[1] = number[CALLED].area;
  Rate.dst[2] = number[CALLED].msn;

  if (getRate(&Rate, &version) != UNKNOWN) {
    c.zone = Rate.z;

    if (!c.zone && (*number[CALLED].country && strcmp(number[CALLED].country, mycountry)))
      c.zone = AUSLAND;
    else if (getSpecial(c.num[CALLED]))
      c.zone = SONDERRUFNUMMERN;
    else if (c.zone == UNKNOWN)
      c.zone = ELSEWHERE;

    c.compute = Rate.Charge;
  }
  else {
    c.zone = ELSEWHERE;
    c.compute = c.pay;
    sprintf(c.error, " ??? %s", version);
  } /* else */

  zonesum[SUBTOTAL][c.zone].ncalls++;
} /* findrate */


int main(int argc, char *argv[], char *envp[])
{
  register char    *pl, *pr, *p, x;
#ifdef DEBUG
  auto     FILE    *f = fopen("/www/log/isdn.log", "r");
#else
  auto     FILE    *f = fopen("/var/log/isdn.log", "r");
#endif
  auto     char     s[BUFSIZ], sx[BUFSIZ];
  auto     int      i, l, col, day, lday = -1, month, lmonth = -1;
  auto     double   dur;
  auto     char    *version;
  auto     char    *myname = basename(argv[0]);
  auto     int      opt;


  if (f != (FILE *)NULL) {

    while ((opt = getopt(argc, argv, options)) != EOF)
      switch (opt) {
        case 'n' : onlynumbers++;
                   break;

        case 'v' : verbose = atoi(optarg);
                   break;

        case 'V' : print_version(myname);
                   exit(0);

        case '?' : printf(usage, argv[0], argv[0], options);
                   return(1);
      } /* switch */


    *home = 0;

    set_print_fct_for_tools(print_in_modules);

    if (!readconfig(myname)) {

      initHoliday(holifile, &version);

      if (verbose)
        fprintf(stderr, "%s\n", version);

      initDest(destfile, &version);

      if (verbose)
        fprintf(stderr, "%s\n", version);

      initRate(rateconf, ratefile, zonefile, &version);

      if (verbose)
        fprintf(stderr, "%s\n", version);


      memset(&msnsum, 0, sizeof(msnsum));
      memset(&provsum, 0, sizeof(provsum));
      memset(&zonesum, 0, sizeof(zonesum));

      partner[0] = (PARTNER *)calloc(knowns, sizeof(PARTNER));
      partner[1] = (PARTNER *)calloc(knowns, sizeof(PARTNER));

      while (fgets(s, BUFSIZ, f)) {
        pl = s;
        col = 0;

        memset(&c, 0, sizeof(c));

        while ((pr = strchr(pl, '|'))) {
          memcpy(sx, pl, (l = (pr - pl)));
          sx[l] = 0;
          pl = pr + 1;

          switch (col++) {
            case  0 :                               break;

            case  1 : deb(sx);
                      strcpy(c.num[CALLING], sx);
                      break;

            case  2 : deb(sx);
                      strcpy(c.num[CALLED], sx);
                      break;

            case  3 : dur = atoi(sx);               break;

            case  4 : c.duration = atol(sx) / HZ;
                      break;

            case  5 : c.connect = atol(sx);         break;
            case  6 : c.units = atoi(sx);           break;
            case  7 : c.dialout = *sx == 'O';       break;
            case  8 : c.cause = atoi(sx);           break;
            case  9 : c.ibytes = atoi(sx);          break;
            case 10 : c.obytes = atoi(sx);          break;
            case 11 : strcpy(c.version, sx);        break;
            case 12 : c.si1 = atoi(sx);             break;
            case 13 : c.si2 = atoi(sx);             break;
            case 14 : c.currency_factor = atof(sx); break;
            case 15 : strcpy(c.currency, sx);       break;
            case 16 : c.pay = atof(sx);             break;
            case 17 : c.provider = atoi(sx);        break;
            case 18 :                               break;
          } /* switch */

        } /* while */


        /* Repair wrong entries from older (or current?) isdnlog-versions ... */

        if (abs((int)dur - (int)c.duration) > 1) {
          if (verbose)
            fprintf(stderr, "REPAIR: Duration %f -> %f\n", c.duration, dur);

          c.duration = dur;
        } /* if */

        if (!memcmp(c.num[CALLED], "+4910", 5)) {
          p = c.num[CALLED] + 7;
          x = *p;
          *p = 0;

          c.provider = atoi(c.num[CALLED] + 5);

          *p = x;

          memmove(c.num[CALLED] + 3, c.num[CALLED] + 8, strlen(c.num[CALLED]) - 7);

          if (verbose)
            fprintf(stderr, "REPAIR: Provider=%d\n", c.provider);
        } /* if */

        if (!c.provider || (c.provider == UNKNOWN)) {
          if (verbose)
            fprintf(stderr, "REPAIR: Provider %d -> %d\n", c.provider, preselect);

          c.provider = preselect;
        } /* if */

        if (c.dialout && (strlen(c.num[CALLED]) > 3) && !getSpecial(c.num[CALLED])) {
          sprintf(s, "0%s", c.num[CALLED] + 3);

          if (getSpecial(s)) {
            if (verbose)
              fprintf(stderr, "REPAIR: Callee %s -> %s\n", c.num[CALLED], s);

            strcpy(c.num[CALLED], s);
          } /* if */
        } /* if */

        if (!c.dialout && (strlen(c.num[CALLING]) > 3) && !getSpecial(c.num[CALLING])) {
          sprintf(s, "0%s", c.num[CALLING] + 3);

          if (getSpecial(s)) {
            if (verbose)
              fprintf(stderr, "REPAIR: Caller %s -> %s\n", c.num[CALLING], s);

            strcpy(c.num[CALLING], s);
          } /* if */
        } /* if */

        /* if (c.dialout) */
        /* if (c.duration) */
        /* if (c.provider != UNKNOWN) */

        if (*c.num[CALLING] && *c.num[CALLED]) {

          when(s, &day, &month);

          if (lmonth == -1)
            lmonth = month;
          else if (month != lmonth) {
            total(SUBTOTAL);
            lmonth = month;
          } /* if */

          if (lday == -1)
            lday = day;
          else if (day != lday) {
            printf("\n");
            lday = day;
          } /* else */

          printf("%s%s ", s, timestr(c.duration));

          normalizeNumber(c.num[CALLING], &number[CALLING], TN_ALL);
          normalizeNumber(c.num[CALLED], &number[CALLED], TN_ALL);

          findme();

          if (c.dialout) {

            findrate();

            msnsum[SUBTOTAL][c.si1][c.ihome].ncalls++;

            justify(number[CALLING].msn, c.num[CALLED], number[CALLED]);

            provsum[SUBTOTAL][c.provider].ncalls++;

            strcpy(s, getProvider(pnum2prefix(c.provider, c.connect)));
            s[PROVLEN] = 0;

            if (c.provider < 100)
              sprintf(c.sprovider, " %s%02d:%-*s", vbn, c.provider, PROVLEN, s);
            else
              sprintf(c.sprovider, "%s%03d:%-*s", vbn, c.provider - 100, PROVLEN, s);


            if (!c.duration) {
              printf("           %s%s", c.country, c.sprovider);

              if ((c.cause != 0x1f) && /* Normal, unspecified */
                  (c.cause != 0x10))   /* Normal call clearing */
                printf(" %s", qmsg(TYPE_CAUSE, VERSION_EDSS1, c.cause));

              if ((c.cause == 0x22) || /* No circuit/channel available */
                  (c.cause == 0x2a) || /* Switching equipment congestion */
                  (c.cause == 0x2f))   /* Resource unavailable, unspecified */
                provsum[SUBTOTAL][c.provider].failed++;

                printf("\n");
                continue;
            } /* if */

            if ((c.zone == 1) || (c.zone == 2)) {
              auto struct tm *tm = localtime(&c.connect);
              auto int        takte;
              auto double     price;


              takte = (c.duration + 59) / 60;

              if ((tm->tm_wday > 0) && (tm->tm_wday < 5)) {   /* Wochentag */
                if ((tm->tm_hour > 8) && (tm->tm_hour < 18))  /* Hauptzeit */
                  price = 0.06;
                else
                  price = 0.03;
              }
              else                                            /* Wochenende */
                price = 0.03;

              c.aktiv = takte * price;

              msnsum[SUBTOTAL][c.si1][c.ihome].aktiv += c.aktiv;
              provsum[SUBTOTAL][c.provider].aktiv += c.aktiv;
              zonesum[SUBTOTAL][c.zone].aktiv += c.aktiv;
            } /* if */

            if (c.pay < 0.0) { /* impossible! */
              c.pay = c.compute;
              c.computed++;
            } /* if */

            if (c.compute && fabs(c.pay - c.compute) > 1.00) {
              c.pay = c.compute;
              c.computed++;
            } /* if */


            if (c.pay)
              printf("%s%7.3f%s ", c.currency, c.pay, c.computed ? "*" : " ");
            else
              printf("           ");

            printf("%s%s%s", c.country, c.sprovider, c.error);

            if (c.aktiv)
              printf(" AktivPlus - %s%6.3f", c.currency, c.pay - c.aktiv);

            msnsum[SUBTOTAL][c.si1][c.ihome].pay += c.pay;
            msnsum[SUBTOTAL][c.si1][c.ihome].duration += c.duration;
            msnsum[SUBTOTAL][c.si1][c.ihome].compute += c.compute;

            provsum[SUBTOTAL][c.provider].pay += c.pay;
            provsum[SUBTOTAL][c.provider].duration += c.duration;
            provsum[SUBTOTAL][c.provider].compute += c.compute;

            zonesum[SUBTOTAL][c.zone].pay += c.pay;
            zonesum[SUBTOTAL][c.zone].duration += c.duration;
            zonesum[SUBTOTAL][c.zone].compute += c.compute;
          }
          else { /* Dialin: */
            justify(number[CALLED].msn, c.num[CALLING], number[CALLING]);
            printf("           %s%s", c.country, c.sprovider);
          } /* else */


	  if (c.known[OTHER] == UNKNOWN) {
            l = UNKNOWN;

            for (i = 0; i < nunknown[c.dialout]; i++) {
              if (!strcmp(unknown[c.dialout][i].msn, c.num[OTHER])) {
                l = i;
                break;
              } /* if */
            } /* for */

            if (l == UNKNOWN) {
              l = nunknown[c.dialout];

              nunknown[c.dialout]++;

              if (!l)
      	        unknown[c.dialout] = (PARTNER *)malloc(sizeof(PARTNER));
              else
      	        unknown[c.dialout] = (PARTNER *)realloc(unknown[c.dialout], sizeof(PARTNER) * nunknown[c.dialout]);

              *unknown[c.dialout][l].msn = 0;
	      unknown[c.dialout][l].ncalls = 0;
	      unknown[c.dialout][l].pay = 0.0;
	      unknown[c.dialout][l].duration = 0.0;
	      unknown[c.dialout][l].compute = 0.0;
            } /* if */

            strcpy(unknown[c.dialout][l].msn, c.num[OTHER]);
            unknown[c.dialout][l].ncalls++;
            unknown[c.dialout][l].pay += c.pay;
            unknown[c.dialout][l].duration += c.duration;
            unknown[c.dialout][l].compute += c.compute;
          }
          else {
            strcpy(partner[c.dialout][c.known[OTHER]].msn, c.num[OTHER]);
            partner[c.dialout][c.known[OTHER]].ncalls++;
            partner[c.dialout][c.known[OTHER]].pay += c.pay;
            partner[c.dialout][c.known[OTHER]].duration += c.duration;
            partner[c.dialout][c.known[OTHER]].compute += c.compute;
          } /* else */

          printf("\n");

        } /* if */
      } /* while */

      fclose(f);
      total(SUBTOTAL);
      total(TOTAL);

      showpartner();

    }
    else
      fprintf(stderr, "%s: Can't read configuration file(s)\n", myname);
  }
  else
    fprintf(stderr, "%s: Can't open \"isdn.log\" file\n", myname);

  return(0);
} /* isdnbill */
