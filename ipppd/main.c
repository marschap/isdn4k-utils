/*
 * main.c - Point-to-Point Protocol main module
 *
 * Fairly patched version for isdn4linux
 * copyright (c) 1995,1996,1997 of all patches by Michael Hipp
 * still no warranties (see disclaimer)
 *
 * Copyright (c) 1989 Carnegie Mellon University.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by Carnegie Mellon University.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

/*
 * PATCHLEVEL 8
 */

#if 0
#ifndef lint
static char rcsid[] = "$Id: main.c,v 1.1 1997/03/07 16:01:29 hipp Exp $";
#endif
#endif

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <syslog.h>
#include <netdb.h>
#include <utmp.h>
#include <pwd.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <net/if.h>

#include "fsm.h"
#include "pppd.h"
#include "magic.h"
#include "lcp.h"
#include "ipcp.h"
#include "ipxcp.h"

#include "upap.h"
#include "chap.h"
#include "cbcp.h"
#include "ccp.h"
#include "pathnames.h"
#include "patchlevel.h"
#include "protos.h"

/*
 * If REQ_SYSOPTIONS is defined to 1, pppd will not run unless
 * /etc/ppp/ioptions exists.
 */
#ifndef	REQ_SYSOPTIONS
#define REQ_SYSOPTIONS	1
#endif

/* interface vars */

extern void make_options_global(int slot);
static char pidfilename[MAXPATHLEN];	/* name of pid file */
char *progname;			/* Name of this program */
char hostname[MAXNAMELEN];	/* Our hostname */
static pid_t	pid;		/* Our pid */

static uid_t uid;		/* Our real user-id */

struct link_struct lns[NUM_PPP];
int numdev = 0;

int kill_link = 0;

u_char outpacket_buf[PPP_MRU+PPP_HDRLEN]; /* buffer for outgoing packet */
static u_char inpacket_buf[PPP_MRU+PPP_HDRLEN]; /* buffer for incoming packet */

static int n_children;		/* # child processes still running */
int baud_rate;
char *no_ppp_msg = "Sorry - this system lacks PPP kernel support.\n"
	"Check whether you configured at least the ippp0 device!\n";

/* prototypes */
static void hup __P((int));
static void term __P((int));
static void chld __P((int));
static void toggle_debug __P((int));
static void open_ccp __P((int));
static void bad_signal __P((int));
static void get_input __P((int));
static int init_unit(int);
static int exit_unit(int);

void remote_sys_options __P((void));

extern	char	*ttyname __P((int));
extern	char	*getlogin __P((void));
extern int daemon(int,int);

#ifdef ultrix
#undef	O_NONBLOCK
#define	O_NONBLOCK	O_NDELAY
#endif

/*
 * PPP Data Link Layer "protocol" table.
 * One entry per supported protocol.
 * The last entry must be NULL.
 */
struct protent *protocols[] = {
    &lcp_protent,
    &pap_protent,
    &chap_protent,
    &cbcp_protent,
    &ipcp_protent,
    &ccp_protent,
    &ipxcp_protent,
    NULL
};

void main(int argc,char **argv)
{
    int i,j;
    struct sigaction sa;
#if 0
    struct cmd *cmdp;
#endif
    FILE *pidfile;
    struct timeval timo;
    sigset_t mask;
	struct protent *protp;

    for(i=0;i<NUM_PPP;i++)
    {
      lns[i].openfails = 0;
      lns[i].initfdflags = -1;
      lns[i].hungup = 1;
      lns[i].bundle_next = &lns[i];
      lns[i].ifname[0] = 0;
      lns[i].ifunit = -1;
      lns[i].open_ccp_flag = 0;
      lns[i].phase = PHASE_WAIT;
      lns[i].fd = -1;
      lns[i].logged_in = 0;
      lns[i].lcp_unit = lns[i].ipcp_unit = lns[i].ccp_unit = -1;
      lns[i].cbcp_unit = -1;
      lns[i].ipxcp_unit = -1;
      lns[i].unit = i;
      lns[i].chap_unit = lns[i].upap_unit = -1;
      lns[i].auth_up_script = 0;
    }

    pid = getpid();

    /* write pid to file */
    (void) sprintf(pidfilename, "%s%s.pid", _PATH_VARRUN, "ipppd" );
    if ((pidfile = fopen(pidfilename, "w")) != NULL) {
	fprintf(pidfile, "%d\n", pid);
	(void) fclose(pidfile);
     } else {
	syslog(LOG_ERR, "Failed to create pid file %s: %m", pidfilename);
	pidfilename[0] = 0;
    }

    if (gethostname(hostname, MAXNAMELEN) < 0 ) {
	perror("couldn't get hostname");
	die(1);
    }
    hostname[MAXNAMELEN-1] = 0;

    uid = getuid();

    /*
     * Initialize to the standard option set, then parse, in order,
     * the system options file, the user's options file, and the command
     * line arguments.
     */
    progname = *argv;

	for (i = 0; (protp = protocols[i]) != NULL; ++i)
		for(j=0;j<NUM_PPP;j++)
			(*protp->init)(j); /* modifies our options .. !!!! */

    if (!options_from_file(_PATH_SYSOPTIONS, REQ_SYSOPTIONS, 0 , 0) ||
#if 0
	!options_from_user() ||
#endif
	!parse_args(argc-1, argv+1) ||
	!options_for_tty() )
	die(1);

    /*
     * copy protocol options for unit 0 to all option fields
     */
    make_options_global(0);

    if (!ppp_available()) {
	fprintf(stderr, no_ppp_msg);
	exit(1);
    }

    remove_sys_options();
    check_auth_options();
    setipdefault();

    sys_init(); /* init log stuff and open socket for ioctl commands */

    if(!numdev)
    {
      fprintf(stderr,"ipppd: No devices found.\n");
      die(1);
    }
    else
    {
      char devstr[1024];
      sprintf(devstr,"Found %d devices: ",numdev);
      for(i=0;i<numdev;i++)
      {
        strcat(devstr,lns[i].devnam);
        strcat(devstr,", ");
      }
      syslog(LOG_NOTICE,devstr);
    }

    /*
     * Initialize magic number package.
     */
    magic_init();

    /*
     * Detach ourselves from the terminal, if required,
     * and identify who is running us.
     */
    if (!nodetach && daemon(0, 0) < 0) {
	perror("Couldn't detach from controlling terminal");
	exit(1);
    }

    syslog(LOG_NOTICE, "ipppd %s.%d (isdn4linux version of pppd by MH) started", VERSION, PATCHLEVEL);
  
    /*
     * Compute mask of all interesting signals and install signal handlers
     * for each.  Only one signal handler may be active at a time.  Therefore,
     * all other signals should be masked when any handler is executing.
     */
    sigemptyset(&mask);
    sigaddset(&mask, SIGHUP);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGTERM);
    sigaddset(&mask, SIGCHLD);

#define SIGNAL(s, handler)	{ \
	sa.sa_handler = handler; \
	if (sigaction(s, &sa, NULL) < 0) { \
	    syslog(LOG_ERR, "Couldn't establish signal handler (%d): %m", s); \
	    die(1); \
	} \
    }

    sa.sa_mask = mask;
    sa.sa_flags = 0;
    SIGNAL(SIGHUP, hup);		/* Hangup */
    SIGNAL(SIGINT, term);		/* Interrupt */
    SIGNAL(SIGTERM, term);		/* Terminate */
    SIGNAL(SIGCHLD, chld);

    SIGNAL(SIGUSR1, toggle_debug);	/* Toggle debug flag */
    SIGNAL(SIGUSR2, open_ccp);		/* Reopen CCP */

    /*
     * Install a handler for other signals which would otherwise
     * cause pppd to exit without cleaning up.
     */
    SIGNAL(SIGABRT, bad_signal);
    SIGNAL(SIGALRM, bad_signal);
    SIGNAL(SIGFPE, bad_signal);
    SIGNAL(SIGILL, bad_signal);
    SIGNAL(SIGPIPE, bad_signal);
    SIGNAL(SIGQUIT, bad_signal);
    SIGNAL(SIGSEGV, bad_signal);
#ifdef SIGBUS
    SIGNAL(SIGBUS, bad_signal);
#endif
#ifdef SIGEMT
    SIGNAL(SIGEMT, bad_signal);
#endif
#ifdef SIGPOLL
    SIGNAL(SIGPOLL, bad_signal);
#endif
#ifdef SIGPROF
    SIGNAL(SIGPROF, bad_signal);
#endif
#ifdef SIGSYS
    SIGNAL(SIGSYS, bad_signal);
#endif
#ifdef SIGTRAP
    SIGNAL(SIGTRAP, bad_signal);
#endif
#ifdef SIGVTALRM
    SIGNAL(SIGVTALRM, bad_signal);
#endif
#ifdef SIGXCPU
    SIGNAL(SIGXCPU, bad_signal);
#endif
#ifdef SIGXFSZ
    SIGNAL(SIGXFSZ, bad_signal);
#endif

        for(i=0;i<numdev;i++)
        {
          if(lns[i].fd == -1)
          {
            syslog(LOG_NOTICE,"init_unit: %d\n",i);
            if(!init_unit(i)) /* start this unit */
            {
              lns[i].hungup = 1;
              lns[i].lcp_unit = lcp_getunit(i);
/* give the same unit number to chap,upap */
              lns[i].upap_unit = lns[i].chap_unit = lns[i].lcp_unit; 
              upap[lns[i].upap_unit].us_unit = chap[lns[i].chap_unit].unit = i; 
              lcp_lowerup(lns[i].lcp_unit);
            }
          }
        }

        for(;!kill_link;)
        {
          fd_set ready;
          int n,i,max=-1;
    
          FD_ZERO(&ready);
          for(i=0;i<numdev;i++)
            if(lns[i].fd >= 0)
            {
              if(lns[i].fd > max)
                max = lns[i].fd;
              FD_SET(lns[i].fd, &ready);
            }

          n = select(max+1, &ready, NULL, NULL, timeleft(&timo) );
          if (n < 0 && errno != EINTR)
          {
	    syslog(LOG_ERR, "select: %m");
	    die(1);
          }

          calltimeout();
          
          for(i=0;i<numdev && n>0;i++)
          {
            if(lns[i].fd >= 0 && FD_ISSET(lns[i].fd,&ready))
            {
            /*  n--; */
              if(lns[i].phase == PHASE_WAIT)
              {
                /* ok, now we (usually) have a unit */
                lns[i].hungup = 0;
                establish_ppp(i);
                syslog(LOG_NOTICE,"PHASE_WAIT -> PHASE_ESTABLISHED, ifunit: %d, linkunit: %d, fd: %d",lns[i].ifunit,i,lns[i].fd);
                lcp_open(lns[i].lcp_unit);
                lns[i].phase = PHASE_ESTABLISH;
              }
              get_input(i);
            }
            if (lns[i].open_ccp_flag) /* ugly: set by SIGUSR2 signal for all units */
            {
              if (lns[i].phase == PHASE_NETWORK) 
              {
                ccp_fsm[lns[i].ccp_unit].flags = OPT_RESTART; /* clears OPT_SILENT */
                (*ccp_protent.open)(lns[i].ccp_unit);
              }
	      lns[i].open_ccp_flag = 0;
	    }
          }
	  reap_kids();	/* Don't leave dead kids lying around */
          for(i=0;i<numdev;i++)
            if(kill_link || lns[i].phase == PHASE_DEAD)
            {
			  if(!kill_link)
                syslog(LOG_NOTICE,"taking down PHASE_DEAD link %d, linkunit: %d",i,lns[i].unit);
              lcp_close(lns[i].lcp_unit,"link closed");
              lcp_lowerdown(lns[i].lcp_unit);
              lcp_freeunit(lns[i].lcp_unit);
              if(debug)
			    syslog(LOG_NOTICE,"LCP is down");
              exit_unit(i);
              syslog(LOG_NOTICE,"link %d closed , linkunit: %d",i,lns[i].unit);
              lns[i].phase = PHASE_WAIT;
            }
          if(!kill_link) {
            for(i=0;i<numdev;i++)
              if(lns[i].fd == -1)
              {
                syslog(LOG_NOTICE,"reinit_unit: %d\n",i);
                if(!init_unit(i)) /* protokolle hier neu initialisieren?? */
                {
                  lns[i].lcp_unit = lcp_getunit(i);
                  upap[i].us_unit = chap[i].unit = lns[i].lcp_unit; /* test */
                  lcp_lowerup(lns[i].lcp_unit);
                }
              }
          }
        }


  if (unlink(pidfilename) < 0 && errno != ENOENT) 
    syslog(LOG_WARNING, "unable to delete pid file: %m");
  pidfilename[0] = 0;
  die(0);
}

/*
 * initialize unit
 */

static int init_unit(int the_unit)
{
  if ((lns[the_unit].fd = open(lns[the_unit].devnam, O_NONBLOCK | O_RDWR, 0)) < 0) {
    if(errno == ENOENT)
    {
      syslog(LOG_ERR, "[%d] Failed to open %s: %m -> Disabling this unit",the_unit,lns[the_unit].devnam);
      syslog(LOG_NOTICE, "[%d] Failed to open %s: %m -> Disabling this unit",the_unit,lns[the_unit].devnam);
      lns[the_unit].fd = -2;
    }
    else 
    {
      syslog(LOG_ERR, "[%d] Failed to open %s: %m",the_unit,lns[the_unit].devnam);
      syslog(LOG_NOTICE, "[%d] Failed to open %s: %m",the_unit,lns[the_unit].devnam);
      lns[the_unit].fd = -1;
    }
    /* die(1); */
/* maybe we should start a callout here to retry the open after a few seconds */
    lns[the_unit].openfails++;
    if(lns[the_unit].openfails >= 16)
    {
      syslog(LOG_ERR, "Too much open fails on %s: %m", lns[the_unit].devnam);
      die(1);
    }
    return -1;
  }
  lns[the_unit].openfails = 0;
  if ((lns[the_unit].initfdflags = fcntl(lns[the_unit].fd, F_GETFL)) == -1) {
    syslog(LOG_ERR, "Couldn't get device fd flags: %m");
    die(1);
  }

#if 0
        lns[the_unit].initfdflags &= ~O_NONBLOCK;
        fcntl(lns[the_unit].fd, F_SETFL, lns[the_unit].initfdflags);

	/* set up the serial device as a ppp interface */
	establish_ppp(the_unit);
#endif

	/*
	 * Set device for non-blocking reads.
	 */
	if (fcntl(lns[the_unit].fd, F_SETFL, lns[the_unit].initfdflags | O_NONBLOCK) == -1) {
	    syslog(LOG_ERR, "Couldn't set device to non-blocking mode: %m");
	    die(1);
	}
	syslog(LOG_NOTICE, "Connect[%d]: %s, fd: %d",the_unit, lns[the_unit].devnam,lns[the_unit].fd);
  return 0;
}


static int exit_unit(int the_unit)
{
	if (lns[the_unit].initfdflags != -1) { 
		lns[the_unit].initfdflags &= ~O_NONBLOCK;
		if(fcntl(lns[the_unit].fd, F_SETFL, lns[the_unit].initfdflags) < 0)
			syslog(LOG_ERR,"Failed to set %s back to blocking mode %m",lns[the_unit].devnam);
		lns[the_unit].initfdflags = -1;
	}
	close_fd(the_unit);
	return 0;
}

char *protocol2name(int p)
{
	int top = (sizeof(protonames) / sizeof(struct protos));
	int bottom = 0;
	int pos = top >> 1;
	int step = (pos+1) >> 1;

	while(pos < top && pos >= bottom) {
		if(protonames[pos].val == p)
			return protonames[pos].name;
		if(protonames[pos].val > p) {
			top = pos;
			pos -= step;
			step = (step+1)>>1;
		}
		else {
			bottom = pos + 1;
			pos += step;
			step = (step+1)>>1;
		}
	}
	return NULL;
}

/*
 * get_input - called when incoming data is available.
 */
static void get_input(int linkunit)
{
    int len, i;
    u_char *p;
    u_short protocol;
	struct protent *protp;

    p = inpacket_buf;	/* point to beginning of packet buffer */

    len = read_packet(inpacket_buf,linkunit);
    if (len < 0)
	return;

    if (len == 0) {
	syslog(LOG_NOTICE, "Modem hangup");
	lns[linkunit].hungup = 1;
#if 0
	lcp_lowerdown(lns[linkunit].lcp_unit);	/* serial link is no longer available */
#endif
	link_terminated(linkunit);
	return;
    }

    if (debug)
	  log_packet(p, len, "rcvd ",linkunit);

    if (len < PPP_HDRLEN) {
	MAINDEBUG((LOG_INFO, "io(): Received short packet."));
	return;
    }

    p += 2;				/* Skip address and control */
    GETSHORT(protocol, p);
    len -= PPP_HDRLEN;

    /*
     * Toss all non-LCP packets unless LCP is OPEN.
     */
    if (protocol != PPP_LCP && lcp_fsm[lns[linkunit].lcp_unit].state != OPENED) {
	MAINDEBUG((LOG_INFO,
		   "io(): Received non-LCP packet when LCP not open."));
	return;
    }

    /*
     * Upcall the proper protocol input routine.
     */

	for (i = 0; (protp = protocols[i]) != NULL; ++i) {
		if (protp->protocol == protocol && protp->enabled_flag) {
			(*protp->input)(linkunit, p, len);
			return;
		}
        if (protocol == (protp->protocol & ~0x8000) && protp->enabled_flag
			&& protp->datainput != NULL) {
			(*protp->datainput)(linkunit, p, len);
			return;
		}
    }

    if (debug) {
		char *s;
		s = protocol2name(protocol);
		if(!s)
    		syslog(LOG_WARNING, "Unknown protocol (0x%x) received", protocol);
		else
			syslog(LOG_WARNING, "Unsupported protocol '%s' (0x%x) received", s,protocol);
	}
    lcp_sprotrej(linkunit, p - PPP_HDRLEN, len + PPP_HDRLEN);
}


/*
 * demuxprotrej - Demultiplex a Protocol-Reject.
 */
void demuxprotrej(int linkunit,u_short protocol)
{
    int i;
	struct protent *protp;

    /*
     * Upcall the proper Protocol-Reject routine.
     */
	for (i = 0; (protp = protocols[i]) != NULL; ++i) {
		if (protp->protocol == protocol) {
			(*protp->protrej)(linkunit);
			return;
		}
	}

    syslog(LOG_WARNING,
	   "demuxprotrej: Unrecognized Protocol-Reject for protocol 0x%x",
	   protocol);
}


/*
 * bad_signal - We've caught a fatal signal.  Clean up state and exit.
 */
static void
bad_signal(sig)
    int sig;
{
    syslog(LOG_ERR, "Fatal signal %d", sig);
    die(1);
}

/*
 * quit - Clean up state and exit (with an error indication).
 */
void 
quit()
{
    die(1);
}

/*
 * die - like quit, except we can specify an exit status.
 */
void die(int status)
{
    int i;
    for(i=0;i<numdev;i++)
      cleanup(0, NULL,i);
    syslog(LOG_INFO, "Exit.");
    exit(status);
}

/*
 * cleanup - restore anything which needs to be restored before we exit
 */
/* ARGSUSED */
void cleanup(int status,caddr_t arg,int tu)
{
    if (lns[tu].fd >= 0)
	close_fd(tu);

    if (pidfilename[0] != 0 && unlink(pidfilename) < 0 && errno != ENOENT) 
	syslog(LOG_WARNING, "unable to delete pid file: %m");
    pidfilename[0] = 0;

#if 0
    if (lockflag)
	unlock();
#endif

}

/*
 * close_fd - restore the terminal device and close it.
 */
void close_fd(int tu)
{
	if (lns[tu].initfdflags != -1 && fcntl(lns[tu].fd, F_SETFL, lns[tu].initfdflags) < 0)
		syslog(LOG_WARNING, "Couldn't restore device fd flags: %m");
	lns[tu].initfdflags = -1;
    close(lns[tu].fd);
    lns[tu].fd = -1;
}


struct	callout {
    struct timeval	c_time;		/* time at which to call routine */
    caddr_t		c_arg;		/* argument to routine */
    void		(*c_func)();	/* routine */
    struct		callout *c_next;
};

static struct callout *callout = NULL;	/* Callout list */
static struct timeval timenow;		/* Current time */

/*
 * timeout - Schedule a timeout.
 *
 * Note that this timeout takes the number of seconds, NOT hz (as in
 * the kernel).
 */
void
timeout(func, arg, time)
    void (*func)();
    caddr_t arg;
    int time;
{
    struct callout *newp, *p, **pp;
  
    MAINDEBUG((LOG_DEBUG, "Timeout %lx:%lx in %d seconds.",
	       (long) func, (long) arg, time));
  
    /*
     * Allocate timeout.
     */
    if ((newp = (struct callout *) malloc(sizeof(struct callout))) == NULL) {
	syslog(LOG_ERR, "Out of memory in timeout()!");
	die(1);
    }
    newp->c_arg = arg;
    newp->c_func = func;
    gettimeofday(&timenow, NULL);
    newp->c_time.tv_sec = timenow.tv_sec + time;
    newp->c_time.tv_usec = timenow.tv_usec;
  
    /*
     * Find correct place and link it in.
     */
    for (pp = &callout; (p = *pp); pp = &p->c_next)
	if (newp->c_time.tv_sec < p->c_time.tv_sec
	    || (newp->c_time.tv_sec == p->c_time.tv_sec
		&& newp->c_time.tv_usec < p->c_time.tv_sec))
	    break;
    newp->c_next = p;
    *pp = newp;
}


/*
 * untimeout - Unschedule a timeout.
 */
void
untimeout(func, arg)
    void (*func)();
    caddr_t arg;
{
#if 0
    struct itimerval itv;
    int reschedule = 0;
#endif
    struct callout **copp, *freep;
  
    MAINDEBUG((LOG_DEBUG, "Untimeout %lx:%lx.", (long) func, (long) arg));
  
    /*
     * Find first matching timeout and remove it from the list.
     */
    for (copp = &callout; (freep = *copp); copp = &freep->c_next)
	if (freep->c_func == func && freep->c_arg == arg) {
	    *copp = freep->c_next;
	    (void) free((char *) freep);
	    break;
	}
}


/*
 * calltimeout - Call any timeout routines which are now due.
 */
void
calltimeout()
{
    struct callout *p;

    while (callout != NULL) {
	p = callout;

	if (gettimeofday(&timenow, NULL) < 0) {
	    syslog(LOG_ERR, "Failed to get time of day: %m");
	    die(1);
	}
	if (!(p->c_time.tv_sec < timenow.tv_sec
	      || (p->c_time.tv_sec == timenow.tv_sec
		  && p->c_time.tv_usec <= timenow.tv_usec)))
	    break;		/* no, it's not time yet */

	callout = p->c_next;
	(*p->c_func)(p->c_arg);

	free((char *) p);
    }
}


/*
 * timeleft - return the length of time until the next timeout is due.
 */
struct timeval *
timeleft(tvp)
    struct timeval *tvp;
{
    if (callout == NULL)
	return NULL;

    gettimeofday(&timenow, NULL);
    tvp->tv_sec = callout->c_time.tv_sec - timenow.tv_sec;
    tvp->tv_usec = callout->c_time.tv_usec - timenow.tv_usec;
    if (tvp->tv_usec < 0) {
	tvp->tv_usec += 1000000;
	tvp->tv_sec -= 1;
    }
    if (tvp->tv_sec < 0)
	tvp->tv_sec = tvp->tv_usec = 0;

    return tvp;
}
    

/*
 * hup - Catch SIGHUP signal.
 *
 * Indicates that the physical layer has been disconnected.
 * We don't rely on this indication; if the user has sent this
 * signal, we just take the link down.
 */
static void hup(int sig)
{
    syslog(LOG_INFO, "Hangup (SIGHUP)");
/*    kill_link = 1; */
}


/*
 * term - Catch SIGTERM signal and SIGINT signal (^C/del).
 *
 * Indicates that we should initiate a graceful disconnect and exit.
 */
/*ARGSUSED*/
static void term(int sig)
{
    syslog(LOG_INFO, "Terminating on signal %d.", sig);
    kill_link = 1;
}


/*
 * chld - Catch SIGCHLD signal.
 * Calls reap_kids to get status for any dead kids.
 */
static void
chld(sig)
    int sig;
{
    reap_kids();
}


/*
 * toggle_debug - Catch SIGUSR1 signal.
 *
 * Toggle debug flag.
 */
/*ARGSUSED*/
static void toggle_debug(int sig)
{
    debug = !debug;
    note_debug_level();
}


/*
 * open_ccp - Catch SIGUSR2 signal.
 *
 * Try to (re)negotiate compression on all links (ugly).
 */
/*ARGSUSED*/
static void open_ccp(int sig)
{
  int i;
  for(i=0;i<NUM_PPP;i++)
    lns[i].open_ccp_flag = 1;
}


/*
 * device_script - run a program to connect or disconnect the
 * serial device.
 */
int device_script(char *program,int in,int out)
{
    int pid;
    int status;
    int errfd;

    pid = fork();

    if (pid < 0) {
	syslog(LOG_ERR, "Failed to create child process: %m");
	die(1);
    }

    if (pid == 0) {
	dup2(in, 0);
	dup2(out, 1);
	errfd = open(_PATH_CONNERRS, O_WRONLY | O_APPEND | O_CREAT, 0644);
	if (errfd >= 0)
	    dup2(errfd, 2);
	setuid(getuid());
	setgid(getgid());
	execl("/bin/sh", "sh", "-c", program, (char *)0);
	syslog(LOG_ERR, "could not exec /bin/sh: %m");
	_exit(99);
	/* NOTREACHED */
    }

    while (waitpid(pid, &status, 0) < 0) {
	if (errno == EINTR)
	    continue;
	syslog(LOG_ERR, "error waiting for (dis)connection process: %m");
	die(1);
    }

    return (status == 0 ? 0 : -1);
}


/*
 * run-program - execute a program with given arguments,
 * but don't wait for it.
 * If the program can't be executed, logs an error unless
 * must_exist is 0 and the program file doesn't exist.
 */
int run_program(char *prog,char **args,int must_exist,int unit)
{
    int pid;
    char *nullenv[1];

    pid = fork();
    if (pid == -1) {
	syslog(LOG_ERR, "Failed to create child process for %s: %m", prog);
	return -1;
    }
    if (pid == 0) {
	int new_fd;

	/* Leave the current location */
	(void) setsid();    /* No controlling tty. */
	(void) umask (S_IRWXG|S_IRWXO);
	(void) chdir ("/"); /* no current directory. */
	setuid(geteuid());
	setgid(getegid());

	/* Ensure that nothing of our device environment is inherited. */
	close (0);
	close (1);
	close (2);
	close (lns[unit].fd);  /* tty interface to the ppp device */
	/* XXX should call sysdep cleanup procedure here */

        /* Don't pass handles to the PPP device, even by accident. */
	new_fd = open (_PATH_DEVNULL, O_RDWR);
	if (new_fd >= 0) {
	    if (new_fd != 0) {
	        dup2  (new_fd, 0); /* stdin <- /dev/null */
		close (new_fd);
	    }
	    dup2 (0, 1); /* stdout -> /dev/null */
	    dup2 (0, 2); /* stderr -> /dev/null */
	}

#ifdef BSD
	/* Force the priority back to zero if pppd is running higher. */
	if (setpriority (PRIO_PROCESS, 0, 0) < 0)
	    syslog (LOG_WARNING, "can't reset priority to 0: %m"); 
#endif

	/* SysV recommends a second fork at this point. */

	/* run the program; give it a null environment */
	nullenv[0] = NULL;
	execve(prog, args, nullenv);
	if (must_exist || errno != ENOENT)
	    syslog(LOG_WARNING, "Can't execute %s: %m", prog);
	_exit(-1);
    }
    MAINDEBUG((LOG_DEBUG, "Script %s started; pid = %d", prog, pid));
    ++n_children;
    return 0;
}


/*
 * reap_kids - get status from any dead child processes,
 * and log a message for abnormal terminations.
 */
void
reap_kids()
{
    int pid, status;

    if (n_children == 0)
	return;
    if ((pid = waitpid(-1, &status, WNOHANG)) == -1) {
	if (errno != ECHILD)
	    syslog(LOG_ERR, "Error waiting for child process: %m");
	return;
    }
    if (pid > 0) {
	--n_children;
	if (WIFSIGNALED(status)) {
	    syslog(LOG_WARNING, "Child process %d terminated with signal %d",
		   pid, WTERMSIG(status));
	}
    }
}


/*
 * log_packet - format a packet and log it.
 */

char line[256];			/* line to be logged accumulated here */
char *linep;

void
log_packet(u_char *p,int len,char *prefix,int linkunit)
{
    strcpy(line, prefix);
    linep = line + strlen(line);
    format_packet(p, len, pr_log, NULL,linkunit);
    if (linep != line)
	syslog(LOG_DEBUG, "%s", line);
}

/*
 * format_packet - make a readable representation of a packet,
 * calling `printer(arg, format, ...)' to output it.
 */
void
format_packet(u_char *p,int len,void (*printer)(void*,char*,...),void *arg,int linkunit)
{
    int i, n;
    u_short proto;
    u_char x;
	struct protent *protp;

    if (len >= PPP_HDRLEN && p[0] == PPP_ALLSTATIONS && p[1] == PPP_UI) {
	p += 2;
	GETSHORT(proto, p);
	len -= PPP_HDRLEN;
	for (i = 0; (protp = protocols[i]) != NULL; ++i) {
		if (proto == protp->protocol)
			break;
	}
        printer(arg,"[%d]",linkunit);
	if (protp) {
	    printer(arg, "[%s", protp->name);
	    n = (*protp->printpkt)(p, len, printer, arg);
	    printer(arg, "]");
	    p += n;
	    len -= n;
	} else {
	    printer(arg, "[proto=0x%x]", proto);
	}
    }

    for (; len > 0; --len) {
	GETCHAR(x, p);
	printer(arg, " %.2x", x);
    }
}

#ifdef __STDC__
#include <stdarg.h>

void
pr_log(void *arg, char *fmt, ...)
{
    int n;
    va_list pvar;
    char buf[256];

    va_start(pvar, fmt);
    vsprintf(buf, fmt, pvar);
    va_end(pvar);

    n = strlen(buf);
    if (linep + n + 1 > line + sizeof(line)) {
	syslog(LOG_DEBUG, "%s", line);
	linep = line;
    }
    strcpy(linep, buf);
    linep += n;
}

#else /* __STDC__ */
#include <varargs.h>

void
pr_log(arg, fmt, va_alist)
void *arg;
char *fmt;
va_dcl
{
    int n;
    va_list pvar;
    char buf[256];

    va_start(pvar);
    vsprintf(buf, fmt, pvar);
    va_end(pvar);

    n = strlen(buf);
    if (linep + n + 1 > line + sizeof(line)) {
	syslog(LOG_DEBUG, "%s", line);
	linep = line;
    }
    strcpy(linep, buf);
    linep += n;
}
#endif

/*
 * print_string - print a readable representation of a string using
 * printer.
 */
void
print_string(p, len, printer, arg)
    char *p;
    int len;
    void (*printer) __P((void *, char *, ...));
    void *arg;
{
    int c;

    printer(arg, "\"");
    for (; len > 0; --len) {
	c = *p++;
	if (' ' <= c && c <= '~')
	    printer(arg, "%c", c);
	else
	    printer(arg, "\\%.3o", c);
    }
    printer(arg, "\"");
}

/*
 * novm - log an error message saying we ran out of memory, and die.
 */
void
novm(msg)
    char *msg;
{
    syslog(LOG_ERR, "Virtual memory exhausted allocating %s\n", msg);
    die(1);
}

/*
 * fmtmsg - format a message into a buffer.  Like sprintf except we
 * also specify the length of the output buffer, and we handle
 * %r (recursive format), %m (error message) and %I (IP address) formats.
 * Doesn't do floating-point formats.
 * Returns the number of chars put into buf.
 */
int fmtmsg(char *buf, int buflen, char *fmt, ...)
{
    va_list args;
    int n;

#if __STDC__
    va_start(args, fmt);
#else
    char *buf;
    int buflen;
    char *fmt;
    va_start(args);
    buf = va_arg(args, char *);
    buflen = va_arg(args, int);
    fmt = va_arg(args, char *);
#endif
    n = vfmtmsg(buf, buflen, fmt, args);
    va_end(args);
    return n;
}

/*
 * vfmtmsg - like fmtmsg, takes a va_list instead of a list of args.
 */
#define OUTCHAR(c)  (buflen > 0? (--buflen, *buf++ = (c)): 0)

int vfmtmsg(char *buf,int buflen,char *fmt,va_list args)
{
    int c, i, n;
    int width, prec, fillch;
    int base, len, neg, quoted;
    unsigned long val=0;
    char *str, *f, *buf0;
    unsigned char *p;
    void *a;
    char num[32];
    time_t t;
    static char hexchars[] = "0123456789abcdef";

    buf0 = buf;
    --buflen;
    while (buflen > 0) {
    for (f = fmt; *f != '%' && *f != 0; ++f)
        ;
    if (f > fmt) {
        len = f - fmt;
        if (len > buflen)
        len = buflen;
        memcpy(buf, fmt, len);
        buf += len;
        buflen -= len;
        fmt = f;
    }
    if (*fmt == 0)
        break;
    c = *++fmt;
    width = prec = 0;
    fillch = ' ';
    if (c == '0') {
        fillch = '0';
        c = *++fmt;
    }
    if (c == '*') {
        width = va_arg(args, int);
        c = *++fmt;
    } else {
        while (isdigit(c)) {
        width = width * 10 + c - '0';
        c = *++fmt;
        }
    }
    if (c == '.') {
        c = *++fmt;
        if (c == '*') {
        prec = va_arg(args, int);
        c = *++fmt;
        } else {
        while (isdigit(c)) {
            prec = prec * 10 + c - '0';
            c = *++fmt;
        }
        }
    }
    str = 0;
    base = 0;
    neg = 0;
    ++fmt;
    switch (c) {
    case 'd':
        i = va_arg(args, int);
        if (i < 0) {
        neg = 1;
        val = -i;
        } else
        val = i;
        base = 10;
        break;
    case 'o':
        val = va_arg(args, unsigned int);
        base = 8;
        break;
    case 'x':
        val = va_arg(args, unsigned int);
        base = 16;
        break;
    case 'p':
        val = (unsigned long) va_arg(args, void *);
        base = 16;
        neg = 2;
        break;
    case 's':
        str = va_arg(args, char *);
        break;
    case 'c':
        num[0] = va_arg(args, int);
        num[1] = 0;
        str = num;
        break;
    case 'm':
        str = strerror(errno);
        break;
    case 'I':
        str = ip_ntoa(va_arg(args, u_int32_t));
        break;
    case 'r':
        f = va_arg(args, char *);
        /*
         * XXX We assume a va_list is either a pointer or an array, so
         * what gets passed for a va_list is like a void * in some sense.
         */
        a = va_arg(args, void *);
        n = vfmtmsg(buf, buflen + 1, f, a);
        buf += n;
        buflen -= n;
        continue;
    case 't':
        time(&t);
        str = ctime(&t);
        str += 4;       /* chop off the day name */
        str[15] = 0;    /* chop off year and newline */
        break;
    case 'v':       /* "visible" string */
    case 'q':       /* quoted string */
        quoted = c == 'q';
        p = va_arg(args, unsigned char *);
        if (fillch == '0' && prec > 0) {
        n = prec;
        } else {
        n = strlen((char *)p);
        if (prec > 0 && prec < n)
            n = prec;
        }
        while (n > 0 && buflen > 0) {
        c = *p++;
        --n;
        if (!quoted && c >= 0x80) {
            OUTCHAR('M');
            OUTCHAR('-');
            c -= 0x80;
        }
        if (quoted && (c == '"' || c == '\\'))
            OUTCHAR('\\');
        if ( (c < 0x20 || 0x7f <= c) && c < 0xa0) {
            if (quoted) {
            OUTCHAR('\\');
            switch (c) {
            case '\t':  OUTCHAR('t');   break;
            case '\n':  OUTCHAR('n');   break;
            case '\b':  OUTCHAR('b');   break;
            case '\f':  OUTCHAR('f');   break;
            default:
                OUTCHAR('x');
                OUTCHAR(hexchars[c >> 4]);
                OUTCHAR(hexchars[c & 0xf]);
            }
            } else {
            if (c == '\t')
                OUTCHAR(c);
            else {
                OUTCHAR('^');
                OUTCHAR(c ^ 0x40);
            }
            }
        } else
            OUTCHAR(c);
        }
        continue;
    default:
        *buf++ = '%';
        if (c != '%')
        --fmt;      /* so %z outputs %z etc. */
        --buflen;
        continue;
    }
    if (base != 0) {
        str = num + sizeof(num);
        *--str = 0;
        while (str > num + neg) {
        *--str = hexchars[val % base];
        val = val / base;
        if (--prec <= 0 && val == 0)
            break;
        }
        switch (neg) {
        case 1:
        *--str = '-';
        break;
        case 2:
        *--str = 'x';
        *--str = '0';
        break;
        }
        len = num + sizeof(num) - 1 - str;
    } else {
        len = strlen(str);
        if (prec > 0 && len > prec)
        len = prec;
    }
    if (width > 0) {
        if (width > buflen)
        width = buflen;
        if ((n = width - len) > 0) {
        buflen -= n;
        for (; n > 0; --n)
            *buf++ = fillch;
        }
    }
    if (len > buflen)
        len = buflen;
    memcpy(buf, str, len);
    buf += len;
    buflen -= len;
    }
    *buf = 0;
    return buf - buf0;
}
 
