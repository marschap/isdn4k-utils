/* $Id: tools_de.c,v 1.6 1998/12/09 20:40:30 akool Exp $
 *
 * ISDN accounting for isdn4linux. (Utilities)
 *
 * Copyright 1995, 1998 by Andreas Kool (akool@isdn4linux.de)
 *
 * splitting into nations 1998 by Michael Reinelt (reinelt@eunet.at)
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
 * $Log: tools_de.c,v $
 * Revision 1.6  1998/12/09 20:40:30  akool
 *  - new option "-0x:y" for leading zero stripping on internal S0-Bus
 *  - new option "-o" to suppress causes of other ISDN-Equipment
 *  - more support for the internal S0-bus
 *  - Patches from Jochen Erwied <mack@Joker.E.Ruhr.DE>, fixes TelDaFax Tarif
 *  - workaround from Sebastian Kanthak <sebastian.kanthak@muehlheim.de>
 *  - new CHARGEINT chapter in the README from
 *    "Georg v.Zezschwitz" <gvz@popocate.hamburg.pop.de>
 *
 * Revision 1.5  1998/11/24 20:53:18  akool
 *  - changed my email-adress
 *  - new Option "-R" to supply the preselected provider (-R24 -> Telepassport)
 *  - made Provider-Prefix 6 digits long
 *  - full support for internal S0-bus implemented (-A, -i Options)
 *  - isdnlog now ignores unknown frames
 *  - added 36 allocated, but up to now unused "Auskunft" Numbers
 *  - added _all_ 122 Providers
 *  - Patch from Jochen Erwied <mack@Joker.E.Ruhr.DE> for Quante-TK-Anlagen
 *    (first dialed digit comes with SETUP-Frame)
 *
 * Revision 1.4  1998/11/17 00:37:57  akool
 *  - fix new Option "-i" (Internal-S0-Bus)
 *  - more Providers (Nikoma, First Telecom, Mox)
 *  - isdnrep-Bugfix from reinhard.karcher@dpk.berlin.fido.de (Reinhard Karcher)
 *  - Configure.help completed
 *
 * Revision 1.3  1998/11/05 19:10:27  akool
 *  - Support for all the new L2 frames from HiSax 3.0d (RR, UA, SABME and
 *    tei management)
 *  - CityWeekend reimplemented
 *    Many thanks to Rainer Gallersdoerfer <gallersd@informatik.rwth-aachen.de>
 *    for the tip
 *  - more providers
 *  - general clean-up
 *
 * Revision 1.2  1998/11/01 08:50:41  akool
 *  - fixed "configure.in" problem with NATION_*
 *  - DESTDIR fixes (many thanks to Michael Reinelt <reinelt@eunet.at>)
 *  - isdnrep: Outgoing calls ordered by Zone/Provider/MSN corrected
 *  - new Switch "-i" -> running on internal S0-Bus
 *  - more providers
 *  - "sonderrufnummern.dat" extended (Frag Fred, Telegate ...)
 *  - added AVM-B1 to the documentation
 *  - removed the word "Teles" from the whole documentation ;-)
 *
 * Revision 1.1  1998/09/26 18:30:24  akool
 *  - quick and dirty Call-History in "-m" Mode (press "h" for more info) added
 *    - eat's one more socket, Stefan: sockets[3] now is STDIN, FIRST_DESCR=4 !!
 *  - Support for tesion)) Baden-Wuerttemberg Tarif
 *  - more Providers
 *  - Patches from Wilfried Teiken <wteiken@terminus.cl-ki.uni-osnabrueck.de>
 *    - better zone-info support in "tools/isdnconf.c"
 *    - buffer-overrun in "isdntools.c" fixed
 *  - big Austrian Patch from Michael Reinelt <reinelt@eunet.at>
 *    - added $(DESTDIR) in any "Makefile.in"
 *    - new Configure-Switches "ISDN_AT" and "ISDN_DE"
 *      - splitted "takt.c" and "tools.c" into
 *          "takt_at.c" / "takt_de.c" ...
 *          "tools_at.c" / "takt_de.c" ...
 *    - new feature
 *        CALLFILE = /var/log/caller.log
 *        CALLFMT  = %b %e %T %N7 %N3 %N4 %N5 %N6
 *      in "isdn.conf"
 *  - ATTENTION:
 *      1. "isdnrep" dies with an seg-fault, if not HTML-Mode (Stefan?)
 *      2. "isdnlog/Makefile.in" now has hardcoded "ISDN_DE" in "DEFS"
 *      	should be fixed soon
 *
 *
 */

#include <isdnlog.h>

char *Providername(int number)
{
  switch (number) {
    case  10 : return("Teleglobe");                                /* GmbH, Gutleutstraße 85, 60329 Frankfurt 01010 */
    case  11 : return("o.tel.o");                                  /* communications GmbH, Am Bonneshof 35, 40474 Düsseldorf 01011 */
    case  12 : return("Tele Danmark");                             /* Internetz GmbH, Süderstraße 75, 20097 Hamburg 01012 */
    case  13 : return("Kinnevik");                                 /* Telecommunciations International S.A., 75, route de Longwy, L-8080 Bertrange, Luxembourg 01013 */
    case  14 : return("EWE TEL");                                  /* GmbH, Donnerschweer Straße 22-26, 26123 Oldenburg 01014 */
    case  15 : return("RSL COM");                                  /* Deutschland GmbH, Lyoner Straße 36, 60528 Frankfurt/Main 01015 */
    case  16 : return("LausitzNet");                               /* Telekommunikationsgesellschaft mbH, Lausitzer Straße 1-7, 03046 Cottbus 01016 */
    case  17 : return("SEC");                                      /* Service AG, Dieskaustraße 246, 04249 Leipzig 01017 */
    case  18 : return("debitel");                                  /* Kommunikationstechnik GmbH & Co KG, Schelmenwasenstr. 37 - 39, 70567 Stuttgart 01018 */
    case  19 : return("CityLine (Mobilcom)");                      /* Telefondienste GmbH, Schwarzer Weg 13, 24837 Schleswig 01019 */
    case  20 : return("ISIS");                                     /* Multimedia Net GmbH, Kaistraße 6, 40221 Düsseldorf 01020 */
    case  21 : return("QS");                                       /* Communication Service GmbH, Oberländer Ufer 180-182, 50968 Köln 01021 */
    case  22 : return("NetCologne");                               /* GmbH, Maarweg 163, 50825 Köln 01022 */
    case  23 : return("tesion");                                   /* Communikationsnetze Süd-West GmbH & Co. KG, Kriegsbergstr. 11, 70174 Stuttgart 01023 */
    case  24 : return("Telepassport");                             /* GmbH, Schwindstraße 3, 60325 Frankfurt 01024 */
    case  25 : return("CityKom");                                  /* Münster, Haferlandweg 8, 48155 Münster 01025 */
    case  26 : return("ECN");                                      /* Telekommunikation GmbH, Schmickstraße 38-40, 60314 Frankfurt 01026 */
    case  27 : return("Communications Netzmanagement");            /* Bremen GmbH, Theodor-Heuss-Allee 20, 28215 Bremen 01027 */
    case  28 : return("Colt");                                     /* Telecom GmbH, Gervinusstraße 18-22, 60323 Frankfurt 01028 */
               /* Reseller von COLT : Telebridge, Global Communications, Nikoma */
    case  29 : return("Unisource");                                /* Carrier Services, Industriestraße 21, CH-8304 Wallisellen/Schweiz 01029 */
    case  30 : return("TelDaFax");                                 /* Telefon-, Daten- und Fax-Transfer GmbH & Co. KG, Schuppertsgasse 30, 35083 Wetter (Hessen) 01030 */
    case  31 : return("HEAG");                                     /* MediaNet GmbH, Luisenplatz 6, 64283 Darmstadt 01031 */
    case  32 : return("KDD");                                      /* Deutschland GmbH, Immermannstraße 45, 40210 Düsseldorf 01032 */
    case  33 : return("Deutsche Telekom");                         /* AG, Friedrich-Ebert-Allee 140, 53113 Bonn 01033 */
    case  34 : return("NEFkom");                                   /* Telekommunikation GmbH & Co. KG, Bärenschanzstraße 4, 90429 Nürnberg 01034 */
    case  35 : return("interCOM");                                 /* GmbH, Hauptstraße 52a, 28876 Oyten 01035 */
    case  36 : return("Hutchison");                                /* Münsterstraße 109, 48155 Münster 01036 */
    case  37 : return("ETS");                                      /* Verwaltung für Sprach- und Datennetze GmbH, Brunnenstraße 4, 61191 Rosbach 01037 */
    case  38 : return("MediaWays");                                /* GmbH, An der Autobahn, 33311 Gütersloh 01038 */
    case  39 : return("First Telecom");                            /* GmbH, Lyoner Straße 51, 60528 Frankfurt/Main 01039 */
    case  40 : return("PLUSNET");                                  /* Gesellschaft für Netzwerk Services mbH, August-Thyssen-Straße 1, 40211 Düsseldorf 01040 */
    case  41 : return("HanseNet");                                 /* Telekommunikation GmbH, Hammerbrookstraße 63, 20097 Hamburg 01041 */
    case  42 : return("TeleBeL");                                  /* Telekommunikation Bergisches Land GmbH, Johannisberg 7, 42103 Wuppertal 01042 */
    case  43 : return("KielNet");                                  /* GmbH, Postfach 41 60, 24100 Kiel 01043 */
    case  44 : return("VEW TELNET");                               /* Gesellschaft für Telekommunikation und Netzdienste mbH, Unterste-Wilms-Straße 29, 44143 Dortmund 01044 */
    case  45 : return("Global Network");                           /* Telephone Deutschland, Grosser Plan 8, 29221 Celle 01045 */
    case  46 : return("KomTel");                                   /* Gesellschaft für Kommunikation und Informationsdienste GmbH, Batteriestraße 48, 24937 Flensburg 01046 */
    case  47 : return("PrimeTec");                                 /* International Europe Ltd. 01047 */
    case  48 : return("DOKOM");                                    /* GmbH, Im Defdahl 5, 44141 Dortmund 01048 */
    case  49 : return("ACC");                                      /* Telekommunikation GmbH, Heinrich-Heine-Allee 53, 40213 Düsseldorf 01049 */
    case  50 : return("TALKLINE");                                 /* PS PhoneServices GmbH, Adenauer Damm 1, 25337 Elmshorn 01050 */
    case  51 : return("ID-Switch");                                /* GmbH, Graf-Adolf-Straße, 40210 Düsseldorf 01051 */
    case  52 : return("Long Distance");                            /* International Ltd., 5 Duke's Gate Acton, London W 4 5 DX, GB 01052 */
    case  53 : return("WOBCOM");                                   /* GmbH Wolfsburg, Heßlinger Str. 1-5, 38440 Wolfsburg 01053 */
    case  54 : return("USFI");                                     /* Global Network Services Germany GmbH, Kindsbacherstraße 41, 66877 Ramstein 01054 */
    case  55 : return("ESPRIT");                                   /* Telecom Deutschland GmbH, Kaistraße 16 A, 40221 Düsseldorf 01055 */
    case  56 : return("BITel");                                    /* Gesellschaft für kommunale Telekommunikation mbh, Schildescher Straße 16, 33611 Bielefeld 01056 */
    case  57 : return("Mox");                                      /* Telecom GmbH, Industriestraße 3a, 40878 Ratingen 01057 */
    case  58 : return("KaTel");                                    /* Gesellschaft für Telekommunikation mbH, Mathias-Brüggen-Straße 87-89, 50829 Köln 01058 */
    case  59 : return("Pacific Gateway Exchange");                 /* Inc., 533 Airport Boulevard, Suite 505, Burlingame, CA 94010, USA 01059 */
    case  60 : return("Telecom-InfoService");                      /* GmbH, Postfach 174, A-1000 Wien (A) 01060 */
    case  61 : return("TelSA");                                    /* Telekommunikationsgesellschaft mbH, Magdeburger Str. 51, 06112 Halle 01061 */
    case  62 : return("DENTEL");                                   /* GmbH i.G. Grimmstraße 4 80336 München 01062 */
    case  63 : return("Conos");                                    /* Corporate Network & Services Aktiengesellschaft Lievelingsweg 125 52119 Bonn 01063 */
    case  64 : return("Hannoversche");                             /* Telekommunikations- und Netzgesellschaft mbH, Glockseestraße 33, 30161 Hannover 01064 */
    case  65 : return("Rapid Link");                               /* Telecommunications GmbH, Donaustraße 68, 68199 Mannheim 01065 */
    case  66 : return("Interoute");                                /* Telecom Deutschland GmbH, Lyoner Str. 15, 60528 Frankfurt 01066 */
    case  67 : return("RegioCom");                                 /* GmbH, Steinfeldstraße 3, 39179 Barleben 01067 */
    case  68 : return("Tele Media");                               /* International, Wilhelm-Leuschner-Strasse 23-25, 60329 Frankfurt/Main 01068 */
    case  69 : return("Primus");                                   /* Telecommunications Ltd. 01069 */
    case  70 : return("Arcor");                                    /* Mannesmann AG & Co., Kölner Str. 12, 65760 Eschborn 01070 */
    case  71 : return("DeTeMobil");                                /* Deutsche Telekom MobilNet GmbH, Landgrabenweg 151, 53227 Bonn 01071 */
    case  72 : return("Mannesmann");                               /* Mobilfunk GmbH, Am Seestern 1, 40547 Düsseldorf 01072 */
    case  73 : return("AXXON");                                    /* Telecom GmbH, Am Seestern 24, 40547 Düsseldorf 01073 */
    case  74 : return("Alpha");                                    /* Telecom GmbH, Beethovenstraße 1-3, 60325 Frankfurt/Main 01074 */
    case  75 : return("ARCIS");                                    /* Media Com Management GmbH, Maximilianstr. 14, 80539 München 01075 */
    case  76 : return("QuickNet");                                 /* Telenetworks & Service AG, Else-Lasker-Schüler-Straße 47, 42107 Wuppertal 01076 */
    case  77 : return("E-Plus");                                   /* Mobilfunk GmbH, E-Plus-Platz 1, 40468 Düsseldorf 01077 */
    case  78 : return("3U");                                       /* Telekommunikation GmbH, Äußere Zittauer Straße 51, 02708 Löbau 01078 */
    case  79 : return("VIAPHONE");                                 /* GmbH, Hanauer Landstraße 187, 60314 Frankfurt 01079 */
    case  80 : return("Telegate");                                 /* Aktiengesellschaft für telefonische Informationsdienste, Bahnhofstraße 26, 82211 Herrsching 01080 */
    case  81 : return("Infotel");                                  /* bV, Onderlangs 120, 6812 CJ Arnhem, Niederlande 01081 */
    case  82 : return("AugustaKom");                               /* Telekommunikation GmbH & Co.KG Hoher Weg 1 86152 Augsburg 01082 */
    case  83 : return("Würzburger");                               /* Telekommunikationsgesellschaft mbH, Bahnhofstraße 12-18, 97070 Würzburg 01083 */
    case  84 : return("Telegroup");                                /* Network Services GmbH, Grossenbaumer Weg 6, 40472 Düsseldorf 01084 */
    case  85 : return("WESTCom");                                  /* GmbH, Hebelstraße 22 c, 69115 Heidelberg 01085 */
    case  86 : return("ONE.TEL");                                  /* Limited (UK), Clements House, 14-18 Gresham Street, London, EC2V 7JE, England 01086 */
    case  87 : return("HAMCOM");                                   /* GmbH Telekommunikation, Südring 1-3, 59065 Hamm 01087 */
    case  88 : return("WorldCom");                                 /* Telecommunication Services GmbH, Brönnerstr. 15, 60313 Frankfurt 01088 */
    case  89 : return("M'net");                                    /* Telekommunikations GmbH, Corneliusstraße 10, 80469 München 01089 */
    case  90 : return("VIAG INTERKOM");                            /* GmbH & Co. KG, Elsenheimer Straße 11, 81927 München 01090 */
    case  91 : return("IDT");                                      /* Global Limited, London, United Kingdom 01091 */
    case  92 : return("Hallo Telefon Service");                    /* GmbH Dinkelbergstraße 6 76684 Östringen 01092 */
    case  93 : return("Kray");                                     /* Informationssysteme, Brüder-Grimm-Allee 73 A, 37085 Göttingen 01093 */
    case  94 : return("Global Communications");                    /* GmbH, Eupener Straße 57 - 59, 50933 Köln 01094 */
    case  95 : return("Teleos");                                   /* GmbH, Bielefelder Straße 3, 32051 Herford 01095 */
    case  96 : return("GELSEN-NET");                               /* Kommunikationsgesellschaft mbH, Munscheidstraße 14, 45886 Gelsenkirchen 01096 */
    case  97 : return("AT & T-Unisource");                         /* Communication Services (Deutschland) GmbH, Lyoner Str. 15, 60528 Frankfurt 01097 */
    case  98 : return("STAR");                                     /* Telecommunications Deutschland GmbH, Voltastraße 1a, 60486 Frankfurt 01098 */
               /* Reseller von STAR : C@llas, Mox */
    case  99 : return("ECONOPHONE");                               /* GmbH, Flughafenstraße 54b, 22335 Hamburg 01099 */
               /* Reseller von ECONOPHONE : Telco */
    case 100 : return("TELELEV");                                  /* Telekommunikation GmbH, Dönhoffstraße 39, 51373 Leverkusen 010000 */
    case 101 : return("Netnet");                                   /* International AB, P.O. Box 6708, S-11385 Stockholm 010001 */
    case 102 : return("EncoTel");                                  /* GmbH & Co.KG, Leibnitzstraße 73, 07548 Gera 010002 */
    case 103 : return("mevacom");                                  /* Telekommunikationsdienstleistungen GmbH, Posthofstraße 16, 14467 Potsdam 010003 */
    case 107 : return("KPN");                                      /* Telecom BV, Prinses Beatrixlaan 23, 2595 AK's Gravenhage, Niederlande 010007 */
    case 108 : return("USA Global Link");                          /* Deutschland GmbH, Theaterstraße 90, 52062 Aachen 010008 */
    case 110 : return("Bertelsmann mediaSystems");                 /* GmbH, An der Autobahn, 33311 Gütersloh 010010 */
    case 111 : return("RED LINE");                                 /* Telekommunikationsgesellschaft mbH, Brückenstraße 17, 63897 Miltenberg 010011 */
    case 114 : return("NordKom");                                  /* Weser-Elbe Telekommunikation GmbH, Stresemannstraße 48, 28207 Bremen 010014 */
    case 115 : return("Delta Three");                              /* Inc., 430 Park Ave, New York 010015 */
    case 119 : return("E.T.H.");                                   /* Telefondienste GmbH, Bettinastraße 25a, 60325 Frankfurt 010019 */
    case 120 : return("Komm Consult");                             /* Oberweg 15, 82024 Taufkirchen bei München 010020 */
    case 122 : return("Motorola");                                 /* Electronic GmbH, Hagenauer Straße 47, 65203 Wiesbaden 010022 */
    case 130 : return("tnp telenet");                              /* potsdam, kommunikationsgesellschaft mbH, Erich-Weinert-Straße 100, 14478 Potsdam 010030 */
    case 131 : return("accom");                                    /* Gesellschaft für Telekommunikationsnetze und -dienstleistungen GmbH & Co.KG, Borngasse 34, 52064 Aachen 010031 */
    case 133 : return("Level 3");                                  /* Communications GmbH, Oberlindau 54 - 56, 60323 Frankfurt/Main 010033 */
    case 142 : return("connection 42");                            /* Ernst-Abbe-Straße 10, 25451 Quickborn 010042 */
    case 144 : return("Carrier 1");                                /* AG, Militärstraße 36, CH-8004 Zürich 010044 */
    case 150 : return("Drillisch");                                /* AG, Horbeller Straße 33, 50858 Köln 010050 */
    case 155 : return("Telia");                                    /* Telekommunikation GmbH, Herrengraben 3, 20459 Hamburg 010055 */
    case 156 : return("NETCOM");                                   /* Kassel GmbH, Königstor 3 - 13, 34117 Kassel 010056 */
    case 160 : return("JelloCom");                                 /* GmbH & Co.KG, Prüssingstraße 41, 07745 Jena 010060 */
    case 166 : return("InterCard");                                /* GmbH, POS-Service, Mehlbeerenstraße 4, 82024 Taufkirchen bei München 010066 */
    case 170 : return("Deutsche Telefon- und Marketing Services"); /* GmbH, Isaac-Fulda-Allee 16, 55124 Mainz 010070 */
    case 173 : return("TFN COM SA");                               /* World Of Communication, Via Greina 2 CP 2950, CH-6901 Lugano 010073 */
    case 177 : return("WorldxChange");                             /* Communications GmbH, Am Seedamm 44, 60489 Frankfurt 010077 */
    case 180 : return("FaciliCom");                                /* Telekommunikation GmbH; Windmühlstraße 1, 60329 Frankfurt/Main 010080 */
    case 184 : return("Startec");                                  /* Global Communications (Germany) GmbH, Broßstraße 6, 60487 Frankfurt 010084 */
    case 188 : return("jk-tel");                                   /* GmbH i.G., Hopfenplatz 5, 39120 Magdeburg 010088 */
    case 190 : return("CNE");                                      /* Corporate Network Essen Gesellschaft für Telekommunikation mbH, Kruppstraße 82 - 100, 45145 Essen 010090 */
    case 195 : return("TeleNec");                                  /* Telekommunikation Neustadt GmbH & Co.KG, Postfach 14 40, 96459 Neustadt 010095 */
    case 199 : return("ESTel");                                    /* Energieversorgung Südsachsen Telekommunikationsgesellschaft mbH, Postfach 2 38, 09002 Chemnitz 010099 */
    default : return("UNKNOWN Provider");
  } /* switch */
} /* Providername */

/****************************************************************************/

char *t2tz(int zeit)
{
  switch (zeit) {
    case  0 : return("Vormittag");    break;
    case  1 : return("Nachmittag");   break;
    case  2 : return("Freizeit");     break;
    case  3 : return("Mondschein");   break;
    case  4 : return("Nacht");        break;
    case  5 : return("Standard");     break;
    case  6 : return("Spartarif");    break;
    case  7 : return("City Weekend"); break;
    case  8 : return("City Plus");    break;
    case  9 : return("Feiertag");     break;
    default : return("");             break;
  } /* switch */
} /* t2tz */

/****************************************************************************/

char *z2s(int zone)
{
  switch (zone) {
    case  1 : return("CityCall");     break;
    case  2 : return("RegioCall");    break;
    case  3 : return("GermanCall");   break;
    case  4 : return("GermanCall");   break;
    case  5 : return("GlobalCall");   break;
    case  6 : return("Vis1");         break;
    case  7 : return("Vis2");         break;
    case  8 : return("Vis3");         break;
    case  9 : return("Welt1");        break;
    case 10 : return("Welt2");        break;
    case 11 : return("Welt3");        break;
    case 12 : return("T-Online");     break;
    case 13 : return("KONF");         break;
    case 14 : return("Inmar");        break;
    case 15 : return("C-Box");        break;
    case 16 : return("T-Box");        break;
    case 21 : return("City");         break; /* City Plus */
    default : return("");             break;
  } /* switch */
} /* z2s */

/****************************************************************************/

