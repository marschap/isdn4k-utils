#ifndef ASN1_H
#define ASN1_H

#include "isdnlog.h"

void buildnumber(char *num, int oc3, int oc3a,
		 char *result, int version, int *provider,
		 int *sondernummer, int *intern, int *internetnumber,
		 int dir, int who);

#define ASN1_TAG_BOOLEAN           (0x01) // is that true?
#define ASN1_TAG_INTEGER           (0x02)
#define ASN1_TAG_BIT_STRING        (0x03)
#define ASN1_TAG_OCTET_STRING      (0x04)
#define ASN1_TAG_NULL              (0x05)
#define ASN1_TAG_OBJECT_IDENTIFIER (0x06)
#define ASN1_TAG_ENUM              (0x0a)
#define ASN1_TAG_SEQUENCE          (0x10)
#define ASN1_TAG_SET               (0x11)
#define ASN1_TAG_NUMERIC_STRING    (0x12)
#define ASN1_TAG_PRINTABLE_STRING  (0x13)
#define ASN1_TAG_IA5_STRING        (0x16)
#define ASN1_TAG_UTC_TIME          (0x17)

#define ASN1_NOT_TAGGED            (-1)


typedef unsigned char uchar;

struct Element;
union Content;

typedef struct Element Element;
typedef union Content Content;

union Content {
  Element *elements;
  uchar     *octets;
};

struct Element {
  uchar     tag;
  int       length;
  Content content;
};

struct Aoc {
  int       type;
  char      currency[11];
  int       amount;
  float     multiplier;
  char      msg[255];
  int       type_of_charging_info;
};

typedef struct Aoc Aoc;

char aoc_deb[BUF_SIZE];

void aoc_debug(int val, char *s);
int splitASN1(char **asnp, int lev, Element* el);
int octets2Int(Element el);
void printASN1(Element el, int lev);

// -- Makros

#define ELEMENT(name) \
int name(Element el, int tag)

#define ELEMENT_1(name, type, parm) \
int name(Element el, int tag, type *parm)

// -- Elements -------------------------------------------------------------

// asn1_generic --

ELEMENT_1(ParseInteger, int, );
ELEMENT_1(ParseOctetString, char, );
ELEMENT(ParseNull);
ELEMENT_1(ParseBoolean, int, );
ELEMENT_1(ParseEnum, int, );
ELEMENT_1(ParseNumericString, char, );
ELEMENT_1(ParseIA5String, char, );

// asn1_comp.c -- EN 300 196-1, Table D.1

ELEMENT_1(ParseComponent, Aoc, );

// asn1_aoc.c -- EN 300 182-1, Table 2

ELEMENT_1(ParseARGAOCDCurrency, Aoc, );
ELEMENT_1(ParseARGAOCDChargingUnit, Aoc, );
ELEMENT_1(ParseARGAOCECurrency, Aoc, );
ELEMENT_1(ParseARGAOCEChargingUnit, Aoc, );
ELEMENT_1(ParseChargingAssociation, char, );

// aoc_address.c -- EN 300 196-1, Table D.3

ELEMENT_1(ParsePresentedAddressScreened, char, );
ELEMENT_1(ParsePresentedAddressUnscreened, char, );
ELEMENT_1(ParsePresentedNumberScreened, char, );
ELEMENT_1(ParsePresentedNumberUnscreened, char, );
ELEMENT_1(ParsePartyNumber, char, );
ELEMENT_1(ParsePartySubaddress, char, );

// aoc_basic_service.c -- EN 300 196-1, Table D.6

#define NBasicService 43
char* BasicService[NBasicService];
ELEMENT_1(ParseBasicService, int, );

// aoc_diversion.c -- EN 300 207-1, Table 3

ELEMENT_1(ParseARGActivationDiversion, char, );
ELEMENT_1(ParseARGDeactivationDiversion, char, );
ELEMENT_1(ParseARGActivationStatusNotificationDiv, char, );
ELEMENT_1(ParseARGDeactivationStatusNotificationDiv, char, );
ELEMENT_1(ParseARGInterrogationDiversion, char, );
ELEMENT_1(ParseARGDiversionInformation, char, );
ELEMENT_1(ParseRESInterrogationDiversion, char, );
ELEMENT_1(ParseRESInterrogateServedUserNumbers, char, );

// -------------------------------------------------------------------------------
// -- Makros

#define SEQ_NOT_TAGGED(a) \
  if (elnr >= el.length) return 0;  \
  if (!a(el.content.elements[elnr++], ASN1_NOT_TAGGED)) { \
    print_msg(PRT_DEBUG_DECODE, " DEBUG> SEQ_NOT_TAGGED: got FALSE\n"); \
    return 0; \
  }

#define SEQ_TAGGED(a, tag) \
  if (elnr >= el.length) return 0;  \
  if (!a(el.content.elements[elnr++], tag)) { \
    print_msg(PRT_DEBUG_DECODE, " DEBUG> SEQ_TAGGED: got FALSE\n"); \
    return 0; \
  }

#define SEQ_NOT_TAGGED_1(a, b) \
  if (elnr >= el.length) return 0;  \
  if (!a(el.content.elements[elnr++], ASN1_NOT_TAGGED, b)) { \
    print_msg(PRT_DEBUG_DECODE, " DEBUG> SEQ_NOT_TAGGED_1: got FALSE\n"); \
    return 0; \
  }

#define SEQ_EXP_TAGGED_1(a, b) \
  if (elnr >= el.length) return 0;  \
  if (!a(el.content.elements[elnr++].content.elements[0], ASN1_NOT_TAGGED, b)) { \
    print_msg(PRT_DEBUG_DECODE, " DEBUG> SEQ_EXP_TAGGED_1: got FALSE\n"); \
    return 0; \
  }

#define SEQ_TAGGED_1(a, tag, b) \
  if (elnr >= el.length) return 0;  \
  if (!a(el.content.elements[elnr++], tag, b)) { \
    print_msg(PRT_DEBUG_DECODE, " DEBUG> SEQ_TAGGED_1: got FALSE\n"); \
    return 0; \
  }

#define SEQOPT_NOT_TAGGED(a) \
  if (elnr < el.length) {  \
    if (!a(el.content.elements[elnr++], ASN1_NOT_TAGGED)) { \
      print_msg(PRT_DEBUG_DECODE, " DEBUG> SEQOPT_NOT_TAGGED: got FALSE\n"); \
      return 0; \
    } \
  }

#define SEQOPT_TAGGED(a, tag) \
  if (elnr < el.length) {  \
    if (!a(el.content.elements[elnr++], tag)) { \
      print_msg(PRT_DEBUG_DECODE, " DEBUG> SEQOPT_TAGGED: got FALSE\n"); \
      return 0; \
    } \
  }

#define SEQOPT_NOT_TAGGED_1(a, b) \
  if (elnr < el.length) {  \
    if (!a(el.content.elements[elnr++], ASN1_NOT_TAGGED, b)) { \
      print_msg(PRT_DEBUG_DECODE, " DEBUG> SEQOPT_NOT_TAGGED_1: got FALSE\n"); \
      return 0; \
    } \
  }

#define SEQOPT_TAGGED_1(a, tag, b) \
  if (elnr < el.length) {  \
    if (!a(el.content.elements[elnr++], tag, b)) { \
      print_msg(PRT_DEBUG_DECODE, " DEBUG> SEQOPT_NOT_TAGGED_1: got FALSE\n"); \
      return 0; \
    } \
  }

#define IMP_TAG(a) ((a) &~ 0xa0)

#define CASE_TAGGED(a, b) \
  case a: if (!b(el, a)) return 0; \
          break;

#define CASE_TAGGED_1(a, b, c) \
  case a: if (!b(el, a, c)) return 0; \
          break;

#define CASE_1(a, b, c) \
  case a: if (!b(el, ASN1_NOT_TAGGED, c)) return 0; \
          break;

#define MY_DEBUG(a) \
    print_msg(PRT_DEBUG_DECODE, " DEBUG> %s\n", a);


#define CHECK_TAG(a) \
  int elnr = 0; \
  \
  elnr = elnr; // makes lint happy \
  if (tag == ASN1_NOT_TAGGED) { \
    if ((el.tag &~ 0x20) != (a)) { \
      print_msg(PRT_DEBUG_DECODE, " DEBUG> native tag %2x not found, %2x instead\n", \
                (a), el.tag); \
      return 0; \
    } \
  } else { \
    if ((el.tag &~ 0x20) != (0x80|(tag))) { \
      print_msg(PRT_DEBUG_DECODE, " DEBUG> given tag %2x not found, %2x instead\n", \
		(tag), el.tag); \
      return 0; \
    } \
  }


#endif
