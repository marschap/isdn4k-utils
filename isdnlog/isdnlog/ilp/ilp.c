/* ilp.c -  isdnlog to procfs interface in /proc/isdnlog 
 *
 * Copyright 2000 by Leopold Toetsch <lt@toetsch.at>
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
 * Changes:
 *
 * 0.10 15.12.2000 lt Initial Version
 */

#include "isdnlog.h"

#define PROC_ISDNLOG "/proc/isdnlog"

#define NE(s) (*s ? s : "-")

void procinfo(int chan, CALL *call, int state) 
{
    static FILE *fp = NULL;
    static int errcount = 0;
    int c;
    char s[80];
    char *msn;
    char *alias;
    char *st;
    char *p;

    print_msg(PRT_INFO, "procinfo: chan %d, state %d\n", chan, state);
        
    /* check for valid B channels and for too much errors */
    if (chan < 1 || chan > 2 || errcount > 2)
	return;
	
    /* special state to clean up */	
    if (state == -1) {
	if (fp)
	    fclose(fp);
	fp = NULL;
	return;
    }	
    
    /* open /proc/isdnlog for writing */
    if (fp == NULL) {
	fp = fopen(PROC_ISDNLOG, "w");
	if (fp == NULL) {
	    print_msg(PRT_ERR, "Failed to open '%s' for writing: %s\n", 
		PROC_ISDNLOG, strerror(errno));
	    errcount++;
	    return;
	}    
    }
    /* make msn i.e. rightmost 3 digits of own num */
    if (strlen(call->num[_ME(call)]) <= 3)
	msn = call->num[_ME(call)];
    else
	msn = call->num[_ME(call)] + strlen(call->num[_ME(call)]) - 3;
	
    /* alias is alias | area | country */
    c = call->confentry[_OTHER(call)];
    alias = c >= 0 ? known[c]->who :
	    call->area[_OTHER(call)] ? call->area[_OTHER(call)] :
	    call->vorwahl[_OTHER(call)]; /* FIXME no country in call? */
	    
    /* format message for channel */
    p = s;
    p += sprintf(s, "%c%2d", chan + '0', chan);	/* channel "1" or "2" */
    switch (state) {
	case CONNECT:
	    st = call->dialin ? "CON__IN" : "CON_OUT";
	    break;
	case RELEASE:
	    st = "HANG_UP";
	    break;
	default:
	    st = "UNKNOWN";    
	    break;
    }    
    p += sprintf(p, " %7s %-3s %c %-25s %-18s %-8s",
	st,
	NE(msn),
	call->dialin ? '<' : '>',
	NE(call->num[_OTHER(call)]),
	NE(alias),
	double2clock((double) (cur_time - call->connect)));
    if (!call->dialin) 
	p += sprintf(p, " %7.3f", call->pay);
    strcpy(p, "\n");	
    fputs(s, fp);
    fflush(fp);
}

