/*
 * $Id: avmcapictrl.c,v 1.2 1997/03/20 00:18:57 luethje Exp $
 * 
 * AVM-B1-ISDN driver for Linux. (Control-Utility)
 * 
 * Copyright 1996 by Carsten Paeth (calle@calle.in-berlin.de)
 * 
 * $Log: avmcapictrl.c,v $
 * Revision 1.2  1997/03/20 00:18:57  luethje
 * inserted the line #include <errno.h> in avmb1/avmcapictrl.c and imon/imon.c,
 * some bugfixes, new structure in isdnlog/isdnrep/isdnrep.c.
 *
 * Revision 1.1  1997/03/04 22:46:32  calle
 * Added program to add and download firmware to AVM-B1 card
 *
 * Revision 2.2  1997/02/12 09:31:39  calle
 * more verbose error messages
 *
 * Revision 1.1  1997/01/31 10:32:20  calle
 * Initial revision
 *
 * 
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <linux/isdn.h>
#include <linux/b1lli.h>
#include <linux/capi.h>

char *cmd;
char *ctrldev;
int arg_ofs;

void usage(void)
{
	fprintf(stderr, "usage: %s add <portbase> <irq> (Add a new card)\n", cmd);
	fprintf(stderr, "   or: %s load <bootcode> [contrnr] (load firmware)\n", cmd);
	fprintf(stderr, "   or: %s reset [contrnr] (reset controller)\n", cmd);
	exit(1);
}

int validports[] =
{0x150, 0x250, 0x300, 0x340, 0};
int validirqs[] =
{ 3, 4, 5, 6, 7, 9, 10, 11, 12, 15, 0};

int main(int argc, char **argv)
{
	int fd;
	int ac;
	capi_manufacturer_cmd ioctl_s;
	avmb1_carddef newcard;
	avmb1_loaddef ldef;
	avmb1_resetdef rdef;
	int port, irq;

	cmd = strrchr(argv[0], '/');
	cmd = (cmd == NULL) ? argv[0] : ++cmd;
	if (argc > 1) {
		arg_ofs = 1;
	} else
		usage();
	ac = argc - (arg_ofs - 1);
	fd = open("/dev/capi20", O_RDWR);
	if (fd < 0) {
		switch (errno) {
		   case ENOENT:
		      perror("Device file /dev/capi20 missing, use instdev");
		      exit(2);
		   case ENODEV:
		      perror("device capi20 not registered");
		      fprintf(stderr, "look in /proc/devices.\n");
		      fprintf(stderr, "maybe the devicefiles are installed with a wrong majornumber,\n");
		      fprintf(stderr, "or you linux kernel version only supports 64 char device (check /usr/include/linux/major.h)\n");
		      exit(2);
		}
		perror("/dev/capi20");
		exit(-1);
	}
	if (!strcmp(argv[arg_ofs], "add")) {
		if (ac >= 4) {
			int i;
			sscanf(argv[arg_ofs + 1], "%i", &port);
			sscanf(argv[arg_ofs + 2], "%i", &irq);
			for (i = 0; validports[i] && port != validports[i]; i++);
			if (!validports[i]) {
				fprintf(stderr, "%s: illegal io-addr 0x%x\n", cmd, port);
				fprintf(stderr, "%s: try one of 0x%x", cmd, validports[0]);
				for (i = 1; validports[i]; i++)
					fprintf(stderr, ", 0x%x", validports[i]);
				fprintf(stderr, "\n");
				exit(-1);
			}
			for (i = 0; validirqs[i] && irq != validirqs[i]; i++);
			if (!validirqs[i]) {
				fprintf(stderr, "%s: illegal irq %d\n", cmd, irq);
				fprintf(stderr, "%s: try one of %d", cmd, validirqs[0]);
				for (i = 1; validirqs[i]; i++)
					fprintf(stderr, ", %d", validirqs[i]);
				fprintf(stderr, "\n");
				exit(-1);
			}
			newcard.port = port;
			newcard.irq = irq;
			ioctl_s.cmd = AVMB1_ADDCARD;
			ioctl_s.data = &newcard;
			if ((ioctl(fd, CAPI_MANUFACTURER_CMD, &ioctl_s)) < 0) {
				perror("ioctl ADDCARD");
				exit(-1);
			}
			close(fd);
			return 0;
		}
	}
	if (!strcmp(argv[arg_ofs], "load")) {
		struct stat st;
		int codefd;
		int contr = 1;

		if (ac > 3)
			contr = atoi(argv[arg_ofs + 2]);

		if (stat(argv[arg_ofs + 1], &st)) {
			perror(argv[arg_ofs + 1]);
			exit(2);
		}
		if (!(codefd = open(argv[arg_ofs + 1], O_RDONLY))) {
			perror(argv[arg_ofs + 1]);
			exit(-1);
		}
		ldef.contr = contr;
		ldef.t4file.len = st.st_size;
		ldef.t4file.data = mmap(0, st.st_size, PROT_READ, MAP_PRIVATE, codefd, 0);
		if (ldef.t4file.data == (unsigned char *) -1) {
			perror("mmap");
			exit(2);
		}
		printf("Loading Bootcode %s ... ", argv[arg_ofs + 1]);
		fflush(stdout);
		ioctl_s.cmd = AVMB1_LOAD;
		ioctl_s.data = &ldef;
		if ((ioctl(fd, CAPI_MANUFACTURER_CMD, &ioctl_s)) < 0) {
			perror("\nioctl LOAD");
			exit(2);
		}
		munmap(ldef.t4file.data, ldef.t4file.len);
		close(codefd);
		close(fd);
		printf("done\n");
		return 0;
	}
	if (!strcmp(argv[arg_ofs], "reset")) {
		int contr = 1;

		if (ac > 2)
			contr = atoi(argv[arg_ofs + 1]);

		rdef.contr = contr;
		ioctl_s.cmd = AVMB1_RESETCARD;
		ioctl_s.data = &rdef;
		if ((ioctl(fd, CAPI_MANUFACTURER_CMD, &ioctl_s)) < 0) {
			perror("\nioctl RESET");
			exit(2);
		}
		close(fd);
		return 0;
	}
	usage();
	return 0;
}
