/*
 * Copyright (C) 1996, Lars Fenneberg <lf@elemental.net>
 * Copyright (C) 1996, Matjaz Godec <gody@elgo.si>
 *
 * This file is provided under the terms and conditions of the GNU general 
 * public license, version 2 or any later version, incorporated herein by 
 * reference. 
 *
 */

#include <syslog.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

#include <radius.h>
#include "ipppd.h"
#include "fsm.h"
#include "lcp.h"
#include "upap.h"
#include "chap.h"
#include "ipcp.h"
#include "ccp.h"
#include "pathnames.h"

extern int idle_time_limit;

char *ip_ntoa __P((u_int32_t));
int	bad_ip_adrs __P((u_int32_t));

int radius_in = FALSE;	
int session_time_limit = 0;


static UINT4 client_port;
static int called_radius_init = 0;

static void radius_acct_start __P((int));

static int radius_init()
{
	fprintf(stderr,"@mla@: radius_init: called\n");
	if (called_radius_init)
		return 0;
	if (rc_read_config (_PATH_ETC_RADIUSCLIENT_CONF) != 0) {
		syslog(LOG_ERR, "RADIUS can't load config file %s", _PATH_ETC_RADIUSCLIENT_CONF);
		return (-1);
	}
	
	if (rc_read_dictionary (rc_conf_str ("dictionary")) != 0) {
		syslog(LOG_ERR, "RADIUS can't load dictionary file %s", rc_conf_str ("dictionary"));
		return (-1);
	}
    	
	if (rc_read_mapfile (rc_conf_str ("mapfile")) != 0) {
		syslog(LOG_ERR, "RADIUS can't load map file %s", rc_conf_str ("mapfile"));
		return (-1);
	}
    	
	called_radius_init = 1;
	
	return 0;
}

int radius_auth_order()
{
	if (!called_radius_init && (radius_init() < 0))
		return AUTH_LOCAL_FST;

	return rc_conf_int("auth_order");
}

int radius_setparams(vp, linkunit)
	VALUE_PAIR *vp;
{
	ipcp_options *wo = &ipcp_wantoptions[0];
	u_int32_t remote = 0;

	/* 
 	 * service type (if not framed then quit), 
 	 * new IP address (so RADIUS can define static IP for some users),
 	 * new netmask (not used at present)
 	 * idle time limit 
 	 * session time limit (not working at present)
 	 */

	while (vp) {
		switch (vp->attribute)
	    {
	    	case PW_SERVICE_TYPE:

	    		if (vp->lvalue != PW_FRAMED) {
					syslog (LOG_NOTICE, "RADIUS wrong service type %ld for %s", 
						vp->lvalue, lns[linkunit].username);
		  			return (-1);
				}
				break;
				
			case PW_FRAMED_PROTOCOL:
			
	    		if (vp->lvalue != PW_PPP) {
					syslog (LOG_NOTICE, "RADIUS wrong framed protocol %ld for %s)", 
									    vp->lvalue, lns[linkunit].username);
		  			return (-1);
				}
				break;

	    	case PW_FRAMED_IP_ADDRESS:
	    
	      		/* 0xfffffffe means NAS should select an ip address       */
	      		/* 0xffffffff means user should be allowed to select one  */
	      		/* the last case probably needs special handling ???      */
	      		if ((remote != 0xfffffffe) && (remote != 0xffffffff)) {
	      			remote = htonl(vp->lvalue);
	      			if (bad_ip_adrs (remote)) {
		  				syslog (LOG_ERR, "RADIUS bad remote IP address %s for %s", 
		  						  		 ip_ntoa (remote), lns[linkunit].username);
		  				return (-1);
					}	
					
					wo->hisaddr = remote;
				}
	      		break;
	      
	    	case PW_FRAMED_IP_NETMASK:
	    
	      		netmask = htonl (vp->lvalue);
	      		break;
	      
	    	case PW_FRAMED_MTU:
	    
	      		lcp_allowoptions[0].mru = vp->lvalue;
	      		break;

	    	case PW_IDLE_TIMEOUT:
	    	
	      		idle_time_limit = vp->lvalue;
	      		break;

			/* doesn't work at the moment */
	    	case PW_SESSION_TIMEOUT:
	    
	      		session_time_limit = vp->lvalue;
	      		break;
	    }
	    
	  	vp = vp->next;
	  
	}

	return 0;
}

#ifdef RADIUS_WTMP_LOGGING
void
radius_wtmp_logging(user)
	char *user;
{
	char *tty;

	syslog(LOG_INFO, "user %s logged in", user);

	tty = devnam;
	if (strncmp(tty, "/dev/", 5) == 0)
		tty += 5;
		
	logwtmp(tty, user, "");
}
#endif

/*
 * rad_pap_auth - Check the user name and password against RADIUS server, 
 * 				  and add accounting start record of the user if OK.
 *
 * Returns:
 *
 *		UPAP_AUTHNAK: Login failed.
 *		UPAP_AUTHACK: Login succeeded.
 *
 *		In either case, msg points to an appropriate message.
 *
 */

int
radius_pap_auth (user, passwd, msg, msglen, linkunit)
     char *user;
     char *passwd;
     char **msg;
     int *msglen;
     int linkunit;
{
	VALUE_PAIR *send, *received;
	UINT4 av_type;
	char username_realm[256]; 
	static char radius_msg[4096];
	int result;
  	char *default_realm;


	send = NULL;

	/* read in the config files if neccessary */
	if(!called_radius_init && (radius_init() < 0))
		return (UPAP_AUTHNAK);

	/*  
	 *  then we define and map tty to port
	 */
  	
  	client_port = rc_map2id (lns[linkunit].devnam);

	/*
 	 * now we define service type and framed protocol
 	 */

	av_type = PW_FRAMED;
  	rc_avpair_add (&send, PW_SERVICE_TYPE, &av_type, 0);

  	av_type = PW_PPP;
  	rc_avpair_add (&send, PW_FRAMED_PROTOCOL, &av_type, 0);

	/* 
 	 * for RADIUS we login in as username@realm eventualy so
 	 * here we add an @realm part of username if not already
 	 * specified.
 	 */
 	
 	strncpy (lns[linkunit].username, user, MAXUSERNAME); 
	strncpy (username_realm, user, MAXUSERNAME);

	default_realm = rc_conf_str ("default_realm");

	if ((strchr (username_realm, '@') == NULL) && default_realm &&
      (*default_realm != '\0'))
    {
      strncat (username_realm, "@", MAXUSERNAME);
      strncat (username_realm, default_realm, MAXUSERNAME);
    }

	/*
 	 * we are sending username and password to RADIUS
 	 */
 	 
	rc_avpair_add (&send, PW_USER_NAME, username_realm, 0);
	rc_avpair_add (&send, PW_USER_PASSWORD, passwd, 0);
	
	/*
 	 * make authentication with RADIUS server
 	 */
 	 
  	result = rc_auth (client_port, send, &received, radius_msg);

  	if (result == OK_RC) {
		if (radius_setparams(received) < 0)
			result = ERROR_RC;
		else {
			radius_in = TRUE;
			radius_acct_start(linkunit);
		}

		rc_avpair_free(received);
	}
	
	/* free value pairs */
	rc_avpair_free (send);
	
	*msg = radius_msg;
	*msglen = strlen(radius_msg);

#ifdef RADIUS_WTMP_LOGGING
	if (result == OK_RC)
		radius_wtmp_logging(user);
#endif
	
	return (result == OK_RC)?UPAP_AUTHACK:UPAP_AUTHNAK;
}

int
radius_chap_auth (user, remmd, cstate, linkunit)
	char *user;
	u_char *remmd;
	chap_state *cstate;
	int linkunit;
{
	VALUE_PAIR *send, *received;
	UINT4 av_type;
	char username_realm[256]; 
	static char radius_msg[4096];
	int result;
  	char *default_realm;
	u_char cpassword[MD5_SIGNATURE_SIZE+1];

	/* we handle md5 digest at the moment */
	if (cstate->chal_type != CHAP_DIGEST_MD5)
			return(-1);

	send = NULL;

	/* read in the config files if neccessary */
	if(!called_radius_init && (radius_init() < 0))
		return (-1);

	/*  
	 *  then we define and map tty to port
	 */
  	
  	client_port = rc_map2id (lns[linkunit].devnam);

	/*
 	 * now we define service type and framed protocol
 	 */

	av_type = PW_FRAMED;
  	rc_avpair_add (&send, PW_SERVICE_TYPE, &av_type, 0);
	
  	av_type = PW_PPP;
  	rc_avpair_add (&send, PW_FRAMED_PROTOCOL, &av_type, 0);
	
	/* 
 	 * for RADIUS we login in as username@realm eventualy so
 	 * here we add an @realm part of username if not already
 	 * specified.
 	 */
 	
 	strncpy (lns[linkunit].username, user, MAXUSERNAME); 
	strncpy (username_realm, user, MAXUSERNAME);
	default_realm = rc_conf_str ("default_realm");
	
	if ((strchr(username_realm, '@') == NULL) && default_realm && (*default_realm != '\0'))
	  {
	    strncat (username_realm, "@", MAXUSERNAME);
	    strncat (username_realm, default_realm,
		     MAXUSERNAME);
	    
	  }
	lns[linkunit].username[MAXUSERNAME] = '\0';
	username_realm[MAXUSERNAME] = '\0';

	/*
 	 * we are sending username and password to RADIUS
 	 */
 	 
	rc_avpair_add (&send, PW_USER_NAME, username_realm, 0);

	/*
	 * add the CHAP-Password and CHAP-Challenge fields 
	 */
	 
	cpassword[0] = cstate->chal_id;
	memcpy(&cpassword[1], remmd, MD5_SIGNATURE_SIZE);

	rc_avpair_add(&send, PW_CHAP_PASSWORD, cpassword, MD5_SIGNATURE_SIZE + 1);
	rc_avpair_add(&send, PW_CHAP_CHALLENGE, cstate->challenge, cstate->chal_len); 
	 
	
	/*
 	 * make authentication with RADIUS server
 	 */
 	 
  	result = rc_auth (client_port, send, &received, radius_msg);

 	 
  	if (result == OK_RC) {

		if (radius_setparams(received) < 0)
			result = ERROR_RC;
		else {
			radius_in = TRUE;
			radius_acct_start(linkunit);
		}
		
		rc_avpair_free(received);
	}
	
	/* free value pairs */
	rc_avpair_free (send);

#ifdef RADIUS_WTMP_LOGGING
	if (result == OK_RC)
		radius_wtmp_logging(user);
#endif
	
	return (result == OK_RC)?0:(-1);
}

struct ifstats {
  long rx_bytes;
  long rx_packets;
  long rx_errors;
  long rx_dropped;
  long rx_fifo_errors;
  long rx_frame_errors;

  long tx_bytes;
  long tx_packets;
  long tx_errors;
  long tx_dropped;
  long tx_fifo_errors;
  long collisions;
  long tx_carrier_errors;
};

static int __inline__ isspace(unsigned char c)
{
  return c == ' ' || c == '\t' || c == '\n';
}


static void if_getipacct(char* ifname, struct ifstats* ifs)
{
	FILE* f = fopen("/proc/net/ip_acct","r");
	char  buf[256];
	char  acctif[128];
	long   dummy;
	long  direction;
	long   packets;
	long   bytes;
	int   rc;

	fprintf(stderr,"if_getipacct called\n");
	if (!f){
		perror("Cannot open /proc/net/ip_acct");
		return;
	}
	fgets(buf, sizeof(buf), f);
	acctif[0] = '\0';
	while (1){
		rc = fscanf(f, "%ld/%ld->%ld/%ld %s %ld %ld %ld %ld %ld %ld", &dummy,&dummy,&dummy,&dummy,acctif,&dummy, &direction, &dummy, &dummy, &packets, &bytes);
		fgets(buf, sizeof(buf), f);
		fprintf(stderr,"fscanf returns %d\n", rc);
		fprintf(stderr,"Interface <%s>\n", acctif);
		if (rc == EOF){
			break;
		}
		if (rc != 11){
			break;
		}
		if (strcmp(ifname, acctif) == 0) {
			fprintf(stderr,"Interface <%s> found\n",ifname);
			if (direction == 1000) { /* incoming bytes */
				fprintf(stderr,"Incoming bytes/packets = %ld/%ld\n", bytes, packets);
				ifs->rx_bytes = bytes;
				ifs->rx_packets = packets;
			} else {
				fprintf(stderr,"Outgoing bytes/packets = %ld/%ld\n", bytes, packets);
				ifs->tx_bytes = bytes;
				ifs->tx_packets = packets;
			}
		}
	}
	fclose(f);
}

static void if_getstats(char *ifname, struct ifstats *ife)
{
  FILE *f = fopen("/proc/net/dev", "r");
  char buf[256];
  int have_byte_counters = 0;
  char *bp;
  
  fprintf(stderr,"@mla@: Reading statistics for interface <%s>\n", ifname);
  if (f==NULL)
    return;
  fgets(buf, 255, f);  /* throw away first line of header */
  fgets(buf, 255, f);
  if (strstr(buf, "bytes")) have_byte_counters=1;
  while(fgets(buf,255,f)) {
    bp=buf;
    while(*bp&&isspace(*bp))
      bp++;
    if(strncmp(bp,ifname,strlen(ifname))==0 && bp[strlen(ifname)]==':') {
      bp=strchr(bp,':');
      bp++;
      if (have_byte_counters) {
	sscanf(bp,"%ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld",
	       &ife->rx_bytes,
	       &ife->rx_packets,
	       &ife->rx_errors,
	       &ife->rx_dropped,
	       &ife->rx_fifo_errors,
	       &ife->rx_frame_errors,
	       
	       &ife->tx_bytes,
	       &ife->tx_packets,
	       &ife->tx_errors,
	       &ife->tx_dropped,
	       &ife->tx_fifo_errors,
	       &ife->collisions,
	       
	       &ife->tx_carrier_errors
	       );
      } else {
	sscanf(bp,"%ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld",
	       &ife->rx_packets,
	       &ife->rx_errors,
	       &ife->rx_dropped,
	       &ife->rx_fifo_errors,
	       &ife->rx_frame_errors,
	       
	       &ife->tx_packets,
	       &ife->tx_errors,
	       &ife->tx_dropped,
	       &ife->tx_fifo_errors,
	       &ife->collisions,
	       
	       &ife->tx_carrier_errors
	       );
	ife->rx_bytes = 0;
	ife->tx_bytes = 0;
      }
      break;
    }
  }
  fclose(f);
}


static void
radius_acct_start(int linkunit)
{
	UINT4 av_type;
	int result;
	VALUE_PAIR *send = NULL;
	struct ifstats ifstats;
	
	lns[linkunit].start_time = time (NULL);
	/*if_getstats(lns[linkunit].ifname, &ifstats); */
	if_getipacct(lns[linkunit].ifname, &ifstats);
	lns[linkunit].rx_bytes = ifstats.rx_bytes;
	lns[linkunit].tx_bytes = ifstats.tx_bytes;
	
	/* generate an id for this session */
	strncpy (lns[linkunit].session_id, rc_mksid (), MAXSESSIONID);
	rc_avpair_add (&send, PW_ACCT_SESSION_ID, lns[linkunit].session_id, 0);
	
	rc_avpair_add (&send, PW_USER_NAME, lns[linkunit].username, 0);

	av_type = PW_STATUS_START;
	rc_avpair_add (&send, PW_ACCT_STATUS_TYPE, &av_type, 0);
      
	av_type = PW_FRAMED;
	rc_avpair_add (&send, PW_SERVICE_TYPE, &av_type, 0);
      
	av_type = PW_PPP;
	rc_avpair_add (&send, PW_FRAMED_PROTOCOL, &av_type, 0);

	av_type = PW_RADIUS;
	rc_avpair_add (&send, PW_ACCT_AUTHENTIC, &av_type, 0);
      
	result = rc_acct (client_port, send);

	rc_avpair_free(send);
	
    if (result != OK_RC) {
	  /* RADIUS server could be down so make this a warning */
	  syslog (LOG_WARNING, "RADIUS accounting START failed for %s", lns[linkunit].username);
	}
}

void
radius_acct_stop (int linkunit)
{
	UINT4 av_type;
	int result;
	VALUE_PAIR *send = NULL;
	struct ifstats ifstats;
	
	/* if_getstats(lns[linkunit].ifname, &ifstats); */
	if_getipacct(lns[linkunit].ifname, &ifstats);
	
	rc_avpair_add (&send, PW_ACCT_SESSION_ID, lns[linkunit].session_id, 0);
	
	rc_avpair_add (&send, PW_USER_NAME, lns[linkunit].username, 0);

	av_type = PW_STATUS_STOP;
	rc_avpair_add (&send, PW_ACCT_STATUS_TYPE, &av_type, 0);
	
	av_type = PW_FRAMED;
	rc_avpair_add (&send, PW_SERVICE_TYPE, &av_type, 0);

	av_type = PW_PPP;
	rc_avpair_add (&send, PW_FRAMED_PROTOCOL, &av_type, 0);

	av_type = PW_RADIUS;
	rc_avpair_add (&send, PW_ACCT_AUTHENTIC, &av_type, 0);
	
	av_type = time (NULL) - lns[linkunit].start_time;
	rc_avpair_add (&send, PW_ACCT_SESSION_TIME, &av_type, 0);

	av_type = ifstats.tx_bytes - lns[linkunit].tx_bytes;
	rc_avpair_add(&send, PW_ACCT_OUTPUT_OCTETS, &av_type, 0);

	av_type = ifstats.rx_bytes - lns[linkunit].rx_bytes;
	rc_avpair_add(&send, PW_ACCT_INPUT_OCTETS, &av_type, 0);
	
	result = rc_acct (client_port, send);

	rc_avpair_free(send);

	if (result != OK_RC)
    {
      syslog(LOG_ERR, "RADIUS accounting STOP failed (%s)", lns[linkunit].username);
    }
    
    /* mark RADIUS as down */
	radius_in = FALSE;
}
