#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <linux/isdn.h>

char           *progname;

void 
usage()
{
	fprintf(stderr, "usage: %s <DriverId> <IoctlCmd> <IoctlArg>\n", progname);
	exit(-1);
}

int 
main(int argc, char *argv[])
{
	int             fd;
	int             cmd;
	isdn_ioctl_struct ioctl_s;

	if ((progname = strrchr(argv[0], '/')))
		progname++;
	else
		progname = argv[0];

	if (strcmp(progname, "report")) {
		if (argc != 4)
			usage();
		strcpy(ioctl_s.drvid, argv[1]);
		ioctl_s.arg = strtol(argv[3], NULL, 0);
		cmd = strtol(argv[2], NULL, 0);
	} else {
		ioctl_s.drvid[0] = '\0';
		ioctl_s.arg = 0;
		cmd = 0;
	}
	fd = open("/dev/isdnctrl", O_RDWR);
	if (fd < 0) {
		perror("/dev/isdnctrl");
		exit(-1);
	}
	if (ioctl(fd, IIOCDRVCTL + cmd, &ioctl_s) < 0)
		perror(argv[1]);
	close(fd);
	return 0;
}
