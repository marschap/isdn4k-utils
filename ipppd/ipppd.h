/*
 * ipppd.h - PPP daemon global declarations.
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
 *
 * $Id: ipppd.h,v 1.6 1997/05/19 10:15:55 hipp Exp $
 */

/*
 * TODO:
 */

#define ISDN4LINUX_PATCH

#ifndef __IPPPD_H__
#define __IPPPD_H__

#include <stdarg.h>
#include <sys/param.h>		/* for MAXPATHLEN and BSD4_4, if defined */
#include <sys/types.h>		/* for u_int32_t, if defined */
#include <sys/bitypes.h>
#include <linux/ppp_defs.h>
#include <linux/isdn_ppp.h>
#include <stdio.h>
#include <net/if.h>

#define NUM_PPP	16		/* 16 PPP interface supported (per process) */

struct wordlist {
  struct wordlist *next;
  char word[1];
};

struct link_struct {
  struct link_struct *bundle_next;
  int fd;       /* link file descriptor */
  int peer_mru; /* link: peer_mru */
  int logged_in;
  int auth_pending;
  struct wordlist *addresses;
  int unit;     /* link unit */
  int lcp_unit;
  int ipcp_unit;
  int ccp_unit;
  int chap_unit;
  int upap_unit;
  int cbcp_unit;
  int ipxcp_unit;
  int phase;
  int auth_up_script;
  int kill_link;
  int open_ccp_flag;
  int ifunit;
  int openfails;
  char ifname[IFNAMSIZ];
  char devnam[MAXPATHLEN];
  char peer_authname[64];
  int initfdflags;
  int hungup;
  struct pppcallinfo pci;
  int has_proxy_arp;
  int attempts;
};

extern struct link_struct lns[NUM_PPP];

/*
 * Limits.
 */

#define MAXWORDLEN	1024	/* max length of word in file (incl null) */
#define MAXARGS		1	/* max # args to a command */
#define MAXNAMELEN	256	/* max length of hostname or name for auth */
#define MAXSECRETLEN	256	/* max length of password or secret */

/*
 * Global variables.
 */

extern char	hostname[];	/* Our hostname */
extern u_char	outpacket_buf[]; /* Buffer for outgoing packets */
extern int	baud_rate;	/* Current link speed in bits/sec */
extern char	*progname;	/* Name of this program */
extern char pidfilename[MAXPATHLEN];

extern void set_userip(char *ruser,int ruserlen);

extern char options_rcsid[];
extern char auth_rcsid[];
extern char lcp_rcsid[];
extern char ipcp_rcsid[];
extern char ipxcp_rcsid[];
extern char cbcp_rcsid[];
extern char main_rcsid[];
extern char chap_rcsid[];
#ifdef USE_MSCHAP
extern char chap_ms_rcsid[];
#endif
extern char upap_rcsid[];
extern char ccp_rcsid[];
extern char fsm_rcsid[];
extern char magic_rcsid[];
extern char sys_rcsid[];

/*
 * Variables set by command-line options.
 */

extern int maxconnect;
extern int      usefirstip,useifip,useifmtu;
extern int      numdev;		/* number of handled devices */
extern int	debug;		/* Debug flag */
extern int	kdebugflag;	/* Tell kernel to print debug messages */
extern int	default_device;	/* Using /dev/tty or equivalent */
extern int	crtscts;	/* Use hardware flow control */
extern int	modem;		/* Use modem control lines */
extern int	inspeed;	/* Input/Output speed requested */
extern u_int32_t netmask;	/* IP netmask to set on interface */
extern int	lockflag;	/* Create lock file to lock the serial dev */
extern int	nodetach;	/* Don't detach from controlling tty */
extern char	*connector;	/* Script to establish physical link */
extern char	*disconnector;	/* Script to disestablish physical link */
extern char	user[];		/* Username for PAP */
extern char	passwd[];	/* Password for PAP */
extern int	auth_required;	/* Peer is required to authenticate */
extern int	proxyarp;	/* Set up proxy ARP entry for peer */
extern int	persist;	/* Reopen link after it goes down */
extern int	uselogin;	/* Use /etc/passwd for checking PAP */
extern int	lcp_echo_interval; /* Interval between LCP echo-requests */
extern int	lcp_echo_fails;	/* Tolerance to unanswered echo-requests */
extern char	our_name[];	/* Our name for authentication purposes */
extern char	remote_name[];	/* Peer's name for authentication */
extern int	usehostname;	/* Use hostname for our_name */
extern int	disable_defaultip; /* Don't use hostname for default IP adrs */
extern char	*ipparam;	/* Extra parameter for ip up/down scripts */
extern int	cryptpap;	/* Others' PAP passwords are encrypted */
#ifdef __linux__
extern int      hostroute;      /* Add a route to the host at the other end? */
#endif

/*
 * Values for phase.
 */
#define PHASE_WAIT		-1
#define PHASE_DEAD		0
#define PHASE_ESTABLISH		1
#define PHASE_AUTHENTICATE	2
#define PHASE_CALLBACK	3
#define PHASE_NETWORK		4
#define PHASE_TERMINATE		5

/*
 * The following struct gives the addresses of procedures to call
 * for a particular protocol.
 */
struct protent {
    u_short protocol;       /* PPP protocol number */
    /* Initialization procedure */
    void (*init) __P((int unit));
    /* Process a received packet */
    void (*input) __P((int unit, u_char *pkt, int len));
    /* Process a received protocol-reject */
    void (*protrej) __P((int unit));
    /* Lower layer has come up */
    void (*lowerup) __P((int unit));
    /* Lower layer has gone down */
    void (*lowerdown) __P((int unit));
    /* Open the protocol */
    void (*open) __P((int unit));
    /* Close the protocol */
    void (*close) __P((int unit, char *reason));
    /* Print a packet in readable form */
    int  (*printpkt) __P((u_char *pkt, int len,
              void (*printer) __P((void *, char *, ...)),
              void *arg));
    /* Process a received data packet */
    void (*datainput) __P((int unit, u_char *pkt, int len));
    int  enabled_flag;      /* 0 iff protocol is disabled */
    char *name;         /* Text name of protocol */
    /* Check requested options, assign defaults */
    void (*check_options) __P((void));
    /* Configure interface for demand-dial */
    int  (*demand_conf) __P((int unit));
    /* Say whether to bring up link for this pkt */
    int  (*active_pkt) __P((u_char *pkt, int len));
};

/* Table of pointers to supported protocols */
extern struct protent *protocols[];

/*
 * Prototypes.
 */
void quit __P((void));	/* Cleanup and exit */
void timeout __P((void (*)(), caddr_t, int));
				/* Look-alike of kernel's timeout() */
void untimeout __P((void (*)(), caddr_t));
				/* Look-alike of kernel's untimeout() */
void output __P((int, u_char *, int));
				/* Output a PPP packet */
void demuxprotrej __P((int,u_short));
				/* Demultiplex a Protocol-Reject */
int  check_passwd __P((int, char *, int, char *, int, char **, int *));
				/* Check peer-supplied username/password */
int  get_secret __P((int, char *, char *, char *, int *, int));
				/* get "secret" for chap */
u_int32_t GetMask __P((u_int32_t)); /* get netmask for address */
void die __P((int));
void check_access __P((FILE *, char *));

int ccp_getunit(int);
int ipcp_getunit(int);
int lcp_getunit(int);
void ccp_freeunit(int);
void ipcp_freeunit(int);
void lcp_freeunit(int);
char *ip_ntoa(u_int32_t);
int bad_ip_adrs(u_int32_t);
int getword(FILE *,char *,int *,char *);
void print_string(char *p,int len,void (*printer)(void *,char *,...),void *arg);
int auth_ip_addr(int unit,u_int32_t addr);
void auth_peer_fail(int,int);
void auth_withpeer_fail(int unit,int protocol);
void auth_peer_success(int unit,int protocol);
void auth_withpeer_success(int unit,int protocol);

void link_required(int);
void link_terminated(int);
void link_down(int);
void link_established(int unit);
int device_script(char *program,int in,int out);
void check_auth_options(void);
void setipdefault(void);
int options_from_file(char *filename,int must_exist,int check_prot,int slot);
int options_for_tty(void);
int options_from_user(void);
int parse_args(int argc,char **argv);
int run_program(char *prog,char **args,int must_exist,int tu);
void establish_ppp __P((int));
void calltimeout __P((void));
struct timeval *timeleft __P((struct timeval *));
void reap_kids __P((void));
void cleanup __P((int, caddr_t,int));
void close_fd __P((int));
void die __P((int));
void novm __P((char *));
void log_packet __P((u_char *, int, char *,int));
void format_packet __P((u_char *,int,void (*) (void *, char *, ...), void *,int));
void pr_log __P((void *, char *, ...));
void sys_init(void);
void note_debug_level (void);
void output (int unit, unsigned char *p, int len);
void wait_input (struct timeval *timo);
int read_packet (unsigned char *buf,int tu);
void ppp_send_config (int unit,int mtu,u_int32_t asyncmap,int pcomp,int accomp);
void ppp_set_xaccm (int unit, ext_accm accm);
void ppp_recv_config (int unit,int mru,u_int32_t asyncmap,int pcomp,int accomp);
int ccp_test (int unit, u_char *opt_ptr, int opt_len, int for_transmit);
void ccp_flags_set (int unit, int isopen, int isup);
int ccp_fatal_error (int unit);
int sifvjcomp (int unit, int vjcomp, int cidcomp, int maxcid);
int sifup (int u);
int sifdown (int u);
int sifbundle(int,int);
int sifaddr (int unit, int our_adr, int his_adr, int net_mask);
int cifaddr (int unit, int our_adr, int his_adr);
int sifdefaultroute (int unit, int gateway);
int cifdefaultroute (int unit, int gateway);
int sifproxyarp (int unit, u_int32_t his_adr);
int cifproxyarp (int unit, u_int32_t his_adr);
int sipxfaddr (int unit, u_int32_t network, unsigned char * node );
int cipxfaddr (int linkunit);
int ppp_available(void);
int logwtmp (int unit,char *line, char *name, char *host);
int lock (char *dev);
void unlock(void);
void setifip(int);
extern void enable_mp(int,int);
void remove_sys_options(void);
u_int32_t magic(void);
int fmtmsg __P((char *, int, char *, ...));              /* sprintf++ */
int vfmtmsg __P((char *, int, char *, va_list)); /* vsprintf++ */
void option_error __P((char *fmt, ...));
void usage __P((void));          /* Print a usage message */

/*
 * This structure is used to store information about certain
 * options, such as where the option value came from (/etc/ppp/options,
 * command line, etc.) and whether it came from a privileged source.
 */

struct option_info {
    int     priv;       /* was value set by sysadmin? */
    char    *source;        /* where option came from */
};

extern struct option_info auth_req_info;
extern struct option_info connector_info;
extern struct option_info disconnector_info;
extern struct option_info welcomer_info;
extern struct option_info devnam_info;


/*
 * Inline versions of get/put char/short/long.
 * Pointer is advanced; we assume that both arguments
 * are lvalues and will already be in registers.
 * cp MUST be u_char *.
 */
#define GETCHAR(c, cp) { \
	(c) = *(cp)++; \
}
#define PUTCHAR(c, cp) { \
	*(cp)++ = (u_char) (c); \
}


#define GETSHORT(s, cp) { \
	(s) = *(cp)++ << 8; \
	(s) |= *(cp)++; \
}
#define PUTSHORT(s, cp) { \
	*(cp)++ = (u_char) ((s) >> 8); \
	*(cp)++ = (u_char) (s); \
}

#define GETLONG(l, cp) { \
	(l) = *(cp)++ << 8; \
	(l) |= *(cp)++; (l) <<= 8; \
	(l) |= *(cp)++; (l) <<= 8; \
	(l) |= *(cp)++; \
}
#define PUTLONG(l, cp) { \
	*(cp)++ = (u_char) ((l) >> 24); \
	*(cp)++ = (u_char) ((l) >> 16); \
	*(cp)++ = (u_char) ((l) >> 8); \
	*(cp)++ = (u_char) (l); \
}

#define INCPTR(n, cp)	((cp) += (n))
#define DECPTR(n, cp)	((cp) -= (n))

#undef  FALSE
#define FALSE	0
#undef  TRUE
#define TRUE	1

/*
 * System dependent definitions for user-level 4.3BSD UNIX implementation.
 */

#define DEMUXPROTREJ(u, p)	demuxprotrej(u, p)

#define TIMEOUT(r, f, t)	timeout((r), (f), (t))
#define UNTIMEOUT(r, f)		untimeout((r), (f))

#define BCOPY(s, d, l)		memcpy(d, s, l)
#define BZERO(s, n)		memset(s, 0, n)
#define EXIT(u)			quit()

#define PRINTMSG(m, l)	{ m[l] = '\0'; syslog(LOG_INFO, "Remote message: %s", m); }

/*
 * MAKEHEADER - Add Header fields to a packet.
 */
#define MAKEHEADER(p, t) { \
    PUTCHAR(PPP_ALLSTATIONS, p); \
    PUTCHAR(PPP_UI, p); \
    PUTSHORT(t, p); }


#ifdef DEBUGALL
#define DEBUGMAIN	1
#define DEBUGFSM	1
#define DEBUGLCP	1
#define DEBUGIPCP	1
#define DEBUGIPXCP	1
#define DEBUGUPAP	1
#define DEBUGCHAP	1
#endif

#ifndef LOG_PPP			/* we use LOG_LOCAL2 for syslog by default */
#if defined(DEBUGMAIN)  || defined(DEBUGFSM)  || defined(DEBUG) \
  || defined(DEBUGLCP)  || defined(DEBUGIPCP) || defined(DEBUGUPAP) \
  || defined(DEBUGCHAP) || defined(DEBUGIPXCP)
#define LOG_PPP LOG_LOCAL2
#else
#define LOG_PPP LOG_DAEMON
#endif
#endif /* LOG_PPP */

#ifdef DEBUGMAIN
#define MAINDEBUG(x)	if (debug) syslog x
#else
#define MAINDEBUG(x)
#endif

#ifdef DEBUGFSM
#define FSMDEBUG(x)	if (debug) syslog x
#else
#define FSMDEBUG(x)
#endif

#ifdef DEBUGLCP
#define LCPDEBUG(x)	if (debug) syslog x
#else
#define LCPDEBUG(x)
#endif

#ifdef DEBUGIPCP
#define IPCPDEBUG(x)	if (debug) syslog x
#else
#define IPCPDEBUG(x)
#endif

#ifdef DEBUGIPXCP
#define IPXCPDEBUG(x)	if (debug) syslog x
#else
#define IPXCPDEBUG(x)
#endif

#ifdef DEBUGUPAP
#define UPAPDEBUG(x)	if (debug) syslog x
#else
#define UPAPDEBUG(x)
#endif

#ifdef DEBUGCHAP
#define CHAPDEBUG(x)	if (debug) syslog x
#else
#define CHAPDEBUG(x)
#endif

#ifndef SIGTYPE
#if defined(sun) || defined(SYSV) || defined(POSIX_SOURCE)
#define SIGTYPE void
#else
#define SIGTYPE int
#endif /* defined(sun) || defined(SYSV) || defined(POSIX_SOURCE) */
#endif /* SIGTYPE */

#ifndef MIN
#define MIN(a, b)	((a) < (b)? (a): (b))
#endif
#ifndef MAX
#define MAX(a, b)	((a) > (b)? (a): (b))
#endif

#endif /* __IPPP_H__ */

