#include "asn1.h"

char* BasicService[NBasicService];
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
