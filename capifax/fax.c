#include <string.h>

#include "fax.h"

char *stationID = "00000000";
char *headLine  = "Unconfigured Cobalt FAXServer";

void SetupB3Config(B3_PROTO_FAXG3  *B3conf, int FAX_Format) {
    int len1;
    int len2;

    B3conf->resolution = 0;
    B3conf->format = (unsigned short)FAX_Format;
    len1 = strlen(stationID);
    B3conf->Infos[0] = (unsigned char)len1;
    strcpy((char *)&B3conf->Infos[1], stationID);
    len2 = strlen(headLine);
    B3conf->Infos[len1 + 1] = (unsigned char)len2;
    strcpy((char *)&B3conf->Infos[len1 + 2], headLine);
    B3conf->len = (unsigned char)(2 * sizeof(unsigned short) + len1 + len2 + 2);
}
