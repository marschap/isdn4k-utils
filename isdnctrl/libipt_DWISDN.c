
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

#if ((CONFIG_ISDN || CONFIG_ISDN_MODULE) &&  			\
		CONFIG_ISDN_WITH_ABC_IPTABLES_NETFILTER)


#define IPT_ISDN_DWISDN_TIPTH_NEED_OPTS 1
#include <linux/isdn_dwabc.h>
#define DWISDN_INSTALLED 1

#else 
#undef DWISDN_INSTALLED
#endif



/* Function which prints out usage message. */
static void help(void)
{
	printf(
	"DWISDN v%d options: (MAX %d option's per rule) iptables-vers. %s\n"
	"\tThe options below will be parsed from left to right\n"

	"--clear      no values\n"
	"\tclear frame-bit's for dont reset huptimer and destination unreach\n"

	"--huptimer   no values\n"
	"\tset frame-bit for dont reset huptimer\n"

	"--unreach    no values\n"
	"\tset frame-bit for destination unreach in case of frames with\n"
	"\tdont reset huptimer bit and interface not online\n"

	"[!] --in_dev no values\n"
	"\tuse in-interface for options follow this\n"
	"\tdefault the kernel will use the out-interface\n"
	"\twith ! --in_dev you can switch back to out-interface\n"
	"\tthis is only usefull in a FORWARD or INPUT rule.\n"

	"--dial       no values\n"
	"\ttrigger dialing even of manual- or off-dialmode\n"

	"--hangup     no values\n"
	"\thangup the line immediately\n"

	"--dialmode   auto | manual | off\n"
	"\tset the interface dialmode to auto, manual or off\n"

	"--hupreset   no values\n"
	"\treset the interface huptimer. Note ! You must say which\n"
	"\tinterface ? in or out. This depends on the hook you use.\n"
	"\tonly --hupreset will use the out interface.\n"
	"\t--in_dev --hupreset  use the in  interface.\n"

	"\n",
#ifdef DWISDN_INSTALLED
	IPTDWISDN_REVISION,
	IPTDWISDN_MAXOPS,
#else
	0,
	0,
#endif
	NETFILTER_VERSION);
}


#ifdef DWISDN_INSTALLED

/* Initialize the traget. */
static void init(struct ipt_entry_target *m, unsigned int *nfcache)
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
      				struct ipt_entry_target **target)
/*************************************************************************
**************************************************************************/
{
	IPTDWISDN_INFO 	*dw = (IPTDWISDN_INFO *)(*target)->data;
	int idx;

	if((idx = dw->parcount) >= IPTDWISDN_MAXOPS) {

		exit_error( PARAMETER_PROBLEM,
		   "DWISDN target: to many parameters.");
	}

	dw->revision = IPTDWISDN_REVISION;
	dw->inst[idx] = c;
	dw->value[idx] = 0;

	switch ((enum iptdwisdn)c) {
	default:
	
		exit_error( PARAMETER_PROBLEM,
			"DWISDN target: unkown option <%s>",optarg);

		return 0;

	case TIPT_DWISDN_DIAL:
	case TIPT_DWISDN_HANGUP:
	case TIPT_DWISDN_CLEAR:
	case TIPT_DWISDN_SET:
	case TIPT_DWISDN_UNREACH:
	case TIPT_DWISDN_HUPRESET:

		if(check_inverse(optarg, &invert))
			optind++;

		if(invert) 
			exit_error( PARAMETER_PROBLEM,
				"DWISDN target --%s: ! not allowed",
					TIPT_dwisdn_opts[c-1].name);

		break;

	case TIPT_DWISDN_DIALMODE:

		if(check_inverse(optarg, &invert))
			optind++;

		if(invert) 
			exit_error( PARAMETER_PROBLEM,
				"DWISDN target --dialmode: ! not allowed");

		if(!strcmp(optarg,"auto")) 
			dw->value[idx] = 1;
		else if(!strcmp(optarg,"manual")) 
			dw->value[idx] = 2;
		else if(!strcmp(optarg,"off")) 
			dw->value[idx] = 3;
		else {

			exit_error( PARAMETER_PROBLEM,
				"DWISDN target --dialmode usage: auto | manual | off");
		}

		break;
	}

	*flags = 1;

	if(invert)
		dw->inst[idx] |= TIPT_DWISDN_NOT;

	dw->parcount++;

	return(1);
}

static void final_check(unsigned int flags)
/************************************************************************
*************************************************************************/
{ 

	if(!flags)
		exit_error( PARAMETER_PROBLEM,
			"DWISDN target: without any option ? Why ?? :-(");
}


static void myprint(IPTDWISDN_INFO *p)
/*************************************************************************
*************************************************************************/
{
	int shl = 0;

	for(;shl < IPTDWISDN_MAXOPS && shl < p->parcount;shl++) {

		int inst = p->inst[shl];
		u_long v = p->value[shl];

		if(inst & TIPT_DWISDN_NOT) 
			printf("! ");

		inst &= ~TIPT_DWISDN_NOT;

		printf("--%s ",TIPT_dwisdn_opts[inst-1].name);

		if(!v)
			continue;

		switch(inst)  {

		case TIPT_DWISDN_DIALMODE:

			switch(v) {

			case 1:		printf("auto");				break;
			case 2:		printf("manual");			break;
			case 3:		printf("off");				break;
			}
		}

		printf(" ");
	}
}

/* Prints out the matchinfo. */
static void print(	const struct ipt_ip *ip,
      				const struct ipt_entry_target *target,
      				int numeric)
/*************************************************************************
**************************************************************************/
{
	IPTDWISDN_INFO *dw = (IPTDWISDN_INFO *)target->data;

	printf("DWISDN ");
	myprint(dw);
}

/* Saves the union ipt_dwisdninfo in parsable form to stdout. */
static void save(const struct ipt_ip *ip, const struct ipt_entry_target *target)
/***************************************************************************
***************************************************************************/
{
	IPTDWISDN_INFO *dw = (IPTDWISDN_INFO *)target->data;
	myprint(dw);
}

#else 

typedef struct DUMMY_ISDN {

	long dummy[10];

} IPTDWISDN_INFO;


static struct option TIPT_dwisdn_opts[] = {
	 {0}
};                                                                              

static void init(struct ipt_entry_target *m, unsigned int *nfcache) {}

static void final_check(unsigned int flags)
/************************************************************************
*************************************************************************/
{ 
	exit_error( PARAMETER_PROBLEM,
		"DWISDN target: Kernel lacks ISDN- or ISDN-IPTABLES-SUPPORT");
}


static int parse( 	int c,
					char **argv,
					int invert,
					unsigned int *flags,
      				const struct ipt_entry *entry,
      				struct ipt_entry_target **target)
/*************************************************************************
**************************************************************************/
{
	final_check(0);
	return(0);
}


static void print(	const struct ipt_ip *ip,
      				const struct ipt_entry_target *target,
      				int numeric)
/*************************************************************************
**************************************************************************/
{ }

static void save(const struct ipt_ip *ip, const struct ipt_entry_target *target)
/***************************************************************************
***************************************************************************/
{ }


#endif

static struct iptables_target DWISDN
= { NULL,
    "DWISDN",
    NETFILTER_VERSION,
    IPT_ALIGN(sizeof(IPTDWISDN_INFO)),
    IPT_ALIGN(sizeof(IPTDWISDN_INFO)),
    &help,
    &init,
    &parse,
    &final_check,
    &print,
    &save,
   	TIPT_dwisdn_opts,
};



void _init(void)
/***************************************************************************
***************************************************************************/
{ 
	register_target(&DWISDN);
}

