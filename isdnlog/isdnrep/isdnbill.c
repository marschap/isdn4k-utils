/* $Id: isdnbill.c,v 1.4 1999/11/28 19:32:41 akool Exp $
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

#undef  DEBUG

#define CALLING          0
#define CALLED           1

#define MAXMYMSN        20

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

#define COUNTRYLEN      30
#define PROVLEN         20

typedef struct {
  char num[2][64];
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
  int    zone;

  double compute;
  int    computed;
  double aktiv;
  char   country[BUFSIZ];
  char   sprovider[BUFSIZ];
  char   error[BUFSIZ];
} CALLER;

typedef struct {
  char    msn[10];
  int     ncalls;
  double  pay;
  double  duration;
  double  compute;
  double  aktiv;
  char	 *alias[8];
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

static CALLER  curcall;
static MSNSUM  msnsum[2][MAXMYMSN];
static PROVSUM provsum[2][MAXPROVIDER];
static ZONESUM zonesum[2][MAXZONE];
static char    rstr[BUFSIZ];

static int     nhome = 0;
static int     homei = 0;

int verbose = 0;


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


static int when(time_t connect, char *s)
{
  auto struct tm *tm = localtime(&connect);


  sprintf(s, "%02d.%02d.%04d %02d:%02d:%02d",
    tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900,
    tm->tm_hour, tm->tm_min, tm->tm_sec);

  return(tm->tm_mon + 1);
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


static void total(int w)
{
  register int i;


  printf("------------------------------------------------------------------------------\n");

  printf("\nMSN    calls  Duration       Charge     Computed\n");
  printf("------------------------------------------------\n");

  for (i = 0; i < nhome; i++) {
    if (msnsum[w][i].ncalls)
      printf("%6s %5d %s  DM %8.3f  DM %8.3f\n",
        msnsum[w][i].msn,
        msnsum[w][i].ncalls,
        timestr(msnsum[w][i].duration),
        msnsum[w][i].pay,
        msnsum[w][i].compute);

    if (w == SUBTOTAL) {
      strcpy(msnsum[TOTAL][i].msn, msnsum[SUBTOTAL][i].msn);
      msnsum[TOTAL][i].ncalls   += msnsum[SUBTOTAL][i].ncalls;
      msnsum[TOTAL][i].duration += msnsum[SUBTOTAL][i].duration;
      msnsum[TOTAL][i].pay      += msnsum[SUBTOTAL][i].pay;
      msnsum[TOTAL][i].compute  += msnsum[SUBTOTAL][i].compute;

      msnsum[SUBTOTAL][i].ncalls = 0;
      msnsum[SUBTOTAL][i].duration = 0;
      msnsum[SUBTOTAL][i].pay = 0;
      msnsum[SUBTOTAL][i].compute = 0;
    } /* if */
  } /* for */


  printf("\nProvider                       calls  Duration  Charge      Computed    failures  avail\n");
  printf("---------------------------------------------------------------------------------------\n");

  for (i = 0; i < MAXPROVIDER; i++) {
    if (provsum[w][i].ncalls) {
      if (i < 100)
        printf(" %s%02d", vbn, i);
      else
        printf("%s%03d", vbn, i - 100);

      printf(":%-24s", getProvider(pnum2prefix(i, 0)));

      printf("%5d %s  DM %8.3f DM %8.3f %8d %5.1f%%\n",
        provsum[w][i].ncalls,
        timestr(provsum[w][i].duration),
        provsum[w][i].pay,
        provsum[w][i].compute,
        provsum[w][i].failed,
        100.0 * (provsum[w][i].ncalls - provsum[w][i].failed) / provsum[w][i].ncalls);


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


  printf("\nZone           calls  Duration   Charge       Computed    Einsparung AktivPlus\n");
  printf("------------------------------------------------------------------------------\n");

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
        zonesum[w][i].pay - zonesum[w][i].aktiv);
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

  printf("\n\n");
} /* total */


static char *numtonam(int n)
{
  register int i, j = -1;


  for (i = 0; i < knowns; i++) {
    if (!num_match(known[i]->num, curcall.num[n])) {
      j = i;

      if (known[i]->si == curcall.si1)
        return(known[i]->who);
    } /* if */
  } /* for */

  return((j == -1) ? NULL : known[j]->who);
} /* numtonam */


static void justify(char *fromnum, char *tonum, int dialout, TELNUM number)
{
  register char *p1, *p2;
  auto 	   char  s[BUFSIZ], sx[BUFSIZ];


  p1 = numtonam(dialout ? CALLED : CALLING);

  if (*number.msn)
    sprintf(sx, "%s%s%s", number.area, (*number.area ? "/" : ""), number.msn);
  else
    strcpy(sx, number.area);

  if (*number.country && strcmp(number.country, mycountry))
    sprintf(s, "%s %s", number.country, sx);
  else
    sprintf(s, "%s%s", (*number.country ? "0" : ""), sx);

  p2 = msnsum[SUBTOTAL][homei].alias[curcall.si1];

  printf("%12s %s %-17s",
    p2 ? p2 : fromnum,
    (dialout ? "->" : "<-"),
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
    } /* if */

    if (!*s)
      sprintf(s, "??? TARGET? (%s,%s,%s)", number.country, number.scountry, number.sarea);
  } /* else */

  s[COUNTRYLEN] = 0; /* clipping */

  sprintf(curcall.country, "%-*s", COUNTRYLEN, s);
} /* justify */


int main(int argc, char *argv[], char *envp[])
{
  register char    *pl, *pr, *p;
#ifdef DEBUG
  auto     FILE    *f = fopen("/www/log/isdn.log", "r");
#else
  auto     FILE    *f = fopen("/var/log/isdn.log", "r");
#endif
  auto     char     s[BUFSIZ], sx[BUFSIZ], home[BUFSIZ];
  auto     int      z, i, l, col, month = -1;
  auto     TELNUM   number[2];
  auto     double   dur;
  auto     RATE     Rate;
  auto     char    *version;
  auto 	   char    *myname = basename(argv[0]);


  if (f != (FILE *)NULL) {

    *home = 0;

    set_print_fct_for_tools(print_in_modules);

    if (readconfig(myname) != 0)
      return(1);

    initHoliday("/usr/lib/isdn/holiday-de.dat", &version);

    initDest("/usr/lib/isdn/dest.gdbm", &version);

    initRate("/etc/isdn/rate.conf", "/usr/lib/isdn/rate-de.dat", "/usr/lib/isdn/zone-de-%s.gdbm", &version);

    memset(&msnsum, 0, sizeof(msnsum));
    memset(&provsum, 0, sizeof(provsum));
    memset(&zonesum, 0, sizeof(zonesum));

    while (fgets(s, BUFSIZ, f)) {
      pl = s;
      col = 0;

      memset(&curcall, 0, sizeof(curcall));

      while ((pr = strchr(pl, '|'))) {
        memcpy(sx, pl, (l = (pr - pl)));
        sx[l] = 0;
        pl = pr + 1;

        switch (col++) {
          case  0 :                                     break;

          case  1 : deb(sx);
                    strcpy(curcall.num[CALLING], sx);
                    break;

          case  2 : deb(sx);
                    strcpy(curcall.num[CALLED], sx);
                    break;

          case  3 : dur = atoi(sx);                     break;

          case  4 : curcall.duration = atol(sx) / HZ;

                    if (abs((int)dur - (int)curcall.duration) > 1)
                      curcall.duration = dur;
                    break;

          case  5 : curcall.connect = atol(sx);         break;
          case  6 : curcall.units = atoi(sx);           break;
          case  7 : curcall.dialout = *sx == 'O';       break;
          case  8 : curcall.cause = atoi(sx);           break;
          case  9 : curcall.ibytes = atoi(sx);          break;
          case 10 : curcall.obytes = atoi(sx);          break;
          case 11 : strcpy(curcall.version, sx);        break;
          case 12 : curcall.si1 = atoi(sx);             break;
          case 13 : curcall.si2 = atoi(sx);             break;
          case 14 : curcall.currency_factor = atof(sx); break;
          case 15 : strcpy(curcall.currency, sx);       break;
          case 16 : curcall.pay = atof(sx);             break;
          case 17 : curcall.provider = atoi(sx);        break;
          case 18 :                                     break;
        } /* switch */

      } /* while */

      /* Repair wrong entries from older isdnlog-versions ... */

      if (!curcall.provider || (curcall.provider == UNKNOWN))
        curcall.provider = preselect;

      if (curcall.dialout && (strlen(curcall.num[CALLED]) > 3) && !getSpecial(curcall.num[CALLED])) {
        sprintf(s, "0%s", curcall.num[CALLED] + 3);

        if (getSpecial(s))
          strcpy(curcall.num[CALLED], s);
      } /* if */

      if (!curcall.dialout && (strlen(curcall.num[CALLING]) > 3) && !getSpecial(curcall.num[CALLING])) {
        sprintf(s, "0%s", curcall.num[CALLING] + 3);

        if (getSpecial(s))
          strcpy(curcall.num[CALLING], s);
      } /* if */

      /* if (curcall.dialout) */
      /* if (curcall.duration) */
      /* if (curcall.provider != UNKNOWN) */

      if (*curcall.num[CALLING] && *curcall.num[CALLED]) {

        i = when(curcall.connect, s);

        if (month == -1)
          month = i;
        else if (i != month) {
          total(SUBTOTAL);
          month = i;
        } /* if */

        printf("%s%s ", s, timestr(curcall.duration));

        normalizeNumber(curcall.num[CALLING], &number[CALLING], TN_ALL);
        normalizeNumber(curcall.num[CALLED], &number[CALLED], TN_ALL);

        if (curcall.dialout) {

          p = strstr(home, number[CALLING].msn);

          if (p == NULL) {
            strcat(home, number[CALLING].msn);
            strcat(home, "|");

            p = strstr(home, number[CALLING].msn);
            homei = (int)(p - home) / 7;

            strcpy(msnsum[SUBTOTAL][homei].msn, number[CALLING].msn);

            i = curcall.si1;

            for (curcall.si1 = 1; curcall.si1 < 8; curcall.si1++)
	      msnsum[SUBTOTAL][homei].alias[curcall.si1] = numtonam(CALLING);

            curcall.si1 = i;

            nhome++;
          } /* if */

          homei = (int)(p - home) / 7;
          msnsum[SUBTOTAL][homei].ncalls++;

          justify(number[CALLING].msn, curcall.num[CALLED], curcall.dialout, number[CALLED]);

          provsum[SUBTOTAL][curcall.provider].ncalls++;

          strcpy(s, getProvider(pnum2prefix(curcall.provider, 0)));
          s[PROVLEN] = 0;

          if (curcall.provider < 100)
            sprintf(curcall.sprovider, " %s%02d:%-*s", vbn, curcall.provider, PROVLEN, s);
          else
            sprintf(curcall.sprovider, "%s%03d:%-*s", vbn, curcall.provider - 100, PROVLEN, s);


          if (!curcall.duration) {
            printf("           %s%s", curcall.country, curcall.sprovider);

            if ((curcall.cause != 0x1f) && /* Normal, unspecified */
                (curcall.cause != 0x10))   /* Normal call clearing */
              printf(" %s", qmsg(TYPE_CAUSE, VERSION_EDSS1, curcall.cause));

            if ((curcall.cause == 0x22) || /* No circuit/channel available */
                (curcall.cause == 0x2a) || /* Switching equipment congestion */
                (curcall.cause == 0x2f))   /* Resource unavailable, unspecified */
              provsum[SUBTOTAL][curcall.provider].failed++;

              printf("\n");
              continue;
          } /* if */


          clearRate(&Rate);
          Rate.start  = curcall.connect;
          Rate.now    = curcall.connect + curcall.duration;
          Rate.prefix = pnum2prefix(curcall.provider, 0);

          Rate.src[0] = number[CALLING].country;
          Rate.src[1] = number[CALLING].area;
          Rate.src[2] = number[CALLING].msn;

          Rate.dst[0] = number[CALLED].country;
          Rate.dst[1] = number[CALLED].area;
          Rate.dst[2] = number[CALLED].msn;

          if (getRate(&Rate, &version) != UNKNOWN) {
            z = Rate.z;

            if (!z && (*number[CALLED].country && strcmp(number[CALLED].country, mycountry)))
              z = AUSLAND;
            else if (getSpecial(curcall.num[CALLED]))
              z = SONDERRUFNUMMERN;
            else if (z == UNKNOWN)
              z = ELSEWHERE;

            zonesum[SUBTOTAL][z].ncalls++;

            curcall.compute = Rate.Charge;
          }
          else {
            z = ELSEWHERE;
            curcall.compute = curcall.pay;
            sprintf(curcall.error, " ??? %s", version);
          } /* else */

          if ((z == 1) || (z == 2)) {
            auto struct tm *tm = localtime(&curcall.connect);
            auto int        takte;
            auto double     price;


            takte = (curcall.duration + 59) / 60;

            if ((tm->tm_wday > 0) && (tm->tm_wday < 5)) {   /* Wochentag */
              if ((tm->tm_hour > 8) && (tm->tm_hour < 18))  /* Hauptzeit */
                price = 0.06;
              else
                price = 0.03;
            }
            else                                            /* Wochenende */
              price = 0.03;

            curcall.aktiv = takte * price;

//          printf("AKTIVPLUS:%8.3f, gespart: %8.3f", curcall.aktiv, curcall.pay - curcall.aktiv);

            msnsum[SUBTOTAL][homei].aktiv += curcall.aktiv;
            provsum[SUBTOTAL][curcall.provider].aktiv += curcall.aktiv;
            zonesum[SUBTOTAL][z].aktiv += curcall.aktiv;
          } /* if */

          if (curcall.pay < 0.0) { /* impossible! */
            curcall.pay = curcall.compute;
            curcall.computed++;
          } /* if */

          if (curcall.compute && fabs(curcall.pay - Rate.Charge) > 1.00) {
            curcall.pay = curcall.compute;
            curcall.computed++;
          } /* if */


          if (curcall.pay)
            printf("%s%7.3f%s ", curcall.currency, curcall.pay, curcall.computed ? "*" : " ");
          else
            printf("           ", curcall.currency, curcall.pay, curcall.computed ? "*" : " ");

          printf("%s%s%s", curcall.country, curcall.sprovider, curcall.error);

          if (curcall.aktiv) {
            printf(" AktivPlus - %s%6.3f", curcall.currency, curcall.pay - curcall.aktiv);
          }

          msnsum[SUBTOTAL][homei].pay += curcall.pay;
          msnsum[SUBTOTAL][homei].duration += curcall.duration;
          msnsum[SUBTOTAL][homei].compute += curcall.compute;

          provsum[SUBTOTAL][curcall.provider].pay += curcall.pay;
          provsum[SUBTOTAL][curcall.provider].duration += curcall.duration;
          provsum[SUBTOTAL][curcall.provider].compute += curcall.compute;

          zonesum[SUBTOTAL][z].pay += curcall.pay;
          zonesum[SUBTOTAL][z].duration += curcall.duration;
          zonesum[SUBTOTAL][z].compute += curcall.compute;
        }
        else { /* Dialin: */
          justify(number[CALLED].msn, curcall.num[CALLING], curcall.dialout, number[CALLING]);
          printf("           %s%s", curcall.country, curcall.sprovider);
        } /* else */

        printf("\n");

      } /* if */
    } /* while */

    fclose(f);
    total(SUBTOTAL);
    total(TOTAL);

  } /* if */

  return(0);
} /* isdnbill */
