/* $Id: asn1_comp.c,v 1.3 1999/12/31 13:30:01 akool Exp $
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
 * $Log: asn1_comp.c,v $
 * Revision 1.3  1999/12/31 13:30:01  akool
 * isdnlog-4.00 (Millenium-Edition)
 *  - Oracle support added by Jan Bolt (Jan.Bolt@t-online.de)
 *
 * Revision 1.2  1999/04/26 22:11:55  akool
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

ELEMENT_1(ParseComponent, Aoc, );

ELEMENT_1(ParseInvokeComponent, Aoc, );
ELEMENT_1(ParseReturnResultComponent, Aoc, );
ELEMENT_1(ParseReturnResultComponentSequence, char, msg);
ELEMENT_1(ParseReturnErrorComponent, Aoc, );
ELEMENT_1(ParseInvokeID, int, );
ELEMENT_1(ParseOperationValue, int, );
ELEMENT_1(ParseErrorValue, int, );

char* OperationValue[] = {
  "?",
  "uUsa",                                        //  1
  "cUGCall",
  "mCIDRequest",
  "beginTPY",
  "endTPY",                                      //  5
  "eCTRequest",
  "activationDiversion",
  "deactivationDiversion",
  "activationStatusNotificationDiv",
  "deactivationStatusNotificationDiv",           // 10
  "interrogationDiversion",
  "diversionInformation",
  "callDeflection",
  "callRerouting",
  "divertingLegInformation2",                    // 15
  "invokeStatus",
  "interrogateServedUserNumbers",
  "divertingLegInformation1",
  "divertingLegInformation3",
  "explicitReservationCreationControl",          // 20
  "explicitReservationManagement",
  "explicitReservationCancel",
  "?",
  "mLPP lfb Query",
  "mLPP Call Request",                           // 25
  "mLPP Call preemption",
  "?",
  "?",
  "?",
  "chargingRequest",                             // 30
  "aOCSCurrency",
  "aOCSSpecialArrangement",
  "aOCDCurrency",
  "aOCDChargingUnit",
  "aOCECurrency",                                // 35
  "aOCEChargingUnit",
  "identificationOfChange",
  "?",
  "?",
  "beginConf",                                   // 40
  "addConf",
  "splitConf",
  "dropConf",
  "IsolateConf",
  "reattachConf",                                // 45
  "partyDISC",
  "floatConf",
  "endConf",
  "identifyConferee",
  "?",                                           // 50
  "?",
  "?",
  "?",
  "?",
  "?",                                           // 55
  "?",
  "?",
  "?",
  "?",
  "requestREV",                                  // 60
};

const int NOperationValue = 61;

char* ErrorValue[] = {
  "notSubscribed",
  "1",                                           //  1
  "2",
  "notAvailable",
  "notImplemented",
  "5",                                           //  5
  "invalidServedUserNr",
  "invalidCallState",
  "basicServiceNotProvided",
  "notIncomingCall",
  "supplementaryServiceInteractionNotAllowed",   // 10
  "resourceUnavailable",
  "invalidDivertedToNr",
  "13",
  "specialServiceNr",
  "diversionToServedUserNr",                     // 15
  "16",
  "17",
  "18",
  "19",
  "20",                                           // 20
  "21",
  "22",
  "incomingCallAccepted",
  "numberOfDiversionsExceeded",
  "25",                                           // 25
  "noChargingInfoAvailable",
  "27",
  "28",
  "29",
  "30",                                           // 30
  "31",
  "32",
  "33",
  "34",
  "35",                                           // 35
  "36",
  "37",
  "38",
  "39",
  "40",                                           // 40
  "41",
  "42",
  "43",
  "44",
  "45",                                           // 45
  "notActivated",
  "47",
  "requestAlreadyAccepted",
};

const int NErrorValue = 49;

// ---------------------------------------

ELEMENT_1(ParseComponent, Aoc, aoc)
{
  MY_DEBUG("ParseComponent");

  strcpy(aoc->msg, "");
  switch (IMP_TAG(el.tag)) {
    CASE_TAGGED_1(1, ParseInvokeComponent, aoc);
    CASE_TAGGED_1(2, ParseReturnResultComponent, aoc);
    CASE_TAGGED_1(3, ParseReturnErrorComponent, aoc);
  default:
    return 0;
  }

#if 0 /* DEBUG */
  if (strcmp(aoc->msg, "") != 0) {
    print_msg(PRT_SHOWNUMBERS, "%s\n", aoc->msg);
  }
#endif

  return 1;
}

ELEMENT_1(ParseInvokeComponent, Aoc, aoc)
{
  int invokeID, linkedID, operation;


  CHECK_TAG(ASN1_TAG_SEQUENCE);
  MY_DEBUG("ParseInvokeComponent");

  SEQ_NOT_TAGGED_1(ParseInvokeID, &invokeID);
  if ((el.content.elements[elnr].tag &~ 0xa0) == 0) {
    SEQ_TAGGED_1(ParseInvokeID, 0, &linkedID);
  }
  SEQ_NOT_TAGGED_1(ParseOperationValue, &operation);

  switch (operation) {
  case  7 : SEQ_NOT_TAGGED_1(ParseARGActivationDiversion, aoc->msg); break;
  case  8 : SEQ_NOT_TAGGED_1(ParseARGDeactivationDiversion, aoc->msg); break;
  case  9 : SEQ_NOT_TAGGED_1(ParseARGActivationStatusNotificationDiv, aoc->msg); break;
  case 10 : SEQ_NOT_TAGGED_1(ParseARGDeactivationStatusNotificationDiv, aoc->msg); break;
  case 11 : SEQ_NOT_TAGGED_1(ParseARGInterrogationDiversion, aoc->msg); break;
  case 12 : SEQ_NOT_TAGGED_1(ParseARGDiversionInformation, aoc->msg); break;

  case 33 : SEQ_NOT_TAGGED_1(ParseARGAOCDCurrency, aoc); break;
  case 34 : SEQ_NOT_TAGGED_1(ParseARGAOCDChargingUnit, aoc); break;
  case 35 : SEQ_NOT_TAGGED_1(ParseARGAOCECurrency, aoc); break;
  case 36 : SEQ_NOT_TAGGED_1(ParseARGAOCEChargingUnit, aoc); break;
  }

  return 1;
}

ELEMENT_1(ParseInvokeID, int, invokeID)
{
  MY_DEBUG("ParseInvokeID");

  if (!ParseInteger(el, tag, invokeID)) return 0;

  print_msg(PRT_DEBUG_DECODE, " DEBUG> invokeID = %d\n",
	    *invokeID);

  return 1;
}

ELEMENT_1(ParseReturnResultComponent, Aoc, aoc)
{
  int invokeID;

  CHECK_TAG(ASN1_TAG_SEQUENCE);
  MY_DEBUG("ParseReturnResultComponent");

  SEQ_NOT_TAGGED_1(ParseInvokeID, &invokeID);
  SEQOPT_NOT_TAGGED_1(ParseReturnResultComponentSequence, aoc->msg);

  return 1;
}

ELEMENT_1(ParseReturnResultComponentSequence, char, msg)
{
  int operationValue;

  CHECK_TAG(ASN1_TAG_SEQUENCE);
  MY_DEBUG("ParseReturnResultComponentSequence");

  SEQ_NOT_TAGGED_1(ParseOperationValue, &operationValue);

  switch (operationValue) {
  case 11 : SEQ_NOT_TAGGED_1(ParseRESInterrogationDiversion, msg); break;
  case 17 : SEQ_NOT_TAGGED_1(ParseRESInterrogateServedUserNumbers, msg); break;
  }

  return 1;
}

ELEMENT_1(ParseReturnErrorComponent, Aoc, aoc)
{
  int invokeID, errorValue;

  CHECK_TAG(ASN1_TAG_SEQUENCE);
  MY_DEBUG("ParseReturnErrorComponent");

  SEQ_NOT_TAGGED_1(ParseInvokeID, &invokeID);
  SEQ_NOT_TAGGED_1(ParseErrorValue, &errorValue);

  sprintf(aoc->msg, "ERROR: %s", ErrorValue[errorValue]);

  switch (errorValue) {
  }

  return 1;
}


ELEMENT_1(ParseOperationValue, int, operationValue)
{
  MY_DEBUG("ParseOperationValue");

  if (!ParseInteger(el, tag, operationValue)) return 0;

  if ((*operationValue < 0) || (*operationValue > NOperationValue))
    return 0;

  print_msg(PRT_DEBUG_DECODE, " DEBUG>  operationValue = %s\n",
	    OperationValue[*operationValue]);
  return 1;
}

ELEMENT_1(ParseErrorValue, int, errorValue)
{
  MY_DEBUG("ParseErrorValue");

  if (!ParseInteger(el, tag, errorValue)) return 0;

  if ((*errorValue < 0) || (*errorValue > NErrorValue))
    return 0;

  print_msg(PRT_DEBUG_DECODE, " DEBUG>  errorValue = %s\n",
	    ErrorValue[*errorValue]);
  return 1;
}
