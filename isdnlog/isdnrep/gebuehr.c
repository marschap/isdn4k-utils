#define ISDNLOG 1
#ifdef ISDNLOG
/*
    1. "http://www.insider.org/tarif/gebuehr.c" saugen
    2. patch -s -p0 < gebuehr.patch
*/
#endif

/*
  Gebührenberechnung - Berechnung der jeweils aktuellen Gebühren für Telefon-
                       anrufe über die verschiedenen auf dem deutschen Markt
                       vertretenen Provider

  Version:
   0.01  06.01.98      erste Version, morgen muß noch die automatische
                       Umschaltung Wochentag/Wochenende reingebaut werden
   0.02  07.01.98      Automatische Umschaltung Wochentag/Wochende
   0.03  08.01.98      Korrektur diverse Tarife (siehe Windows-Prg
                       TelcoExplorer)
   0.04  09.01.98      Korrektur MobilCom-Tarif
   0.05  14.01.98      zusätzliche Provider und Tarife, Neu: Nahbereich
   0.06  29.01.98      neue Provider, Umstellung auf 10tel Sekunden
   0.07  11.02.98      Welt2-Tarif Isis korrigiert
   0.10  08.04.98      Neue Tarife, neue Unterteilung, Berechnung komplett
                       umgeworfen

  (C) 1998 G.Glendown - alle Tarif/Preis-Angaben ohne Gewähr

  Aufruf:             gebuehr [Tarifzone [Dauer]]
                      Tarifzone:
                             1  City
                             2  Nah
                             3  Region 50
                             4  Region 200
                             5  Region fern
                             6  Mobilfunk C
                             7  Mobilfunk D1
                             8  Mobilfunk D2
                             9  Mobilfunk E+
                             10  EuroCity
                             11  Euro 1
                             12  Euro 2
                             13  Welt 1 (Nordamerika)
                             14  Welt 2
                             15  Welt 3
                             16  Welt 4
                             Default: 4
                       Dauer: Dauer des Gesprächs in Sekunden
                             Default: 300 (5 Minuten)

  Bugs/Features:       - Telekom-Tarife aufgrund blöder Taktung (13,5 sek.)
                         teilweise gerundet, da die Telekom jedoch grund-
                         sätzlich (d.h. fast immer) am teuersten ist, sollte
                         das in den Fällen nicht schlimm sein :-)
                       - Bedienung etwas spartanisch, es fehlt wenigstens
                         eine Usage-Ausgabe ...

  eMail:               garry@insider.regio.net
  WWW:                 http://www.insider.org/tarif

*/
#include <sys/types.h>
#include <sys/time.h>

#ifndef ISDNLOG
static void settime();
#endif
void UpdateList();

struct provider {
        char name[16];
        unsigned char zuschl;
        unsigned char flag;
        unsigned long minpr;            /* DM/m */
        unsigned short prefix;
        short longer;
};

#define F_EINH  1               /* Berechnung nach Einheiten */
#define F_NETTO 2               /* Preis ist Netto */
#define F_BER   0x80            /* Preis wurde schon berechnet */

static struct provider prov[] = {
        {"Arcor",6,0,0,70},             /* 0 */
        {"Interoute",0,0,0,66},         /* 1 */
        {"EWE TEL",0,0,0,14},           /* 2 */
        {"Mobilcom",0,0,0,19},          /* 3 */
        {"KomTel",0,0,0,46},    /* 4 */
        {"o.tel.o",6,0,0,11},           /* 5 */
        {"Talkline",0,0,0,50},          /* 6 */
        {"TelDaFax",0,F_NETTO,0,30},            /* 7 */
        {"Telekom",0,F_NETTO|F_EINH,0,33},              /* 8 */
        {"TelePassport",0,0,0,24},      /* 9 */
        {"",0,0,0,0},   /* 10 */
        {"WESTCom",0,0,0,85},           /* 11 */
        {"",0,0,0,0},           /* 12 */
        {"",0,0,0,0},   /* 13 */
        {"Tele2",0,0,0,13},             /* 14 */
        {"",0,0,0,0},           /* 15 */
        {"tesion))",0,F_NETTO,0,39},    /* 16 */
        {"",0,0,0,0},           /* 17 */
        {"",0,0,0,0}            /* 18 */
};
#define NUMPROV 19
int srt[NUMPROV];

#define BWT     0x10
#define BWE     0x20
#define BFT     0x40
#define BWA     0x70

#define  BFCY   1
#define  BFNH   2
#define  BFR5   4
#define  BFR2   8
#define  BFFN   16
#define  BFMC   32
#define  BFD1   64
#define  BFD2   128
#define  BFME   256
#define  BFEY   512
#define  BFE1   1024
#define  BFE2   2048
#define  BFW1   4096
#define  BFW2   8192
#define  BFW3   16384
#define  BFW4   32768
char tarnam[][20]={"City","Nahbereich","Region50","Region200","Fern","C-Netz","D1-Netz", "D2-Netz", "Eplus", "EuroCity","Euro1","Euro2","Nordamerika","Welt2","Welt3","Welt4"};

struct tarif {
        signed char prov;               /* lfd. Nummer Provider */
        unsigned char dist;             /* Entfernung (0x0f), Tag (0x70) */
        unsigned short taktl;           /* Dauer eines Taktes in sek*100 */
        unsigned short minpr;           /* Preis pro Minute in Pf (alt.z. taktp) */
        unsigned char strth;            /*  */
        unsigned char stoph;
};

struct ttable {
        signed char prov;               /* Provider */
        unsigned char tar;              /* lfd. Nummer Tarif     */
        unsigned long tarber;           /* Bit-Feld Tarifbereiche */
};


#ifdef ISDNLOG
static struct ttable tt[] = {
#else
struct ttable tt[] = {
#endif
/* Arcor */
      {  0,      0,      BFNH|BFR5 },
      {  0,      1,      BFR2|BFFN },
      {  0,      2,      BFMC|BFD1 },
      {  0,      3,      BFD2 },
      {  0,      4,      BFME },
      {  0,      5,      BFEY|BFE1 },
      {  0,      6,      BFE2 },
      {  0,      7,      BFW1 },
      {  0,      8,      BFW2 },
      {  0,      9,      BFW3 },
      {  0,      10,     BFW4 },
/* Interoute */
      {  1,      0,      BFNH|BFR5 },
      {  1,      1,      BFR2|BFFN },
      {  1,      2,      BFMC|BFD1|BFD2|BFME },
      {  1,      3,      BFEY|BFE1 },
      {  1,      4,      BFE2 },
      {  1,      5,      BFW1 },
      {  1,      6,      BFW2 },
      {  1,      7,      BFW3 },
      {  1,      8,      BFW4 },
/* EWE TEL */
      {  2,      0,      BFR5 },
      {  2,      1,      BFR2|BFFN },
      {  2,      2,      BFMC },
      {  2,      3,      BFD1|BFD2|BFME },
      {  2,      5,      BFEY },
      {  2,      6,      BFE1 },
      {  2,      7,      BFE2 },
      {  2,      8,      BFW1 },
      {  2,      9,      BFW2 },
      {  2,      10,     BFW3 },
      {  2,      11,     BFW4 },
/* Mobilcom */
      {  3,      0,      BFNH|BFR5|BFR2|BFFN },
      {  3,      1,      BFMC|BFD1|BFD2|BFME },
/* KomTel */
      {  4,      0,      BFNH|BFR5|BFR2|BFFN },
      {  4,      1,      BFMC|BFD1|BFD2|BFME },
      {  4,      2,      BFEY|BFE1 },
      {  4,      3,      BFE2 },
      {  4,      4,      BFW1 },
      {  4,      5,      BFW2 },
      {  4,      6,      BFW3|BFW4 },
/* o.tel.o */
      {  5,      0,      BFNH },
      {  5,      1,      BFR5 },
      {  5,      2,      BFR2|BFFN },
      {  5,      3,      BFMC },
      {  5,      4,      BFD1|BFD2 },
      {  5,      6,      BFME },
      {  5,      7,      BFEY|BFE1|BFW1 },
      {  5,      8,      BFE2 },
      {  5,      10,     BFW2 },
      {  5,      11,     BFW3 },
      {  5,      12,     BFW4 },
/* Talkline */
      {  6,      0,      BFR2|BFFN },
      {  6,      1,      BFMC|BFD1|BFD2|BFME },
      {  6,      2,      BFEY|BFE1|BFW1 },
      {  6,      3,      BFW4 },
/* TelDaFax */
      {  7,      0,      BFNH|BFR5 },
#ifdef ISDNLOG
      {  7,      1,      BFR2|BFFN },
#else
      {  7,      1,      BFR5|BFFN },
#endif
      {  7,      2,      BFMC|BFD1|BFD2|BFME },
      {  7,      3,      BFEY },
      {  7,      4,      BFE1 },
      {  7,      5,      BFE2 },
      {  7,      6,      BFW1 },
      {  7,      7,      BFW2 },
      {  7,      8,      BFW3 },
      {  7,      9,      BFW4 },
/* Telekoma */
      {  8,      0,      BFCY|BFNH },
      {  8,      2,      BFR5 },
      {  8,      3,      BFR2|BFFN },
      {  8,      4,      BFMC|BFD1 },
      {  8,      5,      BFD2 },
      {  8,      6,      BFME },
      {  8,      7,      BFEY },
      {  8,      8,      BFE1 },
      {  8,      9,      BFE2 },
      {  8,      10,     BFW1 },
      {  8,      11,     BFW2 },
      {  8,      12,     BFW3 },
      {  8,      13,     BFW4 },
/* TelePassPort */
      {  9,      0,      BFNH|BFR5 },
      {  9,      1,      BFR2|BFFN },
      {  9,      2,      BFMC|BFD1|BFD2|BFME },
      {  9,      3,      BFEY|BFE1 },
      {  9,      4,      BFE2 },
      {  9,      5,      BFW1 },
      {  9,      6,      BFW2 },
      {  9,      7,      BFW3 },
      {  9,      8,      BFW4 },
/* WestCom */
      {  11,     0,      BFNH|BFR5 },
      {  11,     1,      BFR2 },
      {  11,     2,      BFFN },
      {  11,     3,      BFMC|BFD1|BFD2|BFME },
      {  11,     4,      BFEY|BFE1 },
      {  11,     5,      BFE2 },
      {  11,     6,      BFW1 },
      {  11,     7,      BFW2 },
      {  11,     8,      BFW3 },
      {  11,     9,      BFW4 },
/* tele2 */
      {  14,     0,      BFNH|BFR5|BFR2|BFFN },
      {  14,     2,      BFMC|BFD1|BFD2|BFME },
      {  14,     3,      BFEY|BFW1 },
      {  14,     4,      BFE1 },
      {  14,     5,      BFE2 },
      {  14,     6,      BFW2 },
      {  14,     7,      BFW3 },
      {  14,     8,      BFW4 },
/* tesion */
      {  16,     0,      BFNH|BFR5|BFR2|BFFN },
      {  16,     1,      BFMC|BFD1|BFD2|BFME },
      {  16,     2,      BFEY|BFE1 },
      {  16,     3,      BFE2 },
      {  16,     4,      BFW1 },
      {  16,     5,      BFW2|BFW3 },
      {  16,     6,      BFW4 },
      {  -1,     0,      0 }};

struct tarif tarife[] = {
/* arcor */
{       0,      0|BWT,                  100,    150,    9,17},
{       0,      0|BWA,                  100,    110,    5,20},
{       0,      0|BWA,                  100,    75,     0,23},
{       0,      1|BWT,                  100,    480,    9,17},
{       0,      1|BWT,                  100,    280,    5,20},
{       0,      1|BWA,                  100,    190,    0,23},
{       0,      2|BWT,                  100,    960,    9,17},
{       0,      2|BWA,                  100,    480,    0,23},
{       0,      3|BWT,                  100,    940,    9,17},
{       0,      3|BWA,                  100,    460,    0,23},
{       0,      4|BWT,                  100,    1250,   9,17},
{       0,      4|BWA,                  100,    560,    0,23},
{       0,      5|BWA,                  100,    680,    0,23},
{       0,      6|BWA,                  100,    960,    0,23},
{       0,      7|BWA,                  100,    720,    0,23},
{       0,      8|BWA,                  100,    1690,   0,23},
{       0,      9|BWA,                  100,    2590,   0,23},
{       0,      10|BWA,                 100,    2720,   0,23},
/* Interoute */
{       1,      0|BWT,                  100,    200,    9,17},
{       1,      0|BWA,                  100,    150,    6,20},
{       1,      0|BWA,                  100,    110,    0,23},
{       1,      1|BWT,                  100,    360,    9,17},
{       1,      1|BWA,                  100,    200,    6,20},
{       1,      1|BWA,                  100,    150,    0,23},
{       1,      2|BWT,                  100,    890,    9,17},
{       1,      2|BWA,                  100,    560,    0,23},
{       1,      3|BWT,                  100,    690,    9,17},
{       1,      3|BWA,                  100,    590,    0,23},
{       1,      4|BWA,                  100,    980,    0,23},
{       1,      5|BWA,                  100,    680,    6,20},
{       1,      5|BWA,                  100,    580,    0,23},
{       1,      6|BWA,                  100,    1290,   6,20},
{       1,      6|BWA,                  100,    1190,   0,23},
{       1,      7|BWA,                  100,    1990,   0,23},
{       1,      8|BWA,                  100,    2190,   0,23},
/* EWE TEL */
{       2,      0|BWT,                  100,    210,    9,17},
{       2,      0|BWT,                  100,    160,    18,20},
{       2,      0|BWT,                  100,    120,    0,23},
{       2,      0|BWE,                  100,    120,    9,17},
{       2,      0|BWE,                  100,    100,    0,23},
{       2,      1|BWT,                  100,    420,    9,17},
{       2,      1|BWA,                  100,    220,    18,20},
{       2,      1|BWE,                  100,    240,    9,17},
{       2,      1|BWA,                  100,    190,    0,23},
{       2,      2|BWA,                  100,    1290,   9,17},
{       2,      2|BWA,                  100,    620,    0,23},
{       2,      3|BWA,                  100,    1290,   9,17},
{       2,      3|BWA,                  100,    550,    0,23},
{       2,      5|BWA,                  100,    620,    0,23},
{       2,      6|BWA,                  100,    690,    0,23},
{       2,      7|BWA,                  100,    1040,   0,23},
{       2,      8|BWA,                  100,    690,    0,23},
{       2,      9|BWA,                  100,    1790,   0,23},
{       2,      10|BWA,                 100,    2290,   0,23},
{       2,      11|BWA,                 100,    3090,   0,23},
/* Mobilcom */
{       3,      0|BWA,                  6000,   190,    0,23},
{       3,      1|BWA,                  6000,   990,    0,23},
/* KomTel */
{       4,      0|BWA,          100,    450,    8,19},
{       4,      0|BWA,          100,    300,    0,23},
{       4,      1|BWA,          100,    1000,   8,19},
{       4,      1|BWA,          100,    700,    0,23},
{       4,      2|BWA,          100,    700,    8,19},
{       4,      2|BWA,          100,    600,    0,23},
{       4,      3|BWA,          100,    1500,   0,23},
{       4,      4|BWA,          100,    1000,   0,23},
{       4,      5|BWA,          100,    2000,   0,23},
{       4,      6|BWA,          100,    3000,   0,23},
/* o.tel.o */
{       5,      0|BWT,          100,    120,    9,17},
{       5,      0|BWA,          100,    60,     0,23},
{       5,      1|BWT,          100,    240,    9,17},
{       5,      1|BWA,          100,    120,    0,23},
{       5,      2|BWT,          100,    480,    9,17},
{       5,      2|BWA,          100,    240,    0,23},
{       5,      3|BWT,          100,    1330,   9,17},
{       5,      3|BWA,          100,    630,    0,23},
{       5,      4|BWT,          100,    1330,   9,17},
{       5,      4|BWA,          100,    550,    0,23},
{       5,      6|BWT,          100,    990,    9,17},
{       5,      6|BWA,          100,    490,    0,23},
{       5,      7|BWA,          100,    690,    0,23},
{       5,      8|BWA,          100,    990,    0,23},
{       5,      10|BWA,         100,    1990,   0,23},
{       5,      11|BWA,         100,    2390,   0,23},
{       5,      12|BWA,         100,    2790,   0,23},
/* Talkline */
{       6,      0|BWT,          1000,   440,    9,17},
{       6,      0|BWA,          1000,   220,    0,23},
{       6,      1|BWT,          1000,   990,    9,17},
{       6,      1|BWA,          1000,   550,    0,23},
{       6,      2|BWA,          1000,   790,    0,23},
{       6,      3|BWA,          1000,   3000,   0,23},
/* TelDaFax */
{       7,      0|BWT,          100,    190,    5,8},
{       7,      0|BWA,          100,    160,    5,20},
{       7,      0|BWA,          100,    120,    0,23},
{       7,      1|BWT,          100,    340,    9,17},
{       7,      1|BWA,          100,    190,    5,20},
{       7,      1|BWA,          100,    120,    0,23},
{       7,      2|BWT,          100,    790,    9,17},
{       7,      2|BWA,          100,    410,    0,23},
{       7,      3|BWT,          100,    510,    8,17},
{       7,      3|BWA,          100,    480,    0,23},
{       7,      4|BWT,          100,    590,    8,17},
{       7,      4|BWA,          100,    480,    0,23},
{       7,      5|BWA,          100,    580,    0,23},
{       7,      6|BWA,          100,    590,    0,23},
{       7,      7|BWA,          100,    1580,   0,23},
{       7,      8|BWA,          100,    1850,   0,23},
{       7,      9|BWA,          100,    2100,   0,23},
/* Telekomik */
{       8,      0|BWT,          1043,   900,    9,17},
{       8,      0|BWA,          1043,   1500,   5,20},
{       8,      0|BWA,          1043,   2400,   0,23},
/* telekom Regio Call */
{       8,      2|BWT,          1043,   1200,   2, 4},
{       8,      2|BWT,          1043,   260,    9,11},
{       8,      2|BWT,          1043,   300,    12,17},
{       8,      2|BWA,          1043,   450,    5,20},
{       8,      2|BWA,          1043,   600,    0,23},
/* Telekom German Call */
{       8,      3|BWT,          1043,   1200,   2, 4},
{       8,      3|BWT,          1043,   130,    9,11},
{       8,      3|BWT,          1043,   140,    12,17},
{       8,      3|BWT,          1043,   225,    5,20},
{       8,      3|BWE,          1043,   300,    5,20},
{       8,      3|BWA,          1043,   360,    0,23},
/* Telekom D-Netz */
{       8,      4|BWT,          1043,   75,     9,17},
{       8,      4|BWA,          1043,   150,    0,23},
{       8,      5|BWT,          1043,   54,     9,17},
{       8,      5|BWA,          1043,   130,    0,23},
{       8,      6|BWT,          1043,   53,     9,17},
{       8,      6|BWA,          1043,   128,    0,23},
/* Telekom EuroCity */
{       8,      7|BWA,          1043,   100,    0,23},
/* Telekom Euro1 */
{       8,      8|BWT,          1043,   86,     8,17},
{       8,      8|BWA,          1043,   100,    0,23},
{       8,      9|BWA,          1043,   60,     0,23},
/* Telekom Welt1 */
{       8,      10|BWA,         1043,   100,    3,13},
{       8,      10|BWA,         1043,   86,     0,23},
{       8,      11|BWA,         1043,   33,     0,23},
{       8,      12|BWA,         1043,   26,     0,23},
{       8,      13|BWA,         1043,   23,     0,23},
/* Telepassport */
{       9,      0|BWT,          100,    220,    9,17},
{       9,      0|BWA,          100,    180,    0,23},
{       9,      1|BWT,          100,    380,    9,17},
{       9,      1|BWA,          100,    220,    5,20},
{       9,      1|BWA,          100,    180,    0,23},
{       9,      2|BWT,          100,    880,    9,17},
{       9,      2|BWA,          100,    580,    0,23},
{       9,      3|BWA,          100,    680,    0,23},
{       9,      4|BWA,          100,    980,    0,23},
{       9,      5|BWA,          100,    680,    0,23},
{       9,      6|BWA,          100,    1280,   0,23},
{       9,      7|BWA,          100,    2180,   0,23},
{       9,      8|BWA,          100,    2280,   0,23},
/* Westcom */
{       11,     0|BWA,          100,    80,     2,4},
{       11,     0|BWA,          100,    200,    9,17},
{       11,     0|BWA,          100,    160,    0,23},
{       11,     1|BWA,          100,    80,     2,4},
{       11,     1|BWA,          100,    200,    5,20},
{       11,     1|BWA,          100,    330,    9,17},
{       11,     1|BWA,          100,    160,    0,23},
{       11,     2|BWA,          100,    80,     2,4},
{       11,     2|BWA,          100,    200,    5,8},
{       11,     2|BWA,          100,    330,    9,17},
{       11,     2|BWA,          100,    210,    18,20},
{       11,     2|BWA,          100,    160,    0,23},
{       11,     3|BWA,          100,    740,    9,17},
{       11,     3|BWA,          100,    600,    0,23},
{       11,     4|BWA,          100,    630,    0,23},
{       11,     5|BWA,          100,    970,    0,23},
{       11,     6|BWA,          100,    630,    0,23},
{       11,     7|BWA,          100,    1510,   0,23},
{       11,     8|BWA,          100,    1940,   0,23},
{       11,     9|BWA,          100,    2210,   0,23},
/* Citykom Münster */
/*
{       13,     BFCY|BWT,               1100,   0,      900,    9,19},
{       13,     BFCY|BWA,               1100,   0,      1500,   9,19},
{       13,     BFCY|BWA,               1100,   0,      1650,   0,23},
{       13,     BFNH|BWT,               1100,   0,      900,    9,19},
{       13,     BFNH|BWA,               1100,   0,      1500,   9,19},
{       13,     BFNH|BWA,               1100,   0,      1650,   0,23},
{       13,     BFR5|BWT,               1100,   0,      300,    9,19},
{       13,     BFR5|BWA,               1100,   0,      450,    9,19},
{       13,     BFR5|BWA,               1100,   0,      600,    0,23},
{       13,     BFR2|BWT,               1100,   0,      250,    9,19},
{       13,     BFR2|BWA,               1100,   0,      400,    9,19},
{       13,     BFR2|BWA,               1100,   0,      600,    0,23},
{       13,     BFFN|BWT,               1100,   0,      250,    9,19},
{       13,     BFFN|BWA,               1100,   0,      400,    9,19},
{       13,     BFFN|BWA,               1100,   0,      600,    0,23},
{       13,     BFML|BWT,               1100,   0,      51,     9,19},
{       13,     BFML|BWA,               1100,   0,      112,    9,19},
{       13,     BFML|BWA,               1100,   0,      112,    0,23},
{       13,     BFEY|BWA,               1100,   0,      92,     0,23},
{       13,     BFE1|BWA,               1100,   0,      57,     0,23},
{       13,     BFE2|BWA,               1100,   0,      57,     0,23},
{       13,     BFW1|BWA,               1100,   0,      50,     0,23},
{       13,     BFW2|BWA,               1100,   0,      30,     0,23},
{       13,     BFW3|BWA,               1100,   0,      26,     0,23},
{       13,     BFW4|BWA,               1100,   0,      23,     0,23},
*/
/* Tele2 */
/*
{       14,     BFR5|BWT,               100,    7,      0,      2,4},
{       14,     BFR5|BWT,               100,    23,     0,      9,17},
{       14,     BFR5|BWA,               100,    15,     0,      5,20},
{       14,     BFR5|BWA,               100,    11,     0,      0,23},
{       14,     BFR2|BWT,               100,    7,      0,      2,4},
{       14,     BFR2|BWT,               100,    23,     0,      18,20},
{       14,     BFR2|BWA,               100,    30,     0,      5,20},
{       14,     BFR2|BWA,               100,    20,     0,      0,23},
{       14,     BFFN|BWT,               100,    7,      0,      2,4},
{       14,     BFFN|BWT,               100,    23,     0,      18,20},
{       14,     BFFN|BWA,               100,    30,     0,      5,20},
{       14,     BFFN|BWE,               100,    30,     0,      0,23},
*/
{       14,     0|BWT,          100,    200,    9,17},
{       14,     0|BWT,          100,    150,    0,23},
{       14,     0|BWE,          100,    100,    0,23},
{       14,     2|BWT,          100,    916,    8,17},
{       14,     2|BWA,          100,    824,    0,23},
{       14,     3|BWT,          100,    684,    8,17},
{       14,     3|BWA,          100,    580,    0,23},
{       14,     4|BWT,          100,    804,    8,17},
{       14,     4|BWA,          100,    684,    0,23},
{       14,     5|BWT,          100,    1148,   8,17},
{       14,     5|BWA,          100,    974,    0,23},
{       14,     7|BWT,          100,    2540,   8,17},
{       14,     7|BWA,          100,    2158,   0,23},
{       14,     8|BWT,          100,    2865,   8,17},
{       14,     8|BWA,          100,    2436,   0,23},
/* Debitel, kein CbC */
/*{     15,     BFCY|BWA,               6000,   230,    0,      0,23},
{       15,     BFNH|BWA,               6000,   230,    0,      0,23},
{       15,     BFR5|BWA,               6000,   230,    0,      0,23},
{       15,     BFR2|BWA,               6000,   230,    0,      0,23},
{       15,     BFFN|BWA,               6000,   230,    0,      0,23},*/
/* tesion */
{       16,     0|BWA,          100,    380,    9,20},
{       16,     0|BWA,          100,    260,    0,23},
{       16,     1|BWA,          100,    980,    9,20},
{       16,     1|BWA,          100,    480,    0,23},
{       16,     2|BWA,          100,    590,    9,20},
{       16,     2|BWA,          100,    470,    0,23},
{       16,     3|BWA,          100,    790,    9,20},
{       16,     3|BWA,          100,    530,    0,23},
{       16,     4|BWA,          100,    890,    9,20},
{       16,     4|BWA,          100,    800,    0,23},
{       16,     5|BWA,          100,    1690,   0,23},
{       16,     6|BWA,          100,    1990,   0,23},
/* Hutchison */
/*
{       17,     BFR5|BWT,               1000,   220,    0,      9,20},
{       17,     BFR5|BWA,               1000,   170,    0,      0,23},
{       17,     BFR2|BWT,               1000,   440,    0,      9,20},
{       17,     BFR2|BWA,               1000,   220,    0,      0,23},
{       17,     BFFN|BWT,               1000,   440,    0,      9,20},
{       17,     BFFN|BWA,               1000,   220,    0,      0,23},*/
/* Esprit Telecom */
/*
{       18,     BFCY|BWA,               100,    230,    0,      0,23},
{       18,     BFNH|BWA,               100,    230,    0,      0,23},
{       18,     BFR5|BWA,               100,    230,    0,      8,17},
{       18,     BFR5|BWA,               100,    130,    0,      0,23},
{       18,     BFR2|BWA,               100,    380,    0,      8,17},
{       18,     BFR2|BWA,               100,    170,    0,      0,23},
{       18,     BFFN|BWA,               100,    380,    0,      8,17},
{       18,     BFFN|BWA,               100,    170,    0,      0,23},
{       18,     BFML|BWA,               100,    880,    0,      8,17},
{       18,     BFML|BWA,               100,    360,    0,      0,23},
{       18,     BFEY|BWA,               100,    540,    0,      0,23},
{       18,     BFE1|BWA,               100,    610,    0,      0,23},
{       18,     BFE2|BWA,               100,    1140,   0,      0,23},
{       18,     BFW1|BWA,               100,    860,    0,      0,23},
{       18,     BFW2|BWA,               100,    1440,   0,      0,23},
{       18,     BFW3|BWA,               100,    2070,   0,      0,23},
{       18,     BFW4|BWA,               100,    2340,   0,      0,23},*/
{       -1,     0,                      0,      0,      0,0}};

#ifndef ISDNLOG
int tarifz;
unsigned char wt;
char tmp[10];
int dauer;
#endif

unsigned long ftage[] = {19980410,19980413,19980501,19980521, 19980601,0};

#ifndef ISDNLOG
main(argc,argv)
int argc;
char *argv[];
{
        dauer=300;
        if (argc==1)
            tarifz=4;
        else
            tarifz=atoi(argv[1]);
        if (argc==3)
            dauer=atoi(argv[2]);
        UpdateList();
}
#endif

#ifdef ISDNLOG
double pay(time_t ts, int dauer, int tarifz, int pro)
#else
void UpdateList()
#endif
{
#ifdef ISDNLOG
long  t,z,g,ft;
#else
long  t,z,g,r,s,ft;
#endif
unsigned char tarnum;
struct tm *dt;
#ifndef ISDNLOG
time_t ts;
#endif
unsigned long tarbit;
#ifndef ISDNLOG
    ts=time(NULL);
#endif
#ifdef ISDNLOG
unsigned char wt;
int      e = -1;
#endif
/*    ts-=3600; */
    dt=localtime(&ts);
    ft=(1900+dt->tm_year)*10000+(1+dt->tm_mon)*100+dt->tm_mday;
    if (dt->tm_wday >0 && dt->tm_wday < 6) wt=BWT;
    else wt=BWE;
    t=0;
    while (ftage[t]) {if (ftage[t]==ft) wt=BWE; t++;}
#ifndef ISDNLOG
        printf("%d\n",wt);
#endif
    for (t=0;t<NUMPROV;t++) prov[t].flag&=0x7f,prov[t].minpr=0;
    tarbit=1;
    t=tarifz;
    while (t>1) tarbit<<=1,t--;
#ifndef ISDNLOG
    printf("Tarifbits: %lx\n",tarbit);
#endif
    t=0;
    while (tt[t].prov>=0) {
#ifdef ISDNLOG
        if (!(prov[tt[t].prov].flag&0x80) && (prov[tt[t].prov].prefix == pro)) {
          e = tt[t].prov;
#else
        if (!(prov[tt[t].prov].flag&0x80)) {
#endif
            if (tt[t].tarber&tarbit) {
                tarnum=tt[t].tar;
                z=t;
                t=0;
                while (tarife[t].prov>=0) {
                    if ((tarife[t].prov==tt[z].prov) &&
                        ((tarife[t].dist&0xf)==tarnum) &&
                        (tarife[t].dist&wt)) {
                        if (dt->tm_hour >= tarife[t].strth && dt->tm_hour <= tarife[t].stoph) {
                            prov[tarife[t].prov].flag|=0x80;
                            if (!(prov[tarife[t].prov].flag&F_EINH)) {  /* Preis pro Minute abrechnen */
                                g=(dauer*100L+tarife[t].taktl-1)/tarife[t].taktl;
                                prov[tarife[t].prov].longer=(g*tarife[t].taktl/100)-dauer;
                                g=g*tarife[t].taktl/60;
                                prov[tarife[t].prov].minpr=prov[tarife[t].prov].zuschl+((g*tarife[t].minpr)+500)/1000;
                                if (prov[tarife[t].prov].flag&F_NETTO)
                                    prov[tarife[t].prov].minpr=((prov[tarife[t].prov].minpr*116)+50)/100;
                                goto exitwhile1;
                            }
                            else {      /* Einheiten */
                                g=(10*dauer+tarife[t].minpr-1)/tarife[t].minpr;
                                prov[tarife[t].prov].minpr=g*tarife[t].taktl/100;
                                if (prov[tarife[t].prov].flag&F_NETTO)
                                    prov[tarife[t].prov].minpr=((prov[tarife[t].prov].minpr*116)+50)/100;
                                prov[tarife[t].prov].longer=(short)((g*tarife[t].minpr-10*dauer)/10);
                                goto exitwhile1;
                            }
                        }
                    }
                    t++;
                }
exitwhile1:
                t=z;
            }
        }
        t++;
    }
#ifndef ISDNLOG
    printf("Tarife für %d:%02d Minuten in Tarifzone %s um %d:%02d\n\n",dauer/60, dauer%60,tarnam[tarifz-1],dt->tm_hour, dt->tm_min);
    for (t=0;t<NUMPROV;t++) srt[t]=t;
    for (t=0;t<NUMPROV-1;t++) {
        r=prov[srt[t]].minpr;
        s=t;
        for (g=t+1;g<NUMPROV;g++) {
            if (prov[srt[g]].minpr<r)
                r=prov[srt[g]].minpr,s=g;
        }
        r=srt[t];
        srt[t]=srt[s];
        srt[s]=r;
    }
    for (t=0;t<NUMPROV;t++) {
        if (prov[srt[t]].flag&0x80) {
            printf("%-16s (010%2d)  %3d.%02d\n",prov[srt[t]].name,prov[srt[t]].prefix,prov[srt[t]].minpr/100,
                prov[srt[t]].minpr%100);
        }
    }
#else
  return((e == -1) ? -1.0 : (prov[e].minpr / 100.0));
#endif
}

