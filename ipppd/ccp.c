/*
 * ccp.c - PPP Compression Control Protocol.
 *
 * Copyright (c) 1994 The Australian National University.
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, provided that the above copyright
 * notice appears in all copies.  This software is provided without any
 * warranty, express or implied. The Australian National University
 * makes no representations about the suitability of this software for
 * any purpose.
 *
 * IN NO EVENT SHALL THE AUSTRALIAN NATIONAL UNIVERSITY BE LIABLE TO ANY
 * PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
 * ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF
 * THE AUSTRALIAN NATIONAL UNIVERSITY HAVE BEEN ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * THE AUSTRALIAN NATIONAL UNIVERSITY SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE AUSTRALIAN NATIONAL UNIVERSITY HAS NO
 * OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS,
 * OR MODIFICATIONS.
 */

#ifndef lint
static char rcsid[] = "$Id: ccp.c,v 1.1 1997/03/07 16:01:10 hipp Exp $";
#endif

#include <string.h>
#include <syslog.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <net/ppp_defs.h>
#include <linux/ppp-comp.h>

#include "fsm.h"
#include "pppd.h"
#include "ccp.h"

/*
 * Protocol entry points from main code.
 */
static void ccp_init __P((int unit));
static void ccp_open __P((int unit));
static void ccp_close __P((int unit, char *));
static void ccp_lowerup __P((int unit));
static void ccp_lowerdown __P((int));
static void ccp_input __P((int unit, u_char *pkt, int len));
static void ccp_protrej __P((int unit));
static int  ccp_printpkt __P((u_char *pkt, int len,
                  void (*printer) __P((void *, char *, ...)),
                  void *arg));
static void ccp_datainput __P((int unit, u_char *pkt, int len));

struct protent ccp_protent = {
    PPP_CCP,
    ccp_init,
    ccp_input,
    ccp_protrej,
    ccp_lowerup,
    ccp_lowerdown,
    ccp_open,
    ccp_close,
    ccp_printpkt,
    ccp_datainput,
    1,
    "CCP",
    NULL,
    NULL,
    NULL
};


fsm ccp_fsm[NUM_PPP];
ccp_options ccp_wantoptions[NUM_PPP];	/* what to request the peer to use */
ccp_options ccp_gotoptions[NUM_PPP];	/* what the peer agreed to do */
ccp_options ccp_allowoptions[NUM_PPP];	/* what we'll agree to do */
ccp_options ccp_hisoptions[NUM_PPP];	/* what we agreed to do */

/*
 * Callbacks for fsm code.
 */
static void ccp_resetci __P((fsm *));
static int  ccp_cilen __P((fsm *));
static void ccp_addci __P((fsm *, u_char *, int *));
static int  ccp_ackci __P((fsm *, u_char *, int));
static int  ccp_nakci __P((fsm *, u_char *, int));
static int  ccp_rejci __P((fsm *, u_char *, int));
static int  ccp_reqci __P((fsm *, u_char *, int *, int));
static void ccp_up __P((fsm *));
static void ccp_down __P((fsm *));
static int  ccp_extcode __P((fsm *, int, int, u_char *, int));
static void ccp_rack_timeout __P(());
static char *method_name __P((ccp_options *, ccp_options *));

static fsm_callbacks ccp_callbacks = {
    ccp_resetci,
    ccp_cilen,
    ccp_addci,
    ccp_ackci,
    ccp_nakci,
    ccp_rejci,
    ccp_reqci,
    ccp_up,
    ccp_down,
    NULL,
    NULL,
    NULL,
    NULL,
    ccp_extcode,
    "CCP"
};

/*
 * Do we want / did we get any compression?
 */
#define ANY_COMPRESS(opt)	((opt).deflate || (opt).bsd_compress \
				 || (opt).predictor_1 || (opt).predictor_2)

/*
 * Local state (mainly for handling reset-reqs and reset-acks).
 */
static int ccp_localstate[NUM_PPP];
#define RACK_PENDING	1	/* waiting for reset-ack */
#define RREQ_REPEAT	2	/* send another reset-req if no reset-ack */

#define RACKTIMEOUT	1	/* second */

static int all_rejected[NUM_PPP];

/*
 * ccp_init - initialize CCP.
 */
static void ccp_init(int unit)
{
    fsm *f = &ccp_fsm[unit];

    f->unit = -1;
    f->protocol = PPP_CCP;
    f->callbacks = &ccp_callbacks;
    fsm_init(f);

    memset(&ccp_wantoptions[unit],  0, sizeof(ccp_options));
    memset(&ccp_gotoptions[unit],   0, sizeof(ccp_options));
    memset(&ccp_allowoptions[unit], 0, sizeof(ccp_options));
    memset(&ccp_hisoptions[unit],   0, sizeof(ccp_options));

    ccp_wantoptions[0].deflate = 1;
    ccp_wantoptions[0].deflate_size = DEFLATE_MAX_SIZE;
    ccp_allowoptions[0].deflate = 1;
    ccp_allowoptions[0].deflate_size = DEFLATE_MAX_SIZE;

    ccp_wantoptions[0].bsd_compress = 1;
    ccp_wantoptions[0].bsd_bits = BSD_MAX_BITS;

    ccp_allowoptions[unit].bsd_compress = 1;
    ccp_allowoptions[unit].bsd_bits = BSD_MAX_BITS;
    ccp_allowoptions[unit].predictor_1 = 1;
}

int ccp_getunit(int linkunit)
{
  int i;
  for(i=0;i<NUM_PPP;i++)
    if(!ccp_fsm[i].inuse)
    {
      ccp_fsm[i].inuse = 1;
      ccp_fsm[i].state = INITIAL;
      ccp_fsm[i].flags = 0;
      ccp_fsm[i].id = 0;
      ccp_fsm[i].unit = linkunit;
      return i;
    }
  syslog(LOG_ERR,"No more ccp_units available");
  return -1;
}

void ccp_freeunit(int unit)
{
  ccp_fsm[unit].inuse = 0;
  ccp_fsm[unit].unit = -1; 
}


/*
 * ccp_open - CCP is allowed to come up.
 */
static void ccp_open(int unit)
{
    fsm *f = &ccp_fsm[unit];

	if (f->state != OPENED)
		ccp_flags_set(unit, 1, 0);

    /*
     * Find out which compressors the kernel supports before
     * deciding whether to open in silent mode.
     */
	ccp_resetci(f);
	if (!ANY_COMPRESS(ccp_gotoptions[unit]))
		f->flags |= OPT_SILENT;

    fsm_open(f);
}

/*
 * ccp_close - Terminate CCP.
 */
static void ccp_close(int unit,char *reason)
{
    ccp_flags_set(unit, 0, 0);
    fsm_close(&ccp_fsm[unit],reason);
}

/*
 * ccp_lowerup - we may now transmit CCP packets.
 */
static void ccp_lowerup(int unit)
{
     fsm_lowerup(&ccp_fsm[unit]);
}

/*
 * ccp_lowerdown - we may not transmit CCP packets.
 */
static void ccp_lowerdown(int unit)
{
    fsm_lowerdown(&ccp_fsm[unit]);
}

/*
 * ccp_input - process a received CCP packet.
 */
static void ccp_input(int linkunit,u_char *p,int len)
{
  int unit = lns[linkunit].ccp_unit;

    fsm *f = &ccp_fsm[unit];
    int oldstate;

    /*
     * Check for a terminate-request so we can print a message.
     */
    oldstate = f->state;
    fsm_input(f, p, len);
	if (oldstate == OPENED && p[0] == TERMREQ && f->state != OPENED)
		syslog(LOG_NOTICE, "Compression disabled by peer.");

    /*
     * If we get a terminate-ack and we're not asking for compression,
     * close CCP.
     */
	if (oldstate == REQSENT && p[0] == TERMACK && !ANY_COMPRESS(ccp_gotoptions[unit]))
		ccp_close(unit,"No compression negotiated");
}

/*
 * Handle a CCP-specific code.
 */
static int ccp_extcode(fsm *f,int code,int id,u_char *p,int len)
{
    switch (code) {
    case CCP_RESETREQ:
	if (f->state != OPENED)
	    break;
	/* send a reset-ack, which the transmitter will see and
	   reset its compression state. */
	fsm_sdata(f, CCP_RESETACK, id, NULL, 0);
	break;

    case CCP_RESETACK:
	if (ccp_localstate[lns[f->unit].ccp_unit] & RACK_PENDING && id == f->reqid) {
	    ccp_localstate[lns[f->unit].ccp_unit] &= ~(RACK_PENDING | RREQ_REPEAT);
	    UNTIMEOUT(ccp_rack_timeout, (caddr_t) f);
	}
	break;

    default:
	return 0;
    }

    return 1;
}

/*
 * ccp_protrej - peer doesn't talk CCP.
 */
static void ccp_protrej(int linkunit)
{
  int unit = lns[linkunit].ccp_unit;
  ccp_flags_set(unit, 0, 0);
  fsm_lowerdown(&ccp_fsm[unit]);
}

/*
 * ccp_resetci - initialize at start of negotiation.
 */
static void ccp_resetci(fsm *f)
{
    ccp_options *go = &ccp_gotoptions[lns[f->unit].ccp_unit];
    u_char opt_buf[16];

    *go = ccp_wantoptions[lns[f->unit].ccp_unit];
    all_rejected[lns[f->unit].ccp_unit] = 0;

    /*
     * Check whether the kernel knows about the various
     * compression methods we might request.
     */
    if (go->bsd_compress) {
	opt_buf[0] = CI_BSD_COMPRESS;
	opt_buf[1] = CILEN_BSD_COMPRESS;
	opt_buf[2] = BSD_MAKE_OPT(BSD_CURRENT_VERSION, BSD_MIN_BITS);
	if (ccp_test(lns[f->unit].ccp_unit, opt_buf, CILEN_BSD_COMPRESS, 0) <= 0)
	    go->bsd_compress = 0;
    }
    if (go->deflate) {
	opt_buf[0] = CI_DEFLATE;
	opt_buf[1] = CILEN_DEFLATE;
	opt_buf[2] = DEFLATE_MAKE_OPT(DEFLATE_MIN_SIZE);
	opt_buf[3] = DEFLATE_CHK_SEQUENCE;
	if (ccp_test(lns[f->unit].ccp_unit, opt_buf, CILEN_DEFLATE, 0) <= 0)
	    go->deflate = 0;
    }
    if (go->predictor_1) {
	opt_buf[0] = CI_PREDICTOR_1;
	opt_buf[1] = CILEN_PREDICTOR_1;
	if (ccp_test(lns[f->unit].ccp_unit, opt_buf, CILEN_PREDICTOR_1, 0) <= 0)
	    go->predictor_1 = 0;
    }
    if (go->predictor_2) {
	opt_buf[0] = CI_PREDICTOR_2;
	opt_buf[1] = CILEN_PREDICTOR_2;
	if (ccp_test(lns[f->unit].ccp_unit, opt_buf, CILEN_PREDICTOR_2, 0) <= 0)
	    go->predictor_2 = 0;
    }
}

/*
 * ccp_cilen - Return total length of our configuration info.
 */
static int ccp_cilen(fsm *f)
{
    ccp_options *go = &ccp_gotoptions[lns[f->unit].ccp_unit];

    return (go->bsd_compress? CILEN_BSD_COMPRESS: 0)
	+ (go->deflate? CILEN_DEFLATE: 0)
	+ (go->predictor_1? CILEN_PREDICTOR_1: 0)
	+ (go->predictor_2? CILEN_PREDICTOR_2: 0);
}

/*
 * ccp_addci - put our requests in a packet.
 */
static void ccp_addci(fsm *f,u_char *p,int *lenp)
{
    int res;
    ccp_options *go = &ccp_gotoptions[lns[f->unit].ccp_unit];
    u_char *p0 = p;

    /*
     * Add the compression types that we can receive, in decreasing
     * preference order.  Get the kernel to allocate the first one
     * in case it gets Acked.
     */
    if (go->deflate) {
	p[0] = CI_DEFLATE;
	p[1] = CILEN_DEFLATE;
	p[2] = DEFLATE_MAKE_OPT(go->deflate_size);
	p[3] = DEFLATE_CHK_SEQUENCE;
	for (;;) {
	    res = ccp_test(lns[f->unit].ccp_unit, p, CILEN_DEFLATE, 0);
	    if (res > 0) {
		p += CILEN_DEFLATE;
		break;
	    }
	    if (res < 0 || go->deflate_size <= DEFLATE_MIN_SIZE) {
		go->deflate = 0;
		break;
	    }
	    --go->deflate_size;
	    p[2] = DEFLATE_MAKE_OPT(go->deflate_size);
	}
    }
    if (go->bsd_compress) {
	p[0] = CI_BSD_COMPRESS;
	p[1] = CILEN_BSD_COMPRESS;
	p[2] = BSD_MAKE_OPT(BSD_CURRENT_VERSION, go->bsd_bits);
	if (p != p0) {
	    p += CILEN_BSD_COMPRESS;	/* not the first option */
	} else {
	    for (;;) {
		res = ccp_test(lns[f->unit].ccp_unit, p, CILEN_BSD_COMPRESS, 0);
		if (res > 0) {
		    p += CILEN_BSD_COMPRESS;
		    break;
		}
		if (res < 0 || go->bsd_bits <= BSD_MIN_BITS) {
		    go->bsd_compress = 0;
		    break;
		}
		--go->bsd_bits;
		p[2] = BSD_MAKE_OPT(BSD_CURRENT_VERSION, go->bsd_bits);
	    }
	}
    }
    /* XXX Should Predictor 2 be preferable to Predictor 1? */
    if (go->predictor_1) {
	p[0] = CI_PREDICTOR_1;
	p[1] = CILEN_PREDICTOR_1;
	if (p == p0 && ccp_test(lns[f->unit].ccp_unit, p, CILEN_PREDICTOR_1, 0) <= 0) {
	    go->predictor_1 = 0;
	} else {
	    p += CILEN_PREDICTOR_1;
	}
    }
    if (go->predictor_2) {
	p[0] = CI_PREDICTOR_2;
	p[1] = CILEN_PREDICTOR_2;
	if (p == p0 && ccp_test(lns[f->unit].ccp_unit, p, CILEN_PREDICTOR_2, 0) <= 0) {
	    go->predictor_2 = 0;
	} else {
	    p += CILEN_PREDICTOR_2;
	}
    }

    go->method = (p > p0)? p0[0]: -1;

    *lenp = p - p0;
}

/*
 * ccp_ackci - process a received configure-ack, and return
 * 1 iff the packet was OK.
 */
static int ccp_ackci(fsm *f,u_char *p,int len)
{
    ccp_options *go = &ccp_gotoptions[lns[f->unit].ccp_unit];
    u_char *p0 = p;
    if (go->deflate) {
	if (len < CILEN_DEFLATE
	    || p[0] != CI_DEFLATE || p[1] != CILEN_DEFLATE
	    || p[2] != DEFLATE_MAKE_OPT(go->deflate_size)
	    || p[3] != DEFLATE_CHK_SEQUENCE)
	    return 0;
	p += CILEN_DEFLATE;
	len -= CILEN_DEFLATE;
	/* XXX Cope with first/fast ack */
	if (len == 0)
	    return 1;
    }
    if (go->bsd_compress) {
	if (len < CILEN_BSD_COMPRESS
	    || p[0] != CI_BSD_COMPRESS || p[1] != CILEN_BSD_COMPRESS
	    || p[2] != BSD_MAKE_OPT(BSD_CURRENT_VERSION, go->bsd_bits))
	    return 0;
	p += CILEN_BSD_COMPRESS;
	len -= CILEN_BSD_COMPRESS;
	/* XXX Cope with first/fast ack */
	if (p == p0 && len == 0)
	    return 1;
    }
    if (go->predictor_1) {
	if (len < CILEN_PREDICTOR_1
	    || p[0] != CI_PREDICTOR_1 || p[1] != CILEN_PREDICTOR_1)
	    return 0;
	p += CILEN_PREDICTOR_1;
	len -= CILEN_PREDICTOR_1;
	/* XXX Cope with first/fast ack */
	if (p == p0 && len == 0)
	    return 1;
    }
    if (go->predictor_2) {
	if (len < CILEN_PREDICTOR_2
	    || p[0] != CI_PREDICTOR_2 || p[1] != CILEN_PREDICTOR_2)
	    return 0;
	p += CILEN_PREDICTOR_2;
	len -= CILEN_PREDICTOR_2;
	/* XXX Cope with first/fast ack */
	if (p == p0 && len == 0)
	    return 1;
    }
    if (len != 0)
	return 0;
    return 1;
}

/*
 * ccp_nakci - process received configure-nak.
 * Returns 1 iff the nak was OK.
 */
static int ccp_nakci(fsm *f,u_char *p,int len)
{
    ccp_options *go = &ccp_gotoptions[lns[f->unit].ccp_unit];
    ccp_options no;		/* options we've seen already */
    ccp_options try;		/* options to ask for next time */

    memset(&no, 0, sizeof(no));
    try = *go;

    if (go->deflate && len >= CILEN_DEFLATE
	&& p[0] == CI_DEFLATE && p[1] == CILEN_DEFLATE) {
	no.deflate = 1;
	/*
	 * Peer wants us to use a different code size or something.
	 * Stop asking for Deflate if we don't understand his suggestion.
	 */
	if (DEFLATE_METHOD(p[2]) != DEFLATE_METHOD_VAL
	    || DEFLATE_SIZE(p[2]) < DEFLATE_MIN_SIZE
	    || p[3] != DEFLATE_CHK_SEQUENCE)
	    try.deflate = 0;
	else if (DEFLATE_SIZE(p[2]) < go->deflate_size)
	    go->deflate_size = DEFLATE_SIZE(p[2]);
	p += CILEN_DEFLATE;
	len -= CILEN_DEFLATE;
    }

    if (go->bsd_compress && len >= CILEN_BSD_COMPRESS
	&& p[0] == CI_BSD_COMPRESS && p[1] == CILEN_BSD_COMPRESS) {
	no.bsd_compress = 1;
	/*
	 * Peer wants us to use a different number of bits
	 * or a different version.
	 */
	if (BSD_VERSION(p[2]) != BSD_CURRENT_VERSION)
	    try.bsd_compress = 0;
	else if (BSD_NBITS(p[2]) < go->bsd_bits)
	    try.bsd_bits = BSD_NBITS(p[2]);
	p += CILEN_BSD_COMPRESS;
	len -= CILEN_BSD_COMPRESS;
    }

    /*
     * Predictor-1 and 2 have no options, so they can't be Naked.
     *
     * XXX What should we do with any remaining options?
     */

    if (len != 0)
	return 0;

    if (f->state != OPENED)
	*go = try;
    return 1;
}

/*
 * ccp_rejci - reject some of our suggested compression methods.
 */
static int ccp_rejci(fsm *f,u_char *p,int len)
{
    ccp_options *go = &ccp_gotoptions[lns[f->unit].ccp_unit];
    ccp_options try;		/* options to request next time */

    try = *go;

    /*
     * Cope with empty configure-rejects by ceasing to send
     * configure-requests.
     */
    if (len == 0 && all_rejected[lns[f->unit].ccp_unit])
	return -1;

    if (go->deflate && len >= CILEN_DEFLATE
	&& p[0] == CI_DEFLATE && p[1] == CILEN_DEFLATE) {
	if (p[2] != DEFLATE_MAKE_OPT(go->deflate_size)
	    || p[3] != DEFLATE_CHK_SEQUENCE)
	    return 0;		/* Rej is bad */
	try.deflate = 0;
	p += CILEN_DEFLATE;
	len -= CILEN_DEFLATE;
    }
    if (go->bsd_compress && len >= CILEN_BSD_COMPRESS
	&& p[0] == CI_BSD_COMPRESS && p[1] == CILEN_BSD_COMPRESS) {
	if (p[2] != BSD_MAKE_OPT(BSD_CURRENT_VERSION, go->bsd_bits))
	    return 0;
	try.bsd_compress = 0;
	p += CILEN_BSD_COMPRESS;
	len -= CILEN_BSD_COMPRESS;
    }
    if (go->predictor_1 && len >= CILEN_PREDICTOR_1
	&& p[0] == CI_PREDICTOR_1 && p[1] == CILEN_PREDICTOR_1) {
	try.predictor_1 = 0;
	p += CILEN_PREDICTOR_1;
	len -= CILEN_PREDICTOR_1;
    }
    if (go->predictor_2 && len >= CILEN_PREDICTOR_2
	&& p[0] == CI_PREDICTOR_2 && p[1] == CILEN_PREDICTOR_2) {
	try.predictor_2 = 0;
	p += CILEN_PREDICTOR_2;
	len -= CILEN_PREDICTOR_2;
    }

    if (len != 0)
	return 0;

    if (f->state != OPENED)
	*go = try;

    return 1;
}

/*
 * ccp_reqci - processed a received configure-request.
 * Returns CONFACK, CONFNAK or CONFREJ and the packet modified
 * appropriately.
 */
static int ccp_reqci(fsm *f,u_char *p,int *lenp,int dont_nak)
{
    int ret, newret, res;
    u_char *p0, *retp;
    int len, clen, type, nb;
    ccp_options *ho = &ccp_hisoptions[lns[f->unit].ccp_unit];
    ccp_options *ao = &ccp_allowoptions[lns[f->unit].ccp_unit];

    ret = CONFACK;
    retp = p0 = p;
    len = *lenp;

    memset(ho, 0, sizeof(ccp_options));
    ho->method = (len > 0)? p[0]: -1;

    while (len > 0) {
	newret = CONFACK;
	if (len < 2 || p[1] < 2 || p[1] > len) {
	    /* length is bad */
	    clen = len;
	    newret = CONFREJ;

	} else {
	    type = p[0];
	    clen = p[1];

	    switch (type) {
	    case CI_DEFLATE:
		if (!ao->deflate || clen != CILEN_DEFLATE) {
		    newret = CONFREJ;
		    break;
		}

		ho->deflate = 1;
		ho->deflate_size = nb = DEFLATE_SIZE(p[2]);
		if (DEFLATE_METHOD(p[2]) != DEFLATE_METHOD_VAL
		    || p[3] != DEFLATE_CHK_SEQUENCE
			|| nb > ao->deflate_size || nb < DEFLATE_MIN_SIZE) {
		    newret = CONFNAK;
			if (!dont_nak) {
				p[2] = DEFLATE_MAKE_OPT(ao->deflate_size);
				p[3] = DEFLATE_CHK_SEQUENCE;
			}
			break;
		}

		/*
		 * Check whether we can do Deflate with the window
		 * size they want.  If the window is too big, reduce
		 * it until the kernel can cope and nak with that.
		 * We only check this for the first option.
		 */
		if (p == p0) {
			for (;;) {
				res = ccp_test(f->unit, p, CILEN_DEFLATE, 1);
				if (res > 0)
					break;              /* it's OK now */
				if (res < 0 || nb == DEFLATE_MIN_SIZE || dont_nak) {
					newret = CONFREJ;
					p[2] = DEFLATE_MAKE_OPT(ho->deflate_size);
					break;
				}
				newret = CONFNAK;
				--nb;
				p[2] = DEFLATE_MAKE_OPT(nb);
			}
		}
		break;

	    case CI_BSD_COMPRESS:
		if (!ao->bsd_compress || clen != CILEN_BSD_COMPRESS) {
		    newret = CONFREJ;
		    break;
		}

		ho->bsd_compress = 1;
		ho->bsd_bits = nb = BSD_NBITS(p[2]);
		if (BSD_VERSION(p[2]) != BSD_CURRENT_VERSION
			|| nb > ao->bsd_bits || nb < BSD_MIN_BITS) {
			newret = CONFNAK;
			if (!dont_nak)
				p[2] = BSD_MAKE_OPT(BSD_CURRENT_VERSION, ao->bsd_bits);
			break;
		}

		/*
		 * Check whether we can do BSD-Compress with the code
		 * size they want.  If the code size is too big, reduce
		 * it until the kernel can cope and nak with that.
		 * We only check this for the first option.
		 */
		if (p == p0) {
		    for (;;) {
				res = ccp_test(f->unit, p, CILEN_BSD_COMPRESS, 1);
				if (res > 0)
					break;
				if (res < 0 || nb == BSD_MIN_BITS || dont_nak) {
					newret = CONFREJ;
					p[2] = BSD_MAKE_OPT(BSD_CURRENT_VERSION,
							ho->bsd_bits);
					break;
				}
				newret = CONFNAK;
				--nb;
				p[2] = BSD_MAKE_OPT(BSD_CURRENT_VERSION, nb);
			}
		}
		break;

	    case CI_PREDICTOR_1:
		if (!ao->predictor_1 || clen != CILEN_PREDICTOR_1) {
		    newret = CONFREJ;
		    break;
 		}
 
		ho->predictor_1 = 1;
		if (p == p0 && ccp_test(lns[f->unit].ccp_unit, p, CILEN_PREDICTOR_1, 1) <= 0) {
		    newret = CONFREJ;
		}
		break;

	    case CI_PREDICTOR_2:
		if (!ao->predictor_2 || clen != CILEN_PREDICTOR_2) {
		    newret = CONFREJ;
		    break;
		}

		ho->predictor_2 = 1;
		if (p == p0 && ccp_test(lns[f->unit].ccp_unit, p, CILEN_PREDICTOR_2, 1) <= 0) {
		    newret = CONFREJ;
		}
		break;

	    default:
		newret = CONFREJ;
	    }
	}

	if (newret == CONFNAK && dont_nak)
	    newret = CONFREJ;
	if (!(newret == CONFACK || (newret == CONFNAK && ret == CONFREJ) )) {
	    /* we're returning this option */
	    if (newret == CONFREJ && ret == CONFNAK)
		retp = p0;
	    ret = newret;
	    if (p != retp)
		BCOPY(p, retp, clen);
	    retp += clen;
	}

	p += clen;
	len -= clen;
    }

    if (ret != CONFACK) {
	if (ret == CONFREJ && *lenp == retp - p0)
	    all_rejected[lns[f->unit].ccp_unit] = 1;
	else
	    *lenp = retp - p0;
    }
    return ret;
}

/*
 * Make a string name for a compression method (or 2).
 */
static char *method_name(ccp_options *opt,ccp_options *opt2)
{
    static char result[64];

    if (!ANY_COMPRESS(*opt))
       return "(none)";
    switch (opt->method) {
    case CI_DEFLATE:
       if (opt2 != NULL && opt2->deflate_size != opt->deflate_size)
           sprintf(result, "Deflate (%d/%d)", opt->deflate_size,
                   opt2->deflate_size);
       else
           sprintf(result, "Deflate (%d)", opt->deflate_size);
       break;
    case CI_BSD_COMPRESS:
       if (opt2 != NULL && opt2->bsd_bits != opt->bsd_bits)
           sprintf(result, "BSD-Compress (%d/%d)", opt->bsd_bits,
                   opt2->bsd_bits);
       else
           sprintf(result, "BSD-Compress (%d)", opt->bsd_bits);
       break;
    case CI_PREDICTOR_1:
       return "Predictor 1";
    case CI_PREDICTOR_2:
       return "Predictor 2";
    default:
       sprintf(result, "Method %d", opt->method);
    }
    return result;
}

/*
 * CCP has come up - inform the kernel driver and log a message.
 */
static void ccp_up(fsm *f)
{
    ccp_options *go = &ccp_gotoptions[lns[f->unit].ccp_unit];
    ccp_options *ho = &ccp_hisoptions[lns[f->unit].ccp_unit];
	char method1[64];
 
    ccp_flags_set(lns[f->unit].ccp_unit, 1, 1);
    if (ANY_COMPRESS(*go)) {
       if (ANY_COMPRESS(*ho)) {
           if (go->method == ho->method) {
               syslog(LOG_NOTICE, "%s compression enabled",
                      method_name(go, ho));
           } else {
               strcpy(method1, method_name(go, NULL));
               syslog(LOG_NOTICE, "%s / %s compression enabled",
                      method1, method_name(ho, NULL));
           }
       } else
           syslog(LOG_NOTICE, "%s receive compression enabled",
                  method_name(go, NULL));
    } else if (ANY_COMPRESS(*ho))
       syslog(LOG_NOTICE, "%s transmit compression enabled",
              method_name(ho, NULL));
}

/*
 * CCP has gone down - inform the kernel driver.
 */
static void ccp_down(fsm *f)
{
    if (ccp_localstate[lns[f->unit].ccp_unit] & RACK_PENDING)
	UNTIMEOUT(ccp_rack_timeout, (caddr_t) f);
    ccp_localstate[lns[f->unit].ccp_unit] = 0;
    ccp_flags_set(lns[f->unit].ccp_unit, 1, 0);
}

/*
 * Print the contents of a CCP packet.
 */
static char *ccp_codenames[] = {
    "ConfReq", "ConfAck", "ConfNak", "ConfRej",
    "TermReq", "TermAck", "CodeRej",
    NULL, NULL, NULL, NULL, NULL, NULL,
    "ResetReq", "ResetAck",
};

static int ccp_printpkt(u_char *p,int plen,void (*printer)(void*,char*,...),void *arg)
{
    u_char *p0, *optend;
    int code, id, len;
    int optlen;

    p0 = p;
    if (plen < HEADERLEN)
	return 0;
    code = p[0];
    id = p[1];
    len = (p[2] << 8) + p[3];
    if (len < HEADERLEN || len > plen)
	return 0;

    if (code >= 1 && code <= sizeof(ccp_codenames) / sizeof(char *)
	&& ccp_codenames[code-1] != NULL)
	printer(arg, " %s", ccp_codenames[code-1]);
    else
	printer(arg, " code=0x%x", code);
    printer(arg, " id=0x%x", id);
    len -= HEADERLEN;
    p += HEADERLEN;

    switch (code) {
    case CONFREQ:
    case CONFACK:
    case CONFNAK:
    case CONFREJ:
	/* print list of possible compression methods */
	while (len >= 2) {
	    code = p[0];
	    optlen = p[1];
	    if (optlen < 2 || optlen > len)
		break;
	    printer(arg, " <");
	    len -= optlen;
	    optend = p + optlen;
	    switch (code) {
	    case CI_DEFLATE:
		if (optlen >= CILEN_DEFLATE) {
		    printer(arg, "deflate %d", DEFLATE_SIZE(p[2]));
		    if (DEFLATE_METHOD(p[2]) != DEFLATE_METHOD_VAL)
			printer(arg, " method %d", DEFLATE_METHOD(p[2]));
		    if (p[3] != DEFLATE_CHK_SEQUENCE)
			printer(arg, " check %d", p[3]);
		    p += CILEN_DEFLATE;
		}
		break;
	    case CI_BSD_COMPRESS:
		if (optlen >= CILEN_BSD_COMPRESS) {
		    printer(arg, "bsd v%d %d", BSD_VERSION(p[2]),
			    BSD_NBITS(p[2]));
		    p += CILEN_BSD_COMPRESS;
		}
		break;
	    case CI_PREDICTOR_1:
		if (optlen >= CILEN_PREDICTOR_1) {
		    printer(arg, "predictor 1");
		    p += CILEN_PREDICTOR_1;
		}
		break;
	    case CI_PREDICTOR_2:
		if (optlen >= CILEN_PREDICTOR_2) {
		    printer(arg, "predictor 2");
		    p += CILEN_PREDICTOR_2;
		}
		break;
	    }
	    while (p < optend)
		printer(arg, " %.2x", *p++);
	    printer(arg, ">");
	}
	break;

    case TERMACK:
    case TERMREQ:
       if (len > 0 && *p >= ' ' && *p < 0x7f) {
           print_string(p, len, printer, arg);
           p += len;
           len = 0;
       }
       break;
    }

    /* dump out the rest of the packet in hex */
    while (--len >= 0)
	printer(arg, " %.2x", *p++);

    return p - p0;
}

/*
 * We have received a packet that the decompressor failed to
 * decompress.  Here we would expect to issue a reset-request, but
 * Motorola has a patent on resetting the compressor as a result of
 * detecting an error in the decompressed data after decompression.
 * (See US patent 5,130,993; international patent publication number
 * WO 91/10289; Australian patent 73296/91.)
 *
 * So we ask the kernel whether the error was detected after
 * decompression; if it was, we take CCP down, thus disabling
 * compression :-(, otherwise we issue the reset-request.
 */
static void ccp_datainput(int linkunit,u_char *pkt,int len)
{
  int unit = lns[linkunit].ccp_unit;
    fsm *f;

    f = &ccp_fsm[unit];
    if (f->state == OPENED) {
	if (ccp_fatal_error(unit)) {
	    /*
	     * Disable compression by taking CCP down.
	     */
	    syslog(LOG_ERR, "Lost compression sync: disabling compression");
	    ccp_close(unit,"Lost compression sync");
	} else {
	    /*
	     * Send a reset-request to reset the peer's compressor.
	     * We don't do that if we are still waiting for an
	     * acknowledgement to a previous reset-request.
	     */
	    if (!(ccp_localstate[unit] & RACK_PENDING)) {
		fsm_sdata(f, CCP_RESETREQ, f->reqid = ++f->id, NULL, 0);
		TIMEOUT(ccp_rack_timeout, (caddr_t) f, RACKTIMEOUT);
		ccp_localstate[unit] |= RACK_PENDING;
	    } else
		ccp_localstate[unit] |= RREQ_REPEAT;
	}
    }
}

/*
 * Timeout waiting for reset-ack.
 */
static void ccp_rack_timeout(caddr_t arg)
{
    fsm *f = (fsm *) arg;

    if (f->state == OPENED && ccp_localstate[lns[f->unit].ccp_unit] & RREQ_REPEAT) {
	fsm_sdata(f, CCP_RESETREQ, f->reqid, NULL, 0);
	TIMEOUT(ccp_rack_timeout, (caddr_t) f, RACKTIMEOUT);
	ccp_localstate[lns[f->unit].ccp_unit] &= ~RREQ_REPEAT;
    } else
	ccp_localstate[lns[f->unit].ccp_unit] &= ~RACK_PENDING;
}
