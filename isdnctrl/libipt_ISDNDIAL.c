
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

static struct option TIPT_ISDNDIAL_opts[] = { {0} };                                                                              


/* Function which prints out usage message. */
static void help(void)
{
	printf(
	"ISDNDIAL v%d options: (MAX %d option's per rule) iptables-vers. %s\n"
	"\tonly useable in nat filter on POSTROUTING hook.\n"
	"\tit's support dynamic ip-addr's for syncppp interfaces.\n"
	"\tfor all other interfaces this target is transparent.\n"
	"\tif the interface is offline this target will trigger dialing\n"
	"\tand save all frames that start a connection for later transmitt.\n"
	"\tif the interface comes up and the new ip-addr ist set correctly,\n"
	"\tthe save'd frames are send immediately.\n"
	"\tNOTE:\n"
	"\tthe best place for the ISDNDIAL target is before any\n"
	"\tMASQUERADE target.\n"
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


/* Initialize the traget. */
static void init(struct ipt_entry_target *m, unsigned int *nfcache)
/************************************************************************
*************************************************************************/
{
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
	exit_error( PARAMETER_PROBLEM,"ISDNDIAL target: No options allowed");
	return(1);
}

static void final_check(unsigned int flags)
/************************************************************************
*************************************************************************/
{ 
}


/* Prints out the matchinfo. */
static void print(	const struct ipt_ip *ip,
      				const struct ipt_entry_target *target,
      				int numeric)
/*************************************************************************
**************************************************************************/
{
	printf("ISDNDIAL ");
}

/* Saves the union ipt_dwisdninfo in parsable form to stdout. */
static void save(const struct ipt_ip *ip, const struct ipt_entry_target *target)
/***************************************************************************
***************************************************************************/
{
}


static struct iptables_target ISDNDIAL
= { NULL,
    "ISDNDIAL",
    NETFILTER_VERSION,
    0,
    0,
    &help,
    &init,
    &parse,
    &final_check,
    &print,
    &save,
   	TIPT_ISDNDIAL_opts,
};



void _init(void)
/***************************************************************************
***************************************************************************/
{ 
	register_target(&ISDNDIAL);
}

