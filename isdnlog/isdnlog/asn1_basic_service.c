/* $Id: asn1_basic_service.c,v 1.4 1999/12/31 13:30:01 akool Exp $
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
 * $Log: asn1_basic_service.c,v $
 * Revision 1.4  1999/12/31 13:30:01  akool
 * isdnlog-4.00 (Millenium-Edition)
 *  - Oracle support added by Jan Bolt (Jan.Bolt@t-online.de)
 *
 * Revision 1.3  1999/10/30 13:42:36  akool
 * isdnlog-3.60
 *   - many new rates
 *   - compiler warnings resolved
 *   - fixed "Sonderrufnummer" Handling
 *
 * Revision 1.2  1999/04/26 22:11:54  akool
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

#define _ASN1_BASIC_SERVICE_C
#include "asn1.h"

/* char* BasicService[NBasicService]; -lt- is in asn1.h */
ELEMENT_1(ParseBasicService, int, );

// --

char* BasicService[NBasicService] = {
  "allServices",
  "speech",
  "unrestrictedDigitalInformation",
  "audio3100Hz",
  "unrestrictedDigitalInformationWithTonesAndAnnouncements",
  "multirate", // 5
  "?", "?", "?", "?", "?", "?", "?", "?", "?", "?", // 15
  "?", "?", "?", "?", "?", "?", "?", "?",
  "?", "?", "?", "?", "?", "?", "?", "?", // 31
  "telephony", // 32
  "teletex",
  "telefaxGroup4Class1",
  "videotexSyntaxBased",
  "videotelephony",
  "telefaxGroup2-3",
  "telephony7kHz",
  "euroFileTransfer",
  "fileTransferAndAccessManagement",
  "videoconference",
  "audioGraphicConference", // 42
};

ELEMENT_1(ParseBasicService, int, basicService)
{
  MY_DEBUG("ParseBasicService");

  if (!ParseEnum(el, tag, basicService)) return 0;

  if ((*basicService < 0) || (*basicService > NBasicService))
    return 0;

  print_msg(PRT_DEBUG_DECODE, " DEBUG> basicService = %s\n",
	    BasicService[*basicService]);
  return 1;
}
