/*
 * capiconnect.c - pppd plugin to implement a `minconnect' option.
 *
 * Copyright 2000 Carsten Paeth (calle@calle.de)
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version
 *  2 of the License, or (at your option) any later version.
 */
#include <stddef.h>
#include <time.h>
#include "pppd.h"
#include "capiconn.h"
#include <malloc.h>
#include <string.h>
#include <dlfcn.h>

static capiconn_context *ctx;
static capi_connection *conn = 0;
static int isconnected = 0;
static unsigned applid;

static int opt_contr = 1;
static char *opt_numberprefix = 0;
static char *opt_number = 0;
static char *opt_msn = 0;
static char *opt_proto = 0;

static option_t my_options[] = {
	{
		"controller", o_int, &opt_contr,
		"capi controller"
	},
	{
		"number", o_string, &opt_number,
		"number to call"
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
		"protocol x75 or hdlc"
        },
	{ NULL }
};

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

static void dodisconnect(void)
{
	if (!conn)
		return;
	(void)capiconn_disconnect(conn, 0);
	while (conn) {
		unsigned char *msg = 0;
		struct timeval tv;
		tv.tv_sec = 1;
		tv.tv_usec = 0;
		if (capi20_waitformessage(applid, &tv) == 0) {
			if (capi20_get_message (applid, &msg) == 0)
				capiconn_inject(applid, msg);
		}
	}
}

static void makeconnection(void)
{
	char number[256];

	if (opt_number == 0) {
		fatal("capiplugin: no number");
		return;
	}

	snprintf(number, sizeof(number), "%s%s", 
			opt_numberprefix ? opt_numberprefix : "",
			opt_number);
	if (opt_proto == 0 || strcasecmp(opt_proto, "hdlc") == 0) {
		conn = capiconn_connect(ctx,
				opt_contr, /* contr */
				2, /* cipvalue */
				number, 
				opt_msn,
				0, 1, 0,
				0, 0, 0, 0, 0);
	} else if (strcasecmp(opt_proto, "x75") == 0) {
		conn = capiconn_connect(ctx,
				opt_contr, /* contr */
				2, /* cipvalue */
				number, 
				opt_msn,
				0, 0, 0,
				0, 0, 0, 0, 0);
	} else if (strcasecmp(opt_proto, "modem") == 0) {
		conn = capiconn_connect(ctx,
				opt_contr, /* contr */
				1, /* cipvalue */
				number, 
				opt_msn,
				8, 1, 0,
				0, 0, 0, 0, 0);
	} else {
		fatal("capiplugin: unknown protocol \"%s\"", opt_proto);
		return;
	}
	while (!isconnected && conn) {
		unsigned char *msg = 0;
		struct timeval tv;
		tv.tv_sec = 1;
		tv.tv_usec = 0;
		if (capi20_waitformessage(applid, &tv) == 0) {
			if (capi20_get_message (applid, &msg) == 0)
				capiconn_inject(applid, msg);
		}
	}
	if (!conn)
		fatal("capiplugin: couldn't make connection");
}

static void init_capiconn(void)
{
	static capi_contrinfo cinfo = { 0 , 0, 0 };
	static int init = 0;

	if (init)
		return;
	init = 1;

	if (capiconn_addcontr(ctx, opt_contr, &cinfo) != CAPICONN_OK) {
		(void)capiconn_freecontext(ctx);
		(void)capi20_release(applid);
		fatal("capiplugin: add controller %d failed", opt_contr);
		return;
	}

	add_fd(capi20_fileno(applid));
	setup_timeout();
}

static int capi_new_phase_hook(int phase)
{
	int fd;
	switch (phase) {
		case PHASE_DEAD:
			info("capiplugin: phase dead");
			dodisconnect();
			break;
		case PHASE_INITIALIZE:
			info("capiplugin: phase initialize");
			break;
		case PHASE_SERIALCONN:
			info("capiplugin: phase serialconn");
			init_capiconn();
			makeconnection();
			break;
		case PHASE_DORMANT:
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
			if ((fd = capi20_fileno(applid)) >= 0)
			   remove_fd(fd);
			unsetup_timeout();
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

	snprintf(buf, sizeof(buf),
		"plci=0x%x ncci=0x%x %s",
			cp->plci,
			cp->ncci,
			cp->isincoming ? "incoming" : "outgoing"
			);
	return buf;
}

static void disconnected(capi_connection *cp,
				int localdisconnect,
				unsigned reason,
				unsigned reason_b3)
{
	info("capiplugin: disconnected(%s): %s: 0x%04x (0x%04x) - %s", 
			conninfo(cp),
			localdisconnect ? "local" : "remote",
			reason, reason_b3, capi_info2str(reason));
	conn = 0;
	isconnected = 0;
}

static void connected(capi_connection *cp, _cstruct NCPI)
{
	capi_conninfo *p = capiconn_getinfo(cp);
	char buf[PATH_MAX];
	char *tty;

	tty = capi20ext_get_tty_devname(p->appid, p->ncci, buf, sizeof(buf));
	info("capiplugin: connected(%s) %s", conninfo(cp), tty);
	strcpy(devnam, tty);
	isconnected = 1;
}

void put_message(unsigned appid, unsigned char *msg)
{
	unsigned err;
	err = capi20_put_message (appid, msg);
	if (err)
		fatal("capiplugin: putmessage(appid=%u) = 0x%x", appid, err);
}

/* -------------------------------------------------------------------- */

capiconn_callbacks callbacks = {
	malloc: malloc,
	free: free,

	disconnected: disconnected,
	incoming: 0,
	connected: connected,
	received: 0, 
	datasent: 0, 
	chargeinfo: 0,

	capi_put_message: put_message,

	debugmsg: dbglog,
	infomsg: info,
	errmsg: error
};

void plugin_init(void)
{
	int err;

	info("plugin_init: capiconnect");

	add_options(my_options);

	if ((err = capi20_register (30, 8, 2048, &applid)) != 0) {
		fatal("capiplugin: CAPI_REGISTER failed - 0x%04x", err);
		return;
        }
	if (capi20ext_set_flags(applid, 1) < 0) {
		(void)capi20_release(applid);
		fatal("capiplugin: failed to set highjacking mode");
		return;
	}
		
	if ((ctx = capiconn_getcontext(applid, &callbacks)) == 0) {
		(void)capi20_release(applid);
		fatal("capiplugin: get_context failed");
		return;
	}

        new_phase_hook = capi_new_phase_hook;
}
