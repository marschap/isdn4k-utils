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
