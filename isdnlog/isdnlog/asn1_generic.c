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
