
/*
 *
 * Copyright (C) Eicon Technology Corporation, 2000.
 *
 * This source file is supplied for the exclusive use with Eicon
 * Technology Corporation's range of DIVA Server Adapters.
 *
 * Eicon File Revision :    1.1  
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY OF ANY KIND WHATSOEVER INCLUDING ANY 
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */


#define byte unsigned char
#define word unsigned short
#define dword unsigned long
#define TRUE 1
#define FALSE 0

#include <stdio.h>
#include <string.h>
#include "cau_1tr6.h"
#include "cau_q931.h"

#ifdef	far
#undef  far
#endif

#define far

#define SWAP(a) a

/*----------------------------------------------------------------*/
/* Prints into a buffer instead into a file if PRT2BUF is defined */
#if defined(PRT2BUF)
  #define FILE byte
  #define fprintf o+=sprintf
  #define Out  &stream[o]
#else
  #define Out stream
#endif
static word o;  /* index to buffer, not used if printed to file */
/*----------------------------------------------------------------*/
/* Define max byte count of a message to print before "cont"      */
/* variable can be set externally to change this behaviour        */
word maxout = 30;

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

#define ZERO_BASED_INDEX_VALID(x,t) ((unsigned)x < sizeof(t)/sizeof(t[0]))
#define ONE_BASED_INDEX_VALID(x,t) ((unsigned)x <= sizeof(t)/sizeof(t[0]))

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

#define MT_MAX 34
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
  { 0x24, "HOLD_REQ" },
  { 0x25, "SUSP" },
  { 0x26, "RESUME" },
  { 0x28, "HOLD_ACK" },
  { 0x30, "HOLD_REJ" },
  { 0x31, "RETRIEVE_REQ" },
  { 0x33, "RETRIEVE_ACK" },
  { 0x37, "RETRIEVE_REJ" },
  { 0x2d, "SUSP_ACK" },
  { 0x2e, "RESUME_ACK" },
  { 0x45, "DISC" },
  { 0x46, "RESTART" },
  { 0x4d, "REL" },
  { 0x4e, "RESTART_ACK" },
  { 0x5a, "REL_COM" },
  { 0x60, "SEGMENT" },
  { 0x62, "FACILITY" },
  { 0x64, "REGISTER" },
  { 0x6d, "GINFO" },
  { 0x6e, "NOTIFY" },
  { 0x75, "STATUS_ENQ" },
  { 0x79, "CONGESTION_CTRL" },
  { 0x7d, "STATUS" },
  { 0x7b, "INFO" }
};

#define IE_MAX 41
static struct {
  word code;
  char * name;
} ie[IE_MAX] = {
  { 0x00a1, "Sending complete" },
  { 0x00a0, "MORE" },
  { 0x00b0, "Congestion level" },
  { 0x00d0, "Repeat indicator" },

  { 0x0000, "SMSG" },
  { 0x0004, "Bearer Capability" },
  { 0x0008, "Cause" },
  { 0x000c, "Connected Address" },
  { 0x0010, "Call Identity" },
  { 0x0014, "Call State" },
  { 0x0018, "Channel Id" },
  { 0x001a, "Advice of Charge" },
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
  { 0x004c, "Connected Number" },
  { 0x006c, "Calling Party Number" },
  { 0x006d, "Calling Party Subaddress" },
  { 0x0070, "Called Party Number" },
  { 0x0071, "Called Party Subaddress" },
  { 0x0074, "Redirected Number" },
  { 0x0076, "Redirecting Number" },
  { 0x0078, "TNET" },
  { 0x0079, "Restart Indicator" },
  { 0x007c, "LLC" },
  { 0x007d, "HLC" },
  { 0x007e, "User User Info" },
  { 0x007f, "ESC" },

  { 0x051a, "Advice of Charge" },
  { 0x061a, "Advice of Charge" },
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
  "T309",
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


/* MIPS CPU exeption context structure */
struct xcptcontext {
    dword       sr;
    dword       cr;
    dword       epc;
    dword       vaddr;
    dword       regs[32];
    dword       mdlo;
    dword       mdhi;
    dword       reseverd;
    dword       xclass;
}xcp;

/* MIPS CPU exception classes */
#define XCPC_GENERAL    0
#define XCPC_TLBMISS    1
#define XCPC_XTLBMISS   2
#define XCPC_CACHEERR   3
#define XCPC_CLASS      0x0ff
#define XCPC_USRSTACK   0x100

#define XCPTINTR        0
#define XCPTMOD         1
#define XCPTTLBL        2
#define XCPTTLBS        3
#define XCPTADEL        4
#define XCPTADES        5
#define XCPTIBE         6
#define XCPTDBE         7
#define XCPTSYS         8
#define XCPTBP          9
#define XCPTRI          10
#define XCPTCPU         11
#define XCPTOVF         12
#define XCPTTRAP        13
#define XCPTVCEI        14
#define XCPTFPE         15
#define XCPTCP2         16
#define XCPTRES17       17
#define XCPTRES18       18
#define XCPTRES19       19
#define XCPTRES20       20
#define XCPTRES21       21
#define XCPTRES22       22
#define XCPTWATCH       23
#define XCPTRES24       24
#define XCPTRES25       25
#define XCPTRES26       26
#define XCPTRES27       27
#define XCPTRES28       28
#define XCPTRES29       29
#define XCPTRES30       30
#define XCPTVCED        31
#define NXCPT           32

/* exception classes */
#define XCPC_GENERAL    0
#define XCPC_TLBMISS    1
#define XCPC_XTLBMISS   2
#define XCPC_CACHEERR   3
#define XCPC_CLASS      0x0ff
#define XCPC_USRSTACK   0x100


/*------------------------------------------------------------------*/
/* local function prototypes                                        */
/*------------------------------------------------------------------*/

word xlog(FILE * stream,void * buffer);
void xlog_sig(FILE * stream, struct msg_s * message);
void call_failed_event(FILE * stream, struct msg_s * message, word code);
void display_q931_message(FILE * stream, struct msg_s * message);
void display_ie(FILE * stream, byte pd, word w, byte * ie);
void spid_event(FILE * stream, struct msg_s * message, word code);

word xlog(FILE *stream, void * buf)
{
  word i;
  word n;
  word code;
  XLOG *buffer = buf;

  o = 0;
  switch((byte)SWAP(buffer->code)) {
  case 1:
    n = SWAP(buffer->info.l1.length);
    if(SWAP(buffer->code) &0xff00) fprintf(stream,"B%d-X(%03d) ",SWAP(buffer->code)>>8,n);
    else fprintf(stream,"B-X(%03d) ",n);
    for(i=0;i<n && i<maxout;i++)
      fprintf(stream,"%02X ",buffer->info.l1.i[i]);
    if(n>i) fprintf(stream,"cont");
    break;
  case 2:
    n = SWAP(buffer->info.l1.length);
    if(SWAP(buffer->code) &0xff00) fprintf(stream,"B%d-R(%03d) ",SWAP(buffer->code)>>8,n);
    else fprintf(stream,"B-R(%03d) ",n);
    for(i=0;i<n && i<maxout;i++)
      fprintf(stream,"%02X ",buffer->info.l1.i[i]);
    if(n>i) fprintf(stream,"cont");
    break;
  case 3:
    n = SWAP(buffer->info.l1.length);
    fprintf(stream,"D-X(%03d) ",n);
    for(i=0;i<n && i<maxout;i++)
      fprintf(stream,"%02X ",buffer->info.l1.i[i]);
    if(n>i) fprintf(stream,"cont");
    break;
  case 4:
    n = SWAP(buffer->info.l1.length);
    fprintf(stream,"D-R(%03d) ",n);
    for(i=0;i<n && i<maxout;i++)
      fprintf(stream,"%02X ",buffer->info.l1.i[i]);
    if(n>i) fprintf(stream,"cont");
    break;
  case 5:
    n = SWAP(buffer->info.l2.length);
    fprintf(stream,"SIG-EVENT(%03d)%04X - ",n,SWAP(buffer->info.l2.code));
    for(i=0;i<n && i<maxout;i++)
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
    for(i=0;i<n && i<maxout;i++)
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
    for(i=0;i<n && i<maxout;i++)
      fprintf(stream,"%02X ",buffer->info.l1.i[i]);
    if(n>i) fprintf(stream,"cont");
    break;
  case 13:
    n = SWAP(buffer->info.l1.length);
    fprintf(stream,"X-R(%03d) ",n);
    for(i=0;i<n && i<maxout;i++)
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
                   SWAP(buffer->info.l2.code),buffer->info.l2.i);
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
    if ( ZERO_BASED_INDEX_VALID(code,l1_name) )
      fprintf(stream, l1_name[code]);
	else
      fprintf(stream, "UNKNOWN L1 STATE (0x%04x)", code);
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

  case 24:
    fprintf(stream, buffer->info.text);
    break;

  case 50:
    /*--- register dump comes in two blocks, each 80 Bytes long. Print
      --- content after second part has been arrived */

    if (0 == ((byte)(SWAP(buffer->code)>>8)))  /* first part */
    {
      for (i=0; i<20; i++)
        ((dword far *)(&xcp.sr))[i] = ((dword far *)&buffer->info.text)[i];
      fprintf(stream,"Register Dump part #1 received.");
      break;  /* wait for next */
    }
    for (i=0; i<20; i++)
      ((dword far *)(&xcp.sr))[i+20] = ((dword far *)&buffer->info.text)[i];
    fprintf(stream,"Register Dump part #2 received, printing now:\n");
    fprintf(stream,"CPU Exception Class: %04x Code:",(int) xcp.xclass);

    switch ((xcp.cr &0x0000007c) >> 2)
    {
      case XCPTINTR: fprintf(stream,"interrupt                  "); break;
      case XCPTMOD:  fprintf(stream,"TLB mod  /IBOUND           "); break;
      case XCPTTLBL: fprintf(stream,"TLB load /DBOUND           "); break;
      case XCPTTLBS: fprintf(stream,"TLB store                  "); break;
      case XCPTADEL: fprintf(stream,"Address error load         "); break;
      case XCPTADES: fprintf(stream,"Address error store        "); break;
      case XCPTIBE:  fprintf(stream,"Instruction load bus error "); break;
      case XCPTDBE:  fprintf(stream,"Data load/store bus error  "); break;
      case XCPTSYS:  fprintf(stream,"Syscall                    "); break;
      case XCPTBP:   fprintf(stream,"Breakpoint                 "); break;
      case XCPTCPU:  fprintf(stream,"Coprocessor unusable       "); break;
      case XCPTRI:   fprintf(stream,"Reverd instruction         "); break;
      case XCPTOVF:  fprintf(stream,"Overflow                   "); break;
      case 23:       fprintf(stream,"WATCH                      "); break;
      default:       fprintf(stream,"Unknown Code               "); break;
    }
    fprintf(stream,"\n");

    fprintf(stream,"sr  = %08lx\t",xcp.sr);
    fprintf(stream,"cr  = %08lx\t",xcp.cr);
    fprintf(stream,"epc = %08lx\t",xcp.epc);
    fprintf(stream,"vadr= %08lx\n",xcp.vaddr);

    fprintf(stream,"zero= %08lx\t",xcp.regs[0]);
    fprintf(stream,"at  = %08lx\t",xcp.regs[1]);
    fprintf(stream,"v0  = %08lx\t",xcp.regs[2]);
    fprintf(stream,"v1  = %08lx\n",xcp.regs[3]);
    fprintf(stream,"a0  = %08lx\t",xcp.regs[4]);
    fprintf(stream,"a1  = %08lx\t",xcp.regs[5]);
    fprintf(stream,"a2  = %08lx\t",xcp.regs[6]);
    fprintf(stream,"a3  = %08lx\n",xcp.regs[7]);
    fprintf(stream,"t0  = %08lx\t",xcp.regs[8]);
    fprintf(stream,"t1  = %08lx\t",xcp.regs[9]);
    fprintf(stream,"t2  = %08lx\t",xcp.regs[10]);
    fprintf(stream,"t3  = %08lx\n",xcp.regs[11]);
    fprintf(stream,"t4  = %08lx\t",xcp.regs[12]);
    fprintf(stream,"t5  = %08lx\t",xcp.regs[13]);
    fprintf(stream,"t6  = %08lx\t",xcp.regs[14]);
    fprintf(stream,"t7  = %08lx\n",xcp.regs[15]);
    fprintf(stream,"s0  = %08lx\t",xcp.regs[16]);
    fprintf(stream,"s1  = %08lx\t",xcp.regs[17]);
    fprintf(stream,"s2  = %08lx\t",xcp.regs[18]);
    fprintf(stream,"s3  = %08lx\n",xcp.regs[19]);
    fprintf(stream,"s4  = %08lx\t",xcp.regs[20]);
    fprintf(stream,"s5  = %08lx\t",xcp.regs[21]);
    fprintf(stream,"s6  = %08lx\t",xcp.regs[22]);
    fprintf(stream,"s7  = %08lx\n",xcp.regs[23]);
    fprintf(stream,"t8  = %08lx\t",xcp.regs[24]);
    fprintf(stream,"t9  = %08lx\t",xcp.regs[25]);
    fprintf(stream,"k0  = %08lx\t",xcp.regs[26]);
    fprintf(stream,"k1  = %08lx\n",xcp.regs[27]);
    fprintf(stream,"gp  = %08lx\t",xcp.regs[28]);
    fprintf(stream,"sp  = %08lx\t",xcp.regs[29]);
    fprintf(stream,"s8  = %08lx\t",xcp.regs[30]);
    fprintf(stream,"ra  = %08lx\n",xcp.regs[31]);
    fprintf(stream,"mdlo= %08lx\t",xcp.mdlo);
    fprintf(stream,"mdhi= %08lx\n",xcp.mdhi);
    break;

  case 128:
  case 129:
    n = SWAP(buffer->info.l1.length);
    if(SWAP(buffer->code)==128) fprintf(stream,"CAPI20_PUT(%03d) ",n);
    else fprintf(stream,"CAPI20_GET(%03d) ",n);
    fprintf(stream,"APPL %02x%02x ", buffer->info.l1.i[1], buffer->info.l1.i[0]);

    fprintf(stream,"%02x%02x:%02x%02x%02x%02x ",
        buffer->info.l1.i[5], buffer->info.l1.i[4],
        buffer->info.l1.i[9], buffer->info.l1.i[8],
        buffer->info.l1.i[7], buffer->info.l1.i[6]);

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
    if(SWAP(buffer->code)==130) fprintf(stream,"CAPI11_PUT(%03d) ",n);
    else fprintf(stream,"CAPI11_GET(%03d) ",n);
    fprintf(stream,"APPL %02x%02x ", buffer->info.l1.i[1], buffer->info.l1.i[0]);
    fprintf(stream,"%02x%02x:", buffer->info.l1.i[5], buffer->info.l1.i[4]);

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
  return o;
}

void xlog_sig(FILE * stream, struct msg_s * message)
{
  word n;
  word i;
  word cr;
  byte msg;

  n = message->length;


  msg = TRUE;
  switch(SWAP(*(word *)&message->info[0])) {
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
    fprintf(stream,"SIG-EVENT %04X", SWAP(*(word *)&message->info[0]));
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
  if(i<MT_MAX) fprintf(stream,"%s",mt[i].name);
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
