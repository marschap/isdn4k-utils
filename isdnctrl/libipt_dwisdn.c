

/* 
** Shared library add-on to iptables to add dwisdn matching support. 
** most parts are copied from libipt_mark.c 
** thanks for this
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>

#include <iptables.h>
#include <linux/config.h>

static const char *MY_NETFILTER_VERSION = NETFILTER_VERSION;

#if ((CONFIG_ISDN || CONFIG_ISDN_MODULE) &&  			\
		CONFIG_ISDN_WITH_ABC_IPTABLES_NETFILTER)


#define IPT_ISDN_DWISDN_H_NEED_OPTS 1
#include <linux/isdn_dwabc.h>
#define DWISDN_INSTALLED 1

#else 
#undef DWISDN_INSTALLED
#endif



/* Function which prints out usage message. */
static void help(void)
{
	printf(
	"dwisdn v%d options: (MAX %d option's per rule) iptables-vers. %s\n"
	"\tThe options below will be parsed from left to right\n"

	"[!] --con_stat    [ o | d | c ] \n"
	"\to == offline (not connected or dialing)\n"
	"\td == dialing (not connected or offline)\n"
	"\tc == connect (not dialing or offline)\n"
	"\todc as one option are ORE'd\n"
	"\t--con_stat cd == connected or dialing\n"
	"\tmultiple --con_stat are AND'ed\n"
	"\t--con_stat d --con_stat o == dialing and offline\n"
	"\tperhaps it's never true but  \n"
	"\t--con_stat d ! --con_stat o \n"
	"\tis sometimes true\n"

	"[!] --in_dev     no values possible\n"
	"\tuse in-interface for options follow this\n"
	"\tdefault the kernel will use the out-interface\n"
	"\twith ! --in_dev you can switch back to out-interface\n"
	"\tthis is only usefull in a FORWARD or INPUT rule\n"

	"[!] --charge     X ( X == numeric charge value )\n"
	"\ttrue if the internal charge-counter >= X\n"
	"\twith ! the rule is true if charge-counter < X\n"
	"\ta range is possible with\n"
	"\t--charge 25 ! --charge 50\n"
	"\ttrue if the charge-counter >= 25 and < 50\n"

	"[!] --outgoing   no values possible\n"
	"\ttrue if the interface is dialing or connected as\n"
	"\toutgoing call.\n"
	"\twith ! true if the interface not dialing or \n"
	"\tconnected as incoming call\n"

	"[!] --cbout      no values possible\n"
	"\ttrue if the interface configured for callback out\n"
	"\twith ! the interface must be configured for callback in\n"

	"[!] --dialmode   auto | manual | off\n"
	"\ttrue if the interface configured with this dialmode\n"
	"\t! is only the negation\n"

	"[!] --addr_ok    no values possible\n"
	"\tonly for dynip-interfaces.\n"
	"\ttrue if the interface is online and the ip-addr is set.\n"
	"\talways true for all other interfaces.\n"

	"[!] --f_eq_iadr  no values possible\n"
	"\tonly for dynip-interfaces.\n"
	"\ttrue if the interface is online, ip-addr is set and\n"
	"\tthe frame source-addr and interface-addr are equal.\n"
	"\talways true for all other interfaces.\n"
	"\n",
#ifdef DWISDN_INSTALLED
	IPTDWISDN_REVISION,
	IPTDWISDN_MAXOPS,
#else
	0,
	0,
#endif
	MY_NETFILTER_VERSION);
}


#ifdef DWISDN_INSTALLED

/* Initialize the match. */
static void init(struct ipt_entry_match *m, unsigned int *nfcache)
/************************************************************************
*************************************************************************/
{

	if(m != NULL && m->data != NULL)
		memset(m->data,0,sizeof(IPTDWISDN_INFO));

	/* Can't cache this. */
	*nfcache |= NFC_UNKNOWN;
}



/* Function which parses command options; returns true if it
   ate an option */

static int parse( 	int c,
					char **argv,
					int invert,
					unsigned int *flags,
      				const struct ipt_entry *entry,
      				unsigned int *nfcache,
      				struct ipt_entry_match **match)
/*************************************************************************
**************************************************************************/
{
	IPTDWISDN_INFO 	*dw = (IPTDWISDN_INFO *)(*match)->data;
	char *p = NULL;
	int idx;

	if((idx = dw->parcount) >= IPTDWISDN_MAXOPS) {

		exit_error( PARAMETER_PROBLEM,
		   "dwisdn match: to many parameters.");
	}

	dw->revision = IPTDWISDN_REVISION;
	dw->inst[idx] = c;
	dw->value[idx] = 0;

	switch ((enum iptdwisdn)c) {
	default: return 0;

	case IPT_DWISDN_DIALMODE:

		if(check_inverse(optarg, &invert))
			optind++;

		if(!strcmp(optarg,"auto")) 
			dw->value[idx] = 1;
		else if(!strcmp(optarg,"manual")) 
			dw->value[idx] = 2;
		else if(!strcmp(optarg,"off")) 
			dw->value[idx] = 3;
		else {

			exit_error( PARAMETER_PROBLEM,
				"dwisdn match --dialmode usage: auto | manual | off");
		}

		break;
		
	case IPT_DWISDN_CBOUT:
	case IPT_DWISDN_IDEV:
	case IPT_DWISDN_ADDROK:
	case IPT_DWISDN_OUTGOING:
	case IPT_DWISDN_FEQIADR:

		if(check_inverse(optarg, &invert))
			optind++;

		break;

	case IPT_DWISDN_CHARGE:

		if(check_inverse(optarg, &invert))
			optind++;

		for(p = (char *)optarg; *p; p++) {

			if(*p < '0' || *p > '9') {

				exit_error( PARAMETER_PROBLEM,
					"dwisdn match --charge x ( x must be nemueric )");
			}
		}

		dw->value[idx] = atol(optarg);
		break;

	case IPT_DWISDN_CON:

		if(check_inverse(optarg, &invert))
			optind++;

		for(p = (char *)optarg; *p; p++) {

			switch(*p) {

			default:

			exit_error( PARAMETER_PROBLEM,
				"dwisdn match con_stat: Option %c unknown",*p);

			case ' ':
			case '\t':	continue;

			case 'O':
			case 'o':

				dw->value[idx] |= IPTCS_DWISN_OFFL;
				break;

			case 'D':
			case 'd':

				dw->value[idx] |= IPTCS_DWISN_DIAL;
				break;

			case 'C':
			case 'c':

				dw->value[idx] |= IPTCS_DWISN_ONL;
				break;
			}
		}
	}

	*flags = 1;

	if(invert)
		dw->inst[idx] |= IPT_DWISDN_NOT;

	dw->parcount++;

	return(1);
}

static void final_check(unsigned int flags)
/************************************************************************
*************************************************************************/
{ }


static void myprint(IPTDWISDN_INFO *p)
/*************************************************************************
*************************************************************************/
{
	int shl = 0;

	for(;shl < IPTDWISDN_MAXOPS && shl < p->parcount;shl++) {

		int inst = p->inst[shl];
		u_long v = p->value[shl];

		if(inst & IPT_DWISDN_NOT) 
			printf("! ");

		inst &= ~IPT_DWISDN_NOT;

		printf("--%s ",IPT_dwisdn_opts[inst-1].name);

		if(!v)
			continue;

		switch(inst)  {

		case IPT_DWISDN_CHARGE:

			printf("%lu",v);
			break;

		case IPT_DWISDN_CON:
		
			if(v & IPTCS_DWISN_OFFL)
				printf("o");

			if(v & IPTCS_DWISN_DIAL)
				printf("d");

			if(v & IPTCS_DWISN_ONL)
				printf("c");

			break;
		}

		printf(" ");
	}
}

/* Prints out the matchinfo. */
static void print(	const struct ipt_ip *ip,
      				const struct ipt_entry_match *match,
      				int numeric)
/*************************************************************************
**************************************************************************/
{
	IPTDWISDN_INFO *dw = (IPTDWISDN_INFO *)match->data;

	printf("dwisdn ");
	myprint(dw);
}

/* Saves the union ipt_dwisdninfo in parsable form to stdout. */
static void save(const struct ipt_ip *ip, const struct ipt_entry_match *match)
/***************************************************************************
***************************************************************************/
{
	IPTDWISDN_INFO *dw = (IPTDWISDN_INFO *)match->data;
	myprint(dw);
}

#else 

typedef struct DUMMY_ISDN {

	long dummy[10];

} IPTDWISDN_INFO;


static struct option IPT_dwisdn_opts[] = {
	 {0}
};                                                                              

static void init(struct ipt_entry_match *m, unsigned int *nfcache) {}

static void final_check(unsigned int flags)
/************************************************************************
*************************************************************************/
{ 
	exit_error( PARAMETER_PROBLEM,
		"dwisdn: Kernel lacks ISDN- or ISDN-IPTABLES-SUPPORT");
}


static int parse( 	int c,
					char **argv,
					int invert,
					unsigned int *flags,
      				const struct ipt_entry *entry,
      				unsigned int *nfcache,
      				struct ipt_entry_match **match)
/*************************************************************************
**************************************************************************/
{
	final_check(0);
	return(0);
}


static void print(	const struct ipt_ip *ip,
      				const struct ipt_entry_match *match,
      				int numeric)
/*************************************************************************
**************************************************************************/
{ }

static void save(const struct ipt_ip *ip, const struct ipt_entry_match *match)
/***************************************************************************
***************************************************************************/
{ }


#endif


struct iptables_match dwisdn
= { NULL,
    "dwisdn",
    NETFILTER_VERSION,
    IPT_ALIGN(sizeof(IPTDWISDN_INFO)),
    IPT_ALIGN(sizeof(IPTDWISDN_INFO)),
    &help,
    &init,
    &parse,
    &final_check,
    &print,
    &save,
   	IPT_dwisdn_opts,
};



void _init(void)
/***************************************************************************
***************************************************************************/
{ 
	char *xx = getenv("NETFILTER_VERSION");

	if(xx != NULL) {

		MY_NETFILTER_VERSION = 
		dwisdn.version = xx;
	}

	register_match(&dwisdn);
}

