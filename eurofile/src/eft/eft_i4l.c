/* $Id: eft_i4l.c,v 1.1 1999/06/30 17:18:16 he Exp $ */
/*
 * isdn4linux implementation specific functions
 */

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/param.h>
#include <linux/x25.h>
#include <linux/isdn.h>
#include <errno.h>
#include "../config.h"
#include <eft.h>

/* 
 * get the remote phone number corresponding to the connected socket.
 * isdn_no must be an array of at least 21 chacters
 */
int eft_get_peer_phone(unsigned char * isdn_no, int sk)
{
	int fd, len, i, is, ie, try_first_channel=0, ret;
	unsigned char buf[4001];
	char dev[EFT_DEV_NAME_LEN], *d;	

	isdn_net_ioctl_phone if_phone;

	isdn_no[0] = 0;

#ifdef IIOCNETGPN  
/*
 * If this experimental IOCTL is supported, the peer number is really
 * determined
 * from the network interface
 */
	fd = open("/dev/isdninfo",O_RDONLY);
	if ( fd < 0 ){
		perror("eft_get_peer_phone:open(/dev/isdninfo)");
		return -1;
	}
	d = eft_get_device(dev, EFT_DEV_NAME_LEN, sk);
	if ( ! d ){
		fprintf(stderr,"eft_get_peer_phone: unable to figure out "
			"isdn device\n");
		return -1;
	}
	strncpy(if_phone.name,d,sizeof(if_phone.name));
	ret = ioctl(fd,IIOCNETGPN,&if_phone);
	if(ret == 0){
		strcpy(isdn_no,if_phone.phone);
		close(fd);
		return strlen(isdn_no);
	} else if( errno == EINVAL ) { 
		/* kernel does not support the ioctl above:
		 * use fall back method
		 */
	        fprintf(stderr, "eft_get_peer_phone: probably "
			"no ioctl(IIOCNETGPN), trying fall back method\n");
		try_first_channel=1;
	} else {
		perror("eft_get_peer_phone:ioctl(IIOCNETGPN)");
		close(fd);
		return -1;
	}
#else
#warning   You seem to include old kernel isdn header files. Thus, reliable
#warning   identification of peer isdn phone number will not work.
	try_first_channel=1;
#endif

if( ! try_first_channel ) return 0;

/*
 * Fallback method for kernels which don't support the appropriate ioctl.
 * Currently, this assumes that the connection is on the first
 * channel of the first driver. To enforce that the connection arrives
 * on the first channel of the first HL driver, you need to bind that channel
 * to the interface.
 */
	len = read(fd,buf,4000);
	if( len < 0 ) {
		perror("eft_get_peer_phone:read(/dev/isdninfo)");
		close(fd);
		return len;
	}
	close(fd);
	/* search for position after string "phone:" in isdnfinfo contents */
	for( i=0; i<4000; i++ ){
		if( (buf[i] == ':') && (buf[i-3] == 'o') ){
			is = i+2;
			break;
		} 
	};
	ie = 4000;
	for( i=is; i<4000; i++ ){
		if( buf[i] == ' ') {
			ie = i;
			break;
		} 
	}
	len = ie - is;
	if( len > 20 ) return len; 
	memcpy(isdn_no,buf+is,len);
	isdn_no[len]=0;
	return 0;
}

/* 
 * configure isdn network devices for outgoing eft connection
 */
static int sync_pipe_r=-1;
static int sync_pipe_w=-1;

int eft_get_x25route(struct sockaddr_x25 * x25addr, char * isdn_no)
{
	int filedes_r[2], filedes_w[2], err;
#define FD_STR_SIZE 25
	char *args[5], fd_str_r[FD_STR_SIZE],
		fd_str_w[FD_STR_SIZE], *env[1];
	pid_t pid;

	/*
	 * By convention, the symbolic isdn address "localhost" is
	 * mapped to x25 address "1" and the eftd.sh setup script
	 * may already have configured a corresponding interface and an
	 * x25 route.
	 */
	if( strcmp(isdn_no, "localhost") == 0 ) {
		strcpy(x25addr->sx25_addr.x25_addr, "1");
		return 0;
	}


	if(pipe(filedes_r)){
		perror("eft_setup_route():pipe()");
		return -1;
	}
	sync_pipe_r=filedes_r[0];
	if(pipe(filedes_w)){
		perror("eft_setup_route():pipe()");
		return -1;
	}
	sync_pipe_w=filedes_w[1];
	pid = fork();
	if( pid<0 ){
		perror("eft_setup_route():fork()");
		return -1;
	} else if( pid > 0 ) {
		/* parent process */
		printf("waiting for set up\n");
		close(filedes_w[0]);
		close(filedes_r[1]);
		err=read(sync_pipe_r,x25addr->sx25_addr.x25_addr,
			 sizeof(x25_address));
		if( err < 1 ){
			perror("eft_setup_route():parent read sync_pipe");
			return -1;
		} else {
			x25addr->sx25_addr.x25_addr[err-1]=0;
			printf("set up successful, address \"%s\", len=%d\n",
			       x25addr->sx25_addr.x25_addr, err-1);
                        /*FIXME: we should read the x25address from the pipe*/
                        strcpy(x25addr->sx25_addr.x25_addr, "");
			return 0;
		}
	} else {
		/* child process */
	  /* FIXME: locate this script outside of eftp4linux source tree */
		args[0] = CONFIG_EFT_TOPDIR "/scripts/eftp_setup";
		args[1] = isdn_no;
		err = snprintf(fd_str_w,FD_STR_SIZE,"%d",filedes_w[0]);
		args[2] = fd_str_w;
		err = snprintf(fd_str_r,FD_STR_SIZE,"%d",filedes_r[1]);
		args[3] = fd_str_r;
		args[4] = NULL;
		env[0] = NULL;
		close(filedes_w[1]);
		close(filedes_r[0]);
		execve(args[0],args,env);
		perror("execve");
		exit(1);		
	}
	return -1;
}

/* 
 * release isdn network devices
 */
int eft_release_route(struct sockaddr_x25 * x25addr, char * isdn_no)
{
	close(sync_pipe_w);
	return 0;
}

/* 
 * hangup physical connection of isdn network interface.
 */
void eft_dl_disconnect(unsigned char * iif)
{
	pid_t pid;
	char buf[80];
	sprintf(buf,EFT_ISDNCTRL_PATH " hangup %s",iif);
	/* printf(buf);printf("\n"); */
	/* system(buf) */
	pid=fork();
	if(pid==0){ 
	  /* FIXME: isdnctrl path should not be hardcoded here */
		printf("%s %s %s %s\n",EFT_ISDNCTRL_PATH,"isdnctrl", "hangup", iif);
		execl(EFT_ISDNCTRL_PATH,"isdnctrl", "hangup", iif, NULL);
		perror("execl()");
	} else if(pid>0) {
		int status;
		if( wait(&status) != pid ){
			perror("eft_dl_disconnect: wait failed");
		}
		wait(&status);
	}
}

/*
 * return the name of the network interface which is used by 
 * a connected X.25 socket.
 *
 * This is a hack as it uses /proc file system contents like kernel internal
 * addresses of inode structures and might fail for future linux kernel
 * version.
 */
char * eft_get_device(char * dev, int len, int sock_fd)
{
#define PROC_X25_MAXLEN 180	
	char path_buf[MAXPATHLEN+1], link_buf[MAXPATHLEN+1], *p, *q,
		proc_buf[PROC_X25_MAXLEN+1], *t, *inode, *d;
	FILE *fx25;
	static char dummy[]="dummy";
	int i, inod_col, dev_col;

	/* first, the socket's inode number is exctracted from the
	 * socket file descriptors /proc/self/fd/# symbolic link contents 
	 */
	snprintf(path_buf,MAXPATHLEN+1,"/proc/self/fd/%d",sock_fd);
	len = readlink(path_buf,link_buf,MAXPATHLEN+1);
	if( len < 0 ){
		perror("eft_get_device():readlink failed");
		return NULL;
	}
	p=link_buf;
	while( (p!=0) && ( (*p < '0') || (*p > '9') ) ) ++p;
	inode=q=p;
	while( (*q >= '0') && (*q <= '9')  ) ++q;
	*q=0;
	
	/* fprintf(stderr,"socket %d has inode %s\n", sock_fd,inode); */
	
	/* next, the position of the inode column in /proc/net/x25
	 * is determined. 
	 */
	fx25 = fopen("/proc/net/x25","r");
	if( ! fx25 ) {
		perror("eft_get_device: open /proc/net/x25 failed");
		return NULL;
	}
	q = fgets(proc_buf,PROC_X25_MAXLEN+1,fx25);
	if(!q){
		fprintf(stderr, "/proc/net/x25 fgets error\n");
		return NULL;
	}
	t = proc_buf;
	i=0;
	while(1){
		/* the termination critereon for this loop silently assumes
		 * that "inode" is the last column header.
		 */
		q = strsep(&t," \t\n\r");
		if(!q){
			fprintf(stderr, "/proc/net/x25 strsep error\n");
			return NULL;
		}
#if 0
		fprintf(stderr,"parameter in column %d is \"%s\"\n",i,q);
#endif
		if( strcmp("dev",q)   == 0 )  dev_col = i;
		if( strcmp("inode",q) == 0 ){
			inod_col = i;
			break;
		}
		if( *q ) i++;
	}
	
	/* last, the line with the socket's inode is located
	 * in /proc/net/x25. The device name is extracted from that line
	 */ 
	while( (q=fgets(proc_buf,PROC_X25_MAXLEN+1,fx25)) ){
		t = proc_buf;
		i=0;
		d=dummy;
		while( (q=strsep(&t," \t\n\r")) ){
			if( i == dev_col )  d = q;
			if( i == inod_col ){
#if 0
				fprintf(stderr,"line %d: dev %s, inode %s\n",
					i, d, q); 
#endif
				if( strcmp(inode,q) == 0 ){
					strncpy(dev,d,len);
					dev[len-1]=0;
					return dev;
				}
				break;
			}
			if( *q ) i++;
		};
	};
	fprintf(stderr, "/proc/net/x25 socket inode %s not found\n",inode);
	return dummy;
}
