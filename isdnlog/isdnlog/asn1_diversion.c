/* $Id: asn1_diversion.c,v 1.4 1999/12/31 13:30:01 akool Exp $
 *
 * ISDN accounting for isdn4linux. (ASN.1 parser)
 *
 * Copyright 1995 .. 2000 by Andreas Kool (akool@isdn4linux.de)
 *
 * ASN.1 parser written by Kai Germaschewski <kai@thphy.uni-duesseldorf.de>
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
 * $Log: asn1_diversion.c,v $
 * Revision 1.4  1999/12/31 13:30:01  akool
 * isdnlog-4.00 (Millenium-Edition)
 *  - Oracle support added by Jan Bolt (Jan.Bolt@t-online.de)
 *
 * Revision 1.3  1999/05/04 19:32:33  akool
 * isdnlog Version 3.24
 *
 *  - fully removed "sondernummern.c"
 *  - removed "gcc -Wall" warnings in ASN.1 Parser
 *  - many new entries for "rate-de.dat"
 *  - better "isdnconf" utility
 *
 * Revision 1.2  1999/04/26 22:11:56  akool
 * isdnlog Version 3.21
 *
 *  - CVS headers added to the asn* files
 *  - repaired the "4.CI" message directly on CONNECT
 *  - HANGUP message extended (CI's and EH's shown)
 *  - reactivated the OVERLOAD message
 *  - rate-at.dat extended
 *  - fixes from Michael Reinelt
 *
 *
 * Revision 0.1  1999/04/25 20:00.00  akool
 * Initial revision
 *
 */


#include "asn1.h"

ELEMENT_1(ParseARGActivationDiversion, char, );
ELEMENT_1(ParseARGDeactivationDiversion, char, );
ELEMENT_1(ParseARGActivationStatusNotificationDiv, char, );
ELEMENT_1(ParseARGDeactivationStatusNotificationDiv, char, );
ELEMENT_1(ParseARGInterrogationDiversion, char, );
ELEMENT_1(ParseARGDiversionInformation, char, );
ELEMENT_1(ParseRESInterrogationDiversion, char, );
ELEMENT_1(ParseRESInterrogateServedUserNumbers, char, );

ELEMENT_1(ParseIntResultList, char, );
ELEMENT_1(ParseIntResult, char, );
ELEMENT_1(ParseServedUserNr, char, );
ELEMENT_1(ParseProcedure, int, );
ELEMENT_1(ParseDiversionReason, int, );
ELEMENT_1(ParseBasicService, int, );

char* Procedure[] = {
  "CFU",
  "CFB",
  "CFNR",
};

const int NProcedure = 3;

char* DiversionReason[] = {
  "unknown",
  "CFU",
  "CFB",
  "CFNR",
  "CD Alerting",
  "CD Immediate",
};

const int NDiversionReason = 6;


ELEMENT_1(ParseARGActivationDiversion, char, s)
{
  int procedure, basicService;
  char forwardedToAddress[BUF_SIZE], servedUserNr[BUF_SIZE];
  auto char vsrc[BUFSIZ], vdst[BUFSIZ];
  auto int  a1, a2, a3, a4;


  CHECK_TAG(ASN1_TAG_SEQUENCE)
  MY_DEBUG("ParseARGActivationDiversion");

  SEQ_NOT_TAGGED_1(ParseProcedure, &procedure);
  SEQ_NOT_TAGGED_1(ParseBasicService, &basicService);
  SEQ_NOT_TAGGED_1(ParseAddress, forwardedToAddress);
  SEQ_NOT_TAGGED_1(ParseServedUserNr, servedUserNr);

  buildnumber(servedUserNr, 0, 0, call[6].num[CLIP], VERSION_EDSS1, &a1, &a2, &a3, &a4, 0, 999);
  strcpy(vsrc, vnum(6, CLIP));

  buildnumber(forwardedToAddress, 0, 0, call[6].num[CLIP], VERSION_EDSS1, &a1, &a2, &a3, &a4, 0, 999);
  strcpy(vdst, vnum(6, CLIP));

  sprintf(s, "activate %s %s/%s -> %s",
    Procedure[procedure], vsrc, BasicService[basicService], vdst);
  return 1;
}

ELEMENT_1(ParseARGDeactivationDiversion, char, s)
{
  int procedure, basicService;
  char servedUserNr[BUF_SIZE];
  auto char vsrc[BUFSIZ];
  auto int a1, a2, a3, a4;


  CHECK_TAG(ASN1_TAG_SEQUENCE)
  MY_DEBUG("ParseARGDeactivationDiversion");

  SEQ_NOT_TAGGED_1(ParseProcedure, &procedure);
  SEQ_NOT_TAGGED_1(ParseBasicService, &basicService);
  SEQ_NOT_TAGGED_1(ParseServedUserNr, servedUserNr);

  buildnumber(servedUserNr, 0, 0, call[6].num[CLIP], VERSION_EDSS1, &a1, &a2, &a3, &a4, 0, 999);
  strcpy(vsrc, vnum(6, CLIP));

  sprintf(s, "deactivate %s %s/%s",
    Procedure[procedure], vsrc, BasicService[basicService]);

  return 1;
}

ELEMENT_1(ParseARGActivationStatusNotificationDiv, char, s)
{
  int procedure, basicService;
  char forwardedToAddress[BUF_SIZE], servedUserNr[BUF_SIZE];

  CHECK_TAG(ASN1_TAG_SEQUENCE)
  MY_DEBUG("ParseARGActivationStatusNotificationDiv");

  SEQ_NOT_TAGGED_1(ParseProcedure, &procedure);
  SEQ_NOT_TAGGED_1(ParseBasicService, &basicService);
  SEQ_NOT_TAGGED_1(ParseAddress, forwardedToAddress);
  SEQ_NOT_TAGGED_1(ParseServedUserNr, servedUserNr);

  sprintf(s, "NOTIFICATION: activated %s %s/%s -> %s",
	    Procedure[procedure], servedUserNr, BasicService[basicService],
	    forwardedToAddress);
  return 1;
}

ELEMENT_1(ParseARGDeactivationStatusNotificationDiv, char, s)
{
  int procedure, basicService;
  char servedUserNr[BUF_SIZE];

  CHECK_TAG(ASN1_TAG_SEQUENCE)
  MY_DEBUG("ParseARGDeactivationStatusNotificationDiv");

  SEQ_NOT_TAGGED_1(ParseProcedure, &procedure);
  SEQ_NOT_TAGGED_1(ParseBasicService, &basicService);
  SEQ_NOT_TAGGED_1(ParseServedUserNr, servedUserNr);

  sprintf(s, "NOTIFICATION: deactivated %s %s/%s",
	  Procedure[procedure], servedUserNr, BasicService[basicService]);
  return 1;
}

ELEMENT_1(ParseARGInterrogationDiversion, char, s)
{
  int procedure, basicService;
  char servedUserNr[BUF_SIZE];


  CHECK_TAG(ASN1_TAG_SEQUENCE)
  MY_DEBUG("ParseARGInterrogationDiversion");

  SEQ_NOT_TAGGED_1(ParseProcedure, &procedure);
  SEQ_NOT_TAGGED_1(ParseBasicService, &basicService);  // todo DEFAULT
  SEQ_NOT_TAGGED_1(ParseServedUserNr, servedUserNr);

  sprintf(s, "InterrogationDiversion: %s(%s) %s",
	  Procedure[procedure], BasicService[basicService],
	  servedUserNr);
  return 1;
}

ELEMENT_1(ParseARGDiversionInformation, char, res)
{
  int diversionReason, basicService, lastDivertingReason;
  char servedUserSubaddress[25], callingAddress[255], originalCalledNr[255],
  lastDivertingNr[255]; // , userInfo[255];


  CHECK_TAG(ASN1_TAG_SEQUENCE)
  MY_DEBUG("ParseARGDiversionInformation");

  SEQ_NOT_TAGGED_1(ParseDiversionReason, &diversionReason);
  SEQ_NOT_TAGGED_1(ParseBasicService, &basicService);
  if ((el.content.elements[elnr].tag & 0x80) == 0) {
    SEQ_NOT_TAGGED_1(ParsePartySubaddress, servedUserSubaddress);
  } else {
    strcpy(servedUserSubaddress, "-");
  }
  if ((elnr < el.length) && (el.content.elements[elnr].tag &~ 0x20) == 0x80) {
    SEQ_EXP_TAGGED_1(ParsePresentedAddressScreened, callingAddress);
  } else {
    strcpy(callingAddress, "-");
  }
  if ((elnr < el.length) && (el.content.elements[elnr].tag &~ 0x20) == 0x81) {
    SEQ_EXP_TAGGED_1(ParsePresentedNumberUnscreened, originalCalledNr);
  } else {
    strcpy(originalCalledNr, "-");
  }
  if ((elnr < el.length) && (el.content.elements[elnr].tag &~ 0x20) == 0x82) {
    SEQ_EXP_TAGGED_1(ParsePresentedNumberUnscreened, lastDivertingNr);
  } else {
    strcpy(lastDivertingNr, "-");
  }
  if ((elnr < el.length) && (el.content.elements[elnr].tag &~ 0x20) == 0x83) {
    SEQ_EXP_TAGGED_1(ParseDiversionReason, &lastDivertingReason);
  } else {
    lastDivertingReason = -1;
  }
  //  SEQOPT_NOT_TAGGED_1(ParseQ931InformationElement, userInfo);

  sprintf(res, "DiversionInformation: %s (%s), servedUserSubaddress = %s, callingAddress = %s, originalCalledNr = %s, lastDivertingNr = %s, lastDivertingReason = %s",
	  DiversionReason[diversionReason], BasicService[basicService],
	  servedUserSubaddress, callingAddress, originalCalledNr,
	  lastDivertingNr,
	  (lastDivertingReason == -1)?"-":DiversionReason[lastDivertingReason]);

  return 1;
}

ELEMENT_1(ParseRESInterrogationDiversion, char, res)
{
  char s[BUF_SIZE];

  MY_DEBUG("ParseRESInterrogationDiversion");
  if (!ParseIntResultList(el, tag, s)) return 0;
  sprintf(res, "InterrogationDiversion Result: %s", s);

  return 1;
}

ELEMENT_1(ParseServedUserNumberList, char, res)
{
  int i;
  char num[BUF_SIZE];

  CHECK_TAG(ASN1_TAG_SET);
  MY_DEBUG("ParseServedUserNumberList");

  for (i = 0; i < el.length; i++) {
    if (!ParsePartyNumber(el.content.elements[i], -1, num)) return 0;
    if (i == 0) {
      sprintf(res, "%s", num);
    } else {
      sprintf(res, "%s %s", res, num);
    }
  }

  return 1;
}

ELEMENT_1(ParseRESInterrogateServedUserNumbers, char, res)
{
  char s[BUF_SIZE];

  MY_DEBUG("ParseRESInterrogateServedUserNumbers");
  if (!ParseServedUserNumberList(el, tag, s)) return 0;
  sprintf(res, "InterrogateServedUserNumbers Result: %s", s);

  return 1;
}

ELEMENT_1(ParseProcedure, int, procedure)
{
  MY_DEBUG("ParseProcedure");

  if (!ParseEnum(el, tag, procedure)) return 0;

  if ((*procedure < 0) || (*procedure > NProcedure))
    return 0;

  print_msg(PRT_DEBUG_DECODE, " DEBUG> procedure = %s\n",
	    Procedure[*procedure]);
  return 1;
}

ELEMENT_1(ParseDiversionReason, int, diversionReason)
{
  MY_DEBUG("ParseDiversionReason");

  if (!ParseEnum(el, tag, diversionReason)) return 0;

  if ((*diversionReason < 0) || (*diversionReason > NDiversionReason))
    return 0;

  print_msg(PRT_DEBUG_DECODE, " DEBUG> diversionReason = %s\n",
	    DiversionReason[*diversionReason]);
  return 1;
}

ELEMENT_1(ParseServedUserNr, char, num)
{
  MY_DEBUG("ParseServedUserNr");

  if (el.tag == 0x05) {
    strcpy(num, "allNumbers");
  } else {
    if (!ParsePartyNumber(el, ASN1_NOT_TAGGED, num)) return 0;
  }

  print_msg(PRT_DEBUG_DECODE, " DEBUG> ServedUserNr = %s\n", num);

  return 1;
}

ELEMENT_1(ParseIntResult, char, s)
{
  int procedure, basicService;
  char servedUserNr[BUF_SIZE], forwardedToAddress[BUF_SIZE];

  CHECK_TAG(ASN1_TAG_SEQUENCE);
  MY_DEBUG("ParseIntResult");

  SEQ_NOT_TAGGED_1(ParseServedUserNr, servedUserNr);
  SEQ_NOT_TAGGED_1(ParseBasicService, &basicService);
  SEQ_NOT_TAGGED_1(ParseProcedure, &procedure);
  SEQ_NOT_TAGGED_1(ParseAddress, forwardedToAddress);

  sprintf(s, "%s(%s) %s -> %s", Procedure[procedure],
	  BasicService[basicService], servedUserNr, forwardedToAddress);
  MY_DEBUG(s);

  return 1;
}

ELEMENT_1(ParseIntResultList, char, s)
{
  int i;
  char res[BUF_SIZE];

  CHECK_TAG(ASN1_TAG_SET);
  MY_DEBUG("ParseIntResultList");

  for (i = 0; i < el.length; i++) {
    if (!ParseIntResult(el.content.elements[i], ASN1_NOT_TAGGED, res)) return 0;
    if (i == 0) {
      sprintf(s, "%s", res);
    } else {
      sprintf(s, "%s; %s", s, res);
    }
  }

  return 1;
}
