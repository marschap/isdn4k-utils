/* $Id: diehlctrl.c,v 1.2 1998/06/16 21:20:26 armin Exp $
 *
 * Diehl-ISDN driver for Linux. (Control-Utility)
 *
 * Copyright 1998 by Fritz Elfert (fritz@wuemaus.franken.de)
 * Copyright 1998 by Armin Schindler (mac@gismo.telekom.de)
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
 * $Log: diehlctrl.c,v $
 * Revision 1.2  1998/06/16 21:20:26  armin
 * Added part to load firmware on PRI/PCI.
 *
 * Revision 1.1  1998/06/12 11:11:45  fritz
 * First checkin, not complete, for Armin only.
 *
 */

#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <malloc.h>

#include <linux/isdn.h>
#include <diehl.h>

char *cmd;

static char *dsp_fname[] =
{
 "","kernelpr","hscx","v110","modem","fax","hscx30"
};


void usage() {
  fprintf(stderr,"usage: %s shmem [shmem-addr]          (get/set memaddr)\n",cmd);
  fprintf(stderr,"   or: %s irq   [irq-nr]              (get/set irq)\n",cmd);
  fprintf(stderr,"   or: %s load  <bootcode> <protocol> (load firmware)\n",cmd);
  fprintf(stderr,"   or: %s loadpri <protocol> -c1|2 -n -p -s1|2 -o (load firmware for PRI)\n",cmd);
  exit(-1);
}

int main(int argc, char **argv) {
	int fd;
	int tmp;
	int ac;
	int arg_ofs=0;
	isdn_ioctl_struct ioctl_s;

	cmd = argv[0];
	if (argc > 1) {
		if (!strcmp(argv[1], "-d")) {
			strcpy(ioctl_s.drvid, argv[2]);
			arg_ofs = 3;
		} else {
			ioctl_s.drvid[0] = '\0';
			arg_ofs = 1;
		}
	} else
		usage();
	ac = argc - (arg_ofs - 1);
	if (ac < 2)
		usage();
	fd = open("/dev/isdnctrl",O_RDWR);
	if (fd < 0) {
		perror("/dev/isdnctrl");
		exit(-1);
	}
	if (!strcmp(argv[arg_ofs], "shmem")) {
		if (ac == 3) {
			if (sscanf(argv[arg_ofs + 1], "%i", &tmp) !=1 )
				usage();
			ioctl_s.arg = tmp;
			if (ioctl(fd, DIEHL_IOCTL_SETMMIO + IIOCDRVCTL, &ioctl_s) < 0) {
				perror("ioctl SETMMIO");
				exit(-1);
			}
		}
		if ((tmp = ioctl(fd, DIEHL_IOCTL_GETMMIO + IIOCDRVCTL, &ioctl_s)) < 0) {
			perror("ioctl GETMMIO");
			exit(-1);
		}
		printf("Shared memory at 0x%08lx\n", (unsigned long)tmp);
		close(fd);
		return 0;
	}
	if (!strcmp(argv[arg_ofs], "irq")) {
		if (ac == 3) {
			if (sscanf(argv[arg_ofs + 1], "%i", &tmp) != 1)
				usage();
			ioctl_s.arg = tmp;
			if (ioctl(fd, DIEHL_IOCTL_SETIRQ + IIOCDRVCTL, &ioctl_s) < 0) {
				perror("ioctl SETIRQ");
				exit(-1);
			}
		}
		if ((tmp = ioctl(fd, DIEHL_IOCTL_GETIRQ + IIOCDRVCTL, &ioctl_s)) < 0) {
			perror("ioctl GETIRQ");
			exit(-1);
		}
		printf("Irq is %d\n", tmp);
		close(fd);
		return 0;
	}
	if (!strcmp(argv[arg_ofs], "load")) {
		FILE *code;
		int plen;
		char protoname[1024];
		char bootname[1024];
		u_char protobuf[0x100000];
		u_char bootbuf[0x1000];
		diehl_codebuf *cb;

		if (ac == 4) {
			strcpy(bootname, argv[arg_ofs + 1]);
			strcpy(protoname,argv[arg_ofs + 2]);
			if ((tmp = ioctl(fd,DIEHL_IOCTL_GETTYPE + IIOCDRVCTL, &ioctl_s)) < 1) {
				perror("ioctl GETTYPE");
				exit(-1);
			}
			switch (tmp) {
				case DIEHL_CTYPE_S:
					strcat(bootname,".bin");
					printf("Adapter-type is Diehl-S\n");
					break;
				case DIEHL_CTYPE_SX:
					strcat(protoname,".sx");
					printf("Adapter-type is Diehl-SX\n");
					break;
				case DIEHL_CTYPE_SCOM:
					strcat(protoname,".sy");
					printf("Adapter-type is Diehl-SCOM\n");
					break;
				case DIEHL_CTYPE_QUADRO:
					strcat(protoname,".sq");
					printf("Adapter-type is Diehl-QUADRO\n");
					break;
				case DIEHL_CTYPE_PRI:
					strcat(protoname,".p");
					printf("Adapter-type is Diehl-PRI\n");
					break;
				default:
					fprintf(stderr, "Unknown Adapter type %d\n", tmp);
					exit(-1);
			}
			if (!(code = fopen(protoname,"r"))) {
				perror(protoname);
				exit(-1);
			}
			if ((plen = fread(protobuf, 1, sizeof(protobuf), code))<1) {
				fprintf(stderr, "Read error on %s\n", protoname);
				exit(-1);
			}
			plen = (plen % 256)?((plen/256)+1)*256:plen;
			fclose(code);
			if (!(code = fopen(bootname,"r"))) {
				perror(bootname);
				exit(-1);
			}
			if ((tmp = fread(bootbuf, 1, sizeof(bootbuf), code))<1) {
				fprintf(stderr, "Read error on %s\n", bootname);
				exit(-1);
			}
			fclose(code);
			cb = malloc(sizeof(diehl_codebuf) + tmp + plen );
			memset(cb, 0, sizeof(diehl_codebuf));
			memcpy(&cb->isa.code, bootbuf, tmp);
			memcpy(&cb->isa.code[tmp], protobuf, plen);
			cb->isa.bootstrap_len = tmp;
			cb->isa.firmware_len = plen;
			cb->isa.boot_opt = DIEHL_ISA_BOOT_MEMCHK;
			printf("Loading Protocol %s ...\n", &protobuf[4]);
			ioctl_s.arg = (ulong)cb;
			if (ioctl(fd, DIEHL_IOCTL_LOADBOOT + IIOCDRVCTL, &ioctl_s) < 0) {
				perror("ioctl LOADBOOT");
				exit(-1);
			}
			close(fd);
			return 0;
		}
		usage();
	}
        if (!strcmp(argv[arg_ofs], "loadpri")) {
                FILE *code;
                int plen = 0;
		int tmp[9];
		int j;
                char protoname[1024];
                char filename[1024];
                u_char protobuf[0x100000];
                diehl_codebuf *cb;

                        strcpy(protoname,argv[++arg_ofs]);
                        if ((plen = ioctl(fd,DIEHL_IOCTL_GETTYPE + IIOCDRVCTL, &ioctl_s)) < 1) {
                                perror("ioctl GETTYPE");
                                exit(-1);
                        }
                        switch (plen) {
                                case DIEHL_CTYPE_MAESTRAP:
                                        printf("Adapter-type is Diva Server PRI/PCI\n");
                                        break;
                                default:
                                        fprintf(stderr, "Adapter type %d not supported\n", plen);
                                        exit(-1);
                        }
			plen = 0;
	        	for (j=0; j<(sizeof(dsp_fname)/sizeof(char *));j++) {
		  		if (j==0) sprintf(filename, "te_%s.pm", protoname);
			     	 else sprintf(filename, "%s.bin", dsp_fname[j]);
                        	if (!(code = fopen(filename,"r"))) {
                                	perror(filename);
                                	exit(-1);
                        	}
	                        if ((tmp[j] = fread(protobuf+plen, 1, (sizeof(protobuf)-plen), code))<1) {
        	                        fprintf(stderr, "Read error on %s\n", filename);
                	                exit(-1);
                        	}
                       		fclose(code);
				plen += tmp[j];
			}
                        cb = malloc(sizeof(diehl_codebuf) + plen );
                        memset(cb, 0, sizeof(diehl_codebuf));
                        memcpy(&cb->pci.code, protobuf, plen);
	        	for (j=0; j<(sizeof(dsp_fname)/sizeof(char *));j++) {
				if (j==0) cb->pci.protocol_len = tmp[0];
                        	 else cb->pci.dsp_code_len[j] = tmp[j];
			}
                        cb->pci.dsp_code_num = (sizeof(dsp_fname)/sizeof(char *)) - 1;
			cb->pci.tei = 1;
			cb->pci.nt2 = 0;
			cb->pci.WatchDog = 0;
			cb->pci.Permanent = 0;
			cb->pci.XInterface = 0;
			cb->pci.StableL2 = 0;
			cb->pci.NoOrderCheck = 0;
			cb->pci.HandsetType = 0;
			cb->pci.LowChannel = 0;
			cb->pci.ProtVersion = 0;
			cb->pci.Crc4 = 0;
                        /* parse extented PRI options */
                        while(ac > (arg_ofs + 1)) {
                                arg_ofs++;
                                if (!strcmp(argv[arg_ofs], "-c1")) {
                                        cb->pci.Crc4 = 1;
                                        continue;
                                }
                                if (!strcmp(argv[arg_ofs], "-c2")) {
                                        cb->pci.Crc4 = 2;
                                        continue;
                                }
                                if (!strcmp(argv[arg_ofs], "-n")) {
                                        cb->pci.nt2 = 1;
                                        continue;
                                }
                                if (!strcmp(argv[arg_ofs], "-p")) {
                                        cb->pci.Permanent = 1;
                                        continue;
                                }
                                if (!strcmp(argv[arg_ofs], "-o")) {
                                        cb->pci.NoOrderCheck = 1;
                                        continue;
                                }
                                if (!strcmp(argv[arg_ofs], "-s1")) {
                                        cb->pci.StableL2 = 1;
                                        continue;
                                }
                                if (!strcmp(argv[arg_ofs], "-s2")) {
                                        cb->pci.StableL2 = 2;
                                        continue;
                                }
                        }
                        printf("Loading Code ...\n");
                        ioctl_s.arg = (ulong)cb;
                        if (ioctl(fd, DIEHL_IOCTL_LOADPRI + IIOCDRVCTL, &ioctl_s) < 0) {
                                perror("ioctl LOADPRI");
                                exit(-1);
                        }
                        close(fd);
                        return 0;
        }
	return 0;
}
