/* $Id: eiconctrl.c,v 1.10 2000/01/12 07:05:09 armin Exp $
 *
 * Eicon-ISDN driver for Linux. (Control-Utility)
 *
 * Copyright 1998    by Fritz Elfert (fritz@isdn4linux.de)
 * Copyright 1998,99 by Armin Schindler (mac@melware.de)
 * Copyright 1999    Cytronics & Melware (info@melware.de)
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
 * $Log: eiconctrl.c,v $
 * Revision 1.10  2000/01/12 07:05:09  armin
 * Fixed error on loading old S card.
 *
 * Revision 1.9  1999/11/21 12:41:25  armin
 * Added further check for future driver changes.
 *
 * Revision 1.8  1999/10/12 18:01:52  armin
 * Backward compatible to older driver versions.
 *
 * Revision 1.7  1999/09/06 08:03:24  fritz
 * Changed my mail-address.
 *
 * Revision 1.6  1999/08/18 20:20:45  armin
 * Added XLOG functions for all cards.
 *
 * Revision 1.5  1999/03/29 11:05:23  armin
 * Installing and Loading the old type Eicon ISA-cards.
 * New firmware in one tgz-file.
 * Commandline Management Interface.
 *
 * Revision 1.4  1999/03/02 11:35:57  armin
 * Change of email address.
 *
 * Revision 1.3  1999/02/25 22:35:14  armin
 * Did not compile with new version.
 *
 * Revision 1.2  1999/01/20 21:16:45  armin
 * Added some debugging features.
 *
 * Revision 1.1  1999/01/01 17:27:57  armin
 * First checkin of new created control utility for Eicon.Diehl driver.
 * diehlctrl is obsolete.
 *
 *
 */

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <malloc.h>
#include <signal.h>
#ifdef HAVE_NCURSES_H
#	include <ncurses.h>
#endif
#ifdef HAVE_CURSES_H
#	include <curses.h>
#endif
#ifdef HAVE_CURSES_NCURSES_H
#	include <curses/ncurses.h>
#endif
#ifdef HAVE_NCURSES_CURSES_H
#	include <ncurses/curses.h>
#endif


#include <linux/types.h>
#include <linux/isdn.h>
#include <eicon.h>


#define COMBIFILE "dspdload.bin"

#define MIN(a,b) ((a)>(b) ? (b) : (a))

#define EICON_CTRL_VERSION 2 

char *cmd;
int verbose = 0;
eicon_manifbuf *mb;

WINDOW *statwin;
WINDOW *headwin;
static int h_line;
static int stat_y;

int num_directory_entries;
int usage_mask_size;
int download_count;
int directory_size;
unsigned int no_of_downloads = 0;
int total_bytes_in_download = 0;
__u32 download_pos;
#ifdef HAVE_NPCI
t_dsp_download_desc p_download_table[35];
#endif

int usage_bit;
int usage_byte;
char *no_of_tables;
__u32 table_count = 0;
int fd;
isdn_ioctl_struct ioctl_s;

char Man_Path[80];
int  man_ent_count;

struct man_s {
	u_char type;
	u_char attribute;
	u_char status;
	u_char var_length;
	u_char path_length;
	char   Name[180];
	char   Var[180];
} man_ent[60];

char *spid_state[] =
{
  "Idle",
  "Up",
  "Disconnecting",
  "Connecting",
  "SPID Initializing",
  "SPID Initialized",
  "UNKNOWN STATE"
};


#ifdef HAVE_NPCI
#ifdef HAVE_XLOG
/*********** XLOG stuff **********/

#define byte __u8
#define word __u16
#define dword __u32

/* #define SWAP(a) ((((a) & 0xff) << 8) | (((a) &0xff00)>>8)) */
#define SWAP(a) (a)

static char * cau_q931[] = {
  "0",
  "Unallocated (unassigned) number",
  "No route to specified transit network",
  "No route to destination",
  "4",
  "5",
  "Channel unacceptable",
  "Call awarded and being deliverd in an established channel",
  "8",
  "9",
  "10",
  "11",
  "12",
  "13",
  "14",
  "15",
  "Normal call clearing",
  "User busy",
  "No user responding",
  "No answer from user (user alerted)",
  "20",
  "Call rejected",
  "Number changed",
  "23",
  "24",
  "25",
  "Non-selected user clearing",
  "Destination out of order",
  "Invalid number format",
  "Facility rejected",
  "Response to STATUS ENQUIRY",
  "Normal, unspecified",
  "32",
  "33",
  "No circuit/channel available",
  "35",
  "36",
  "37",
  "Network out of order",
  "39",
  "40",
  "Temporary failure",
  "Switching equipment congestion",
  "Access information discarded",
  "Requested circuit/channel not available",
  "45",
  "46",
  "Resources unavailable, unspecified",
  "48",
  "Quality of service unavailable",
  "Requested facility not subscribed",
  "51",
  "52",
  "53",
  "54",
  "55",
  "56",
  "Bearer capability not authorized",
  "Bearer capabliity not presently available",
  "59",
  "60",
  "61",
  "62",
  "Service or option not available, unspecified",
  "64",
  "Bearer capability not implemented",
  "Channel type not implemented",
  "67",
  "68",
  "Requested facility not implemented",
  "Only restricted digital information bearer capability is available",
  "71",
  "72",
  "73",
  "74",
  "75",
  "76",
  "77",
  "78",
  "Service or option not implemented, unspecified",
  "80",
  "Invalid call reference value",
  "Identified channel does not exist",
  "A suspended call exists, but this call identity does not",
  "Call identity in use",
  "No call suspended",
  "Call having the requested call identity has been cleared",
  "87",
  "Incompatible destination",
  "89",
  "90",
  "Invalid transit network selection",
  "92",
  "93",
  "94",
  "Invalid message, unspecified",
  "Mandatory information element is missing",
  "Message type non-existent or not implemented",
  "Message not compatible with call state or not implemented",
  "Information element non-existent or not implemented",
  "Invalid information element contents",
  "Message not compatible with call state",
  "Recovery on timer expiry",
  "103",
  "104",
  "105",
  "106",
  "107",
  "108",
  "109",
  "110",
  "Protocol error, unspecified",
  "112",
  "113",
  "114",
  "115",
  "116",
  "117",
  "118",
  "119",
  "120",
  "121",
  "122",
  "123",
  "124",
  "125",
  "126",
  "Internetworking, unspecified"
};

static char * cau_1tr6[] = {
  "0",
  "Invalid call reference",
  "2",
  "Bearer service not implemented",
  "4",
  "5",
  "Channel unacceptable",
  "Call identity does not exist",
  "Call identity in use",
  "9",
  "No channel available",
  "11",
  "12",
  "13",
  "14",
  "15",
  "Requested facility not implemented",
  "Requested facility not subscribed",
  "18",
  "19",
  "20",
  "21",
  "22",
  "23",
  "24",
  "25",
  "26",
  "27",
  "28",
  "29",
  "30",
  "31",
  "Outgoing call barred",
  "User access busy",
  "Wrong closed user group",
  "35",
  "36",
  "Semipermanent connection not allowed",
  "38",
  "39",
  "40",
  "41",
  "42",
  "43",
  "44",
  "45",
  "46",
  "47",
  "Reverse charging not allowed at originating end",
  "49",
  "50",
  "51",
  "52",
  "Destination not obtainable",
  "54",
  "55",
  "Number changed",
  "Out of order",
  "No User responding",
  "User busy",
  "60",
  "Incoming calls barred",
  "Call rejected",
  "63",
  "64",
  "65",
  "66",
  "67",
  "68",
  "69",
  "70",
  "71",
  "72",
  "73",
  "74",
  "75",
  "76",
  "77",
  "78",
  "79",
  "80",
  "81",
  "82",
  "83",
  "84",
  "85",
  "86",
  "87",
  "Incompatible destination",
  "Network congestion",
  "Remote user initiated",
  "91",
  "92",
  "93",
  "94",
  "95",
  "96",
  "97",
  "98",
  "99",
  "100",
  "101",
  "102",
  "103",
  "104",
  "105",
  "106",
  "107",
  "108",
  "109",
  "110",
  "111",
  "Local procedure error",
  "Remote procedure error",
  "Remote user suspended",
  "Remote user resumed",
  "116",
  "117",
  "118",
  "119",
  "120",
  "121",
  "122",
  "123",
  "124",
  "125",
  "126",
  "User info discarded locally"
};

struct l1s {
  short length;
  unsigned char i[22];
};

struct l2s {
  short code;
  short length;
  unsigned char i[20];
};

union par {
  char text[42];
  struct l1s l1;
  struct l2s l2;
};

typedef struct xlog_s XLOG;
struct xlog_s {
  short code;
  union par info;
};

typedef struct log_s LOG;
struct log_s {
  word length;
  word code;
  word timeh;
  word timel;
  byte buffer[80];
};

static char *ll_name[13] = {
  "LL_UDATA",
  "LL_ESTABLISH",
  "LL_RELEASE",
  "LL_DATA",
  "LL_LOCAL",
  "LL_REMOTE",
  "",
  "LL_TEST",
  "LL_MDATA",
  "LL_BUDATA",
  "",
  "LL_XID",
  "LL_XID_R"
};

static char *ns_name[13] = {
  "N-MDATA",
  "N-CONNECT",
  "N-CONNECT ACK",
  "N-DISC",
  "N-DISC ACK",
  "N-RESET",
  "N-RESET ACK",
  "N-DATA",
  "N-EDATA",
  "N-UDATA",
  "N-BDATA",
  "N-DATA ACK",
  "N-EDATA ACK"
};

static char * l1_name[] = {
  "SYNC_LOST",
  "SYNC_GAINED",
  "L1_DOWN",
  "L1_UP",
  "ACTIVATION_REQ",
  "PS1_DOWN",
  "PS1_UP",
  "L1_FC1",
  "L1_FC2",
  "L1_FC3",
  "L1_FC4"
};

static struct {
  byte code;
  char * name;
} capi20_command[] = {
  { 0x01, "ALERT" },
  { 0x02, "CONNECT" },
  { 0x03, "CONNECT_ACTIVE" },
  { 0x04, "DISCONNECT" },
  { 0x05, "LISTEN" },
  { 0x08, "INFO" },

  { 0x41, "SELECT_B" },

  { 0x80, "FACILITY" },
  { 0x82, "CONNECT_B3" },
  { 0x83, "CONNECT_B3_ACTIVE" },
  { 0x84, "DISCONNECT_B3" },
  { 0x86, "DATA_B3" },
  { 0x87, "RESET_B3" },
  { 0x88, "CONNECT_B3_T90_ACTIVE" },

  { 0xd4, "PORTABILITY" }, /* suspend/resume */

  { 0xff, "MANUFACTURER" }
};

static struct {
  byte code;
  char * name;
} capi11_command[] = {
  { 0x01, "RESET_B3" },
  { 0x02, "CONNECT" },
  { 0x03, "CONNECT_ACTIVE" },
  { 0x04, "DISCONNECT" },
  { 0x05, "LISTEN" },
  { 0x06, "GET_PARMS" },
  { 0x07, "INFO" },
  { 0x08, "DATA" },
  { 0x09, "CONNECT_INFO" },

  { 0x40, "SELECT_B2" },
  { 0x80, "SELECT_B3" },

  { 0x81, "LISTEN_B3" },
  { 0x82, "CONNECT_B3" },
  { 0x83, "CONNECT_B3_ACTIVE" },
  { 0x84, "DISCONNECT_B3" },
  { 0x85, "GET_B3_PARMS" },
  { 0x86, "DATA_B3" },
  { 0x87, "HANDSET" },
};

static struct {
  byte code;
  char * name;
} capi20_subcommand[] = {
  { 0x80, "REQ" },
  { 0x81, "CON" },
  { 0x82, "IND" },
  { 0x83, "RES" }
};

static struct {
  byte code;
  char * name;
} capi11_subcommand[] = {
  { 0x00, "REQ" },
  { 0x01, "CON" },
  { 0x02, "IND" },
  { 0x03, "RES" }
};

#define PD_MAX 4
static struct {
  byte code;
  char * name;
} pd[PD_MAX] = {
  { 0x08, "Q.931  " },
  { 0x0c, "CORNET " },
  { 0x40, "DKZE   " },
  { 0x41, "1TR6   " }
};

#define MT_MAX 26
static struct {
  byte code;
  char * name;
} mt[MT_MAX] = {
  { 0x01, "ALERT" },
  { 0x02, "CALL_PROC" },
  { 0x03, "PROGRESS" },
  { 0x05, "SETUP" },
  { 0x07, "CONN" },
  { 0x0d, "SETUP_ACK" },
  { 0x0f, "CONN_ACK" },
  { 0x20, "USER_INFO" },
  { 0x21, "SUSP_REJ" },
  { 0x22, "RESUME_REJ" },
  { 0x25, "SUSP" },
  { 0x26, "RESUME" },
  { 0x2d, "SUSP_ACK" },
  { 0x2e, "RESUME_ACK" },
  { 0x45, "DISC" },
  { 0x46, "RESTART" },
  { 0x4d, "REL" },
  { 0x4e, "RESTART_ACK" },
  { 0x5a, "REL_COM" },
  { 0x60, "SEGMENT" },
  { 0x62, "FACILITY" },
  { 0x6d, "GINFO" },
  { 0x6e, "NOTIFY" },
  { 0x75, "STATUS_ENQ" },
  { 0x7d, "STATUS" },
  { 0x7b, "INFO" }
};

#define IE_MAX 36
static struct {
  word code;
  char * name;
} ie[IE_MAX] = {
  { 0x00a0, "MORE" },
  { 0x00a1, "Sending complete" },
  { 0x00b0, "Congestion level" },
  { 0x00d0, "Repeat indicator" },

  { 0x0000, "SMSG" },
  { 0x0004, "Bearer Capability" },
  { 0x0008, "Cause" },
  { 0x000c, "Connected Address" },
  { 0x0010, "Call Identity" },
  { 0x0014, "Call State" },
  { 0x0018, "Channel Id" },
  { 0x001c, "Facility" },
  { 0x001e, "Progress Indicator" },
  { 0x0020, "NSF" },
  { 0x0027, "Notify Indicator" },
  { 0x0028, "Display" },
  { 0x0029, "Date" },
  { 0x002c, "Key " },
  { 0x0034, "Signal" },
  { 0x003a, "SPID" },
  { 0x003b, "Endpoint Id"  },
  { 0x006c, "Calling Party Number" },
  { 0x006d, "Calling Party Subaddress" },
  { 0x0070, "Called Party Number" },
  { 0x0071, "Called Party Subaddress" },
  { 0x0074, "RDN" },
  { 0x0078, "TNET" },
  { 0x0079, "RI" },
  { 0x007c, "LLC" },
  { 0x007d, "HLC" },
  { 0x007e, "User User Info" },
  { 0x007f, "ESC" },

  { 0x0601, "Service Indicator" },
  { 0x0602, "Charging Info" },
  { 0x0603, "Date" },
  { 0x0607, "Called Party State" }
};

static char * sig_state[] = {
  "Null state",
  "Call initiated",
  "Overlap sending",
  "Outgoing call proceeding",
  "Call delivered",
  "",
  "Call present",
  "Call received",
  "Connect request",
  "Incoming call proceeding",
  "Active",
  "Disconnect request",
  "Disconnect indication",
  "",
  "",
  "Suspend request",
  "",
  "Resume request",
  "",
  "Release request",
  "",
  "",
  "",
  "",
  "",
  "Overlap receiving"
};

static char * sig_timeout[] = {
  "",
  "T303",
  "",
  "",
  "",
  "",
  "",
  "",
  "T313",
  "",
  "",
  "T305",
  "",
  "",
  "",
  "T319",
  "",
  "T318",
  "",
  "T308",
  "",
  "",
  "",
  "",
  "",
  "T302"
};

struct msg_s {
  word code;
  word length;
  byte info[1000];
} message = {0,0,{0}};

void xlog(FILE * stream,void * buf);
void xlog_sig(FILE * stream, struct msg_s * message);
void call_failed_event(FILE * stream, struct msg_s * message, word code);
void display_q931_message(FILE * stream, struct msg_s * message);
void display_ie(FILE * stream, byte pd, word w, byte * ie);
void spid_event(FILE * stream, struct msg_s * message, word code);


void xlog( FILE *stream, void * buf)
{
  word i;
  word n;
  word code;
  XLOG *buffer;

  buffer = (XLOG *)buf;

  switch((byte)SWAP(buffer->code)) {
  case 1:
    n = SWAP(buffer->info.l1.length);
    if(SWAP(buffer->code) &0xff00) fprintf(stream,"B%d-X(%03d) ",SWAP(buffer->code)>>8,n);
    else fprintf(stream,"B-X(%03d) ",n);
    for(i=0;i<n && i<30;i++)
      fprintf(stream,"%02X ",buffer->info.l1.i[i]);
    if(n>i) fprintf(stream,"cont");
    break;
  case 2:
    n = SWAP(buffer->info.l1.length);
    if(SWAP(buffer->code) &0xff00) fprintf(stream,"B%d-R(%03d) ",SWAP(buffer->code)>>8,n);
    else fprintf(stream,"B-R(%03d) ",n);
    for(i=0;i<n && i<30;i++)
      fprintf(stream,"%02X ",buffer->info.l1.i[i]);
    if(n>i) fprintf(stream,"cont");
    break;
  case 3:
    n = SWAP(buffer->info.l1.length);
    fprintf(stream,"D-X(%03d) ",n);
    for(i=0;i<n && i<38;i++)
      fprintf(stream,"%02X ",buffer->info.l1.i[i]);
    if(n>i) fprintf(stream,"cont");
    break;
  case 4:
    n = SWAP(buffer->info.l1.length);
    fprintf(stream,"D-R(%03d) ",n);
    for(i=0;i<n && i<38;i++)
      fprintf(stream,"%02X ",buffer->info.l1.i[i]);
    if(n>i) fprintf(stream,"cont");
    break;
  case 5:
    n = SWAP(buffer->info.l2.length);
    fprintf(stream,"SIG-EVENT(%03d)%04X - ",n,buffer->info.l2.code);
    for(i=0;i<n && i<28;i++)
      fprintf(stream,"%02X ",buffer->info.l2.i[i]);
    if(n>i) fprintf(stream,"cont");
    break;
  case 6:
    code = SWAP(buffer->info.l2.code);
    if(code && code <= 13)
      fprintf(stream,"%s IND",ll_name[code-1]);
    else
      fprintf(stream,"UNKNOWN LL IND");
    break;
  case 7:
    code = SWAP(buffer->info.l2.code);
    if(code && code <= 13)
      fprintf(stream,"%s REQ",ll_name[code-1]);
    else
      fprintf(stream,"UNKNOWN LL REQ");
    break;
  case 8:
    n = SWAP(buffer->info.l2.length);
    fprintf(stream,"DEBUG%04X - ",SWAP(buffer->info.l2.code));
    for(i=0;i<n && i<38;i++)
      fprintf(stream,"%02X ",buffer->info.l2.i[i]);
    if(n>i) fprintf(stream,"cont");
    break;
  case 9:
    fprintf(stream,"MDL-ERROR(%s)",buffer->info.text);
    break;
  case 10:
    fprintf(stream,"UTASK->PC(%02X)",SWAP(buffer->info.l2.code));
    break;
  case 11:
    fprintf(stream,"PC->UTASK(%02X)",SWAP(buffer->info.l2.code));
    break;
  case 12:
    n = SWAP(buffer->info.l1.length);
    fprintf(stream,"X-X(%03d) ",n);
    for(i=0;i<n && i<30;i++)
      fprintf(stream,"%02X ",buffer->info.l1.i[i]);
    if(n>i) fprintf(stream,"cont");
    break;
  case 13:
    n = SWAP(buffer->info.l1.length);
    fprintf(stream,"X-R(%03d) ",n);
    for(i=0;i<n && i<30;i++)
      fprintf(stream,"%02X ",buffer->info.l1.i[i]);
    if(n>i) fprintf(stream,"cont");
    break;
  case 14:
    code = SWAP(buffer->info.l2.code)-1;
    if((code &0x0f)<=12)
      fprintf(stream,"%s IND",ns_name[code &0x0f]);
    else
      fprintf(stream,"UNKNOWN NS IND");
    break;
  case 15:
    code = SWAP(buffer->info.l2.code)-1;
    if((code & 0x0f)<=12)
      fprintf(stream,"%s REQ",ns_name[code &0x0f]);
    else
      fprintf(stream,"UNKNOWN NS REQ");
    break;
  case 16:
    fprintf(stream,"TASK %02i: %s",
                   buffer->info.l2.code,buffer->info.l2.i);
    break;
  case 18:
    code = SWAP(buffer->info.l2.code);
    fprintf(stream,"IO-REQ %02x",code);
    break;
  case 19:
    code = SWAP(buffer->info.l2.code);
    fprintf(stream,"IO-CON %02x",code);
    break;
  case 20:
    code = SWAP(buffer->info.l2.code);
    fprintf(stream,l1_name[code]);
    break;
  case 21:
    for(i=0;i<(word)SWAP(buffer->info.l2.length);i++)
      message.info[message.length++] = buffer->info.l2.i[i];

    if(SWAP(buffer->info.l2.code)) {

      switch(SWAP(buffer->info.l2.code) &0xff) {
      case 1:
        xlog_sig(stream,&message);
        break;
      default:
        fprintf(stream,"MSG%04X - ",SWAP(buffer->info.l2.code));
        for(i=0;i<message.length;i++)
          fprintf(stream,"%02X ",message.info[i]);
        break;
      }
      message.length = 0;
    }
    else {
      fprintf(stream,"MORE");
    }
    break;
  case 22:
    for(i=0;i<(word)SWAP(buffer->info.l2.length);i++)
      message.info[message.length++] = buffer->info.l2.i[i];

    if(SWAP(buffer->info.l2.code)) {

      switch(SWAP(buffer->info.l2.code) &0xff) {
      case 1:
        call_failed_event(stream, &message, SWAP(buffer->info.l2.code));
        break;
      case 2:
        spid_event(stream, &message, SWAP(buffer->info.l2.code));
        break;
      default:
        fprintf(stream,"EVENT%04X - ",SWAP(buffer->info.l2.code));
        for(i=0;i<message.length;i++)
          fprintf(stream,"%02X ",message.info[i]);
        break;
      }
      message.length = 0;
    }
    else {
      fprintf(stream,"MORE");
    }
    break;
  case 23:
    fprintf(stream,"EYE");
    for(i=0; i<(word)SWAP(buffer->info.l1.length); i+=2) {
      fprintf(stream," %02x%02x",buffer->info.l1.i[i+1],buffer->info.l1.i[i]);
    }
    break;
  case 128:
  case 129:
    n = SWAP(buffer->info.l1.length);
    if(SWAP(buffer->code)==128) fprintf(stream,"CAPI20_PUT(%03d) ",n);
    else fprintf(stream,"CAPI20_GET(%03d) ",n);
    fprintf(stream,"APPL %04x ",*(word *)&buffer->info.l1.i[0]);

    fprintf(stream,"%04x:%08x ",*(word *)&buffer->info.l1.i[4],
                                 *(dword *)&buffer->info.l1.i[6]);

    for(i=0;i<15 && buffer->info.l1.i[2]!=capi20_command[i].code;i++);
    if(i<15) fprintf(stream,"%s ",capi20_command[i].name);
    else fprintf(stream,"CMD(%x) ",buffer->info.l1.i[2]);

    for(i=0;i<4 && buffer->info.l1.i[3]!=capi20_subcommand[i].code;i++);
    if(i<4) fprintf(stream,"%s",capi20_subcommand[i].name);
    else fprintf(stream,"SUB(%x)",buffer->info.l1.i[3]);

    fprintf(stream,"\n                                             ");
    for(i=10;i<n-2;i++)
      fprintf(stream,"%02X ",buffer->info.l1.i[i]);
    break;
  case 130:
  case 131:
    n = SWAP(buffer->info.l1.length);
    if(buffer->code==130) fprintf(stream,"CAPI11_PUT(%03d) ",n);
    else fprintf(stream,"CAPI11_GET(%03d) ",n);
    fprintf(stream,"APPL %04x ",*(word *)&buffer->info.l1.i[0]);

    fprintf(stream,"%04x:",*(word *)&buffer->info.l1.i[4]);

    for(i=0;i<18 && buffer->info.l1.i[2]!=capi11_command[i].code;i++);
    if(i<18) fprintf(stream,"%s ",capi11_command[i].name);
    else fprintf(stream,"CMD(%x) ",buffer->info.l1.i[2]);

    for(i=0;i<4 && buffer->info.l1.i[3]!=capi11_subcommand[i].code;i++);
    if(i<4) fprintf(stream,"%s",capi11_subcommand[i].name);
    else fprintf(stream,"SUB(%x)",buffer->info.l1.i[3]);

    fprintf(stream,"\n                                             ");
    for(i=6;i<n-2;i++)
      fprintf(stream,"%02X ",buffer->info.l1.i[i]);
    break;
  }
  fprintf(stream,"\n");
}

void xlog_sig(FILE * stream, struct msg_s * message)
{
  word n;
  word i;
  word cr;
  byte msg;

  n = message->length;


  msg = TRUE;
  switch(SWAP(*(word *)&(message->info[0]))) {
  case 0x0000:
    fprintf(stream,"SIG-x(%03i)",n-4);
    break;
  case 0x0010:
  case 0x0013:
    fprintf(stream,"SIG-R(%03i)",n-4);
    cr ^=0x8000;
    break;
  case 0x0020:
    fprintf(stream,"SIG-X(%03i)",n-4);
    break;
  default:
    fprintf(stream,"SIG-EVENT %04X",SWAP(*(word *)&message->info[0]));
    msg = FALSE;
    break;
  }
  for(i=0; (i<n-4) && (n>4); i++)
    fprintf(stream," %02X",message->info[4+i]);
  fprintf(stream,"\n");

  if(msg) display_q931_message(stream, message);
}

void call_failed_event(FILE * stream, struct msg_s * message, word code)
{
  fprintf(stream, "EVENT: Call failed in State '%s'\n", sig_state[code>>8]);
  switch(SWAP(*(word *)&message->info[0])) {
  case 0x0000:
  case 0x0010:
  case 0x0020:
    display_q931_message(stream, message);
    break;
  case 0xff00:
    fprintf(stream,"                     ");
    fprintf(stream,"Signaling Timeout %s",sig_timeout[code>>8]);
    break;
  case 0xffff:
    fprintf(stream,"                     ");
    fprintf(stream,"Link disconnected");
    switch(message->info[4]) {
    case 8:
      fprintf(stream,", Layer-1 error (cable or NT)");
      break;
    case 9:
      fprintf(stream,", Layer-2 error");
      break;
    case 10:
      fprintf(stream,", TEI error");
      break;
    }
    break;
  }
}

void display_q931_message(FILE * stream, struct msg_s * message)
{
  word i;
  word cr;
  word p;
  word w;
  word wlen;
  byte codeset,lock;

  fprintf(stream,"                     ");

  cr = 0;

        /* read one byte call reference */
  if(message->info[5]==1) {
    cr = message->info[6] &0x7f;
  }

        /* read two byte call reference */
  if(message->info[5]==2) {
    cr = message->info[7];
    cr += (message->info[6] &0x7f)<<8;
  }
        /* read call reference flag */
  if(message->info[5]) {
    if(message->info[6] &0x80) cr |=0x8000;
  }

  for(i=0;i<PD_MAX && message->info[4]!=pd[i].code;i++);
  if(i<PD_MAX) fprintf(stream,"%sCR%04x ",pd[i].name,cr);
  else fprintf(stream,"PD(%02xCR%04x)  ",message->info[4],cr);

  p = 6+message->info[5];
  for(i=0;i<MT_MAX && message->info[p]!=mt[i].code;i++);
  if(i<MT_MAX) fprintf(stream,"%s,%d",mt[i].name,cr);
  else fprintf(stream,"MT%02x",message->info[p]);
  p++;

  codeset = 0;
  lock = 0;
  while(p<message->length) {
    fprintf(stream,"\n                            ");

      /* read information element id and length                   */
    w = message->info[p];
    if(w & 0x80) {
      w &=0xf0;
      wlen = 1;
    }
    else {
      wlen = message->info[p+1]+2;
    }

    if(lock & 0x80) lock &=0x7f;
    else codeset = lock;

    if(w==0x90) {
      codeset = message->info[p];
      if(codeset &8) fprintf(stream,"SHIFT %02x",codeset &7);
      else fprintf(stream,"SHIFT LOCK %02x",codeset &7);
      if(!(codeset & 0x08)) lock = (byte)(codeset & 7);
      codeset &=7;
      lock |=0x80;
    }
    else {
      w |= (codeset<<8);
      for(i=0;i<IE_MAX && w!=ie[i].code;i++);
      if(i<IE_MAX) fprintf(stream,"%s",ie[i].name);
      else fprintf(stream,"%01x/%02x ",codeset,(byte)w);

      if((p+wlen) > message->length) {
        fprintf(stream,"IE length error");
      }
      else {
        if(wlen>1) display_ie(stream,message->info[4],w,&message->info[1+p]);
      }
    }

      /* check if length valid (not exceeding end of packet)      */
    p+=wlen;
  }
}

void display_ie(FILE * stream, byte pd, word w, byte * ie)
{
  word i;

  switch(w) {

  case 0x08:
    for(i=0;i<ie[0];i++) fprintf(stream," %02x",ie[1+i]);
    switch(pd) {
    case 0x08:
      for(i=0; i<ie[0] && !(ie[1+i]&0x80); i++);
      fprintf(stream, " '%s'",cau_q931[ie[2+i]&0x7f]);
      break;
    case 0x41:
      if(ie[0]) fprintf(stream, " '%s'",cau_1tr6[ie[1]&0x7f]);
      else fprintf(stream, " '%s'",cau_1tr6[0]);
      break;
    }
    break;

  case 0x0c:
  case 0x6c:
  case 0x6d:
  case 0x70:
  case 0x71:
    for(i=0; i<ie[0] && !(ie[1+i]&0x80); i++)
      fprintf(stream, " %02x",ie[1+i]);
    fprintf(stream, " %02x",ie[1+i++]);
    fprintf(stream," '");
    for( ; i<ie[0]; i++) fprintf(stream, "%c",ie[1+i]);
    fprintf(stream,"'");
    break;

  case 0x2c:
  case 0x603:
    fprintf(stream," '");
    for(i=0 ; i<ie[0]; i++) fprintf(stream, "%c",ie[1+i]);
    fprintf(stream,"'");
    break;

  default:
    for(i=0;i<ie[0];i++) fprintf(stream," %02x",ie[1+i]);
    break;
  }
}

void spid_event(FILE * stream, struct msg_s * message, word code)
{
  word i;

  switch(code>>8) {
  case 1:
    fprintf(stream, "EVENT: SPID rejected");
    break;
  case 2:
    fprintf(stream, "EVENT: SPID accepted");
    break;
  case 3:
    fprintf(stream, "EVENT: Timeout, resend SPID");
    break;
  case 4:
    fprintf(stream, "EVENT: Layer-2 failed, resend SPID");
    break;
  case 5:
    fprintf(stream, "EVENT: Call attempt on inactive SPID");
    break;
  }
  fprintf(stream, " '");
  for(i=0;i<message->info[0];i++) fprintf(stream,"%c",message->info[1+i]);
  fprintf(stream, "'");
}

/*********** XLOG stuff end **********/
#endif /* XLOG */
#endif /* NPCI */

void usage() {
  fprintf(stderr,"usage: %s add <DriverID> <membase> <irq>              (add card)\n",cmd);
  fprintf(stderr,"   or: %s [-d <DriverID>] membase [membase-addr]      (get/set memaddr)\n",cmd);
  fprintf(stderr,"   or: %s [-d <DriverID>] irq   [irq-nr]              (get/set irq)\n",cmd);
  fprintf(stderr,"   or: %s [-d <DriverID>] [-v] load <protocol> [options]\n",cmd);
  fprintf(stderr,"   or: %s [-d <DriverID>] debug [<debug value>]\n",cmd);
  fprintf(stderr,"   or: %s [-d <DriverID>] manage [read|exec <path>]   (management-tool)\n",cmd);
#ifdef HAVE_NPCI
#ifdef HAVE_XLOG
  fprintf(stderr,"   or: %s [-d <DriverID>] xlog [cont]                 (request XLOG)\n",cmd);
#endif
#endif
  fprintf(stderr,"load firmware:\n");
  fprintf(stderr," basics  : -d <DriverID> ID defined when eicon module was loaded/card added\n");
  fprintf(stderr,"         : -v            verbose\n");
  fprintf(stderr," options : -l[channel#]  channel allocation policy\n");
  fprintf(stderr,"         : -c[1|2]       CRC4 Multiframe usage [off|on]\n");
  fprintf(stderr,"         : -tTEI         use the fixed TEI\n");
  fprintf(stderr,"         : -h            no HSCX 30 mode (PRI only)\n");
  fprintf(stderr,"         : -n            NT2 mode\n");
  fprintf(stderr,"         : -p            leased line D channel\n");
  fprintf(stderr,"         : -s[1|2]       LAPD layer 2 session strategy\n");
  fprintf(stderr,"         : -o            allow disordered info elements\n");
  fprintf(stderr,"         : -z            switch to loopback mode\n");
  exit(-1);
}


#ifdef HAVE_NPCI
/*--------------------------------------------------------------
 * display_combifile_details()
 *
 * Displays the information in the combifile header
 * Arguments: Pointer to the combifile structure in memory
 * Returns:   The address of the begining of directory which is
 *            directly after the file description.
 *-------------------------------------------------------------*/

t_dsp_combifile_directory_entry *display_combifile_details(char *details)
{
        __u32 offset=0;
        t_dsp_combifile_header *file_header;
        char *description;
        t_dsp_combifile_directory_entry *return_ptr = NULL;

        file_header = (t_dsp_combifile_header *)details;

        printf("%s\n", file_header->format_identification);
#ifdef __DEBUGVAR__ 
        printf("\tFormat Version: 0x%.4x\n", file_header->format_version_bcd);
        printf("\tNumber of directory entries : %d\n", file_header->directory_entries);
        printf("\tDownload count: %d\n", file_header->download_count);
#endif

        description = (char *)file_header + (file_header->header_size);

        printf("%s\n", description);

        num_directory_entries = file_header->directory_entries;
        usage_mask_size = file_header->usage_mask_size;
        download_count = file_header->download_count;
        directory_size = file_header->directory_size;

        return_ptr = (t_dsp_combifile_directory_entry *) (unsigned int)file_header ;
        offset  += (file_header->header_size);
        offset  += (file_header->combifile_description_size);
        offset += (__u32)return_ptr;

        return (t_dsp_combifile_directory_entry *)offset;

}


__u32 store_download(char *data, __u16 size, char *store)
{
        __u16 real_size;
        static int first = 1;
        static char* position;
        static char* initial;
        static __u32 addr;
        __u32 data_start_addr;
        __u32 align;

#ifdef __DEBUGVAR__
        printf("Writing Data to memory block\n");
#endif
                if(first)
                {
                addr = 0xa03a0000 + (((sizeof(__u32)+sizeof(p_download_table)) + ~0xfffffffc) & 0xfffffffc);
                position = initial = (char *)store;
                first = 0;
                }

/*Starting address to where the data is written in the download_block*/
	data_start_addr = addr;

        real_size = size;

        align = ((addr + (real_size + ~0xfffffffc)) & 0xfffffffc) -
                        (addr + real_size);

        memcpy(position, data, real_size);

        position += real_size;
        addr += real_size;

        bzero(position, align);

        position += align;
        addr += align;

        total_bytes_in_download = position - initial;

#ifdef __DEBUGVAR__
        printf("total_bytes written so far is 0x%x\n",total_bytes_in_download);
	printf("align value for this download is: %d\n",align);
#endif
        return (data_start_addr);
}



__u32 get_download(char *download_block, char *download_area)
{
        int n, i;
	char Text[100];
        char *usage_mask;
        char test_byte=0;
        __u32 length=0;
        __u32 addr;
    	unsigned int table_index;
        t_dsp_file_header *file_header;

        t_dsp_download_desc *p_download_desc;
        char *data;

	i=0;
        no_of_downloads = 0;
        test_byte = 0x01;
        test_byte <<= usage_bit;
        usage_mask = malloc(usage_mask_size);

        if(!usage_mask)
        {
                printf("Error allocating memory for usage mask");
                return 0;
        }
        bzero(usage_mask, usage_mask_size);

        for(n = 0; n < download_count; n++)
        {
                memcpy(usage_mask, download_area, usage_mask_size);
#ifdef __DEBUGVAR__ 
        printf("  \n");
                printf("Usage mask = 0x%.2x ", usage_mask[0]);
                if(usage_mask_size > 1)
                {
                        for(i=1; i<usage_mask_size; i++)
                        {
                                printf("0x%.2x ", usage_mask[i]);
                        }
                }
#endif
                download_area += usage_mask_size;

                file_header = (t_dsp_file_header *)download_area;
#ifdef __DEBUGVAR__
                printf("%s %d\n", file_header->format_identification,n);
#endif

                if( test_byte & usage_mask[usage_byte] )
                {
                no_of_downloads++;
	        table_index = (no_of_downloads - 1);

#ifdef __DEBUGVAR__
	            printf("*****DSP DOWNLOAD %d REQUIRED******\n", n);
        	    printf("download required count is now %d\n",no_of_downloads);
	            printf("   \n");
#endif


			/*This is the length of the memory to malloc */

                        length += ((__u32)((__u16)((file_header->header_size)
                                                - sizeof(t_dsp_file_header))))
                        + ((__u32)(file_header->download_description_size))
                        + ((__u32)(file_header->memory_block_table_size))
                        + ((__u32)(file_header->segment_table_size))
                        + ((__u32)(file_header->symbol_table_size))
                        + ((__u32)(file_header->total_data_size_dm))
                        + ((__u32)(file_header->total_data_size_pm));

                        if(download_block)
                        {
                        data = (char *)file_header;
                        data += ((__u32)(file_header->header_size));

                                p_download_desc = &(p_download_table[table_index]);
                                p_download_desc->download_id = file_header->download_id;
                                p_download_desc->download_flags = file_header->download_flags;
                                p_download_desc->required_processing_power = file_header->required_processing_power;
                                p_download_desc->interface_channel_count = file_header->interface_channel_count;
                                p_download_desc->excess_header_size = ((__u16)((file_header->header_size) - (__u16)sizeof(t_dsp_file_header)));
                                p_download_desc->memory_block_count = file_header->memory_block_count;
                                p_download_desc->segment_count = file_header->segment_count;
                                p_download_desc->symbol_count = file_header->symbol_count;
                                p_download_desc->data_block_count_dm = file_header->data_block_count_dm;
                                p_download_desc->data_block_count_pm = file_header->data_block_count_pm;

                                p_download_desc->p_excess_header_data = NULL;
                                if (((p_download_desc->excess_header_size) != 0))
                                {
#ifdef __DEBUGVAR__
                        printf("1.store_download called from get_download\n");
#endif
                                        addr = store_download(data, p_download_desc->excess_header_size,
                                                                                        download_block);
                                        p_download_desc->p_excess_header_data = (char *)addr;
                                        data += ((p_download_desc->excess_header_size));
                                }
                                p_download_desc->p_download_description = NULL;
                                if (((file_header->download_description_size) != 0))
                                {
#ifdef __DEBUGVAR__
                        printf("2.store_download called from get_download\n");
#endif
                                        addr = store_download(data, file_header->download_description_size,
                                                                                        download_block);
				/* Showing details of DSP-Task */
			if (verbose) {
				strncpy(Text, data, file_header->download_description_size);
				Text[file_header->download_description_size] = 0;
				printf("\t%s\n", Text);
			}

                                        p_download_desc->p_download_description = (char *)addr;
                                        data += ((file_header->download_description_size));
                                }
                                p_download_desc->p_memory_block_table = NULL;
                                if ((file_header->memory_block_table_size) != 0)
                                {
#ifdef __DEBUGVAR__
                        printf("3.store_download called from get_download\n");
#endif
                                        addr = store_download(data, file_header->memory_block_table_size,
                                                                                        download_block);
                                        p_download_desc->p_memory_block_table = (t_dsp_memory_block_desc *)addr;
                                        data += ((file_header->memory_block_table_size));
                                }
                                p_download_desc->p_segment_table = NULL;
                                if ((file_header->segment_table_size) != 0)
                                {
#ifdef __DEBUGVAR__
                        printf("4.store_download called from get_download\n");
#endif
                                        addr = store_download(data, file_header->segment_table_size,
                                                                                        download_block);
                                        p_download_desc->p_segment_table = (t_dsp_segment_desc *)addr;
                                        data += (file_header->segment_table_size);
                                }
                                p_download_desc->p_symbol_table = NULL;
                                if ((file_header->symbol_table_size) != 0)
                                {
#ifdef __DEBUGVAR__
                        printf("5.store_download called from get_download\n");
#endif
                                        addr = store_download(data, file_header->symbol_table_size,
                                                                                        download_block);
                                        p_download_desc->p_symbol_table = (t_dsp_symbol_desc *)addr;
                                        data += (file_header->symbol_table_size);
                                }
                                p_download_desc->p_data_blocks_dm = NULL;
                                if ((file_header->total_data_size_dm) != 0)
                                {
#ifdef __DEBUGVAR__
                        printf("6.store_download called from get_download\n");
#endif
                                        addr = store_download(data, file_header->total_data_size_dm,
                                                                                        download_block);
                                        p_download_desc->p_data_blocks_dm = (__u16 *)addr;
                                        data += (file_header->total_data_size_dm);
                                }
                                p_download_desc->p_data_blocks_pm = NULL;
                                if ((file_header->total_data_size_pm) != 0)
                                {
#ifdef __DEBUGVAR__
                        printf("7.store_download called from get_download\n");
#endif
                                        addr = store_download(data, file_header->total_data_size_pm,
                                                                                        download_block);
                                        p_download_desc->p_data_blocks_pm = (__u16 *)addr;
                                        data += (file_header->total_data_size_pm);
                                }
                        }
                }

                download_area += ((__u32)((__u16)((file_header->header_size))));
                download_area += ((__u32)((file_header->download_description_size)));
                download_area += ((__u32)((file_header->memory_block_table_size)));
                download_area += ((__u32)((file_header->segment_table_size)));
                download_area += ((__u32)((file_header->symbol_table_size)));
                download_area += ((__u32)((file_header->total_data_size_dm)));
                download_area += ((__u32)((file_header->total_data_size_pm)));


        }

	table_count=no_of_downloads;
	/**no_of_tables=table_count;*/
	bzero(no_of_tables,sizeof(__u32));
	memcpy(no_of_tables,&table_count,sizeof(__u32));

#ifdef __DEBUGVAR__
    printf("***0x%x bytes of memory required for %d downloads***\n", length, no_of_downloads);
#endif
        free(usage_mask);
        return length;
}


eicon_codebuf *load_combifile(int card_type, u_char *protobuf, int *plen)
{
        int fd;
	int tmp[9];
	char combifilename[100];
        int count, j;
        int file_set_number = 0;
        struct stat file_info;
        char *combifile_start;
        char *usage_mask_ptr;
        char *download_block;
        t_dsp_combifile_directory_entry *directory;
        t_dsp_combifile_directory_entry *tmp_directory;
        __u32 download_size;
	eicon_codebuf *cb;

	sprintf(combifilename, "%s/%s", __DATADIR__, COMBIFILE);
        if ((fd = open(combifilename, O_RDONLY, 0)) == -1)
        {
                perror("Error opening Eicon combifile");
                return(0);
        }

        if (fstat(fd, &file_info))
        {
                perror("Error geting file details of Eicon.Diehl combifile");
                close(fd);
                return(0);
        }

        if ( file_info.st_size <= 0 )
        {
                perror("Invalid file length in Eicon.Diehl combifile");
                close(fd);
                return(0);
        }

        combifile_start = malloc(file_info.st_size);

        if(!combifile_start)
        {
                perror("Error allocating memory for Eicon.Diehl combifile");
                close(fd);
                return(0);
        }

#ifdef __DEBUGVAR__
                printf("File mapped to address 0x%x\n", (__u32)combifile_start);
#endif


        if((read(fd, combifile_start, file_info.st_size)) != file_info.st_size)
        {
                perror("Error reading Eicon.Diehl combifile into memory");
                free(combifile_start);
                close(fd);
                return(0);
        }

        close(fd); /* We're done with the file */

        directory = display_combifile_details(combifile_start);

#ifdef __DEBUGVAR__
        printf("Directory mapped to address 0x%x, offset = 0x%x\n", (__u32)directory,
                                                ((unsigned int)directory - (unsigned int)combifile_start));
#endif

        tmp_directory = directory;

        for(count = 0; count < num_directory_entries; count++)
        {
                if((tmp_directory->card_type_number) == card_type)
                {
#ifdef __DEBUGVAR__
                        printf("Found entry in directory slot %d\n", count);
            		printf("Matched Card %d is %d. Fileset number is %d\n", count,
                       		(tmp_directory->card_type_number),
	                        (tmp_directory->file_set_number));

#endif
                        file_set_number = tmp_directory->file_set_number;
                        break;
                }
#ifdef __DEBUGVAR__
                printf("Card %d is %d. Fileset number is %d\n", count,
                                                (tmp_directory->card_type_number),
                                                (tmp_directory->file_set_number));
#endif
                tmp_directory++; 
        }

        if(count == num_directory_entries)
        {
                printf("Card not found in directory\n");
                free(combifile_start);
                return(0);
        }

        usage_bit = file_set_number%8;
        usage_byte= file_set_number/8;

#ifdef __DEBUGVAR__
        printf("Bit field is bit %d in byte %d of the usage mask\n",
                                usage_bit,
                                usage_byte);
#endif

        usage_mask_ptr = (char *)(directory);
        usage_mask_ptr += directory_size;

#ifdef __DEBUGVAR__
        printf("First mask at address 0x%x, offset = 0x%x\n", (__u32)usage_mask_ptr,
                                                (__u32)(usage_mask_ptr - combifile_start));
#endif

        no_of_tables = malloc((sizeof(__u32)));
        download_size = get_download(NULL, usage_mask_ptr);

#ifdef __DEBUGVAR__
        printf("Initial size of download_size is 0x%x\n",download_size);
#endif

        if(!download_size)
        {
                printf("Error getting details on DSP downloads\n");
                free(combifile_start);
                return(0);
        }

        download_block = malloc((download_size + (no_of_downloads * 100)));

#ifdef __DEBUGVAR__
        printf("download_block size = (download_size + alignments) is: 0x%x\n",(download_size + (no_of_downloads*100)));
#endif

        if(!download_block)
        {
                printf("Error allocating memory for download\n");
                free(combifile_start);
                return(0);
        }

#ifdef __DEBUGVAR__
        printf("Calling get_download to write into download_block\n");
#endif

        if(!(get_download(download_block, usage_mask_ptr)))
        {
                printf("Error getting data for DSP download\n");
                free(download_block);
                free(combifile_start);
	        free(no_of_tables);
                return(0);
        }

	tmp[0] = *plen;

	memcpy(protobuf + *plen, download_block, total_bytes_in_download);
	tmp[1] = total_bytes_in_download;
	*plen +=  total_bytes_in_download;

	memcpy(protobuf + *plen, no_of_tables, sizeof(table_count));
	tmp[2] = sizeof(table_count);
	*plen +=  sizeof(table_count);

	memcpy(protobuf + *plen, (char *)p_download_table, sizeof(p_download_table));
	tmp[3] = sizeof(p_download_table);
	*plen +=  sizeof(p_download_table);

	cb = malloc(sizeof(eicon_codebuf) + *plen);
	memset(cb, 0, sizeof(eicon_codebuf));
        memcpy(&cb->pci.code, protobuf, *plen);
    	for (j=0; j < 4; j++) {
		if (j==0) cb->pci.protocol_len = tmp[0];
		else cb->pci.dsp_code_len[j] = tmp[j];
	}
	cb->pci.dsp_code_num = 3;

 free(no_of_tables);
 free(download_block);
 free(combifile_start);
 return (cb);
}
#endif /* NPCI */

void beep2(void)
{
 beep();
 fflush(stdout);
 refresh();
}

int get_manage_element(char *m_dir, int request)
{
	int i,j,o,k,tmp;
	int len, vlen = 0;
	long unsigned duint;
	u_char buf[100];

	if (strlen(m_dir)) {
		if (m_dir[0] == '\\') m_dir++;
	}
	len = strlen(m_dir);
        mb->count = request;
        mb->pos = 0;
        mb->length[0] = len + 5;
        memset(&mb->data, 0, 690);

	if (len)
	        strncpy(&mb->data[5], m_dir, len);
        mb->data[4] = len;

        ioctl_s.arg = (ulong)mb;
        if (ioctl(fd, EICON_IOCTL_MANIF + IIOCDRVCTL, &ioctl_s) < 0) {
		return(-1);
        }
	if (request == 0x04) return 0;

	mb->pos = 0;
	man_ent_count = mb->count;
        for (i = 0; i < mb->count; i++) {
		man_ent[i].type = mb->data[mb->pos++];
		man_ent[i].attribute = mb->data[mb->pos++];
		man_ent[i].status = mb->data[mb->pos++];
		man_ent[i].var_length = mb->data[mb->pos++];
		man_ent[i].path_length = mb->data[mb->pos++];

		memcpy(man_ent[i].Name, &mb->data[mb->pos] + len, man_ent[i].path_length - len);
		man_ent[i].Name[man_ent[i].path_length - len] = 0;
		if (man_ent[i].Name[0] == '\\') strcpy(man_ent[i].Name, man_ent[i].Name + 1);
		mb->pos += man_ent[i].path_length;

		if (man_ent[i].type &0x80) 
			vlen = man_ent[i].var_length;
		else
			vlen = mb->length[i] - man_ent[i].path_length - 5; 

		memcpy(man_ent[i].Var, &mb->data[mb->pos], vlen); 
		man_ent[i].Var[vlen] = 0;
		mb->pos += (mb->length[i]  - man_ent[i].path_length - 5);
		o = 0;
		if (vlen) {
			switch(man_ent[i].type) {
				case 0x04:
					if (man_ent[i].Var[0])
					{
						j=0;
						do
						{
							buf[o++] = man_ent[i].Var[++j];
							buf[o] = 0;
						}
						while (j < man_ent[i].Var[0]);
						strcpy(man_ent[i].Var, buf);
					}
					break;
				case 0x05:
					if (man_ent[i].Var[0])
					{
						j=0;
						do
						{
							o+=sprintf(&buf[o],"%02x ",(__u8)man_ent[i].Var[++j]);
						}
						while (j < man_ent[i].Var[0]);
						strcpy(man_ent[i].Var, buf);
					}
					break;
				case 0x81:
					duint = man_ent[i].Var[vlen-1]&0x80 ? -1 : 0; 
					for (j=0; j<vlen; j++) ((__u8 *)&duint)[j] = man_ent[i].Var[j];
					sprintf(man_ent[i].Var,"%ld",(long)duint);
					break;
				case 0x82:
					for (j=0,duint=0; j<vlen; j++) 
						duint += ((unsigned char)man_ent[i].Var[j]) << (8 * j);
					sprintf(man_ent[i].Var,"%lu", duint);
					break;
				case 0x83:
					for (j=0,duint=0; j<vlen; j++)
						duint += ((unsigned char)man_ent[i].Var[j]) << (8 * j);
					sprintf(man_ent[i].Var,"%lx", duint);
					break;
				case 0x84:
					for (j=0; j<vlen; j++) o+=sprintf(&buf[o], "%02x", man_ent[i].Var[j]);
					strcpy(man_ent[i].Var, buf);
					break;
				case 0x85:
					for (j=0,duint=0; j<vlen; j++)
						duint += ((unsigned char) man_ent[i].Var[j]) << (8 * j);
					if (duint) sprintf(man_ent[i].Var,"TRUE");
					else sprintf(man_ent[i].Var,"FALSE");
					break;
				case 0x86:
					for (j=0; j<vlen; j++)
					{
						if (j) o+=sprintf(&buf[o], ".");
						o+=sprintf(&buf[o], "%03i", man_ent[i].Var[j]);
					}
					strcpy(man_ent[i].Var, buf);
					break;
				case 0x87:
					for (j=0,duint=0; j<vlen; j++)
						duint += ((unsigned char) man_ent[i].Var[j]) << (8 * j);
					for (j=0; j<vlen; j++)
					{
						if (j) o+=sprintf(&buf[o], " ");
						tmp = (__u8)(duint >> (vlen-j-1)*8);
						for (k=0; k<8; k++)
						{
							o+=sprintf(&buf[o], "%d", (tmp >> (7-k)) &0x1);
						}
					}
					strcpy(man_ent[i].Var, buf);
					break;
				case 0x88:
					j = MIN(sizeof(spid_state)/sizeof(__u8 *)-1, man_ent[i].Var[0]);
					sprintf(man_ent[i].Var, "%s", spid_state[j]);
					break;
			}
		}
		else 
		if (man_ent[i].type == 0x02) strcpy(man_ent[i].Var, "COMMAND");
	}
  return(0);
}

void eicon_manage_head(void)
{
	int ctype;
        mvwaddstr(headwin, 0,0,"Management for Eicon DIVA Server cards                      Cytronics & Melware");
        if ((ctype = ioctl(fd, EICON_IOCTL_GETTYPE + IIOCDRVCTL, &ioctl_s)) < 0) {
                return;
        }
        switch (ctype) {
                case EICON_CTYPE_MAESTRAP:
                        mvwaddstr(headwin, 1,0,"Adapter-type is Diva Server PRI/PCI");
                        break;
                case EICON_CTYPE_MAESTRA:
                        mvwaddstr(headwin, 1,0,"Adapter-type is Diva Server BRI/PCI");
                        break;
                default:
                        mvwaddstr(headwin, 1,0,"Adapter-type is unknown");
                        return;
        }
}

void eicon_manage_init_ncurses(void)
{
        initscr();
        noecho();
        nonl();
        refresh();
        cbreak();
        keypad(stdscr,TRUE);
	curs_set(0);
	statwin = newpad(50,80);
	headwin = newpad(5,80);
        start_color();
	
}

void show_man_entries(void)
{
	int i;
	char MLine[80];
	char AttSt[7];

        for(i = 0; i < man_ent_count; i++) {
                if (man_ent[i].type == 0x01) {
                        sprintf(AttSt, "<DIR>");
                } else {
                        sprintf(AttSt,"%c%c%c%c%c",
                                (man_ent[i].attribute &0x01) ? 'w' : '-',
                                (man_ent[i].attribute &0x02) ? 'e' : '-',
                                (man_ent[i].status &0x01) ? 'l' : '-',
                                (man_ent[i].status &0x02) ? 'e' : '-',
                                (man_ent[i].status &0x04) ? 'p' : '-');
                }
                sprintf(MLine, "%-17s %s %s\n",
                        man_ent[i].Name,
                        AttSt,
                        man_ent[i].Var);
                printf(MLine);
        }
}

void eicon_manage_draw(void)
{
	int i;
	int max_line = 0;
	char MLine[80];
	char AttSt[7];

	mvwaddstr(headwin, 2, 0, "                                                                                ");
        mvwaddstr(headwin, 3,0,"Name              Flags Variable");
        mvwaddstr(headwin, 4,0,"-------------------------------------------------------------------------------");

        max_line =  man_ent_count;
        for(i = 0; i < max_line; i++) {
                if (man_ent[i].type == 0x01) {
                        sprintf(AttSt, "<DIR>");
                } else {
                        sprintf(AttSt,"%c%c%c%c%c",
                                (man_ent[i].attribute &0x01) ? 'w' : '-',
                                (man_ent[i].attribute &0x02) ? 'e' : '-',
                                (man_ent[i].status &0x01) ? 'l' : '-',
                                (man_ent[i].status &0x02) ? 'e' : '-',
                                (man_ent[i].status &0x04) ? 'p' : '-');
                }
                sprintf(MLine, "%-17s %s %-55s",
                        man_ent[i].Name,
                        AttSt,
                        man_ent[i].Var);
                if (i == h_line) wattron(statwin, A_REVERSE);
                mvwaddstr(statwin, i, 0, MLine);
                wattroff(statwin, A_REVERSE);
        }
        for(i = max_line; i < 50; i++) {
		mvwaddstr(statwin, i, 0, "                                                                                ");
	}
        prefresh(statwin, stat_y, 0, 5, 0, LINES-4, COLS);
        mvwaddstr(headwin, 2,0,"Directory : ");
        waddstr(headwin, Man_Path);
        prefresh(headwin, 0, 0, 0, 0, 5, COLS);
	refresh();
}

void do_manage_resize(int dummy) {
        endwin();
	eicon_manage_init_ncurses();
	eicon_manage_head();
	eicon_manage_draw();
	eicon_manage_draw();
	refresh();
	signal(SIGWINCH, do_manage_resize);
}

void eicon_management(void)
{
	int Key;
	int i;
	h_line = 0;
	stat_y = 0;

	signal(SIGWINCH, do_manage_resize);

	eicon_manage_init_ncurses();
	eicon_manage_head();
	eicon_manage_draw();
	redraw1:
	eicon_manage_draw();
	Keyboard:
	Key = getch();
	switch(Key) {
		case 27:
		case 'q':
			break;
		case KEY_UP:
			if (h_line) {
				h_line--;
				if (stat_y > h_line) stat_y--;
			}
			goto redraw1;
		case KEY_DOWN:
			if (h_line < man_ent_count - 1) {
				h_line++;
				if ((stat_y + LINES - 9) < h_line) stat_y++;
			}
			goto redraw1;
		case KEY_LEFT:
			if ((strcmp(Man_Path,"\\")) && (strlen(Man_Path) > 1)) {
				for(i=strlen(Man_Path); i >= 0; i--) {
					if (Man_Path[i] == '\\') {
						Man_Path[i] = 0;
						break;
					}
				}
				if (strlen(Man_Path) == 0) strcpy(Man_Path,"\\");
				if (get_manage_element(Man_Path, 0x02) < 0) {
					clear();
		       	                mvaddstr(0,0, "Error ioctl Management-interface");
					refresh();
               			        return;
				}
				h_line = 0;
				stat_y = 0;
				goto redraw1;
			}
			beep2();
			goto Keyboard;
		case 10:
		case 13:
		case KEY_RIGHT:
			if (man_ent[h_line].type == 0x01) { /* Directory */
				if (Man_Path[strlen(Man_Path)-1] != '\\') strcat(Man_Path, "\\");
				strcat(Man_Path, man_ent[h_line].Name);
				if (get_manage_element(Man_Path, 0x02) < 0) {
					clear();
		       	                mvaddstr(0,0, "Error ioctl Management-interface");
					refresh();
               			        return;
				}
				h_line = 0;
				stat_y = 0;
				goto redraw1;
			}
			if (man_ent[h_line].type == 0x02) { /* Executable function */
				i = strlen(Man_Path);
				if (Man_Path[strlen(Man_Path)-1] != '\\') strcat(Man_Path, "\\");
				strcat(Man_Path, man_ent[h_line].Name);
				if (get_manage_element(Man_Path, 0x04) < 0) {
					clear();
		       	                mvaddstr(0,0, "Error ioctl Management-interface");
					refresh();
               			        return;
				}
				sleep(1);
				Man_Path[i] = 0;
				if (get_manage_element(Man_Path, 0x02) < 0) {
					clear();
		       	                mvaddstr(0,0, "Error ioctl Management-interface");
					refresh();
               			        return;
				}
				h_line = 0;
				stat_y = 0;
				goto redraw1;

			}
			beep2();
			goto Keyboard;
		case 'r':
			if (get_manage_element(Man_Path, 0x02) < 0) {
				clear();
	       	                mvaddstr(0,0, "Error ioctl Management-interface");
				refresh();
       			        return;
			}
			goto redraw1;
		default:
			beep2();
			goto Keyboard;
	}

}

void filter_slash(char *s)
{
	int i;
	for (i=0; i < strlen(s); i++)
		if (s[i] == '/') s[i] = '\\';
}

void load_startup_code(char *startupcode, char *fileext)
{
	FILE *code;
	u_char bootbuf[0x1000];
	char filename[100];
	int tmp;
	eicon_codebuf *cb;

	sprintf(filename, "%s/%s", __DATADIR__, startupcode);

	if (!(code = fopen(filename,"r"))) {
		perror(filename);
		exit(-1);
	}
	if ((tmp = fread(bootbuf, 1, sizeof(bootbuf), code))<1) {
		fprintf(stderr, "Read error on %s\n", filename);
		exit(-1);
	}
	fclose(code);
	cb = malloc(sizeof(eicon_codebuf) + tmp);
	memset(cb, 0, sizeof(eicon_codebuf));
	memcpy(&cb->isa.code, bootbuf, tmp);
	cb->isa.bootstrap_len = tmp;
	cb->isa.boot_opt = EICON_ISA_BOOT_NORMAL;
	printf("Loading Startup Code (%s %d bytes)...\n", startupcode, tmp);
	ioctl_s.arg = (ulong)cb;
	if (ioctl(fd, EICON_IOCTL_LOADBOOT + IIOCDRVCTL, &ioctl_s) < 0) {
		perror("ioctl LOADBOOT");
		exit(-1);
	}
	if ((tmp = ioctl(fd, EICON_IOCTL_GETTYPE + IIOCDRVCTL, &ioctl_s)) < 0) {
		perror("ioctl GETTYPE");
		exit(-1);
	}
	switch (tmp) {
		case EICON_CTYPE_S:
			strcpy(fileext,".bin");
			printf("Adapter-type is Eicon-S\n");
			break;
		case EICON_CTYPE_SX:
			strcpy(fileext,".sx");
			printf("Adapter-type is Eicon-SX\n");
			break;
		case EICON_CTYPE_SCOM:
			strcpy(fileext,".sy");
			printf("Adapter-type is Eicon-SCOM\n");
			break;
		case EICON_CTYPE_QUADRO:
			strcpy(fileext,".sq");
			printf("Adapter-type is Eicon-QUADRO\n");
			break;
		case EICON_CTYPE_S2M:
			strcpy(fileext,".p");
			printf("Adapter-type is Eicon-S2M\n");
			break;
		default:
			fprintf(stderr, "Unknown Adapter type %d for ISA-load\n", tmp);
			exit(-1);
	}
}


int main(int argc, char **argv) {
	int tmp;
	int ac;
	int arg_ofs=1;

	cmd = argv[0];
	if (argc > 1) {
		if (!strcmp(argv[arg_ofs], "-d")) {
			arg_ofs++;
			strcpy(ioctl_s.drvid, argv[arg_ofs++]);
			if (!strcmp(argv[arg_ofs], "-v")) {
				arg_ofs++;
				verbose = 1;
			}
		} else {
			ioctl_s.drvid[0] = '\0';
			if (!strcmp(argv[arg_ofs], "-v")) {
				arg_ofs++;
				verbose = 1;
			}
		}
	} else
		usage();
	ac = argc - (arg_ofs - 1);
	if (ac < 2)
		usage();
	fd = open("/dev/isdnctrl",O_RDWR);
	if (fd < 0) {
		perror("/dev/isdnctrl");
		exit(-1);
	}

	if ((tmp = ioctl(fd, EICON_IOCTL_GETVER + IIOCDRVCTL, &ioctl_s)) < 0) {
		fprintf(stderr, "Driver ID %s not found or\n", ioctl_s.drvid);
		fprintf(stderr, "Eicon kernel driver is too old !\n"); 
		exit(-1);
	}
	if (tmp < EICON_CTRL_VERSION) {
		fprintf(stderr, "Eicon kernel driver is older than eiconctrl !\n"); 
		fprintf(stderr, "Please update !\n");
	}
	if (tmp > EICON_CTRL_VERSION) {
		fprintf(stderr, "Eicon kernel driver is newer than eiconctrl !\n"); 
		fprintf(stderr, "Please update !\n");
	}

	if (!strcmp(argv[arg_ofs], "add")) {
		eicon_cdef *cdef;
		if (ac != 5) 
			usage();
		cdef = malloc(sizeof(eicon_cdef));
		strcpy(cdef->id, argv[arg_ofs + 1]);
		if (strlen(cdef->id) < 1)
			usage();
		if (sscanf(argv[arg_ofs + 2], "%i", &cdef->membase) !=1 )
			usage();
		if (sscanf(argv[arg_ofs + 3], "%i", &cdef->irq) !=1 )
			usage();
		ioctl_s.arg = (ulong)cdef;
		if (ioctl(fd, EICON_IOCTL_ADDCARD + IIOCDRVCTL, &ioctl_s) < 0) {
			perror("ioctl ADDCARD");
			exit(-1);
		}
		printf("Card added.\n");
		close(fd);
		return 0;
	}
	if (!strcmp(argv[arg_ofs], "membase")) {
		if (ac == 3) {
			if (sscanf(argv[arg_ofs + 1], "%i", &tmp) !=1 )
				usage();
			ioctl_s.arg = tmp;
			if (ioctl(fd, EICON_IOCTL_SETMMIO + IIOCDRVCTL, &ioctl_s) < 0) {
				perror("ioctl SETMMIO");
				exit(-1);
			}
		}
		if ((tmp = ioctl(fd, EICON_IOCTL_GETMMIO + IIOCDRVCTL, &ioctl_s)) < 0) {
			perror("ioctl GETMMIO");
			exit(-1);
		}
		printf("Shared memory at 0x%x\n", tmp);
		close(fd);
		return 0;
	}
	if (!strcmp(argv[arg_ofs], "irq")) {
		if (ac == 3) {
			if (sscanf(argv[arg_ofs + 1], "%i", &tmp) != 1)
				usage();
			ioctl_s.arg = tmp;
			if (ioctl(fd, EICON_IOCTL_SETIRQ + IIOCDRVCTL, &ioctl_s) < 0) {
				perror("ioctl SETIRQ");
				exit(-1);
			}
		}
		if ((tmp = ioctl(fd, EICON_IOCTL_GETIRQ + IIOCDRVCTL, &ioctl_s)) < 0) {
			perror("ioctl GETIRQ");
			exit(-1);
		}
		printf("Irq is %d\n", tmp);
		close(fd);
		return 0;
	}

        if ((!strcmp(argv[arg_ofs], "load")) || (!strcmp(argv[arg_ofs], "loadpci"))) {
                FILE *code;
		int isabus = 0;
                int plen = 0;
		int ctype = 0;
		int card_type = 0;
		int tmp, i;
		int tei = 255;
		char fileext[5];
                char protoname[1024];
                char filename[1024];
                u_char protobuf[0x100000];
                eicon_codebuf *cb = NULL;

		if (argc <= (arg_ofs + 1))
                       	strcpy(protoname,"etsi");
		else {
			if (argv[arg_ofs + 1][0] == '-')
                       		strcpy(protoname,"etsi");
			else	
	                        strcpy(protoname,argv[++arg_ofs]);
		}

		if ((ctype = ioctl(fd, EICON_IOCTL_GETTYPE + IIOCDRVCTL, &ioctl_s)) < 1) {
			perror("ioctl GETTYPE");
			exit(-1);
		}
		switch (ctype) {
			case EICON_CTYPE_MAESTRAP:
				printf("Adapter-type is Diva Server PRI/PCI\n");
				card_type = 23;
#ifdef HAVE_NPCI
				strcpy(fileext, ".pm");
				tei = 1;
				break;
#else
				fprintf(stderr, "Adapter-type not supported for load !\n");
				fprintf(stderr, "Update of util package is necessary, ");
				fprintf(stderr, "because of major changes in driver code.\n");
                          	exit(-1);
#endif
			case EICON_CTYPE_MAESTRA:
				printf("Adapter-type is Diva Server BRI/PCI\n");
				card_type = 21;
#ifdef HAVE_NPCI
				strcpy(fileext, ".sm");
				tei = 0;
				break;
#else
				fprintf(stderr, "Adapter-type not supported for load !\n");
				fprintf(stderr, "Update of util package is necessary, ");
				fprintf(stderr, "because of major changes in driver code.\n");
                          	exit(-1);
#endif
			case EICON_CTYPE_S:
			case EICON_CTYPE_SX:
			case EICON_CTYPE_SCOM:
			case EICON_CTYPE_QUADRO:
			case EICON_CTYPE_ISABRI:
				isabus = 1;
				tei = 0;
				load_startup_code("dnload.bin", fileext);
				break;
			case EICON_CTYPE_S2M:
			case EICON_CTYPE_ISAPRI:
				isabus = 1;
				tei = 1;
				load_startup_code("prload.bin", fileext);
				break;
			default:
				fprintf(stderr, "Adapter type %d not supported\n", ctype);
                          	exit(-1);
		}

	  	sprintf(filename, "%s/te_%s%s", __DATADIR__, protoname, fileext);
               	if (!(code = fopen(filename,"r"))) {
                               	perror(filename);
                               	exit(-1);
               	}
		printf("Protocol File : %s ", filename);
		if ((tmp = fread(protobuf, 1, sizeof(protobuf), code))<1) {
			fclose(code);
			fprintf(stderr, "Read error on %s\n", filename);
       	                exit(-1);
		}
               	fclose(code);
               	printf("(%d bytes)\n", tmp);
		plen += tmp;
			
		if (verbose) {
			if (isabus) {
				printf("Protocol: %s\n", &protobuf[4]);
				plen = (plen % 256)?((plen/256)+1)*256:plen;
			} else {
				strncpy(filename, &protobuf[0x80], 100);
				for (i=0; filename[i] && filename[i]!='\r' && filename[i]!='\n'; i++);
				filename[i] = 0;
				printf("%s\n", filename);
			}
		}

		if (isabus) {
			if(!(cb = malloc(sizeof(eicon_codebuf) + plen ))) {
        	                fprintf(stderr, "Out of Memory\n");
       	        	        exit(-1);
			}
			memset(cb, 0, sizeof(eicon_codebuf));
			memcpy(&cb->isa.code, protobuf, plen);
			cb->isa.firmware_len = plen;
		} else {
#ifdef HAVE_NPCI
			if (!(cb = load_combifile(card_type, protobuf, &plen))) {
        	                fprintf(stderr, "Error loading Combifile\n");
       	        	        exit(-1);
			}
#endif
		}

		if (isabus) {
			cb->isa.tei = tei;
			cb->isa.nt2 = 0;
			cb->isa.WatchDog = 0;
			cb->isa.Permanent = 0;
			cb->isa.XInterface = 0;
			cb->isa.StableL2 = 0;
			cb->isa.NoOrderCheck = 0;
			cb->isa.HandsetType = 0;
			cb->isa.LowChannel = 0;
			cb->isa.ProtVersion = 0;
			cb->isa.Crc4 = 0;
			cb->isa.Loopback = 0;
		} else {
#ifdef HAVE_NPCI
			cb->pci.tei = tei;
			cb->pci.nt2 = 0;
			cb->pci.WatchDog = 0;
			cb->pci.Permanent = 0;
			cb->pci.XInterface = 0;
			cb->pci.StableL2 = 0;
			cb->pci.NoOrderCheck = 0;
			cb->pci.HandsetType = 0;
			cb->pci.LowChannel = 0;
			cb->pci.ProtVersion = 0;
			cb->pci.Crc4 = 0;
			cb->pci.Loopback = 0;
			cb->pci.NoHscx30Mode = 0;
#endif /* NPCI */
		}

		/* parse extented options */
		while(ac > (arg_ofs + 1)) {
			arg_ofs++;
			if (!strncmp(argv[arg_ofs], "-l", 2)) {
				if (isabus) {
					cb->isa.LowChannel = atoi(argv[arg_ofs] + 2);
					if (!cb->isa.LowChannel) cb->isa.LowChannel = 1;
				} else {
#ifdef HAVE_NPCI
					cb->pci.LowChannel = atoi(argv[arg_ofs] + 2);
					if (!cb->pci.LowChannel) cb->pci.LowChannel = 1;
#endif /* NPCI */
				}
                                continue;
                        }
                        if (!strncmp(argv[arg_ofs], "-t", 2)) {
				if (isabus) {
                                	cb->isa.tei = atoi(argv[arg_ofs] + 2);
	                                cb->isa.tei <<= 1;
        	                        cb->isa.tei |= 0x01;
				} else {
#ifdef HAVE_NPCI
                                	cb->pci.tei = atoi(argv[arg_ofs] + 2);
	                                cb->pci.tei <<= 1;
        	                        cb->pci.tei |= 0x01;
#endif
				}
                                continue;
                        }
                        if (!strcmp(argv[arg_ofs], "-z")) {
				if (isabus) 
	                                cb->isa.Loopback = 1;
#ifdef HAVE_NPCI
				else
	                                cb->pci.Loopback = 1;
#endif
                                continue;
                        }
                        if (!strcmp(argv[arg_ofs], "-p")) {
				if (isabus) 
                                	cb->isa.Permanent = 1;
#ifdef HAVE_NPCI
				else
                                	cb->pci.Permanent = 1;
#endif
                                continue;
                        }
                        if (!strcmp(argv[arg_ofs], "-w")) {
				if (isabus) 
                        	        cb->isa.WatchDog = 1;
#ifdef HAVE_NPCI
				else
                        	        cb->pci.WatchDog = 1;
#endif
                                continue;
                        }
                        if (!strcmp(argv[arg_ofs], "-c")) {
				if (isabus) 
                	                cb->isa.Crc4 = 1;
#ifdef HAVE_NPCI
				else
                	                cb->pci.Crc4 = 1;
#endif
                                continue;
                        }
                        if (!strcmp(argv[arg_ofs], "-c1")) {
				if (isabus) 
        	                        cb->isa.Crc4 = 1;
#ifdef HAVE_NPCI
				else
        	                        cb->pci.Crc4 = 1;
#endif
                                continue;
                        }
                        if (!strcmp(argv[arg_ofs], "-c2")) {
				if (isabus) 
	                                cb->isa.Crc4 = 2;
#ifdef HAVE_NPCI
				else
	                                cb->pci.Crc4 = 2;
#endif
                                continue;
                        }
                        if (!strcmp(argv[arg_ofs], "-n")) {
				if (isabus) 
                                	cb->isa.nt2 = 1;
#ifdef HAVE_NPCI
				else
                                	cb->pci.nt2 = 1;
#endif
                                continue;
                        }
                        if (!strcmp(argv[arg_ofs], "-p")) {
				if (isabus) 
                        	        cb->isa.Permanent = 1;
#ifdef HAVE_NPCI
				else
                        	        cb->pci.Permanent = 1;
#endif
                                continue;
                        }
                        if (!strcmp(argv[arg_ofs], "-h")) {
#ifdef HAVE_NPCI
                                if (!isabus) cb->pci.NoHscx30Mode = 1;
#endif
                                continue;
                        }
                        if (!strcmp(argv[arg_ofs], "-o")) {
				if (isabus) 
                	                cb->isa.NoOrderCheck = 1;
#ifdef HAVE_NPCI
				else
                	                cb->pci.NoOrderCheck = 1;
#endif
                                continue;
                        }
                        if (!strcmp(argv[arg_ofs], "-s")) {
				if (isabus) 
        	                        cb->isa.StableL2 = 1;
#ifdef HAVE_NPCI
				else
        	                        cb->pci.StableL2 = 1;
#endif
                                continue;
                        }
                        if (!strcmp(argv[arg_ofs], "-s1")) {
				if (isabus) 
	                                cb->isa.StableL2 = 1;
#ifdef HAVE_NPCI
				else
	                                cb->pci.StableL2 = 1;
#endif
                                continue;
                        }
                        if (!strcmp(argv[arg_ofs], "-s2")) {
				if (isabus) 
                                	cb->isa.StableL2 = 2;
#ifdef HAVE_NPCI
				else
                                	cb->pci.StableL2 = 2;
#endif
                                continue;
                        }
		}
		printf("Downloading Code (%d bytes)...\n", plen);
		ioctl_s.arg = (ulong)cb;
		tmp = (isabus) ? EICON_IOCTL_LOADISA : EICON_IOCTL_LOADPCI;
		if (ioctl(fd, tmp + IIOCDRVCTL, &ioctl_s) < 0) {
			printf("\n");
			perror("ioctl LOAD");
			exit(-1);
		}
		printf("completed.\n");
		close(fd);
		return 0;
        }

        if (!strcmp(argv[arg_ofs], "debug")) {
		if (argc <= (arg_ofs + 1))
			ioctl_s.arg = 1;
		else	
			ioctl_s.arg = atol(argv[arg_ofs + 1]);
		if (ioctl(fd, EICON_IOCTL_DEBUGVAR + IIOCDRVCTL, &ioctl_s) < 0) {
			perror("ioctl DEBUG VALUE");
			exit(-1);
		}
		return 0;
	}

        if (!strcmp(argv[arg_ofs], "freeit")) {
		if (ioctl(fd, EICON_IOCTL_FREEIT + IIOCDRVCTL, &ioctl_s) < 0) {
			perror("ioctl FREEIT");
			exit(-1);
		}
		return 0;
	}

#ifdef HAVE_NPCI
#ifdef HAVE_XLOG
        if (!strcmp(argv[arg_ofs], "xlog")) {
		int cont = 0;
		int ii;
		int ret_val;
		int end = 0;
		__u32 sec, msec;
		mi_pc_maint_t *pcm;
		xlogreq_t xlogreq;
		xlog_entry_t xlog_entry;
		xlog_entry_t swap_entry;
		__u8 *xlog_byte, *swap_byte;

		xlog_byte = (__u8 *)&xlog_entry;
		swap_byte = (__u8 *)&swap_entry;

		if ((argc > (++arg_ofs)) && (!strcmp(argv[arg_ofs], "cont")))
			cont = 1;
		printf("Getting log%s...\n", ((cont) ? ", CTRL-C to quit" : ""));

		pcm = &xlogreq.pcm;

		while(1)
		{
			fflush(stdout);

			memset(&xlogreq, 0, sizeof(xlogreq_t));
			xlogreq.command = 1;
			ioctl_s.arg = (ulong)&xlogreq;
			if ((ret_val = ioctl(fd, EICON_IOCTL_GETXLOG + IIOCDRVCTL, &ioctl_s)) < 0) {
				perror("ioctl XLOG");
                		close(fd); 
				exit(-1);
			}

			*(MIPS_BUFFER *)&xlog_entry = pcm->data;
			msec = (((__u32)xlog_entry.timeh)<<16)+xlog_entry.timel;
			sec=msec/1000;
			switch (ret_val)
			{
				case XLOG_OK:
					for(ii = 0; ii < MIPS_BUFFER_SZ; ii += 2)
					{
						swap_byte[ii] = xlog_byte[ii+1];
						swap_byte[ii+1] = xlog_byte[ii];
					}

					switch(xlog_entry.code)
					{
						case 1:
							msec = (((__u32)SWAP(xlog_entry.timeh))<<16)+SWAP(xlog_entry.timel);
							sec=msec/1000;
							printf("%5ld:%04ld:%03ld - ",
								(long)sec/3600,
								(long)sec%3600,
								(long)msec%1000 );
							printf("%s\n", xlog_entry.buffer );
							break;

						case 2:
							msec = (((__u32)SWAP(xlog_entry.timeh))<<16)+SWAP(xlog_entry.timel);
							sec=msec/1000;
							printf("%5ld:%04ld:%03ld - ",
								(long)sec/3600,
								(long)sec%3600,
								(long)msec%1000 );
							xlog(stdout, xlog_entry.buffer);
							break;

						default:
							printf("\n unknown code %d\n", SWAP(xlog_entry.code));
							end++;
							break;
					}

					break;
				case XLOG_ERR_DONE:
					if(cont) {
						sleep(2);
					} else {
						end++;
					}
					break;

				case XLOG_ERR_CARD_STATE:
					printf("Card in wrong state for tracing\n");
					end++;
					break;

				case XLOG_ERR_CMD:
					printf("Command error doing ioctl\n");
					end++;
					break;

				case XLOG_ERR_TIMEOUT:
					if(cont) {
						sleep(2);
					} else {
						printf("Xlog timeout\n");
						end++;
					}
					break;

				case XLOG_ERR_UNKNOWN:
					printf("Unknown error during ioctl\n");
					end++;
					break;

				 default:
					printf("Returned (%d)\n", ret_val);
					perror("Error doing ioctl");
					end++;
					break;
			}
			if( end )
				break;
		}
                close(fd); 
		return 0;
	}
#endif /* XLOG */
#endif /* NPCI */

        if (!strcmp(argv[arg_ofs], "manage")) {
		mb = malloc(sizeof(eicon_manifbuf));

		if (argc > (++arg_ofs)) {
			if (!strcmp(argv[arg_ofs], "read")) {
				if (argc <= (arg_ofs + 1)) {
					fprintf(stderr, "Path to be read is missing\n");
					exit(-1);
				}
				strcpy(Man_Path, argv[arg_ofs + 1]);
				filter_slash(Man_Path);
				if (get_manage_element(Man_Path, 0x02) < 0) {
		       	                fprintf(stderr, "Error ioctl Management-interface\n");
               			        exit(-1);
				}
				show_man_entries();
				close(fd);
				return 0;
			}
			if (!strcmp(argv[arg_ofs], "exec")) {
				if (argc <= (arg_ofs + 1)) {
					fprintf(stderr, "Path to be executed is missing\n");
					exit(-1);
				}
				strcpy(Man_Path, argv[arg_ofs + 1]);
				filter_slash(Man_Path);
				if (get_manage_element(Man_Path, 0x04) < 0) {
		       	                fprintf(stderr, "Error ioctl Management-interface\n");
               			        exit(-1);
				}
				close(fd);
				return 0;
			}
			fprintf(stderr, "Unknown command for Management-interface\n");
			exit(-1);
		}

		strcpy (Man_Path, "\\");

		if (get_manage_element(Man_Path, 0x02) < 0) {
       	                fprintf(stderr, "Error ioctl Management-interface\n");
               	        exit(-1);
		}

		eicon_management();
		move(22,0);
		refresh();
		endwin();

                close(fd); 
		return 0;
	}

	fprintf(stderr, "unknown command\n");
	exit(-1);
}
