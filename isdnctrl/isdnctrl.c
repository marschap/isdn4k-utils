/* $Id: isdnctrl.c,v 1.4 1997/06/24 23:35:26 luethje Exp $
 * ISDN driver for Linux. (Control-Utility)
 *
 * Copyright 1994,95 by Fritz Elfert (fritz@wuemaus.franken.de)
 * Copyright 1995 Thinking Objects Software GmbH Wuerzburg
 *
 * This file is part of Isdn4Linux.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * $Log: isdnctrl.c,v $
 * Revision 1.4  1997/06/24 23:35:26  luethje
 * isdnctrl can use a config file
 *
 * Revision 1.3  1997/06/22 11:58:21  fritz
 * Added ability to adjust slave triggerlevel.
 *
 * Revision 1.2  1997/03/10 09:51:24  fritz
 * Bugfix: mapping was broken.
 *
 * Revision 1.1  1997/02/17 00:09:21  fritz
 * New CVS tree
 *
 * Revision 1.14  1996/06/06 22:08:46  fritz
 * Bugfix: verbose and getconf checked wrong number of parameters.
 * thanks to Andreas Jaeger <aj@arthur.pfalz.de>
 *
 * Revision 1.13  1996/04/30 12:48:35  fritz
 * Added Michael's ippp-bind patch.
 *
 * Revision 1.12  1996/04/30 12:43:18  fritz
 * Changed ioctl-names according to kernel-version
 *
 * Revision 1.11  1996/01/04 02:44:52  fritz
 * Changed copying policy to GPL
 * Added addslave, dial, sdelay and mapping.
 *
 * Revision 1.10  1995/12/18  18:03:19  fritz
 * New License, minor cleanups.
 *
 * Revision 1.9  1995/10/29  21:38:51  fritz
 * Changed all references to driver-numbers to new DriverId's
 *
 * Revision 1.8  1995/07/15  20:39:56  fritz
 * Added support for cisco_h Encapsulation.
 * Added suppurt for pre-binding an interface to a channel.
 *
 * Revision 1.7  1995/04/29  13:13:44  fritz
 * Added new command verbose.
 *
 * Revision 1.6  1995/04/23  13:38:34  fritz
 * Adapted addphone and delphone to support changes in isdn.c
 *
 * Revision 1.5  1995/03/25  23:35:35  fritz
 * Added ihup-Feature.
 *
 * Revision 1.4  1995/03/15  12:44:15  fritz
 * Added generic conversion-routines for keyword<->value conversion.
 * Added display of phone-numbers in list-routine
 * Corrected some typos
 *
 * Revision 1.3  1995/02/20  03:38:59  fritz
 * Added getmax and rmax for performance-tests of tty-driver.
 *
 * Revision 1.2  1995/01/29  23:27:52  fritz
 * Added keywords: list, l2_proto, l3_proto, huptimeout, chargehup and
 * encap.
 *
 * Revision 1.1  1995/01/09  07:35:35  fritz
 * Initial revision
 *
 *
 */

#undef  ISDN_DEBUG_MODEM_SENDOPT

#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include <linux/isdn.h>
#include <linux/isdnif.h>

#define _ISDNCTRL_C_
#include "isdnctrl.h"

#ifdef I4L_CTRL_CONF
#	include "../lib/libisdn.h"
#	include "ctrlconf.h"
#endif /* I4L_CTRL_CONF */

char nextlistif[10];

void usage(void)
{
        fprintf(stderr, "%s version %s\n", cmd, VERSION);
        fprintf(stderr, "usage: %s <command> <options>\n", cmd);
        fprintf(stderr, "\n");
        fprintf(stderr, "where <command> is one of the following:\n");
        fprintf(stderr, "\n");
        fprintf(stderr, "    addif [name]               add net-interface\n");
        fprintf(stderr, "    delif name                 remove net-interface\n");
        fprintf(stderr, "    addphone name in|out num   add phone-number to interface\n");
        fprintf(stderr, "    delphone name in|out num   remove phone-number from interface\n");
        fprintf(stderr, "    eaz name [eaz|msn]         get/set eaz for interface\n");
        fprintf(stderr, "    huptimeout name [seconds]  get/set hangup-timeout for interface\n");
        fprintf(stderr, "    ihup name [on|off]         get/set incoming-hangup for interface\n");
        fprintf(stderr, "    chargehup name [on|off]    get/set charge-hangup for interface\n");
        fprintf(stderr, "    chargeint name [seconds]   get/set charge-interval if not given by telco\n");
        fprintf(stderr, "    secure name [on|off]       get/set secure-feature for interface\n");
        fprintf(stderr, "    callback name [in|outon|off]\n");
        fprintf(stderr, "                               get/set active callback-feature for interface\n");
        fprintf(stderr, "    cbhup name [on|off]        get/set reject-before-callback for interface\n");
        fprintf(stderr, "    cbdelay name [seconds]     get/set delay before callback for interface\n");
        fprintf(stderr, "    dialmax name [num]         get/set number of dial-atempts for interface\n");
        fprintf(stderr, "    encap name [encapname]     get/set packet-encapsulation for interface\n");
        fprintf(stderr, "    l2_prot name [protocol]    get/set layer-2-protocol for interface\n");
        fprintf(stderr, "    l3_prot name [protocol]    get/set layer-3-protocol for interface\n");
        fprintf(stderr, "    bind name [drvId,channel [exclusive]]\n");
        fprintf(stderr, "                               pre-bind interface to a channel\n");
        fprintf(stderr, "    unbind name                delete pre-binding\n");
        fprintf(stderr, "    list name|all              show current setup of interface(s)\n");
        fprintf(stderr, "    verbose num                set verbose-level\n");
        fprintf(stderr, "    hangup name                force hangup of interface\n");
        fprintf(stderr, "    busreject drvId on|off     set bus-reject-mode\n");
        fprintf(stderr, "    mapping drvId [MSN,MSN...] set MSN<->EAZ-Mapping\n");
        fprintf(stderr, "    addslave name slavename    add slave-interface\n");
        fprintf(stderr, "    sdelay mastername delay    set slave-activation delay\n");
        fprintf(stderr, "    trigger mastername cps     set slave trigger level\n");
        fprintf(stderr, "    dial name                  force dialing of interface\n");
        fprintf(stderr, "    system on|off              switch isdn-system on or off\n");
        fprintf(stderr, "    addlink name               MPPP, increase number of links\n");
        fprintf(stderr, "    removelink name            MPPP, decrease number of links\n");
        fprintf(stderr, "    pppbind name [devicenum]   PPP, bind interface to ippp-device (exclusive)\n");
        fprintf(stderr, "    pppunbind name             PPP, remove ippp-device binding\n");
#ifdef I4L_CTRL_CONF
        fprintf(stderr, "    writeconf [file]           write the settings to file\n");
        fprintf(stderr, "    readconf [file]            read the settings from file\n");
#endif /* I4L_CTRL_CONF */

        exit(-2);
}

int key2num(char *key, char **keytable, int *numtable)
{
        int i = -1;
        while (strlen(keytable[++i]))
                if (!strcmp(keytable[i], key))
                        return numtable[i];
        return -1;
}

char * num2key(int num, char **keytable, int *numtable)
{
        int i = -1;
        while (numtable[++i] >= 0)
                if (numtable[i] == num)
                        return keytable[i];
        return "???";
}

static void listbind(char *s, int e)
{
        if (strlen(s)) {
                char *p = strchr(s, ',');
                int ch;
                sscanf(p + 1, "%d", &ch);
                *p = '\0';
                printf("%s, Channel %d%s\n", s, ch, (e > 0) ? ", exclusive" : "");
        } else
                printf("Nothing\n");
}

static void listif(int isdnctrl, char *name, int errexit)
{
        isdn_net_ioctl_cfg cfg;
        union p {
                isdn_net_ioctl_phone phone;
                char n[1024];
        } ph;
        char nn[1024];

        strcpy(cfg.name, name);
        if (ioctl(isdnctrl, IIOCNETGCF, &cfg) < 0) {
                if (errexit) {
                        perror(name);
                        exit(-1);
                } else
                        return;
        }
        strcpy(ph.phone.name, name);
        ph.phone.outgoing = 0;
        if (ioctl(isdnctrl, IIOCNETGNM, &ph.phone) < 0) {
                if (errexit) {
                        perror(name);
                        exit(-1);
                } else
                        return;
        }
        strcpy(nn, ph.n);
        strcpy(ph.phone.name, name);
        ph.phone.outgoing = 1;
        if (ioctl(isdnctrl, IIOCNETGNM, &ph.phone) < 0) {
                if (errexit) {
                        perror(name);
                        exit(-1);
                } else
                        return;
        }
        printf("\nCurrent setup of interface '%s':\n\n", cfg.name);
        printf("EAZ/MSN:                %s\n", cfg.eaz);
        printf("Phone number(s):\n");
        printf("  Outgoing:	            %s\n", ph.n);
        printf("  Incoming:	            %s\n", nn);
        printf("Secure:        	        %s\n", cfg.secure ? "on" : "off");
        printf("Callback:               %s\n", num2callb[cfg.callback]);
        if (cfg.callback == 2)
                printf("Hangup after Dial       %s\n", cfg.cbdelay ? "on" : "off");
        else
                printf("Reject before Callback: %s\n", cfg.cbhup ? "on" : "off");
        printf("Callback-delay:         %d\n", cfg.cbdelay / 5);
        printf("Dialmax:                %d\n", cfg.dialmax);
        printf("Hangup-Timeout:         %d\n", cfg.onhtime);
        printf("Incoming-Hangup:        %s\n", cfg.ihup ? "on" : "off");
        printf("ChargeHangup:           %s\n", cfg.chargehup ? "on" : "off");
        printf("Charge-Units:           %d\n", cfg.charge);
        printf("Charge-Interval:        %d\n", cfg.chargeint);
        printf("Layer-2-Protocol:       %s\n", num2key(cfg.l2_proto, l2protostr, l2protoval));
        printf("Layer-3-Protocol:       %s\n", num2key(cfg.l3_proto, l3protostr, l3protoval));
        printf("Encapsulation:          %s\n", num2key(cfg.p_encap, pencapstr, pencapval));
        printf("Slave Interface:        %s\n", strlen(cfg.slave) ? cfg.slave : "None");
        printf("Slave delay:            %d\n", cfg.slavedelay);
        printf("Slave trigger:          %d cps\n", cfg.triggercps);
        printf("Master Interface:       %s\n", strlen(cfg.master) ? cfg.master : "None");
        printf("Pre-Bound to:           ");
        listbind(cfg.drvid, cfg.exclusive);
        printf("PPP-Bound to:           ");
        if (cfg.pppbind >= 0)
        	printf("%d\n", cfg.pppbind);
        else
        	printf("Nothing\n");

        if (cfg.slave && strlen(cfg.slave))
                strcpy(nextlistif, cfg.slave);
        else
                nextlistif[0] = '\0';
}

static void get_setup(int isdnctrl, char *name)
{
        isdn_net_ioctl_cfg cfg;
        char buffer[0x1000];
        char *p;
        FILE *f;

        if (ioctl(isdnctrl, IIOCGETSET, &buffer) < 0) {
                perror("ioctl GET_SETUP");
                exit(-1);
        }
		if (strcmp(name,"-"))
        	f = fopen(name, "w");
		else
			f = stdout;
        if (!f) {
                perror(name);
                exit(-1);
        }
        p = buffer;
		    fprintf(f,"[isdnctrl]\n");
        while (strlen(p)) {
          if (strlen(cfg.master)) {
			      		fprintf(f, "  netslave=\"%s\"\n", p);
					      fprintf(f, "    master=\"%s\"\n", p);
				} else
               	fprintf(f, "  netif=\"%s\"\n", p);
                p += 10;
                memcpy((char *) &cfg, p, sizeof(cfg));
                fprintf(f, "    eazmsn=\"%s\"\n", cfg.eaz);
                fprintf(f, "    secure=%d\n", cfg.secure);
                fprintf(f, "    callback=%d\n", cfg.callback);
                fprintf(f, "    huptimeout=%d\n", cfg.onhtime);
                fprintf(f, "    ihup=%d\n", cfg.ihup);
                fprintf(f, "    chargehup=%d\n", cfg.chargehup);
                fprintf(f, "    l2prot=%d\n", cfg.l2_proto);
                fprintf(f, "    l3prot=%d\n", cfg.l3_proto);
                fprintf(f, "    encap=%d\n", cfg.p_encap);
                fprintf(f, "    chargeint=%d\n", cfg.chargeint);
                fprintf(f, "    slavedelay=%d\n", cfg.slavedelay);
                fprintf(f, "    triggercps=%d\n", cfg.triggercps);
                fprintf(f, "    exclusive=%d\n", cfg.exclusive);
                fprintf(f, "    binddev=\"%s\"\n", cfg.drvid);
                p += sizeof(cfg);
                fprintf(f, "    outphones=\"%s\"\n", p);
                p += (strlen(p) + 1);
                fprintf(f, "    inphones=\"%s\"\n", p);
                p += (strlen(p) + 1);
        }
		if (f != stdout)
        	fclose(f);
}

int findcmd(char *str)
{
	int i;

	if (str != NULL)
		for (i = 0; cmds[i].cmd; i++)
			if (!strcmp(cmds[i].cmd, str))
				return i;

	return -1;
}

int exec_args(int fd, int argc, char **argv)
{
	int i,
	 n,
	 args;
	int result;
	FILE *iflst;
	char *p;
	char s[255];
	isdn_net_ioctl_phone phone;
	isdn_net_ioctl_cfg cfg;
	isdn_ioctl_struct iocts;
	unsigned long j;
	char nstring[255];
#ifdef I4L_CTRL_CONF
	char conffile[PATH_MAX];
#endif /* I4L_CTRL_CONF */
	char *id;
	char *arg1;
	char *arg2;


	for (; *argv != NULL; argv++, argc--) {
		if ((i = findcmd(argv[0])) < 0) {    /* Unknown command */
			fprintf(stderr, "The given command \"%s\" is unknown.\n\n", argv[0]);
			usage();
			return -1;
		}

		args = cmds[i].argno[0] - '0';
		id = argv[1];

		if (args > argc - 1) {
			fprintf(stderr, "Too few arguments given for \"%s\".\n\n", argv[0]);
			usage();
			return -1;
		}

#ifdef I4L_CTRL_CONF
		if (id != NULL && i != GETCONF && i != WRITECONF && i != READCONF) {
#else
		if (id != NULL && i != GETCONF) {
#endif /* I4L_CTRL_CONF */
			if (strlen(id) > 8) {
				fprintf(stderr, "Interface name must not exceed 8 characters!\n");
				close(fd);
				return -1;
			}
		}

		for (n = 1; cmds[i].argno[n]; n++) {
			args = cmds[i].argno[n] - '0';
			if (((args > argc - 1) || findcmd(argv[args]) >= 0)) {
				args = cmds[i].argno[n - 1] - '0';
				break;
			}
		}

		arg1 = (args > 1) ? argv[2] : "";
		arg2 = (args > 2) ? argv[3] : "";
		argc -= args;
		argv += args;

		switch (i) {
			case ADDIF:
			        strcpy(s, args?id:"");
			        if ((result = ioctl(fd, IIOCNETAIF, s)) < 0) {
			        	perror("addif");
			        	return -1;
			        }
			        printf("%s added\n", s);
			        break;

			case ADDSLAVE:
			        if (strlen(arg1) > 8) {
			        	fprintf(stderr, "slavename must not exceed 8 characters\n");
			        	return -1;
			        }
			        sprintf(s, "%s,%s", id, arg1);
			        if ((result = ioctl(fd, IIOCNETASL, s)) < 0) {
			        	perror("addslave");
			        	return -1;
			        }
			        printf("%s added as slave to %s\n", s, id);
			        break;

			case DELIF:
			        if ((result = ioctl(fd, IIOCNETDIF, id)) < 0) {
			        	perror(id);
			        	return -1;
			        }
			        printf("%s deleted\n", id);
			        break;

			case DIAL:
			        if ((result = ioctl(fd, IIOCNETDIL, id)) < 0) {
			        	perror(id);
			        	return -1;
			        }
			        printf("Dialing of %s triggered\n", id);
			        break;

			case BIND:
			        if (args == 3)
			        	if (strncmp(arg2, "excl", 4))
			        		usage();
			        strcpy(cfg.name, id);
			        if ((result = ioctl(fd, IIOCNETGCF, &cfg)) < 0) {
			        	perror(id);
			        	return -1;
			        }
			        if (args > 1)
			        {
			        	sscanf(arg1, "%s", cfg.drvid);
			        	cfg.exclusive = (args == 3);
			        	if ((result = ioctl(fd, IIOCNETSCF, &cfg)) < 0) {
			        		sprintf(s, "%s or %s", id, arg2);
			        		perror(s);
			        		return -1;
			        	}
			        }
			        printf("%s bound to ", id);
			        listbind(cfg.drvid, cfg.exclusive);
			        break;

			case UNBIND:
			        strcpy(cfg.name, id);
			        if ((result = ioctl(fd, IIOCNETGCF, &cfg)) < 0) {
			        	perror(id);
			        	return -1;
			        }
			        if (!strlen(cfg.drvid)) {
			        	printf("%s was not bound to anything\n", id);
			        	return -1;
			        }
			        cfg.drvid[0] = '\0';
			        cfg.exclusive = -1;
			        if ((result = ioctl(fd, IIOCNETSCF, &cfg)) < 0) {
			        	perror(id);
			        	return -1;
			        }
			        printf("%s unbound successfully\n", id);
			        break;

			case PPPBIND:
			        strcpy(cfg.name, id);
			        if ((result = ioctl(fd, IIOCNETGCF, &cfg)) < 0) {
                                perror(id);
                                return -1;
			        }
			        if (args > 1) {
			        	sscanf(arg1, "%d", &cfg.pppbind);
			        	if ((result = ioctl(fd, IIOCNETSCF, &cfg)) < 0) {
			        		sprintf(s, "%s or %s", id, arg1);
			        		perror(s);
			        		return -1;
              	}
			        }
			        printf("%s bound to ", id);
			        if (cfg.pppbind >= 0)
			        	printf("%d\n", cfg.pppbind);
			        else
			        	printf("nothing\n");
			        break;

			case PPPUNBIND:
			        strcpy(cfg.name, id);
			        if ((result = ioctl(fd, IIOCNETGCF, &cfg)) < 0) {
			        	perror(id);
			        	return -1;
			        }
			        if (cfg.pppbind < 0) {
			        	printf("%s was not bound to anything\n", id);
			        	return -1;
			        }
			        cfg.pppbind = -1;
			        if ((result = ioctl(fd, IIOCNETSCF, &cfg)) < 0) {
			        	perror(id);
			        	return -1;
			        }
			        printf("%s unbound successfully\n", id);
			        break;

			case BUSREJECT:
			        strcpy(iocts.drvid, id);
			        if (strcmp(arg1, "on") && strcmp(arg1, "off")) {
			        	fprintf(stderr, "Bus-Reject must be 'on' or 'off'\n");
			        	return -1;
			        }
			        iocts.arg = strcmp(arg1, "off");
			        if ((result = ioctl(fd, IIOCSETBRJ, &iocts)) < 0) {
			        	perror(id);
			        	return -1;
			        }
			        break;

			case MAPPING:
			        strcpy(iocts.drvid, id);
			        if (args == 1) {
			        	iocts.arg = (unsigned long) &nstring;
			        	if ((result = ioctl(fd, IIOCGETMAP, &iocts)) < 0) {
			        		perror(id);
			        		return -1;
			        	}
			        	printf("MSN/EAZ-mapping for %s:\n%s\n", id, nstring);
			        } else {
			        	char buf[400];
			        	strncpy(buf, arg1, sizeof(buf) - 1);
			        	iocts.arg = (unsigned long) buf;
			        	if ((result = ioctl(fd, IIOCSETMAP, &iocts)) < 0) {
			        		perror(id);
			        		return -1;
			        	}
			        }
			        break;

			case SYSTEM:
			        if (strcmp(id, "on") && strcmp(id, "off")) {
			        	fprintf(stderr, "System-Mode must be 'on' or 'off'\n");
			        	return -1;
			        }
			        j = strcmp(id, "on");
			        if ((result = ioctl(fd, IIOCSETGST, j)) < 0) {
			        	perror(id);
			        	return -1;
			        }
			        break;

			case HANGUP:
			        if ((result = ioctl(fd, IIOCNETHUP, id)) < 0) {
			        	perror(id);
			        	return -1;
			        }
			        if (result)
			        	printf("%s not connected\n", id);
			        else
			        	printf("%s hung up\n", id);
			        break;

			case ADDPHONE:
			        if (strcmp(arg1, "in") && strcmp(arg1, "out")) {
			        	fprintf(stderr, "Direction must be \"in\" or \"out\"\n");
			        	return -1;
			        }
			        phone.outgoing = strcmp(arg1, "out") ? 0 : 1;
			        if (strlen(arg2) > 20) {
			        	fprintf(stderr, "phone-number must not exceed 20 characters\n");
			        	return -1;
			        }
			        strcpy(phone.name, id);
			        strcpy(phone.phone, arg2);
			        if ((result = ioctl(fd, IIOCNETANM, &phone)) < 0) {
			        	perror(id);
			        	return -1;
			        }
			        break;

			case DELPHONE:
			        if (strcmp(arg1, "in") && strcmp(arg1, "out")) {
			        	fprintf(stderr, "Direction must be \"in\" or \"out\"\n");
			        	return -1;
			        }
			        phone.outgoing = strcmp(arg1, "out") ? 0 : 1;
			        if (strlen(arg2) > 20) {
			        	fprintf(stderr, "phone-number must not exceed 20 characters\n");
			        	return -1;
			        }
			        strcpy(phone.name, id);
			        strcpy(phone.phone, arg2);
			        if ((result = ioctl(fd, IIOCNETDNM, &phone)) < 0) {
			        	perror(id);
			        	return -1;
			        }
			        break;

			case LIST:
			        if (!strcmp(id, "all")) {
			        	char name[10];
			        	if ((iflst = fopen("/proc/net/dev", "r")) == NULL) {
			        		perror("/proc/net/dev");
			        		return -1;
			        	}
			        	while (!feof(iflst)) {
			        		fgets(s, sizeof(s), iflst);
			        		if ((p = strchr(s, ':'))) {
			        			*p = 0;
			        			sscanf(s, "%s", name);
			        			listif(fd, name, 0);
			        			while (strlen(nextlistif))
			        				listif(fd, nextlistif, 0);
			        		}
			        	}
			        	fclose(iflst);
			        } else
			        	listif(fd, id, 1);
			        break;

			case EAZ:
			        strcpy(cfg.name, id);
			        if ((result = ioctl(fd, IIOCNETGCF, &cfg)) < 0) {
			        	perror(id);
			        	return -1;
			        }
			        if (args == 2) {
			        	i = -1;
			        	strncpy(cfg.eaz, arg1, sizeof(cfg.eaz) - 1);
			        	if ((result = ioctl(fd, IIOCNETSCF, &cfg)) < 0) {
			        		perror(id);
			        		return -1;
			        	}
			        }
			        printf("EAZ/MSN for %s is %s\n", cfg.name, cfg.eaz);
			        break;

			case VERBOSE:
			        i = -1;
			        sscanf(id, "%d", &i);
			        if (i < 0) {
			        	fprintf(stderr, "Verbose-level must be >= 0\n");
			        	return -1;
			        }
			        if ((result = ioctl(fd, IIOCSETVER, i)) < 0) {
			        	perror("IIOCSETVER");
			        	return -1;
			        }
			        printf("Verbose-level set to %d.\n", i);
			        break;

			case GETCONF:
			        if (args == 0)
			        	id = "-";
			        get_setup(fd, id);
			        printf("Configuration written to %s.\n", id);
			        break;

			case HUPTIMEOUT:
			        strcpy(cfg.name, id);
			        if ((result = ioctl(fd, IIOCNETGCF, &cfg)) < 0) {
			        	perror(id);
			        	return -1;
			        }
			        if (args == 2) {
			        	i = -1;
			        	sscanf(arg1, "%d", &i);
			        	if (i < 0) {
			        		fprintf(stderr, "Hangup-Timeout must be >= 0\n");
			        		return -1;
			        	}
			        	cfg.onhtime = i;
			        	if ((result = ioctl(fd, IIOCNETSCF, &cfg)) < 0) {
			        		perror(id);
			        		return -1;
			        	}
			        }
			        printf("Hangup-Timeout for %s is %d sec.\n", cfg.name, cfg.onhtime);
			        break;

			case CBDELAY:
			        strcpy(cfg.name, id);
			        if ((result = ioctl(fd, IIOCNETGCF, &cfg)) < 0) {
			        	perror(id);
			        	return -1;
			        }
			        if (args == 2) {
			        	i = -1;
			        	sscanf(arg1, "%d", &i);
			        	if (i < 0) {
			        		fprintf(stderr, "Callback delay must be >= 0\n");
			        		return -1;
			        	}
			        	cfg.cbdelay = i * 5;
			        	if ((result = ioctl(fd, IIOCNETSCF, &cfg)) < 0) {
			        		perror(id);
			        		return -1;
			        	}
			        }
			        printf("Callback delay for %s is %d sec.\n", cfg.name, cfg.cbdelay / 5);
			        break;

			case CHARGEINT:
			        strcpy(cfg.name, id);
			        if ((result = ioctl(fd, IIOCNETGCF, &cfg)) < 0) {
			        	perror(id);
			        	return -1;
			        }
			        if (args == 2) {
			        	i = -1;
			        	sscanf(arg1, "%d", &i);
			        	if (i < 0) {
			        		fprintf(stderr, "Charge interval must be >= 0\n");
			        		return -1;
			        	}
			        	cfg.chargeint = i;
			        	if ((result = ioctl(fd, IIOCNETSCF, &cfg)) < 0) {
			        		perror(id);
			        		return -1;
			        	}
			        }
			        printf("Charge Interval for %s is %d sec.\n", cfg.name, cfg.chargeint);
			        break;

			case DIALMAX:
			        strcpy(cfg.name, id);
			        if ((result = ioctl(fd, IIOCNETGCF, &cfg)) < 0) {
			        	perror(id);
			        	return -1;
			        }
			        if (args == 2) {
			        	i = -1;
			        	sscanf(arg1, "%d", &i);
			        	if (i < 1) {
			        		fprintf(stderr, "Dialmax must be > 0\n");
			        		return -1;
			        	}
			        	cfg.dialmax = i;
			        	if ((result = ioctl(fd, IIOCNETSCF, &cfg)) < 0) {
			        		perror(id);
			        		return -1;
			        	}
			        }
			        printf("Dialmax for %s is %d times.\n", cfg.name, cfg.dialmax);
			        break;

			case SDELAY:
			        strcpy(cfg.name, id);
			        if ((result = ioctl(fd, IIOCNETGCF, &cfg)) < 0) {
			        	perror(id);
			        	return -1;
			        }
			        if (args == 2) {
			        	i = -1;
			        	sscanf(arg1, "%d", &i);
			        	if (i < 1) {
			        		fprintf(stderr, "Slave-activation delay must be >= 1\n");
			        		return -1;
			        	}
			        	cfg.slavedelay = i;
			        	if ((result = ioctl(fd, IIOCNETSCF, &cfg)) < 0) {
			        		perror(id);
			        		return -1;
			        	}
			        }
			        printf("Slave-activation delay for %s is %d sec.\n", cfg.name,
			               cfg.slavedelay);
			        break;

			case TRIGGER:
			        strcpy(cfg.name, id);
			        if ((result = ioctl(fd, IIOCNETGCF, &cfg)) < 0) {
			        	perror(id);
			        	exit(-1);
			        }
			        if (args) {
			        	i = -1;
			        	sscanf(arg1, "%d", &i);
			        	if (i < 0) {
			        		fprintf(stderr, "Slave triggerlevel must be >= 0\n");
			        		exit(-1);
			        	}
			        	cfg.triggercps = i;
			        	if ((result = ioctl(fd, IIOCNETSCF, &cfg)) < 0) {
			        		perror(id);
			        		exit(-1);
			        	}
			        }
			        printf("Slave triggerlevel for %s is %d cps.\n", cfg.name,
			        cfg.triggercps);
			        break;

			case CHARGEHUP:
			        strcpy(cfg.name, id);
			        if ((result = ioctl(fd, IIOCNETGCF, &cfg)) < 0) {
			        	perror(id);
			        	return -1;
			        }
			        if (args == 2) {
			        	i = -1;
			        	if (strcmp(arg1, "on") && strcmp(arg1, "off")) {
			        		fprintf(stderr, "Charge-Hangup must be 'on' or 'off'\n");
			        		return -1;
			        	}
			        	cfg.chargehup = strcmp(arg1, "off");
			        	if ((result = ioctl(fd, IIOCNETSCF, &cfg)) < 0) {
			        		perror(id);
			        		return -1;
			        	}
			        }
			        printf("Charge-Hangup for %s is %s\n", cfg.name, cfg.chargehup ? "on" : "off");
			        break;

			case CBHUP:
			        strcpy(cfg.name, id);
			        if ((result = ioctl(fd, IIOCNETGCF, &cfg)) < 0) {
			        	perror(id);
			        	return -1;
			        }
			        if (args == 2) {
			        	i = -1;
			        	if (strcmp(arg1, "on") && strcmp(arg1, "off")) {
			        		fprintf(stderr, "Callback-Hangup must be 'on' or 'off'\n");
			        		return -1;
			        	}
			        	cfg.cbhup = strcmp(arg1, "off");
			        	if ((result = ioctl(fd, IIOCNETSCF, &cfg)) < 0) {
			        		perror(id);
			        		return -1;
			        	}
			        }
			        printf("Reject before Callback for %s is %s\n", cfg.name, cfg.cbhup ? "on" : "off");
			        break;

			case IHUP:
			        strcpy(cfg.name, id);
			        if ((result = ioctl(fd, IIOCNETGCF, &cfg)) < 0) {
			        	perror(id);
			        	return -1;
			        }
			        if (args == 2) {
			        	i = -1;
			        	if (strcmp(arg1, "on") && strcmp(arg1, "off")) {
			        		fprintf(stderr, "Incoming-Hangup must be 'on' or 'off'\n");
			        		return -1;
			        	}
			        	cfg.ihup = strcmp(arg1, "off");
			        	if ((result = ioctl(fd, IIOCNETSCF, &cfg)) < 0) {
			        		perror(id);
			        		return -1;
			        	}
			        }
			        printf("Incoming-Hangup for %s is %s\n", cfg.name, cfg.ihup ? "on" : "off");
			        break;

			case SECURE:
			        strcpy(cfg.name, id);
			        if ((result = ioctl(fd, IIOCNETGCF, &cfg)) < 0) {
			        	perror(id);
			        	return -1;
			        }
			        if (args == 2) {
			        	i = -1;
			        	if (strcmp(arg1, "on") && strcmp(arg1, "off")) {
			        		fprintf(stderr, "Secure-parameter must be 'on' or 'off'\n");
			        		return -1;
			        	}
			        	cfg.secure = strcmp(arg1, "off");
			        	if ((result = ioctl(fd, IIOCNETSCF, &cfg)) < 0) {
			        		perror(id);
			        		return -1;
			        	}
			        }
			        printf("Security for %s is %s\n", cfg.name, cfg.secure ? "on" : "off");
			        break;

			case CALLBACK:
			        strcpy(cfg.name, id);
			        if ((result = ioctl(fd, IIOCNETGCF, &cfg)) < 0) {
			        	perror(id);
			        	return -1;
			        }
			        if (args == 2) {
			        	i = -1;
			        	if (strcmp(arg1, "on") && strcmp(arg1, "off") &&
                    strcmp(arg1, "in") && strcmp(arg1, "out")) {
			        		fprintf(stderr, "Callback-parameter must be 'on', 'in', 'out' or 'off'\n");
			        		return -1;
			        	}
			        	cfg.callback = strcmp(arg1, "off") ? 1 : 0;
			        	if (!strcmp(arg1, "out"))
			        		cfg.callback = 2;
			        	if ((result = ioctl(fd, IIOCNETSCF, &cfg)) < 0) {
			        		perror(id);
			        		return -1;
			        	}
			        }
			        printf("Callback for %s is %s\n", cfg.name, num2callb[cfg.callback]);
			        break;

			case L2_PROT:
			        strcpy(cfg.name, id);
			        if ((result = ioctl(fd, IIOCNETGCF, &cfg)) < 0) {
			        	perror(id);
			        	return -1;
			        }
			        if (args == 2) {
			        	i = key2num(arg1, l2protostr, l2protoval);
			        	if (i < 0) {
			        		fprintf(stderr, "Layer-2-Protocol must be one of the following:\n");
			        		i = 0;
			        		while (strlen(l2protostr[i]))
			        			fprintf(stderr, "\t\"%s\"\n", l2protostr[i++]);
			        		return -1;
			        	}
			        	cfg.l2_proto = i;
			        	if ((result = ioctl(fd, IIOCNETSCF, &cfg)) < 0) {
			        		perror(id);
			        		return -1;
			        	}
			        }
			        printf("Layer-2-Protocol for %s is %s\n", cfg.name,
			          num2key(cfg.l2_proto, l2protostr, l2protoval));
			        break;

			case L3_PROT:
			        strcpy(cfg.name, id);
			        if ((result = ioctl(fd, IIOCNETGCF, &cfg)) < 0) {
			        	perror(id);
			        	return -1;
			        }
			        if (args == 2) {
			        	i = key2num(arg1, l3protostr, l3protoval);
			        	if (i < 0) {
			        		fprintf(stderr, "Layer-3-Protocol must be one of the following:\n");
			        		i = 0;
			        		while (strlen(l3protostr[i]))
			        			fprintf(stderr, "\t\"%s\"\n", l3protostr[i++]);
			        		return -1;
			        	}
			        	cfg.l3_proto = i;
			        	if ((result = ioctl(fd, IIOCNETSCF, &cfg)) < 0) {
			        		perror(id);
			        		return -1;
			        	}
			        }
			        printf("Layer-3-Protocol for %s is %s\n", cfg.name,
			          num2key(cfg.l3_proto, l3protostr, l3protoval));
			        break;

			case ADDLINK:
			        if ((result = ioctl(fd, IIOCNETALN, id)) < 0) {
			        	perror(id);
			        	return -1;
			        }
			        if (result)
			        	printf("Can't increase number of links: %d\n", result);
			        else
			        	printf("Ok, added a new link. (dialing)\n");
			        break;

			case REMOVELINK:
			        if ((result = ioctl(fd, IIOCNETDLN, id)) < 0) {
			        	perror(id);
			        	return -1;
			        }
			        if (result)
			        	printf("Can't decrease number of links: %d\n", result);
			        else
			        	printf("Ok, removed a link. (hangup)\n");
			        break;

			case ENCAP:
			        strcpy(cfg.name, id);
			        if ((result = ioctl(fd, IIOCNETGCF, &cfg)) < 0) {
			        	perror(id);
			        	return -1;
			        }
			        if (args == 2) {
			        	i = key2num(arg1, pencapstr, pencapval);
			        	if (i < 0) {
			        		fprintf(stderr, "Encapsulation must be one of the following:\n");
			        		i = 0;
			        		while (strlen(pencapstr[i]))
			        			fprintf(stderr, "\t\"%s\"\n", pencapstr[i++]);
			        		return -1;
			        	}
			        	cfg.p_encap = i;
			        	if ((result = ioctl(fd, IIOCNETSCF, &cfg)) < 0) {
			        		perror(id);
			        		return -1;
			        	}
			        }
			        printf("Encapsulation for %s is %s\n", cfg.name,
			             num2key(cfg.p_encap, pencapstr, pencapval));
			        break;

#ifdef I4L_CTRL_CONF
			case WRITECONF:
			        if (args == 0) {
			          sprintf(conffile, "%s%c%s", confdir(), C_SLASH, CONFFILE);
			          id = conffile;
			        }

			        if (writeconfig(fd, id))
			        	return -1;

			        printf("ISDN Configuration written to %s.\n", id);
			        break;

			case READCONF:
			        if (args == 0) {
			          sprintf(conffile, "%s%c%s", confdir(), C_SLASH, CONFFILE);
			          id = conffile;
			        }

			        if (readconfig(fd, id))
			        	return -1;

			        printf("ISDN Configuration read from %s.\n", id);
			        break;
#endif /* I4L_CTRL_CONF */
		}

		if (argc > 1) {
			printf("args=%d nextcmd %s\n",args, argv[1]);
		}
	}

	return 0;
}

void main(int argc, char **argv)
{
	int fd;

	if ((cmd = strrchr(argv[0], '/')) != NULL)
		*cmd++ = '\0';
	else
		cmd = argv[0];

	fd = open("/dev/isdnctrl", O_RDWR);
	if (fd < 0) {
		perror("/dev/isdnctrl");
		exit(-1);
	}

	if (argc == 1) {
		usage();
		exit(-1);
	}

	exec_args(fd,argc-1,argv+1);
	close(fd);
}
