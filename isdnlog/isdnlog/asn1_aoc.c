#include "asn1.h"

ELEMENT_1(ParseARGAOCDCurrency, Aoc, );
ELEMENT_1(ParseARGAOCDChargingUnit, Aoc, );
ELEMENT_1(ParseARGAOCECurrency, Aoc, );
ELEMENT_1(ParseARGAOCEChargingUnit, Aoc, );

ELEMENT_1(ParseAOCDCurrencyInfo, Aoc, );
ELEMENT_1(ParseAOCDSpecificCurrency, Aoc, );
ELEMENT_1(ParseAOCDChargingUnitInfo, Aoc, );
ELEMENT_1(ParseAOCDSpecificChargingUnits, Aoc, );
ELEMENT_1(ParseRecordedCurrency, Aoc, );
ELEMENT_1(ParseRecordedUnitsList, Aoc, );
ELEMENT_1(ParseTypeOfChargingInfo, int, );
ELEMENT_1(ParseRecordedUnits, Aoc, );
ELEMENT_1(ParseRecordedUnitsChoice, Aoc, );
ELEMENT_1(ParseAOCDBillingId, int, );
ELEMENT_1(ParseAOCECurrencyInfo, Aoc, );
ELEMENT_1(ParseAOCECurrencyInfoChoice, Aoc, );
ELEMENT_1(ParseAOCESpecificCurrency, Aoc, );
ELEMENT_1(ParseAOCEChargingUnitInfo, Aoc, );
ELEMENT_1(ParseAOCEChargingUnitInfoChoice, Aoc, );
ELEMENT_1(ParseAOCESpecificChargingUnits, Aoc, );
ELEMENT_1(ParseAOCEBillingId, int, );
ELEMENT_1(ParseCurrency, char, msg);
ELEMENT_1(ParseAmount, Aoc, aoc);
ELEMENT_1(ParseCurrencyAmount, int, );
ELEMENT_1(ParseMultiplier, int, );
ELEMENT_1(ParseTypeOfUnit, int, );
ELEMENT_1(ParseNumberOfUnits, int, );
ELEMENT_1(ParseChargingAssociation, char, );
ELEMENT_1(ParseChargeIdentifier, int, );

char* XTypeOfChargingInfo[] = {
  "subTotal",
  "total",
};

const int NTypeOfChargingInfo = 2;

char* AOCDBillingId[] = {
  "normalCharging",
  "reverseCharging",
  "creditCardCharging",
};

const int NAOCDBillingId = 3;

char* AOCEBillingId[] = {
  "normalCharging",
  "reverseCharging",
  "creditCardCharging",
  "callForwardingUnconditional",
  "callForwardingBusy",
  "callForwardingNoReply",
  "callDeflection",
  "callTransfer",
};

const int NAOCEBillingId = 8;

float XMultiplier[] = {
  0.001,
  0.01,
  0.1,
  1,
  10,
  100,
  1000,
};

const int NMultiplier = 7;

// ---------------------------------------

ELEMENT_1(ParseARGAOCDCurrency, Aoc, aoc)
{
  char tmp[255];

  CHECK_TAG(ASN1_TAG_SEQUENCE);
  MY_DEBUG("ParseARGAOCDCurrency");

  aoc->type = 33; // AOCDCurrency
  if (ParseAOCDCurrencyInfo(el, ASN1_NOT_TAGGED, aoc)) {
    ;
  } else if (ParseNull(el, ASN1_NOT_TAGGED)) {
    aoc->amount = -1; // chargeNotAvailable
    strcpy(aoc->msg, "chargeNotAvailable");
    strcpy(aoc->currency, "");
  } else {
    return 0;
  }

  sprintf(tmp, "AOC-D: %f %s %s\n",
	  aoc->amount*aoc->multiplier, aoc->currency, aoc->msg);
  strcpy(aoc->msg, tmp);
  print_msg(PRT_DEBUG_DECODE, " DEBUG> %s\n", aoc->msg);

  return 1;
}

ELEMENT_1(ParseARGAOCDChargingUnit, Aoc, aoc)
{
  char tmp[255];

  MY_DEBUG("ParseARGAOCDChargingUnit");

  aoc->type = 34; // AOCDChargingUnit
  if (ParseAOCDChargingUnitInfo(el, ASN1_NOT_TAGGED, aoc)) {
    ;
  } else if (ParseNull(el, ASN1_NOT_TAGGED)) {
    aoc->amount = 1; // chargeNotAvailable
    strcpy(aoc->msg, "chargeNotAvailable");
  } else {
    return 0;
  }

  sprintf(tmp, "AOC-D: %d EH, %s", aoc->amount, aoc->msg);
  strcpy(aoc->msg, tmp);
  print_msg(PRT_DEBUG_DECODE, " DEBUG> %s\n", aoc->msg);

  return 1;
}

ELEMENT_1(ParseARGAOCECurrency, Aoc, aoc)
{
  char tmp[255];

  CHECK_TAG(ASN1_TAG_SEQUENCE);
  MY_DEBUG("ParseARGAOCECurrency");

  aoc->type = 35; // AOCECurrency
  if (ParseAOCECurrencyInfo(el, ASN1_NOT_TAGGED, aoc)) {
    ;
  } else if (ParseNull(el, ASN1_NOT_TAGGED)) {
    aoc->amount = -1; // chargeNotAvailable
    strcpy(aoc->msg, "chargeNotAvailable");
    strcpy(aoc->currency, "");
  } else {
    return 0;
  }

  sprintf(tmp, "AOC-D: %f %s %s\n",
	  aoc->amount*aoc->multiplier, aoc->currency, aoc->msg);
  strcpy(aoc->msg, tmp);
  print_msg(PRT_DEBUG_DECODE, " DEBUG> %s\n", aoc->msg);

  return 1;
}

ELEMENT_1(ParseARGAOCEChargingUnit, Aoc, aoc)
{
  char tmp[255];

  MY_DEBUG("ParseARGAOCEChargingUnit");

  aoc->type = 36; // AOCEChargingUnit
  if (ParseAOCEChargingUnitInfo(el, ASN1_NOT_TAGGED, aoc)) {
    ;
  } else if (ParseNull(el, ASN1_NOT_TAGGED)) {
    aoc->amount = -1; // chargeNotAvailable
    strcpy(aoc->msg, "chargeNotAvailable");
  } else {
    return 0;
  }

  sprintf(tmp, "AOC-E: %d EH, %s", aoc->amount, aoc->msg);
  strcpy(aoc->msg, tmp);
  print_msg(PRT_DEBUG_DECODE, " DEBUG> %s\n", aoc->msg);

  return 1;
}

ELEMENT_1(ParseAOCDCurrencyInfo, Aoc, aoc)
{
 MY_DEBUG("ParseAOCDCurrencyInfo");

  if (ParseAOCDSpecificCurrency(el, tag, aoc)) {
    ;
  } else if (ParseNull(el, 1)) {
    aoc->amount = 0; // freeOfCharge
    strcpy(aoc->msg, "freeOfCharge");
  } else {
    return 0;
  }

  return 1;
}

ELEMENT_1(ParseAOCDSpecificCurrency, Aoc, aoc)
{
  int billingId = -1;

  CHECK_TAG(ASN1_TAG_SEQUENCE);
  MY_DEBUG("ParseAOCDSpecificCurrency");

  SEQ_TAGGED_1(ParseRecordedCurrency, 1, aoc);
  SEQ_TAGGED_1(ParseTypeOfChargingInfo, 2, &aoc->type_of_charging_info);
  SEQOPT_TAGGED_1(ParseAOCDBillingId, 3, &billingId);

  sprintf(aoc->msg, "typeOfChargingInfo = %s, billingId = %s",
	  XTypeOfChargingInfo[aoc->type_of_charging_info],
	  (billingId==-1)?"-":AOCDBillingId[billingId]);

  return 1;
}

ELEMENT_1(ParseAOCDChargingUnitInfo, Aoc, aoc)
{
  MY_DEBUG("ParseAOCDChargingUnitInfo");

  if (ParseAOCDSpecificChargingUnits(el, tag, aoc)) {
    ;
  } else if (ParseNull(el, 1)) {
    aoc->amount = 0; // freeOfCharge
    strcpy(aoc->msg, "freeOfCharge");
  } else {
    return 0;
  }

  return 1;
}

ELEMENT_1(ParseAOCDSpecificChargingUnits, Aoc, aoc)
{
  int billingId = -1;

  CHECK_TAG(ASN1_TAG_SEQUENCE);
  MY_DEBUG("ParseAOCDSpecificChargingUnits");

  SEQ_TAGGED_1(ParseRecordedUnitsList, 1, aoc);
  SEQ_TAGGED_1(ParseTypeOfChargingInfo, 2, &aoc->type_of_charging_info);
  SEQOPT_TAGGED_1(ParseAOCDBillingId, 3, &billingId);

  sprintf(aoc->msg, "typeOfChargingInfo = %s, billingId = %s",
	  XTypeOfChargingInfo[aoc->type_of_charging_info],
	  (billingId==-1)?"-":AOCDBillingId[billingId]);

  return 1;
}

ELEMENT_1(ParseRecordedCurrency, Aoc, aoc)
{
  CHECK_TAG(ASN1_TAG_SEQUENCE);
  MY_DEBUG("ParseRecordedCurrency");

  SEQ_TAGGED_1(ParseCurrency, 1, aoc->currency);
  SEQ_TAGGED_1(ParseAmount, 2, aoc);

  return 1;
}

ELEMENT_1(ParseRecordedUnitsList, Aoc, aoc)
{
  CHECK_TAG(ASN1_TAG_SEQUENCE);
  MY_DEBUG("ParseRecordedUnitsList");

  aoc->amount = 0;
  while (elnr < el.length) {
    SEQ_NOT_TAGGED_1(ParseRecordedUnits, aoc);
  }

  return 1;
}

ELEMENT_1(ParseTypeOfChargingInfo, int, typeOfChargingInfo)
{
  MY_DEBUG("ParseTypeOfChargingInfo");

  if (!ParseEnum(el, tag, typeOfChargingInfo)) return 0;

  if ((*typeOfChargingInfo < 0) || (*typeOfChargingInfo > NTypeOfChargingInfo))
    return 0;

  print_msg(PRT_DEBUG_DECODE, " DEBUG> typeOfChargingInfo = %s\n",
	    XTypeOfChargingInfo[*typeOfChargingInfo]);
  return 1;
}

ELEMENT_1(ParseRecordedUnits, Aoc, aoc)
{
  int typeOfUnit = -1;

  CHECK_TAG(ASN1_TAG_SEQUENCE);
  MY_DEBUG("ParseRecordedUnits");

  SEQ_NOT_TAGGED_1(ParseRecordedUnitsChoice, aoc);
  SEQOPT_NOT_TAGGED_1(ParseTypeOfUnit, &typeOfUnit);

  if (typeOfUnit != -1) {
    sprintf(aoc->msg, "typeOfUnit = %d", typeOfUnit);
  }
  return 1;
}

ELEMENT_1(ParseRecordedUnitsChoice, Aoc, aoc)
{
  int eh;

  MY_DEBUG("ParseRecordedUnitsChoice");

  if (ParseNumberOfUnits(el, ASN1_NOT_TAGGED, &eh)) {
    aoc->amount += eh;
  } else if (ParseNull(el, ASN1_NOT_TAGGED)) {
    aoc->amount = -1; // notAvailable
  } else {
    return 0;
  }
  return 1;
}

ELEMENT_1(ParseAOCDBillingId, int, billingId)
{
  MY_DEBUG("ParseAOCDBillingId");

  if (!ParseEnum(el, tag, billingId)) return 0;

  if ((*billingId < 0) || (*billingId > NAOCDBillingId))
    return 0;

  print_msg(PRT_DEBUG_DECODE, " DEBUG> AOCDBillingId = %s\n",
	    AOCDBillingId[*billingId]);
  return 1;
}

ELEMENT_1(ParseAOCECurrencyInfo, Aoc, aoc)
{
  char msg[21] = "";

  CHECK_TAG(ASN1_TAG_SEQUENCE);
  MY_DEBUG("ParseARGAOCECurrencyInfo");

  SEQ_NOT_TAGGED_1(ParseAOCECurrencyInfoChoice, aoc);
  SEQOPT_NOT_TAGGED_1(ParseChargingAssociation, msg);

  if (strcmp(msg, "") != 0) {
    strcat(aoc->msg, " chargingAssociation: ");
    strcat(aoc->msg, msg);
  }

  return 1;
}

ELEMENT_1(ParseAOCECurrencyInfoChoice, Aoc, aoc)
{
 MY_DEBUG("ParseAOCECurrencyInfoChoice");

  if (ParseAOCESpecificCurrency(el, tag, aoc)) {
    ;
  } else if (ParseNull(el, 1)) {
    aoc->amount = 0; // freeOfCharge
    strcpy(aoc->msg, "freeOfCharge");
  } else {
    return 0;
  }

  return 1;
}

ELEMENT_1(ParseAOCESpecificCurrency, Aoc, aoc)
{
  int billingId = -1;

  CHECK_TAG(ASN1_TAG_SEQUENCE);
  MY_DEBUG("ParseAOCESpecificCurrency");

  SEQ_TAGGED_1(ParseRecordedCurrency, 1, aoc);
  SEQOPT_TAGGED_1(ParseAOCEBillingId, 2, &billingId);

  if (billingId != -1) {
    strcpy(aoc->msg, "billingId = ");
    strcat(aoc->msg, AOCEBillingId[billingId]);
  } else {
    strcpy(aoc->msg, "");
  }
  return 1;
}

ELEMENT_1(ParseAOCEChargingUnitInfo, Aoc, aoc)
{
  char msg[21] = "";

  CHECK_TAG(ASN1_TAG_SEQUENCE);
  MY_DEBUG("ParseAOCEChargingUnitInfo");

  SEQ_NOT_TAGGED_1(ParseAOCEChargingUnitInfoChoice, aoc);
  SEQOPT_NOT_TAGGED_1(ParseChargingAssociation, msg);

  if (strcmp(msg, "") != 0) {
    strcat(aoc->msg, " ChargingAssociation = ");
    strcat(aoc->msg, msg);
  }

  return 1;
}

ELEMENT_1(ParseAOCEChargingUnitInfoChoice, Aoc, aoc)
{
  MY_DEBUG("ParseAOCEChargingUnitInfoChoice");

  if (ParseAOCESpecificChargingUnits(el, tag, aoc)) {
    ;
  } else if (ParseNull(el, 1)) {
    aoc->amount = 0; // freeOfCharge
    strcpy(aoc->msg, "freeOfCharge");
  } else {
    return 0;
  }
  return 1;
}

ELEMENT_1(ParseAOCESpecificChargingUnits, Aoc, aoc)
{
  int billingId = -1;

  CHECK_TAG(ASN1_TAG_SEQUENCE);
  MY_DEBUG("ParseAOCESpecificChargingUnits");

  SEQ_TAGGED_1(ParseRecordedUnitsList, 1, aoc);
  SEQOPT_TAGGED_1(ParseAOCEBillingId, 2, &billingId);

  if (billingId != -1) {
    strcpy(aoc->msg, "billingId = ");
    strcat(aoc->msg, AOCEBillingId[billingId]);
  } else {
    strcpy(aoc->msg, "");
  }

  return 1;
}


ELEMENT_1(ParseAOCEBillingId, int, billingId)
{
  MY_DEBUG("ParseAOCEBillingId");

  if (!ParseEnum(el, tag, billingId)) return 0;

  if ((*billingId < 0) || (*billingId > NAOCEBillingId))
    return 0;

  print_msg(PRT_DEBUG_DECODE, " DEBUG> billingId = %s\n",
	    AOCEBillingId[*billingId]);
  return 1;
}

ELEMENT_1(ParseCurrency, char, msg)
{
  MY_DEBUG("ParseCurrency");

  if (!ParseIA5String(el, tag, msg)) return 0;

  print_msg(PRT_DEBUG_DECODE, " DEBUG> Currency = %s\n",
	    msg);

  return 1;
}

ELEMENT_1(ParseAmount, Aoc, aoc)
{
  int multiplier;

  CHECK_TAG(ASN1_TAG_SEQUENCE);

  MY_DEBUG("ParseAmount");

  SEQ_TAGGED_1(ParseCurrencyAmount, 1, &aoc->amount);
  SEQ_TAGGED_1(ParseMultiplier, 2, &multiplier);

  aoc->multiplier = XMultiplier[multiplier];

  print_msg(PRT_DEBUG_DECODE, " DEBUG> Amount = %d * %f\n",
	    aoc->amount, aoc->multiplier);

  return 1;
}

ELEMENT_1(ParseCurrencyAmount, int, amount)
{
  MY_DEBUG("ParseCurrencyAmount");

  if (!ParseInteger(el, tag, amount)) return 0;

  print_msg(PRT_DEBUG_DECODE, " DEBUG> CurrencyAmount = %d\n",
	    *amount);

  return 1;
}

ELEMENT_1(ParseMultiplier, int, multiplier)
{
  MY_DEBUG("ParseMultiplier");

  if (!ParseEnum(el, tag, multiplier)) return 0;

  if ((*multiplier < 0) || (*multiplier > NMultiplier))
    return 0;

  print_msg(PRT_DEBUG_DECODE, " DEBUG> Multiplier = %f\n",
	    XMultiplier[*multiplier]);

  return 1;
}

ELEMENT_1(ParseTypeOfUnit, int, typeOfUnit)
{
  MY_DEBUG("ParseTypeOfUnit");

  if (!ParseInteger(el, tag, typeOfUnit)) return 0;

  print_msg(PRT_DEBUG_DECODE, " DEBUG> typeOfUnit = %d\n",
	    *typeOfUnit);

  return 1;
}

ELEMENT_1(ParseNumberOfUnits, int, numberOfUnits)
{
  MY_DEBUG("ParseNumberOfUnits");

  if (!ParseInteger(el, tag, numberOfUnits)) return 0;

  print_msg(PRT_DEBUG_DECODE, " DEBUG> numberOfUnits = %d\n",
	    *numberOfUnits);

  return 1;
}

ELEMENT_1(ParseChargingAssociation, char, s)
{
  int chargeIdentifier;

  MY_DEBUG("ParseChargingAssociation");

  if ((el.tag &~ 0xa0) == 0) {
    if (!ParsePartyNumber(el.content.elements[0], ASN1_NOT_TAGGED, s))
      return 0;
  } else {
    if (!ParseChargeIdentifier(el, ASN1_NOT_TAGGED, &chargeIdentifier)) {
      return 0;
    } else {
      sprintf(s, "%d", chargeIdentifier);
    }
  }
  return 1;
}

ELEMENT_1(ParseChargeIdentifier, int, chargeIdentifier)
{
  MY_DEBUG("ParseChargeIdentifier");

  if (!ParseInteger(el, tag, chargeIdentifier)) return 0;

  print_msg(PRT_DEBUG_DECODE, " DEBUG> chargeIdentifier = %d\n",
	    *chargeIdentifier);

  return 1;
}
