/* $Id: asn1_generic.c,v 1.3 1999/12/31 13:30:01 akool Exp $
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
 * $Log: asn1_generic.c,v $
 * Revision 1.3  1999/12/31 13:30:01  akool
 * isdnlog-4.00 (Millenium-Edition)
 *  - Oracle support added by Jan Bolt (Jan.Bolt@t-online.de)
 *
 * Revision 1.2  1999/04/26 22:11:57  akool
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

ELEMENT_1(ParseInteger, int, integer)
{
  CHECK_TAG(ASN1_TAG_INTEGER);

  *integer = octets2Int(el);

  print_msg(PRT_DEBUG_DECODE, " DEBUG> Integer = %d\n", *integer);

  return 1;
}

ELEMENT_1(ParseOctetString, char, s)
{
  char *px;
  int i;

  CHECK_TAG(ASN1_TAG_OCTET_STRING);

  px = s;

  for (i = 0; i < el.length; i++) {
    px += strlen(px);
    sprintf(px, "%2x ", el.content.octets[i]);
  }

  print_msg(PRT_DEBUG_DECODE, " DEBUG> Octet String = %s\n", s);

  return 1;
}

ELEMENT(ParseNull)
{
  CHECK_TAG(ASN1_TAG_NULL);

  return 1;
}

ELEMENT_1(ParseEnum, int, num)
{
  CHECK_TAG(ASN1_TAG_ENUM);

  *num = octets2Int(el);

  print_msg(PRT_DEBUG_DECODE, " DEBUG> Enum = %d\n", *num);

  return 1;
}

ELEMENT_1(ParseBoolean, int, bl)
{
  CHECK_TAG(ASN1_TAG_BOOLEAN);

  if (octets2Int(el))
    *bl = 1;
  else
    *bl = 0;

  print_msg(PRT_DEBUG_DECODE, " DEBUG> Bool = %d\n", *bl);

  return 1;
}

ELEMENT_1(ParseNumericString, char, s)
{
  CHECK_TAG(ASN1_TAG_NUMERIC_STRING);

  strncpy(s, el.content.octets, el.length);
  s[el.length] = 0;

  print_msg(PRT_DEBUG_DECODE, " DEBUG> NumericString = %s\n", s);

  return 1;
}

ELEMENT_1(ParseIA5String, char, s)
{
  CHECK_TAG(ASN1_TAG_IA5_STRING);

  strncpy(s, el.content.octets, el.length);
  s[el.length] = 0;

  print_msg(PRT_DEBUG_DECODE, " DEBUG> IA5String = %s\n", s);

  return 1;
}
