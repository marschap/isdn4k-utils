/* tty line ISDN status monitor
 *
 * (c) 1995-97 Volker Götz
 *
 * $Id: imontty.c,v 1.1 1997/03/03 04:10:12 fritz Exp $
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <linux/isdn.h>
#include "imontty.h"

void scan_str(char * buffer, char * field[], int max) {
    char * buf = buffer;
    int i;
    char * token = strtok(buf, "\t");

    for(i=0; i < max; i++) {
        token = strtok(NULL, " ");
        if ((field[i] = malloc(strlen(token) * sizeof(char))) != NULL)
            strcpy(field[i], token);
    }
}


void scan_int(char * buffer, int (*field)[], int max) {
    char * buf = buffer;
    int i;
    char * token = strtok(buf, "\t");

    for(i=0; i < max; i++) {
        token = strtok(NULL, " ");
        (*field)[i] = atoi(token);
    }
}

void main(void) {

    FILE * isdninfo;
    char buf[IM_BUFSIZE];

    char * idmap[ISDN_MAX_CHANNELS];
    int chmap[ISDN_MAX_CHANNELS];
    int drmap[ISDN_MAX_CHANNELS];
    int usage[ISDN_MAX_CHANNELS];
    int flags[ISDN_MAX_DRIVERS];
    char * phone[ISDN_MAX_CHANNELS];

    int i, lines;

    if (!(isdninfo = fopen(PATH_ISDNINFO, "r"))) {
        fprintf(stderr, "ERROR: Can't open '%s'\n", PATH_ISDNINFO);
        exit(1);
    }

    fgets(buf, IM_BUFSIZE, isdninfo);
    scan_str(buf, idmap, ISDN_MAX_CHANNELS);

    fgets(buf, IM_BUFSIZE, isdninfo);
    scan_int(buf, &chmap, ISDN_MAX_CHANNELS);

    fgets(buf, IM_BUFSIZE, isdninfo);
    scan_int(buf, &drmap, ISDN_MAX_CHANNELS);

    fgets(buf, IM_BUFSIZE, isdninfo);
    scan_int(buf, &usage, ISDN_MAX_CHANNELS);

    fgets(buf, IM_BUFSIZE, isdninfo);
    scan_int(buf, &flags, ISDN_MAX_DRIVERS);

    fgets(buf, IM_BUFSIZE, isdninfo);
    scan_str(buf, phone, ISDN_MAX_CHANNELS);

    fclose(isdninfo);

    printf("\n%s\n\n", "ISDN channel status:");
    printf("%-20.20s\t%-6.6s%-6.6s%s\n", "Channel", "Usage", "Type", "Number");
    printf("----------------------------------------------------------------------\n");

    lines = 0;
    for(i=0; i < ISDN_MAX_CHANNELS; i++)
        if (chmap[i] >= 0 && drmap[i] >= 0) {

	    /* printf("%-20.20s/%i\t", idmap[i], chmap[i]); */
	    printf("%-20.20s\t", idmap[i]);

            if (usage[i] & ISDN_USAGE_OUTGOING)
		printf("%-6.6s", "Out");
            else if (usage[i] == ISDN_USAGE_EXCLUSIVE)
		printf("%-6.6s", "Excl.");
            else if (usage[i])
		printf("%-6.6s", "In");
            else
		printf("%-6.6s", "Off");

            switch(usage[i] & ISDN_USAGE_MASK) {
		case ISDN_USAGE_RAW:
		    printf("%-6.6s", "raw");
		    break;
		case ISDN_USAGE_MODEM:
		    printf("%-6.6s", "Modem");
		    break;
		case ISDN_USAGE_NET:
		    printf("%-6.6s", "Net");
		    break;
		case ISDN_USAGE_VOICE:
		    printf("%-6.6s", "Voice");
		    break;
		case ISDN_USAGE_FAX:
		    printf("%-6.6s", "Fax");
		    break;
		default:
		    printf("%-6.6s", "");
		    break;
	    }

	    if(usage[i] & ISDN_USAGE_MASK)
		printf("%s\n", phone[i]);
            else
		printf("\n");

	    if( !(++lines % 2) )
		printf("\n");
	}
}
