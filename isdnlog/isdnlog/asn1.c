#include "asn1.h"

int splitASN1(char **asnp, int lev, Element* el)
{
  uchar c, l;
  int ll, result;
  char s[BUF_SIZE];
  char *px;
  int len = 0;

  strcpy(s, "                ");
  px = s + lev;

  el->tag = strtol(*asnp += 3, NIL, 16); len++;
  sprintf(px, "ASN TAG = %2x", el->tag);
  aoc_debug(el->tag, s);

  l = strtol(*asnp += 3, NIL, 16); len++;
  sprintf(px, "ASN l0 = %2x", l);
  aoc_debug(l, s);

  if (l == 0x80) { // indefinite length
    el->length = -1;
  } else if (l & 0x80) {
    l &= ~0x80;
    el->length = 0;
    while (l-- > 0) {
      c = strtol(*asnp += 3, NIL, 16); len++;
      sprintf(px, "ASN l1 = %2x", c);
      aoc_debug(c, s);
      el->length = (el->length >> 8) + c;
    }
  } else {
    el->length = l;
  }

  sprintf(px, "ASN length = %d", el->length);
  aoc_debug(-2, s);

  if (el->tag & 0x20) { // constructed
    el->content.elements = calloc(10, sizeof(Element));
    if (el->length == -1) { // indefinite length
      el->length = 0;
      while ((c = strtol(*asnp + 3, NIL, 16)) != 0x00) {
	len += splitASN1(asnp, lev + 1, &el->content.elements[el->length++]);
      }
      *asnp += 3; len ++;
      sprintf(px, "ASN ENDTAG = %2x", c);
      aoc_debug(c, s);
      c = strtol(*asnp += 3, NIL, 16); len++;
      sprintf(px, "ASN ENDTAG = %2x", c);
      aoc_debug(c, s);
      if (c != 0x00) return -1;
    } else {
      ll = el->length;
      el->length = 0;
      while (ll > 0) {
	result = splitASN1(asnp, lev + 1, &el->content.elements[el->length++]);
	ll -= result;
	len += result;
      }
    }
  } else {
    el->content.octets = calloc(200, sizeof(uchar));
    ll = el->length;
    el->length = 0;
    while (ll--) {
      c = strtol(*asnp += 3, NIL, 16); len++;
      sprintf(px, "ASN contents = %2x", c);
      aoc_debug(c, s);

      el->content.octets[el->length++] = c;
    }
  }

  sprintf(px, "ASN parsed len = %d", len);
  aoc_debug(-2, s);

  return len;
}

int octets2Int(Element el)
{
  int integer = 0;
  int i;

  for (i = 0; i < el.length; i++) {
    integer = (integer >> 8) + el.content.octets[i];
  }

  return integer;
}

void printASN1(Element el, int lev)
{
  char s[BUF_SIZE];
  char *px;
  int i;

  strcpy(s, "                ");
  px = s + lev;

  switch (el.tag) {
  case 0x02:
    sprintf(px, "Integer = %d", octets2Int(el));
    aoc_debug(-2, s);
    return;
  case 0x0a:
    sprintf(px, "Enum = %d", octets2Int(el));
    aoc_debug(-2, s);
    return;
  case 0x12:
    strcpy(px, "NumberDigits = ");
    px += strlen(px);
    for (i = 0; i < el.length; i++) {
      *px++ = el.content.octets[i];
    }
    *px = 0;
    aoc_debug(-2, s);
    return;

  case 0x30:
    sprintf(px, "SEQUENCE");
    break;
  case 0x31:
    sprintf(px, "SET");
    break;

  default:
    sprintf(px, "TAG = %2x", el.tag);
  }
  aoc_debug(-2, s);

  sprintf(px, "LENGTH = %d", el.length);
  aoc_debug(-2, s);

  if (el.tag & 0x20) { // constructed
    for (i = 0; i < el.length; i++) {
      printASN1(el.content.elements[i], lev+1);
    }
  } else { // primitive
    strcpy(px, "CONTENT = ");

    for (i = 0; i < el.length; i++) {
      px += strlen(px);
      sprintf(px, "%2x ", el.content.octets[i]);
    }
    aoc_debug(-2, s);
  }
}
