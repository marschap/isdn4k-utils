/*
 * $Id: convert.c,v 1.4 1998/10/23 12:20:44 fritz Exp $
 *
 * $Log: convert.c,v $
 * Revision 1.4  1998/10/23 12:20:44  fritz
 * Added some missing functions.
 *
 * Revision 1.3  1998/08/30 09:57:21  calle
 * I hope it is know readable for everybody.
 *
 * Revision 1.1  1998/08/25 16:33:23  calle
 * Added CAPI2.0 library. First Version.
 *
 */
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <time.h>
#include <ctype.h>

#include "capi20.h"

typedef struct {
    int typ;
    unsigned off;
} _cdef;

#define _CBYTE	       1
#define _CWORD	       2
#define _CDWORD        3
#define _CSTRUCT       4
#define _CMSTRUCT      5
#define _CEND	       6

static _cdef cdef[] = {
    /*00*/{_CEND},
    /*01*/{_CEND},
    /*02*/{_CEND},
    /*03*/{_CDWORD,     offsetof(_cmsg,adr.adrController)	},
    /*04*/{_CMSTRUCT,   offsetof(_cmsg,AdditionalInfo)	},
    /*05*/{_CSTRUCT,    offsetof(_cmsg,B1configuration)	},
    /*06*/{_CWORD,      offsetof(_cmsg,B1protocol)	},
    /*07*/{_CSTRUCT,    offsetof(_cmsg,B2configuration)	},
    /*08*/{_CWORD,      offsetof(_cmsg,B2protocol)	},
    /*09*/{_CSTRUCT,    offsetof(_cmsg,B3configuration)	},
    /*0a*/{_CWORD,      offsetof(_cmsg,B3protocol)	},
    /*0b*/{_CSTRUCT,    offsetof(_cmsg,BC)	},
    /*0c*/{_CSTRUCT,    offsetof(_cmsg,BChannelinformation)	},
    /*0d*/{_CMSTRUCT,   offsetof(_cmsg,BProtocol)	},
    /*0e*/{_CSTRUCT,    offsetof(_cmsg,CalledPartyNumber)	},
    /*0f*/{_CSTRUCT,    offsetof(_cmsg,CalledPartySubaddress)	},
    /*10*/{_CSTRUCT,    offsetof(_cmsg,CallingPartyNumber)	},
    /*11*/{_CSTRUCT,    offsetof(_cmsg,CallingPartySubaddress)	},
    /*12*/{_CDWORD,     offsetof(_cmsg,CIPmask)	},
    /*13*/{_CDWORD,     offsetof(_cmsg,CIPmask2)	},
    /*14*/{_CWORD,      offsetof(_cmsg,CIPValue)	},
    /*15*/{_CDWORD,     offsetof(_cmsg,Class)	},
    /*16*/{_CSTRUCT,    offsetof(_cmsg,ConnectedNumber)	},
    /*17*/{_CSTRUCT,    offsetof(_cmsg,ConnectedSubaddress)	},
    /*18*/{_CDWORD,     offsetof(_cmsg,Data)	},
    /*19*/{_CWORD,      offsetof(_cmsg,DataHandle)	},
    /*1a*/{_CWORD,      offsetof(_cmsg,DataLength)	},
    /*1b*/{_CSTRUCT,    offsetof(_cmsg,FacilityConfirmationParameter)	},
    /*1c*/{_CSTRUCT,    offsetof(_cmsg,Facilitydataarray)	},
    /*1d*/{_CSTRUCT,    offsetof(_cmsg,FacilityIndicationParameter)	},
    /*1e*/{_CSTRUCT,    offsetof(_cmsg,FacilityRequestParameter)	},
    /*1f*/{_CSTRUCT,    offsetof(_cmsg,FacilityResponseParameters)	},
    /*20*/{_CWORD,      offsetof(_cmsg,FacilitySelector)	},
    /*21*/{_CWORD,      offsetof(_cmsg,Flags)	},
    /*22*/{_CDWORD,     offsetof(_cmsg,Function)	},
    /*23*/{_CSTRUCT,    offsetof(_cmsg,HLC)	},
    /*24*/{_CWORD,      offsetof(_cmsg,Info)	},
    /*25*/{_CSTRUCT,    offsetof(_cmsg,InfoElement)	},
    /*26*/{_CDWORD,     offsetof(_cmsg,InfoMask)	},
    /*27*/{_CWORD,      offsetof(_cmsg,InfoNumber)	},
    /*28*/{_CSTRUCT,    offsetof(_cmsg,Keypadfacility)	},
    /*29*/{_CSTRUCT,    offsetof(_cmsg,LLC)	},
    /*2a*/{_CSTRUCT,    offsetof(_cmsg,ManuData)	},
    /*2b*/{_CDWORD,     offsetof(_cmsg,ManuID)	},
    /*2c*/{_CSTRUCT,    offsetof(_cmsg,NCPI)	},
    /*2d*/{_CWORD,      offsetof(_cmsg,Reason)	},
    /*2e*/{_CWORD,      offsetof(_cmsg,Reason_B3)	},
    /*2f*/{_CWORD,      offsetof(_cmsg,Reject)	},
    /*30*/{_CSTRUCT,    offsetof(_cmsg,Useruserdata)	},
};

static unsigned char *cpars[] = {
    /*00*/ 0,
    /*01 ALERT_REQ*/
           (unsigned char*)"\x03\x04\x0c\x28\x30\x1c\x01\x01",
    /*02 CONNECT_REQ*/
          (unsigned char*)"\x03\x14\x0e\x10\x0f\x11\x0d\x06\x08\x0a\x05\x07\x09\x01\x0b\x29\x23\x04\x0c\x28\x30\x1c\x01\x01",
    /*03*/ 0,
    /*04 DISCONNECT_REQ*/
          (unsigned char*)"\x03\x04\x0c\x28\x30\x1c\x01\x01",
    /*05 LISTEN_REQ*/
          (unsigned char*)"\x03\x26\x12\x13\x10\x11\x01",
    /*06*/ 0,
    /*07*/ 0,
    /*08 INFO_REQ*/
          (unsigned char*)"\x03\x0e\x04\x0c\x28\x30\x1c\x01\x01",
    /*09 FACILITY_REQ*/
          (unsigned char*)"\x03\x20\x1e\x01",
    /*0a SELECT_B_PROTOCOL_REQ*/
          (unsigned char*)"\x03\x0d\x06\x08\x0a\x05\x07\x09\x01\x01",
    /*0b CONNECT_B3_REQ*/ 
          (unsigned char*)"\x03\x2c\x01",
    /*0c*/ 0,
    /*0d DISCONNECT_B3_REQ*/
          (unsigned char*)"\x03\x2c\x01",
    /*0e*/ 0,
    /*0f DATA_B3_REQ*/
          (unsigned char*)"\x03\x18\x1a\x19\x21\x01",
    /*10 RESET_B3_REQ*/
          (unsigned char*)"\x03\x2c\x01",
    /*11*/ 0,
    /*12*/ 0,
    /*13 ALERT_CONF*/
          (unsigned char*)"\x03\x24\x01",
    /*14 CONNECT_CONF*/
          (unsigned char*)"\x03\x24\x01",
    /*15*/ 0,
    /*16 DISCONNECT_CONF*/
          (unsigned char*)"\x03\x24\x01",
    /*17 LISTEN_CONF*/
          (unsigned char*)"\x03\x24\x01",
    /*18 MANUFACTURER_REQ*/
          (unsigned char*)"\x03\x2b\x15\x22\x2a\x01",
    /*19*/ 0,
    /*1a INFO_CONF*/
          (unsigned char*)"\x03\x24\x01",
    /*1b FACILITY_CONF*/
          (unsigned char*)"\x03\x24\x20\x1b\x01",
    /*1c SELECT_B_PROTOCOL_CONF*/
          (unsigned char*)"\x03\x24\x01",
    /*1d CONNECT_B3_CONF*/
          (unsigned char*)"\x03\x24\x01",
    /*1e*/ 0,
    /*1f DISCONNECT_B3_CONF*/
          (unsigned char*)"\x03\x24\x01",
    /*20*/ 0,
    /*21 DATA_B3_CONF*/
          (unsigned char*)"\x03\x19\x24\x01",
    /*22 RESET_B3_CONF*/
          (unsigned char*)"\x03\x24\x01",
    /*23*/ 0,
    /*24*/ 0,
    /*25*/ 0,
    /*26 CONNECT_IND*/
          (unsigned char*)"\x03\x14\x0e\x10\x0f\x11\x0b\x29\x23\x04\x0c\x28\x30\x1c\x01\x01",
    /*27 CONNECT_ACTIVE_IND*/
          (unsigned char*)"\x03\x16\x17\x29\x01",
    /*28 DISCONNECT_IND*/
          (unsigned char*)"\x03\x2d\x01",
    /*29*/ 0,
    /*2a MANUFACTURER_CONF*/ 
          (unsigned char*)"\x03\x2b\x15\x22\x2a\x01",
    /*2b*/ 0,
    /*2c INFO_IND*/  
          (unsigned char*)"\x03\x27\x25\x01",
    /*2d FACILITY_IND*/ 
          (unsigned char*)"\x03\x20\x1d\x01",
    /*2e*/ 0,
    /*2f CONNECT_B3_IND*/
          (unsigned char*)"\x03\x2c\x01",
    /*30 CONNECT_B3_ACTIVE_IND*/
          (unsigned char*)"\x03\x2c\x01",
    /*31 DISCONNECT_B3_IND*/
          (unsigned char*)"\x03\x2e\x2c\x01",
    /*32*/ 0,
    /*33 DATA_B3_IND*/
          (unsigned char*)"\x03\x18\x1a\x19\x21\x01",
    /*34 RESET_B3_IND*/ 
          (unsigned char*)"\x03\x2c\x01",
    /*35 CONNECT_B3_T90_ACTIVE_IND*/
          (unsigned char*)"\x03\x2c\x01",
    /*36*/ 0,
    /*37*/ 0,
    /*38 CONNECT_RESP*/
          (unsigned char*)"\x03\x2f\x0d\x06\x08\x0a\x05\x07\x09\x01\x16\x17\x29\x04\x0c\x28\x30\x1c\x01\x01",
    /*39 CONNECT_ACTIVE_RESP*/
          (unsigned char*)"\x03\x01",
    /*3a DISCONNECT_RESP*/
          (unsigned char*)"\x03\x01",
    /*3b*/ 0,
    /*3c MANUFACTURER_IND*/
          (unsigned char*)"\x03\x2b\x15\x22\x2a\x01",
    /*3d*/ 0,
    /*3e INFO_RESP*/ 
          (unsigned char*)"\x03\x01",
    /*3f FACILITY_RESP*/
          (unsigned char*)"\x03\x20\x1f\x01",
    /*40*/ 0,
    /*41 CONNECT_B3_RESP*/
          (unsigned char*)"\x03\x2f\x2c\x01",
    /*42 CONNECT_B3_ACTIVE_RESP*/
          (unsigned char*)"\x03\x01",
    /*43 DISCONNECT_B3_RESP*/
          (unsigned char*)"\x03\x01",
    /*44*/ 0,
    /*45 DATA_B3_RESP*/
          (unsigned char*)"\x03\x19\x01",
    /*46 RESET_B3_RESP*/
          (unsigned char*)"\x03\x01",
    /*47 CONNECT_B3_T90_ACTIVE_RESP*/
          (unsigned char*)"\x03\x01",
    /*48*/ 0,
    /*49*/ 0,
    /*4a*/ 0,
    /*4b*/ 0,
    /*4c*/ 0,
    /*4d*/ 0,
    /*4e MANUFACTURER_RESP*/
          (unsigned char*)"\x03\x2b\x15\x22\x2a\x01",
};

/*-------------------------------------------------------*/

#define byteTLcpy(x,y)        *(_cbyte *)(x)=*(_cbyte *)(y);
#define wordTLcpy(x,y)        *(_cword *)(x)=*(_cword *)(y);
#define dwordTLcpy(x,y)       memcpy(x,y,4);
#define structTLcpy(x,y,l)    memcpy (x,y,l)
#define structTLcpyovl(x,y,l) memmove (x,y,l)

#define byteTRcpy(x,y)        *(_cbyte *)(y)=*(_cbyte *)(x);
#define wordTRcpy(x,y)        *(_cword *)(y)=*(_cword *)(x);
#define dwordTRcpy(x,y)       memcpy(y,x,4);
#define structTRcpy(x,y,l)    memcpy (y,x,l)
#define structTRcpyovl(x,y,l) memmove (y,x,l)

/*-------------------------------------------------------*/
static unsigned command_2_index (unsigned c, unsigned sc) {
    if (c & 0x80) c = 0x9+(c&0x0f);
    else if (c<=0x0f) ;
    else if (c==0x41) c = 0x9+0x1;
    else if (c==0xff) c = 0x00;
    return (sc&3)*(0x9+0x9)+c;
}

/*-------------------------------------------------------*/
#define TYP (cdef[cmsg->par[cmsg->p]].typ)
#define OFF (((char *)cmsg)+cdef[cmsg->par[cmsg->p]].off)

static void jumpcstruct (_cmsg *cmsg) {
    unsigned layer;
    for (cmsg->p++,layer=1; layer;) {
	assert (cmsg->p);
	cmsg->p++;
	switch (TYP) {
	    case _CMSTRUCT:
		layer++;
		break;
	    case _CEND:
		layer--;
		break;
	}
    }
}
/*-------------------------------------------------------*/
static void PARS_2_MESSAGE (_cmsg *cmsg) {

    for (;TYP != _CEND; cmsg->p++) {
	switch (TYP) {
	    case _CBYTE:
		byteTLcpy (cmsg->m+cmsg->l, OFF);
		cmsg->l++;
		break;
	    case _CWORD:
		wordTLcpy (cmsg->m+cmsg->l, OFF);
		cmsg->l+=2;
		break;
	    case _CDWORD:
		dwordTLcpy (cmsg->m+cmsg->l, OFF);
		cmsg->l+=4;
		break;
	    case _CSTRUCT:
		if (*(CAPI_MESSAGE *) OFF == 0) {
		    *(cmsg->m+cmsg->l)='\0';
		    cmsg->l++;
		}
		else if(**(_cstruct*)OFF != 0xff) {
		    structTLcpy (cmsg->m+cmsg->l, *(_cstruct*)OFF, 1+**(_cstruct*)OFF);
		    cmsg->l+=1+**(_cstruct*)OFF;
		}
		else {
		    _cstruct s = *(_cstruct*)OFF;
		    structTLcpy (cmsg->m+cmsg->l, s, 3+*(_cword*)(s+1));
		    cmsg->l+= 3+*(_cword*)(s+1);
		}
		break;
	    case _CMSTRUCT:
		/*----- Metastruktur 0 -----*/
		if (*(_cmstruct*)OFF == CAPI_DEFAULT) {
		    *(cmsg->m+cmsg->l)='\0';
		    cmsg->l++;
		    jumpcstruct (cmsg);
		}
		/*----- Metastruktur wird composed -----*/
		else {
		    unsigned _l = cmsg->l;
		    unsigned _ls;
		    cmsg->l++;
		    cmsg->p++;
		    PARS_2_MESSAGE (cmsg);
		    _ls = cmsg->l-_l-1;
		    if (_ls < 255)
			(cmsg->m+_l)[0] = (_cbyte)_ls;
		    else {
			structTLcpyovl (cmsg->m+_l+3, cmsg->m+_l+1, _ls);
			(cmsg->m+_l)[0] = 0xff;
			wordTLcpy (cmsg->m+_l+1, &_ls);
                        cmsg->l+=2;
		    }
		}
		break;
	}
    }
}

/*-------------------------------------------------------*/
unsigned capi_cmsg2message (_cmsg *cmsg, CAPI_MESSAGE msg)
{
    cmsg->m = msg;
    cmsg->l = 8;
    cmsg->p = 0;
    cmsg->par = cpars [command_2_index (cmsg->Command,cmsg->Subcommand)];

    PARS_2_MESSAGE (cmsg);

    wordTLcpy (msg+0, &cmsg->l);
    byteTLcpy (cmsg->m+4, &cmsg->Command);
    byteTLcpy (cmsg->m+5, &cmsg->Subcommand);
    wordTLcpy (cmsg->m+2, &cmsg->ApplId);
    wordTLcpy (cmsg->m+6, &cmsg->Messagenumber);

    return 0;
}

/*-------------------------------------------------------*/
static void MESSAGE_2_PARS (_cmsg *cmsg) {
    for (;TYP != _CEND; cmsg->p++) {

	switch (TYP) {
	    case _CBYTE:
		byteTRcpy (cmsg->m+cmsg->l, OFF);
		cmsg->l++;
		break;
	    case _CWORD:
		wordTRcpy (cmsg->m+cmsg->l, OFF);
		cmsg->l+=2;
		break;
	    case _CDWORD:
		dwordTRcpy (cmsg->m+cmsg->l, OFF);
		cmsg->l+=4;
		break;
	    case _CSTRUCT:
		*(CAPI_MESSAGE *)OFF = cmsg->m+cmsg->l;

		if (cmsg->m[cmsg->l] != 0xff)
		    cmsg->l+= 1+ cmsg->m[cmsg->l];
		else
		    cmsg->l+= 3+ *(_cword *)(cmsg->m+cmsg->l+1);
		break;
	    case _CMSTRUCT:
		/*----- Metastruktur 0 -----*/
		if (cmsg->m[cmsg->l] == '\0') {
		    *(_cmstruct*)OFF = CAPI_DEFAULT;
		    cmsg->l++;
		    jumpcstruct (cmsg);
		}
		else {
		    unsigned _l = cmsg->l;
		    *(_cmstruct*)OFF = CAPI_COMPOSE;
		    cmsg->l = (cmsg->m+_l)[0] == 255 ? cmsg->l+3 : cmsg->l+1;
		    cmsg->p++;
		    MESSAGE_2_PARS (cmsg);
		}
		break;
	}
    }
}

/*-------------------------------------------------------*/
unsigned capi_message2cmsg (_cmsg *cmsg, CAPI_MESSAGE msg)
{
    memset (cmsg, 0, sizeof(_cmsg));
    cmsg->m = msg;
    cmsg->l = 8;
    cmsg->p = 0;
    byteTRcpy (cmsg->m+4, &cmsg->Command);
    byteTRcpy (cmsg->m+5, &cmsg->Subcommand);
    cmsg->par = cpars [command_2_index (cmsg->Command,cmsg->Subcommand)];

    MESSAGE_2_PARS (cmsg);

    wordTRcpy (msg+0, &cmsg->l);
    wordTRcpy (cmsg->m+2, &cmsg->ApplId);
    wordTRcpy (cmsg->m+6, &cmsg->Messagenumber);

    return 0;
}

/*-------------------------------------------------------*/
unsigned capi_cmsg_answer (_cmsg *cmsg)
{
    cmsg->Subcommand |= 0x01;
    return 0;
}

/*-------------------------------------------------------*/
unsigned capi_cmsg_header (_cmsg *cmsg, _cword _ApplId,
				 _cbyte _Command, _cbyte _Subcommand,
				 _cword _Messagenumber, _cdword _Controller) {
    memset (cmsg, 0, sizeof(_cmsg));
    cmsg->ApplId = _ApplId ;
    cmsg->Command = _Command ;
    cmsg->Subcommand = _Subcommand ;
    cmsg->Messagenumber = _Messagenumber;
    cmsg->adr.adrController = _Controller ;
    return 0;
}

/*-------------------------------------------------------*/
unsigned capi_put_cmsg (_cmsg *cmsg)
{
    static unsigned char msg[2048];

    capi_cmsg2message(cmsg, (CAPI_MESSAGE)msg);
    return capi20_put_message((CAPI_MESSAGE)msg, cmsg->ApplId);
}

/*-------------------------------------------------------*/
unsigned capi_get_cmsg (_cmsg *cmsg, unsigned applid)
{
    MESSAGE_EXCHANGE_ERROR rtn;
    CAPI_MESSAGE msg;

    rtn = capi20_get_message(applid, &msg);
    if (rtn == 0x0000)
        capi_message2cmsg(cmsg, msg);
    return rtn;
}
