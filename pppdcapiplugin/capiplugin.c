/*
 * capiconnect.c - pppd plugin to make ISDN connections over CAPI2.0
 *
 * Copyright 2000 Carsten Paeth (calle@calle.in-berlin.de)
 * Copyright 2000 AVM GmbH Berlin (info@avm.de)
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version
 *  2 of the License, or (at your option) any later version.
 */
#include <unistd.h>
#include <stddef.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include "pppd.h"
#include "capiconn.h"
#include <malloc.h>
#include <string.h>
#include <dlfcn.h>
#include <errno.h>

static char *revision = "$Revision: 1.12 $";

static capiconn_context *ctx;
static capi_connection *conn = 0;
static int isconnected = 0;
static unsigned applid;
#define CM(x)	(1<<(x))
#define	CIPMASK_ALL	0x1FFF03FF
#define	CIPMASK_VOICE	(CM(1)|CM(4)|CM(5)|CM(16)|CM(26))
#define	CIPMASK_DATA	(CM(2)|CM(3))
static unsigned long cipmask = CIPMASK_ALL;
static int controller = 1;
static capi_contrinfo cinfo = { 0 , 0, 0 };

static char *opt_controller = "1";
static char *opt_numberprefix = 0;
static char *opt_number = 0;
static char *opt_callbacknumber = 0;
static char *opt_msn = 0;
static char *opt_inmsn = 0;
static char *opt_proto = 0;
static char *opt_channels = 0;
static unsigned char AdditionalInfo[1+2+2+31];
static int opt_dialtimeout = 60;
static int opt_dialmax = 4;
static int opt_redialdelay = 5;
static int opt_cbdelay = 2;
static int opt_connectdelay = 0;

static char *opt_cli = 0;
static int opt_cbflag  = 0;
static int opt_cbwait = 60;
static int opt_acceptdelayflag  = 0;

typedef struct stringlist {
    struct stringlist *next;
	char              *s;
} STRINGLIST;

static STRINGLIST *numbers;
static STRINGLIST *callbacknumbers;
static STRINGLIST *clis;
static STRINGLIST *parsed_controller;
static STRINGLIST *inmsns;

static int optcb(void)
{
	return opt_cbflag = 1;
}
static int optacceptdelay(void)
{
	return opt_acceptdelayflag = 1;
}

static option_t my_options[] = {
	{
		"controller", o_string, &opt_controller,
		"capi controller specification"
	},
	{
		"number", o_string, &opt_number,
		"number to call (may be comma separated)"
        },
	{
		"numberprefix", o_string, &opt_numberprefix,
		"prefix for number"
        },
	{
		"msn", o_string, &opt_msn,
		"number to call from"
        },
	{
		"protocol", o_string, &opt_proto,
		"protocol x75, hdlc or modem"
        },
	{
		"inmsn", o_string, &opt_inmsn,
		"called number for incoming calls"
        },
	{
		"cli", o_string, &opt_cli,
		"calling number for incoming calls (may be comma separated list)"
        },
	{
		"clicb", o_special_noarg, &optcb,
		"call number and wait for callback"
	},
	{
		"cbwait", o_int, &opt_cbwait,
		"number of seconds to wait for callback"
	},
	{
		"dialtimeout", o_int, &opt_dialtimeout,
		"number of seconds to wait for connection or reject"
	},
	{
		"dialmax", o_int, &opt_dialmax,
		"number of dial retries"
	},
	{
		"redialdelay", o_int, &opt_redialdelay,
		"number of seconds to wait between dial retries"
	},
	{
		"channels", o_string, &opt_channels,
		"channel to use for leased line (may be comma separated list)"
        },

	{
		"cbdelay", o_int, &opt_cbdelay,
		"number of seconds to wait before calling back"
	},
	{
		"cbnumber", o_string, &opt_callbacknumber,
		"number to call (may be comma separated)"
        },
	{
		"connectdelay", o_int, &opt_connectdelay,
		"number of seconds to wait after connection is established"
	},
	{
		"acceptdelay", o_special_noarg, &optacceptdelay,
		"wait 1 second before accept incoming call"
	},
	{ NULL }
};

/* -------------------------------------------------------------------- */

static void stringlist_free(STRINGLIST **pp)
{
	STRINGLIST *p, *next;

	p = *pp;
	while (p) {
		next = p->next;
  		if (p->s) free(p->s);
  		free(p);
  		p = next; 
	}
	*pp = 0;
}

static int stringlist_append_string(STRINGLIST **pp, char *s)
{
	STRINGLIST *p;
	for (; *pp; pp = &(*pp)->next) ;
	if ((p = (STRINGLIST *)malloc(sizeof(STRINGLIST))) == 0)
		return -1;
	memset(p, 0, sizeof(STRINGLIST));
	if ((p->s = strdup(s)) == 0) {
		free(p);
		return -1;
	}
	p->next = 0;
	*pp = p;
	return 0;
}

static STRINGLIST *stringlist_split(char *tosplit, char *seps)
{
	STRINGLIST *p = 0;
	char *str = strdup(tosplit);
	char *s;
	if (!str) return 0;
	for (s = strtok(str, seps); s; s = strtok(0, seps)) {
		if (*s == 0) continue; /* if strtok is not working correkt */
		if (stringlist_append_string(&p, s) < 0) {
 			stringlist_free(&p);
 			free(str);
 			return 0;
		}
	}
	free(str);
	return p;
}

/* -------------------------------------------------------------------- */

static int timeoutrunning = 0;
static int timeoutshouldrun = 0;

static void timeoutfunc(void *arg)
{
	unsigned char *msg = 0;
	/* info("capiplugin: checking for capi messages"); */
	while (capi20_get_message (applid, &msg) == 0)
		capiconn_inject(applid, msg);
	if (timeoutshouldrun)
		timeout (timeoutfunc, 0, 1);
}

static void setup_timeout(void)
{
	timeoutshouldrun = 1;
	if (!timeoutrunning)
		timeout (timeoutfunc, 0, 1);
}

static void unsetup_timeout(void)
{
	timeoutshouldrun = 0;
	if (timeoutrunning)
		untimeout (timeoutfunc, 0);
	timeoutrunning = 0;
}

/* -------------------------------------------------------------------- */

static void handlemessages(void) 
{
	unsigned char *msg = 0;
	struct timeval tv;
	tv.tv_sec  = 1;
	tv.tv_usec = 0;
	if (capi20_waitformessage(applid, &tv) == 0) {
		if (capi20_get_message(applid, &msg) == 0)
			capiconn_inject(applid, msg);
	}
}

/* -------------------------------------------------------------------- */

static void dodisconnect(void)
{
	time_t t;
	if (!conn)
		return;
	(void)capiconn_disconnect(conn, 0);
	t = time(0)+10;
        while (conn && time(0) < t)
	    handlemessages();
	if (conn)
		fatal("capiplugin: timeout while waiting for disconnect");
}

static void setupconnection(char *num)
{
	char number[256];

	snprintf(number, sizeof(number), "%s%s", 
			opt_numberprefix ? opt_numberprefix : "", num);

	if (opt_proto == 0 || strcasecmp(opt_proto, "hdlc") == 0) {
		conn = capiconn_connect(ctx,
				controller, /* contr */
				2, /* cipvalue */
				opt_channels ? 0 : number, 
				opt_channels ? 0 : opt_msn,
				0, 1, 0,
				0, 0, 0,
				opt_channels ? AdditionalInfo : 0,
				0);
	} else if (strcasecmp(opt_proto, "x75") == 0) {
		conn = capiconn_connect(ctx,
				controller, /* contr */
				2, /* cipvalue */
				opt_channels ? 0 : number, 
				opt_channels ? 0 : opt_msn,
				0, 0, 0,
				0, 0, 0,
				opt_channels ? AdditionalInfo : 0,
				0);
	} else if (strcasecmp(opt_proto, "v42bis") == 0) {
		conn = capiconn_connect(ctx,
				controller, /* contr */
				2, /* cipvalue */
				opt_channels ? 0 : number, 
				opt_channels ? 0 : opt_msn,
				0, 8, 0,
				0, 0, 0,
				opt_channels ? AdditionalInfo : 0,
				0);
	} else if (strcasecmp(opt_proto, "modem") == 0) {
		conn = capiconn_connect(ctx,
				controller, /* contr */
				1, /* cipvalue */
				opt_channels ? 0 : number, 
				opt_channels ? 0 : opt_msn,
				8, 1, 0,
				0, 0, 0,
				opt_channels ? AdditionalInfo : 0,
				0);
	} else {
		fatal("capiplugin: unknown protocol \"%s\"", opt_proto);
		return;
	}
	if (opt_channels) {
		info("capiplugin: leased line (%s)",
				opt_proto ? opt_proto : "hdlc");
	} else {
		info("capiplugin: dial %s (%s)",
				number, opt_proto ? opt_proto : "hdlc");
	}
}

static void makeleasedline(void)
{
	time_t t;

	setupconnection("");

	t = time(0)+opt_dialtimeout;
	do {
		handlemessages();
		if (status != EXIT_OK && conn)
 			dodisconnect();
	} while (time(0) < t && conn && !isconnected);

	if (status != EXIT_OK)
		die(status);

        if (conn && isconnected) {
		t = time(0)+opt_connectdelay;
		do {
			handlemessages();
		} while (time(0) < t);
	}

	if (status != EXIT_OK)
		die(status);

	if (!conn) 
	   fatal("capiplugin: couldn't make connection");
}

static void makeconnection(STRINGLIST *numbers)
{
	time_t t;
	STRINGLIST *p;
	int retry = 0;

	do {
	   for (p = numbers; p; p = p->next) {
		   if (retry || p != numbers) {
		      t = time(0)+opt_redialdelay;
		      do {
			 handlemessages();
			 if (status != EXIT_OK)
			    die(status);
		      } while (time(0) < t);
		   }

		   setupconnection(p->s);

		   t = time(0)+opt_dialtimeout;
		   do {
		      handlemessages();
		      if (status != EXIT_OK && conn)
			 dodisconnect();
		   } while (time(0) < t && conn && !isconnected);

		   if (conn && isconnected)
		      goto connected;

		   if (status != EXIT_OK)
		      die(status);
	   }
	} while (++retry < opt_dialmax);
connected:

        if (!conn)
	   fatal("capiplugin: couldn't make connection after %d retries",
			retry);
}

static void makeconnection_with_callback(void)
{
	STRINGLIST *p;
	time_t t;

   	for (p = numbers; p; p = p->next) {

		setupconnection(p->s);

		info("capiplugin: wait for call reject");
		/* Wait specific time for the server rejecting the call */
		t = time(0)+opt_dialtimeout;
		do {
		      handlemessages();
		      if (status != EXIT_OK)
			 die(status);
		} while (time(0) < t && conn && !isconnected);

		if (!conn) {	/* Call has been rejected */
		
			(void) capiconn_listen(ctx, controller, cipmask, 0);
			info("capiplugin: waiting for callback...");

			/* Wait for server calling back */
			t = time(0)+opt_cbwait;
			do {
				handlemessages();
				if (status != EXIT_OK) {
				   (void) capiconn_listen(ctx, controller, 0, 0);
				   die(status);
				}
			} while (!isconnected && time(0) < t);

			if (isconnected) {
				add_fd(capi20_fileno(applid));
				setup_timeout();
				return;
			}
			if (p->next == 0)
				fatal("capiplugin: callback failed (no call)");
			else
				info("capiplugin: callback failed (no call)");
		} else {
			dodisconnect();
			fatal("capiplugin: callback failed - other side answers the call (no reject)");
		}
	}
}

static void makecallback(void)
{
	time_t t;

	t = time(0)+opt_cbdelay;
	do {
	      handlemessages();
	      if (status != EXIT_OK)
		 die(status);
	} while (time(0) < t);

	makeconnection(callbacknumbers);
}

static void waitforcall(void)
{
	(void) capiconn_listen(ctx, controller, cipmask, 0);
	info("capiplugin: waiting for incoming call ...");

	do {
		handlemessages();
		if (status != EXIT_OK) {
		   (void) capiconn_listen(ctx, controller, 0, 0);
		   die(status);
		}
	} while (!isconnected);

	add_fd(capi20_fileno(applid));
	setup_timeout();
}

static void init_capiconn(void)
{
	static int init = 0;

	if (init)
		return;
	init = 1;

	if (capiconn_addcontr(ctx, controller, &cinfo) != CAPICONN_OK) {
		(void)capiconn_freecontext(ctx);
		(void)capi20_release(applid);
		fatal("capiplugin: add controller %d failed", controller);
		return;
	}
	if (cinfo.ddi) 
		dbglog("capiplugin: contr=%d ddi=\"%s\" n=%d",
			controller, cinfo.ddi, cinfo.ndigits);
	else
		dbglog("capiplugin: contr=%d", controller);

	(void) capiconn_listen(ctx, controller, 0, 0);
	handlemessages();
	add_fd(capi20_fileno(applid));
	setup_timeout();
}

/*
 * Handle leased lines (CAPI-Bundling)
 */

static int decodechannels(char *teln, unsigned long *bmaskp, int *activep)
{
	unsigned long bmask = 0;
	int active = !0;
	char *s;
	int channel;
	int i;

	s = teln;
	while (*s && *s == ' ') s++;
	if (!*s)
		fatal("capiplugin; option channels: list empty");
	if (*s == 'p' || *s == 'P') {
		active = 0;
		s++;
	}
	if (*s == 'a' || *s == 'A') {
		active = !0;
		s++;
	}
	while (*s) {
		int digit1 = 0;
		int digit2 = 0;
		if (!isdigit(*s))
			goto illegal;
		while (isdigit(*s)) { digit1 = digit1*10 + (*s - '0'); s++; }
		channel = digit1;
		if (channel <= 0 && channel > 30)
			goto rangeerror;
		if (*s == 0 || *s == ',' || *s == ' ') {
			bmask |= (1 << digit1);
			digit1 = 0;
			if (*s) s++;
			continue;
		}
		if (*s != '-') 
			goto illegal;
		s++;
		if (!isdigit(*s)) return -3;
		while (isdigit(*s)) { digit2 = digit2*10 + (*s - '0'); s++; }
		channel = digit2;
		if (channel <= 0 && channel > 30)
			goto rangeerror;
		if (*s == 0 || *s == ',' || *s == ' ') {
			if (digit1 > digit2)
				for (i = digit2; i <= digit1 ; i++)
					bmask |= (1 << i);
			else 
				for (i = digit1; i <= digit2 ; i++)
					bmask |= (1 << i);
			digit1 = digit2 = 0;
			if (*s) s++;
			continue;
		}
		goto illegal;
	}
	if (activep) *activep = active;
	if (bmaskp) *bmaskp = bmask;
	return 0;
illegal:
	fatal("capiplugin: option channels: illegal octet '%c'", *s);
	return -1;
rangeerror:
	fatal("capiplugin: option channels: channel %d out of range", channel);
	return -1;
}

static int channels2capi20(char *teln, unsigned char *AdditionalInfo)
{
	unsigned long bmask;
	int active;
	int i;
   
	decodechannels(teln, &bmask, &active);
	/* info("capiplugin: \"%s\" 0x%lx %d\n", teln, bmask, active); */
	/* Length */
	AdditionalInfo[0] = 2+2+31;
        /* Channel: 3 => use channel allocation */
        AdditionalInfo[1] = 3; AdditionalInfo[2] = 0;
	/* Operation: 0 => DTE mode, 1 => DCE mode */
        if (active) {
   		AdditionalInfo[3] = 0; AdditionalInfo[4] = 0;
   	} else {
   		AdditionalInfo[3] = 1; AdditionalInfo[4] = 0;
	}
	/* Channel mask array */
	AdditionalInfo[5] = 0; /* no D-Channel */
	for (i=1; i <= 30; i++)
		AdditionalInfo[5+i] = (bmask & (1 << i)) ? 0xff : 0;
	return 0;
}

static void check_options(void)
{
	static int init = 0;

	if (init)
		return;
	init = 1;

	if (   opt_proto
	    && strcasecmp(opt_proto, "hdlc")
	    && strcasecmp(opt_proto, "x75")
	    && strcasecmp(opt_proto, "v42bis")
	    && strcasecmp(opt_proto, "modem")) {
		option_error("capiplugin: unknown protocol \"%s\"", opt_proto);
		die(1);
	}
        if (strcasecmp(opt_proto, "modem") == 0)
		cipmask = CIPMASK_VOICE;
	else cipmask = CIPMASK_DATA;

	if (opt_channels) {
		channels2capi20(opt_channels, AdditionalInfo);
		if (opt_number)
			option_error("capiplugin: option number ignored");
		if (opt_numberprefix)
			option_error("capiplugin: option numberprefix ignored");
		if (opt_callbacknumber)
			option_error("capiplugin: option callbacknumber ignored");
		if (opt_msn)
			option_error("capiplugin: option msn ignored");
		if (opt_inmsn)
			option_error("capiplugin: option inmsn ignored");
	} else if (opt_number) {
		stringlist_free(&numbers);
		numbers = stringlist_split(opt_number, " \t,");
		if (opt_callbacknumber)
			option_error("capiplugin: option callbacknumber ignored");
	} else if (opt_cbflag) {
		if (opt_callbacknumber == 0) {
			option_error("capiplugin: option callbacknumber missing");
			die(1);
		}
		stringlist_free(&callbacknumbers);
		callbacknumbers = stringlist_split(opt_callbacknumber, " \t,");
	} else {
		if (opt_callbacknumber) {
			opt_cbflag = 1;
			stringlist_free(&callbacknumbers);
			callbacknumbers = stringlist_split(opt_callbacknumber, " \t,");
		}
	}
	if (opt_cli) {
		stringlist_free(&clis);
		clis = stringlist_split(opt_cli, " \t,");
	}
	if (opt_inmsn) {
		stringlist_free(&inmsns);
		inmsns = stringlist_split(opt_inmsn, " \t,");
	}
	if (opt_controller) {
		STRINGLIST *sl;
		char *tmp;
		stringlist_free(&parsed_controller);
		memset(&cinfo, 0, sizeof(cinfo));
	   	parsed_controller = stringlist_split(opt_controller, " \t,");
		sl = parsed_controller;
		if (!sl) goto illcontr;
		tmp = sl->s;
		controller = strtol(sl->s, &tmp, 10);
		if (tmp == sl->s || *tmp) goto illcontr;
		if (sl->next) {
			sl = sl->next;
			cinfo.ddi = sl->s;
			if (sl->next && sl->next->s) {
			        sl = sl->next;
				cinfo.ndigits = strtol(sl->s, &tmp, 10);
				if (tmp == sl->s || *tmp) goto illcontr;
			}
		}
	} else {
		memset(&cinfo, 0, sizeof(cinfo));
		controller = 1;
	}
	return;

illcontr:
	option_error("capiplugin: illegal controller specification \"%s\"",
				opt_controller);
	die(1);
}

static int capi_new_phase_hook(int phase)
{
	int fd;
	switch (phase) {
		case PHASE_DEAD:
			info("capiplugin: phase dead");
			if ((fd = capi20_fileno(applid)) >= 0)
			   remove_fd(fd);
			unsetup_timeout();
			dodisconnect();
			break;
		case PHASE_INITIALIZE:
			info("capiplugin: phase initialize");
			break;
		case PHASE_SERIALCONN:
			info("capiplugin: phase serialconn%s",
					opt_cbflag ? " (callback)" : "");
			check_options();
			init_capiconn();
			if (opt_number) {
				if (opt_cbflag) {
					makeconnection_with_callback();
				} else {
					makeconnection(numbers);
				}
			} else if (opt_channels) {
				makeleasedline();
			} else {
				waitforcall();
			}
			break;
		case PHASE_DORMANT:
			check_options();
			init_capiconn();
			info("capiplugin: phase dormant");
			break;
		case PHASE_ESTABLISH:
			info("capiplugin: phase establish");
			break;
		case PHASE_AUTHENTICATE:
			info("capiplugin: phase authenticate");
			break;
		case PHASE_CALLBACK:
			info("capiplugin: phase callback");
			break;
		case PHASE_NETWORK:
			info("capiplugin: phase network");
			break;
		case PHASE_RUNNING:
			info("capiplugin: phase running");
			break;
		case PHASE_TERMINATE:
			info("capiplugin: phase terminate");
			break;
		case PHASE_DISCONNECT:
			info("capiplugin: phase disconnect");
			break;
		case PHASE_HOLDOFF:
			info("capiplugin: phase holdoff");
			break;
	}
	return 0;
}

/* -------------------------------------------------------------------- */

static char *conninfo(capi_connection *p)
{
	static char buf[1024];
	capi_conninfo *cp = capiconn_getinfo(p);
	char *callingnumber = "";
	char *callednumber = "";

	if (cp->callingnumber && cp->callingnumber[0] > 2)
		callingnumber = cp->callingnumber+3;
	if (cp->callednumber && cp->callednumber[0] > 1)
		callednumber = cp->callednumber+2;

	if (debug) {
		snprintf(buf, sizeof(buf),
			"\"%s\" -> \"%s\" %s (pcli=0x%x/ncci=0x%x)",
				callingnumber, callednumber,
				cp->isincoming ? "incoming" : "outgoing",
				cp->plci, cp->ncci
				);
	} else {
		snprintf(buf, sizeof(buf),
			"\"%s\" -> \"%s\" %s",
				callingnumber, callednumber,
				cp->isincoming ? "incoming" : "outgoing");
	}
	buf[sizeof(buf)-1] = 0;
	return buf;
}

static void disconnected(capi_connection *cp,
				int localdisconnect,
				unsigned reason,
				unsigned reason_b3)
{
	if (reason != 0x3304 || debug) /* Another Applikation got the call */
		info("capiplugin: disconnect(%s): %s 0x%04x (0x%04x) - %s", 
			localdisconnect ? "local" : "remote",
			conninfo(cp),
			reason, reason_b3, capi_info2str(reason));
	conn = 0;
	isconnected = 0;
}

static void incoming(capi_connection *cp,
				unsigned contr,
				unsigned cipvalue,
				char *callednumber,
				char *callingnumber)
{
	STRINGLIST *p;
        char *s;

	info("capiplugin: incoming call: %s (0x%x)", conninfo(cp), cipvalue);
	
	if (opt_inmsn) {
	   for (p = inmsns; p; p = p->next) {
	      if (   (s = strstr(callednumber, p->s)) != 0
                  && strcmp(s, p->s) == 0)
		 break;
	   }
	   if (!p) {
	           info("capiplugin: ignoring call, msn %s not in \"%s\"",
				callednumber, opt_inmsn);
		   (void) capiconn_ignore(cp);
		   return;
           }
        } else if (opt_msn) {
	   if (   (s = strstr(callednumber, opt_msn)) == 0
               || strcmp(s, opt_msn) != 0) {
	           info("capiplugin: ignoring call, msn mismatch (%s != %s)",
				opt_msn, callednumber);
		   (void) capiconn_ignore(cp);
		   return;
           }
        }

	if (opt_cli) {
	   for (p = clis; p; p = p->next) {
	       if (   (s = strstr(callingnumber, p->s)) != 0
                   && strcmp(s, p->s) == 0)
		   break;
	   }
	   if (!p) {
	           info("capiplugin: ignoring call, cli mismatch (%s != %s)",
				opt_cli, callingnumber);
		   (void) capiconn_ignore(cp);
		   return;
	   }
        } else if (opt_number) {
	   for (p = numbers; p; p = p->next) {
	       if (   (s = strstr(callingnumber, p->s)) != 0
                   && strcmp(s, p->s) == 0)
		   break;
           }
	   if (!p) {
	           info("capiplugin: ignoring call, number mismatch (%s != %s)",
				opt_number, callingnumber);
		   (void) capiconn_ignore(cp);
		   return;
	   }
        } else if (opt_acceptdelayflag) {
		/*
		* non cli or number match,
		* give more specific listen a chance (bad)
		*/
		info("capiplugin: accept delayed, no cli or number match");
		sleep(1);
	}

	switch (cipvalue) {
		case 1:	 /* Speech */
		case 4:  /* 3.1 kHz audio */
		case 5:  /* 7 kHz audio */
		case 16: /* Telephony */
		case 26: /* 7 kHz telephony */
	                if (opt_proto && strcasecmp(opt_proto, "modem") == 0) {
			   if (opt_cbflag) goto callback;
		           (void) capiconn_accept(cp, 8, 1, 0, 0, 0, 0, 0);
			   goto accepted;
                        } else {
	                   info("capiplugin: ignoring speech call from %s",
			   		callingnumber);
			   (void) capiconn_ignore(cp);
			}
			break;
		case 2:  /* Unrestricted digital information */
		case 3:  /* Restricted digital information */
	                if (opt_proto == 0
			    || strcasecmp(opt_proto, "hdlc") == 0) {
			   if (opt_cbflag) goto callback;
			   (void) capiconn_accept(cp, 0, 1, 0, 0, 0, 0, 0);
			   goto accepted;
			} else if (strcasecmp(opt_proto, "x75") == 0) {
			   if (opt_cbflag) goto callback;
			   (void) capiconn_accept(cp, 0, 0, 0, 0, 0, 0, 0);
			   goto accepted;
			} else if (strcasecmp(opt_proto, "v42bis") == 0) {
			   if (opt_cbflag) goto callback;
			   (void) capiconn_accept(cp, 0, 8, 0, 0, 0, 0, 0);
			   goto accepted;
			} else {
	                   info("capiplugin: ignoring digital call from %s",
			   		callingnumber);
			   (void) capiconn_ignore(cp);
			}
			break;
		case 17: /* Group 2/3 facsimile */
		        info("capiplugin: ignoring fax call from %s",
			   		callingnumber);
			(void) capiconn_ignore(cp);
			break;
		default:
		        info("capiplugin: ignoring type %d call from %s",
			   		cipvalue, callingnumber);
			(void) capiconn_ignore(cp);
			break;
	}
	return;
accepted:
	(void) capiconn_listen(ctx, controller, 0, 0);
	return;
callback:
	(void) capiconn_listen(ctx, controller, 0, 0);
	info("capiplugin: rejecting call: %s (0x%x)", conninfo(cp), cipvalue);
	capiconn_reject(cp);
        makecallback();
	return;
}

static void connected(capi_connection *cp, _cstruct NCPI)
{
	capi_conninfo *p = capiconn_getinfo(cp);
	char *callingnumber = "";
	char *callednumber = "";
	char buf[PATH_MAX];
	char *tty;
	int retry = 0;
	int serrno;

        info("capiplugin: connected: %s", conninfo(cp));

        tty = capi20ext_get_tty_devname(p->appid, p->ncci, buf, sizeof(buf));
        serrno = errno;
	while (tty == 0 && serrno == ESRCH) {
	   if (++retry > 4) 
	      break;
	   info("capiplugin: capitty not ready, waiting for driver ...");
	   sleep(1);
	   tty = capi20ext_get_tty_devname(p->appid, p->ncci, buf, sizeof(buf));
	   serrno = errno;
	}
	if (tty == 0) {
	   if (serrno == EINVAL) {
	      fatal("capiplugin: failed to get tty devname - CAPI Middleware Support not enabled in kernel ?");
	   }
	   fatal("capiplugin: failed to get tty devname - %s (%d)",
			strerror(serrno), serrno);
	}
	if (access(tty, 0) != 0 && errno == ENOENT) {
	      fatal("capiplugin: tty %s doesn't exist - CAPI Filesystem Support not enabled in kernel or not mounted ?", tty);
	}
	info("capiplugin: using %s: %s", tty, conninfo(cp));
	strcpy(devnam, tty);
	if (opt_connectdelay)
		sleep(opt_connectdelay);

	if (p->callingnumber && p->callingnumber[0] > 2)
		callingnumber = p->callingnumber+3;
	if (p->callednumber && p->callednumber[0] > 1)
		callednumber = p->callednumber+2;
        script_setenv("CALLEDNUMBER", callednumber, 0);
        script_setenv("CALLINGNUMBER", callingnumber, 0);
	sprintf(buf, "%d", p->cipvalue); script_setenv("CIPVALUE", buf, 0);
	sprintf(buf, "%d", p->b1proto); script_setenv("B1PROTOCOL", buf, 0);
	sprintf(buf, "%d", p->b2proto); script_setenv("B2PROTOCOL", buf, 0);
	sprintf(buf, "%d", p->b3proto); script_setenv("B3PROTOCOL", buf, 0);

	isconnected = 1;
}

void chargeinfo(capi_connection *cp, unsigned long charge, int inunits)
{
	if (inunits) {
        	info("capiplugin: %s: charge in units: %d", conninfo(cp), charge);
	} else {
        	info("capiplugin: %s: charge in currency: %d", conninfo(cp), charge);
	}
}

void put_message(unsigned appid, unsigned char *msg)
{
	unsigned err;
	err = capi20_put_message (appid, msg);
	if (err)
		fatal("capiplugin: putmessage(appid=%d) = 0x%x", appid, err);
}

/* -------------------------------------------------------------------- */

capiconn_callbacks callbacks = {
	malloc: malloc,
	free: free,

	disconnected: disconnected,
	incoming: incoming,
	connected: connected,
	received: 0, 
	datasent: 0, 
	chargeinfo: chargeinfo,

	capi_put_message: put_message,

	debugmsg: dbglog,
	infomsg: info,
	errmsg: error
};

void plugin_init(void)
{
	int serrno;
	int err;

	info("capiplugin: %s", revision);
	info("capiconn: %s", capiconn_version());

	add_options(my_options);

	if ((err = capi20_register (30, 8, 2048, &applid)) != 0) {
		serrno = errno;
		fatal("capiplugin: CAPI_REGISTER failed - %s (0x%04x) [%s (%d)]",
				capi_info2str(err), err,
				strerror(serrno), errno);
		return;
        }
	if (capi20ext_set_flags(applid, 1) < 0) {
		serrno = errno;
		(void)capi20_release(applid);
		fatal("capiplugin: failed to set highjacking mode - %s (%d)",
				strerror(serrno), serrno);
		return;
	}
		
	if ((ctx = capiconn_getcontext(applid, &callbacks)) == 0) {
		(void)capi20_release(applid);
		fatal("capiplugin: get_context failed");
		return;
	}

        new_phase_hook = capi_new_phase_hook;
}
