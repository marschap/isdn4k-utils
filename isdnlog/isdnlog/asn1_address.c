#include "asn1.h"

ELEMENT_1(ParsePresentedAddressScreened, char, );
ELEMENT_1(ParsePresentedAddressUnscreened, char, );
ELEMENT_1(ParsePresentedNumberScreened, char, );
ELEMENT_1(ParsePresentedNumberUnscreened, char, );
ELEMENT_1(ParseAddressScreened, char, );
ELEMENT_1(ParseNumberScreened, char, );
ELEMENT_1(ParseAddress, char, );
ELEMENT_1(ParsePartyNumber, char, );
ELEMENT_1(ParsePublicPartyNumber, char, );
ELEMENT_1(ParsePrivatePartyNumber, char, );
ELEMENT_1(ParseNumberDigits, char, );
ELEMENT_1(ParsePublicTypeOfNumber, int, );
ELEMENT_1(ParsePrivateTypeOfNumber, int, );
ELEMENT_1(ParsePartySubaddress, char, );
ELEMENT_1(ParseUserSpecifiedSubaddress, char, );
ELEMENT_1(ParseNSAPSubaddress, char, );
ELEMENT_1(ParseSubaddressInformation, char, );
ELEMENT_1(ParseScreeningIndicator, int, );

char* PublicTypeOfNumber[] = {
  "unknown",
  "internationalNumber",
  "nationalNumber",
  "networkSpecificNumber",
  "subscriberNumber",
  "--",
  "abbreviatedNumber",
};

const int NPublicTypeOfNumber = 7;

char* PrivateTypeOfNumber[] = {
  "unknown",
  "level2RegionalNumber",
  "level1RegionalNumber",
  "pTNSpecificNumber",
  "localNumber",
  "--",
  "abbreviatedNumber",
};

const int NPrivateTypeOfNumber = 7;

char* ScreeningIndicator[] = {
  "userProvidedNotScreened",
  "userProvidedVerifiedAndPassed",
  "userProvidedVerifiedAndFailed",
  "networkProvided",
};

const int NScreeningIndicator = 4;


ELEMENT_1(ParsePresentedAddressScreened, char, num)
{
  char tmp[255];

  MY_DEBUG("ParsePresentedAddressScreened");

  switch (IMP_TAG(el.tag)) {
    CASE_TAGGED_1(0, ParseAddressScreened, tmp);
    CASE_TAGGED_1(3, ParseAddressScreened, tmp);
  }
  switch (IMP_TAG(el.tag)) {
  case 0 : sprintf(num, "presentationAllowedAddress: %s", tmp); break;
  case 1 : strcpy(num, "presentationRestricted"); break;
  case 2 : strcpy(num, "numberNotAvailableDueToInterworking"); break;
  case 3 : sprintf(num, "presentationRestrictedAddress: %s", tmp); break;
  default : return 0;
  }

  print_msg(PRT_DEBUG_DECODE, " DEBUG> %s\n", num);

  return 1;
}

ELEMENT_1(ParsePresentedAddressUnscreened, char, num)
{
  char tmp[255];

  MY_DEBUG("ParsePresentedAddressUnscreened");

  switch (IMP_TAG(el.tag)) {
    CASE_TAGGED_1(0, ParseAddress, tmp);
    CASE_TAGGED_1(3, ParseAddress, tmp);
  }
  switch (IMP_TAG(el.tag)) {
  case 0 : sprintf(num, "presentationAllowedAddress: %s", tmp); break;
  case 1 : strcpy(num, "presentationRestricted"); break;
  case 2 : strcpy(num, "numberNotAvailableDueToInterworking"); break;
  case 3 : sprintf(num, "presentationRestrictedAddress: %s", tmp); break;
  default : return 0;
  }

  print_msg(PRT_DEBUG_DECODE, " DEBUG> %s\n", num);

  return 1;
}

ELEMENT_1(ParsePresentedNumberScreened, char, num)
{
  char tmp[255];

  MY_DEBUG("ParsePresentedNumberScreened");

  switch (IMP_TAG(el.tag)) {
    CASE_TAGGED_1(0, ParseNumberScreened, tmp);
    CASE_TAGGED_1(3, ParseNumberScreened, tmp);
  }
  switch (IMP_TAG(el.tag)) {
  case 0 : sprintf(num, "presentationAllowedNumber: %s", tmp); break;
  case 1 : strcpy(num, "presentationRestricted"); break;
  case 2 : strcpy(num, "numberNotAvailableDueToInterworking"); break;
  case 3 : sprintf(num, "presentationRestrictedNumber: %s", tmp); break;
  default : return 0;
  }

  print_msg(PRT_DEBUG_DECODE, " DEBUG> %s\n", num);

  return 1;
}

ELEMENT_1(ParsePresentedNumberUnscreened, char, num)
{
  char tmp[255];

  MY_DEBUG("ParsePresentedNumberUnscreened");

  switch (IMP_TAG(el.tag)) {
    CASE_TAGGED_1(0, ParsePartyNumber, tmp);
    CASE_TAGGED_1(3, ParsePartyNumber, tmp);
  }
  switch (IMP_TAG(el.tag)) {
  case 0 : sprintf(num, "presentationAllowedNumber: %s", tmp); break;
  case 1 : strcpy(num, "presentationRestricted"); break;
  case 2 : strcpy(num, "numberNotAvailableDueToInterworking"); break;
  case 3 : sprintf(num, "presentationRestrictedNumber: %s", tmp); break;
  default : return 0;
  }

  print_msg(PRT_DEBUG_DECODE, " DEBUG> %s\n", num);

  return 1;
}

ELEMENT_1(ParseAddressScreened, char, addr)
{
  char num[BUF_SIZE];
  char sa[BUF_SIZE];
  int screeningIndicator;

  CHECK_TAG(ASN1_TAG_SEQUENCE);
  MY_DEBUG("ParseAddressScreened");

  strcpy(sa, "");
  SEQ_NOT_TAGGED_1(ParsePartyNumber, num);
  SEQ_NOT_TAGGED_1(ParseScreeningIndicator, &screeningIndicator);
  SEQOPT_NOT_TAGGED_1(ParsePartySubaddress, sa);

  if (strcmp(sa, "") != 0)
    sprintf(addr, "%s SUB %s", num, sa);
  else
    strcpy(addr, num);
  return 1;
}

ELEMENT_1(ParseNumberScreened, char, addr)
{
  int screeningIndicator;

  CHECK_TAG(ASN1_TAG_SEQUENCE);
  MY_DEBUG("ParseNumberScreened");

  SEQ_NOT_TAGGED_1(ParsePartyNumber, addr);
  SEQ_NOT_TAGGED_1(ParseScreeningIndicator, &screeningIndicator);

  return 1;
}

ELEMENT_1(ParseAddress, char, addr)
{
  char num[BUF_SIZE];
  char sa[BUF_SIZE];

  CHECK_TAG(ASN1_TAG_SEQUENCE);
  MY_DEBUG("ParseAddress");

  strcpy(sa, "");
  SEQ_NOT_TAGGED_1(ParsePartyNumber, num);
  SEQOPT_NOT_TAGGED_1(ParsePartySubaddress, sa);

  if (strcmp(sa, "") != 0)
    sprintf(addr, "%s SUB %s", num, sa);
  else
    strcpy(addr, num);
  return 1;
}

ELEMENT_1(ParsePartyNumber, char, num)
{
  int a1, a2, a3, a4;

  MY_DEBUG("ParsePartyNumber");

  switch (IMP_TAG(el.tag)) {
    CASE_TAGGED_1(0, ParseNumberDigits, num);       // unknownPartyNumber
    CASE_TAGGED_1(1, ParsePublicPartyNumber, num);  // publicPartyNumber
    CASE_TAGGED_1(3, ParseNumberDigits, num);       // dataPartyNumber
    CASE_TAGGED_1(4, ParseNumberDigits, num);       // telexPartyNumber
    CASE_TAGGED_1(5, ParsePrivatePartyNumber, num); // privatePartyNumber
    CASE_TAGGED_1(8, ParseNumberDigits, num);       // nationalStandardPartyNumber
  default:
    return 0;
  }
  if (el.tag == 0) {
    buildnumber(num, 0, 0, call[6].num[CLIP],
		VERSION_EDSS1, &a1, &a2, &a3, &a4, 0, 999);
    strcpy(num, vnum(6, CLIP));
  }
  return 1;
}

ELEMENT_1(ParsePublicPartyNumber, char, num)
{
  int pton;
  char digits[BUF_SIZE];
  int a1, a2, a3, a4;

  CHECK_TAG(ASN1_TAG_SEQUENCE);
  MY_DEBUG("ParsePublicPartyNumber");

  SEQ_NOT_TAGGED_1(ParsePublicTypeOfNumber, &pton);
  SEQ_NOT_TAGGED_1(ParseNumberDigits, digits);

  buildnumber(digits, pton << 4, 0, call[6].num[CLIP],
	      VERSION_EDSS1, &a1, &a2, &a3, &a4, 0, 999);
  strcpy(num, vnum(6, CLIP));

  return 1;
}

ELEMENT_1(ParsePrivatePartyNumber, char, num)
{
  int pton;
  char digits[BUF_SIZE];


  CHECK_TAG(ASN1_TAG_SEQUENCE);
  MY_DEBUG("ParsePrivatePartyNumber");

  SEQ_NOT_TAGGED_1(ParsePrivateTypeOfNumber, &pton);
  SEQ_NOT_TAGGED_1(ParseNumberDigits, digits);

  sprintf(num, "%s %s", PrivateTypeOfNumber[pton], digits);

  return 1;
}

ELEMENT_1(ParseNumberDigits, char, s)
{
  MY_DEBUG("ParseNumberDigits");

  return ParseNumericString(el, tag, s);
}

ELEMENT_1(ParsePublicTypeOfNumber, int, publicTypeOfNumber)
{
  MY_DEBUG("ParsePublicTypeOfNumber");

  if (!ParseEnum(el, tag, publicTypeOfNumber)) return 0;

  if ((*publicTypeOfNumber < 0) || (*publicTypeOfNumber > NPublicTypeOfNumber))
    return 0;

  print_msg(PRT_DEBUG_DECODE, " DEBUG> publicTypeOfNumber = %s\n",
	    PublicTypeOfNumber[*publicTypeOfNumber]);
  return 1;
}

ELEMENT_1(ParsePrivateTypeOfNumber, int, privateTypeOfNumber)
{
  MY_DEBUG("ParsePrivateTypeOfNumber");

  if (!ParseEnum(el, tag, privateTypeOfNumber)) return 0;

  if ((*privateTypeOfNumber < 0) || (*privateTypeOfNumber > NPrivateTypeOfNumber))
    return 0;

  print_msg(PRT_DEBUG_DECODE, " DEBUG> publicTypeOfNumber = %s\n",
	    PrivateTypeOfNumber[*privateTypeOfNumber]);
  return 1;
}

ELEMENT_1(ParsePartySubaddress, char, sa)
{
  MY_DEBUG("ParsePartySubaddress");

  switch (el.tag &~ 0x20) {
    CASE_1(ASN1_TAG_SEQUENCE, ParseUserSpecifiedSubaddress, sa);
    CASE_1(ASN1_TAG_OCTET_STRING, ParseNSAPSubaddress, sa);
  default:
    return 0;
  }
  return 1;
}

ELEMENT_1(ParseUserSpecifiedSubaddress, char, sa)
{
  int oddCountIndicator = -2;

  CHECK_TAG(ASN1_TAG_SEQUENCE);
  MY_DEBUG("ParsePublicPartyNumber");

  SEQ_NOT_TAGGED_1(ParseSubaddressInformation, sa);
  SEQOPT_NOT_TAGGED_1(ParseBoolean, &oddCountIndicator);

  if (oddCountIndicator != -2) {
    sprintf(sa, "%s %s", sa, oddCountIndicator?"odd":"even");
  }
  return 1;
}

ELEMENT_1(ParseNSAPSubaddress, char, sa)
{
  MY_DEBUG("ParseNSAPSubaddress");

  return ParseOctetString(el, tag, sa);
}

ELEMENT_1(ParseSubaddressInformation, char, sa)
{
  MY_DEBUG("ParseSubaddressInformation");

  return ParseOctetString(el, tag, sa);
}

ELEMENT_1(ParseScreeningIndicator, int, screeningIndicator)
{
  MY_DEBUG("ParseScreeningIndicator");

  if (!ParseEnum(el, tag, screeningIndicator)) return 0;

  if ((*screeningIndicator < 0) || (*screeningIndicator > NScreeningIndicator))
    return 0;

  print_msg(PRT_DEBUG_DECODE, " DEBUG> screeningIndicator = %s\n",
	    ScreeningIndicator[*screeningIndicator]);
  return 1;
}
