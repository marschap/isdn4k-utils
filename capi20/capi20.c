/*
 * $Id: capi20.c,v 1.8 1999/09/15 08:10:44 calle Exp $
 * 
 * $Log: capi20.c,v $
 * Revision 1.8  1999/09/15 08:10:44  calle
 * Bugfix: error in 64Bit extention.
 *
 * Revision 1.7  1999/09/10 17:20:33  calle
 * Last changes for proposed standards (CAPI 2.0):
 * - AK1-148 "Linux Extention"
 * - AK1-155 "Support of 64-bit Applications"
 *
 * Revision 1.6  1999/09/06 17:40:07  calle
 * Changes for CAPI 2.0 Spec.
 *
 * Revision 1.5  1999/04/20 19:52:19  calle
 * Bugfix in capi20_get_profile: wrong size in memcpy from
 * Kai Germaschewski <kai@thphy.uni-duesseldorf.de>
 *
 * Revision 1.4  1998/11/18 17:05:44  paul
 * fixed a (harmless) warning
 *
 * Revision 1.3  1998/08/30 09:57:14  calle
 * I hope it is know readable for everybody.
 *
 * Revision 1.1  1998/08/25 16:33:16  calle
 * Added CAPI2.0 library. First Version.
 *
 */
#include <sys/types.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <linux/capi.h>
#include "capi20.h"

#define	CAPIMSG_LEN(m)		(m[0] | (m[1] << 8))
#define	CAPIMSG_COMMAND(m)	(m[4])
#define	CAPIMSG_SUBCOMMAND(m)	(m[5])
#define CAPIMSG_DATALEN(m)	(m[16] | (m[17]<<8))


static int                  capi_fd = -1;
static capi_ioctl_struct    ioctl_data;
static unsigned char        rcvbuf[128+2048];   /* message + data */
static unsigned char        sndbuf[128+2048];   /* message + data */

unsigned capi20_isinstalled (void)
{
    if (capi_fd >= 0)
        return 1;

    /*----- open managment link -----*/
    if ((capi_fd = open("/dev/capi20", O_RDWR, 0666)) < 0)
        return CapiRegNotInstalled;

    if (ioctl(capi_fd, CAPI_INSTALLED, 0) == 0)
	return CapiNoError;
    return CapiRegNotInstalled;
}

/*
 * managment of application ids
 */

static struct capi_applidmap {
    int used;
    int fd;
} capi_applidmap[CAPI_MAXAPPL] = {{0,0}};

static inline unsigned allocapplid(int fd)
{
   unsigned i;
   for (i=0; i < CAPI_MAXAPPL; i++) {
      if (capi_applidmap[i].used == 0) {
         capi_applidmap[i].used = 1;
         capi_applidmap[i].fd = fd;
         return i+1;
      }
   }
   return 0;
}

static inline void freeapplid(unsigned applid)
{
    capi_applidmap[applid-1].used = 0;
    capi_applidmap[applid-1].fd = -1;
}

static inline int validapplid(unsigned applid)
{
    return applid > 0 && applid <= CAPI_MAXAPPL
                      && capi_applidmap[applid-1].used;
}

static inline int applid2fd(unsigned applid)
{
    if (applid < CAPI_MAXAPPL)
	    return capi_applidmap[applid-1].fd;
    return -1;
}

/* 
 * CAPI2.0 functions
 */

unsigned
capi20_register (unsigned MaxB3Connection,
		 unsigned MaxB3Blks,
		 unsigned MaxSizeB3,
		 unsigned *ApplID)
{
    unsigned applid = 0;
    char buf[PATH_MAX];
    int i, fd = -1;

    *ApplID = applid;

    if (!capi20_isinstalled())
       return CapiRegNotInstalled;

    for (i=0; fd < 0; i++) {
        /*----- open pseudo-clone device -----*/
        sprintf(buf, "/dev/capi20.%02d", i);
        if ((fd = open(buf, O_RDWR|O_NONBLOCK, 0666)) < 0) {
            switch (errno) {
            case EEXIST:
                break;
            default:
                return CapiRegOSResourceErr;
            }
        }
    }

    if ((applid = allocapplid(fd)) == 0)
        return CapiRegOSResourceErr;

    ioctl_data.rparams.level3cnt = MaxB3Connection;
    ioctl_data.rparams.datablkcnt = MaxB3Blks;
    ioctl_data.rparams.datablklen = MaxSizeB3;

    if (ioctl(fd, CAPI_REGISTER, &ioctl_data) < 0) {
        if (errno == EIO) {
            if (ioctl(fd, CAPI_GET_ERRCODE, &ioctl_data) < 0)
                return CapiRegOSResourceErr;
            return (unsigned)ioctl_data.errcode;
        }
        return CapiRegOSResourceErr;
    }
    *ApplID = applid;
    return CapiNoError;
}

unsigned
capi20_release (unsigned ApplID)
{
    if (!capi20_isinstalled())
        return CapiRegNotInstalled;
    if (!validapplid(ApplID))
        return CapiIllAppNr;
    (void)close(applid2fd(ApplID));
    freeapplid(ApplID);
    return CapiNoError;
}

unsigned
capi20_put_message (unsigned char *Msg, unsigned ApplID)
{
    unsigned ret;
    int len = (Msg[0] | (Msg[1] << 8));
    int cmd = Msg[4];
    int subcmd = Msg[5];
    int rc;
    int fd;

    if (!capi20_isinstalled())
        return CapiRegNotInstalled;

    if (!validapplid(ApplID))
        return CapiIllAppNr;

    fd = applid2fd(ApplID);

    memcpy(sndbuf, Msg, len);

    if (cmd == CAPI_DATA_B3 && subcmd == CAPI_REQ) {
        int datalen = (Msg[16] | (Msg[17] << 8));
        void *dataptr;
        if (sizeof(void *) != 4) {
	    if (len >= 30) { /* 64Bit CAPI-extention */
	       u_int64_t data64;
	       memcpy(&data64,Msg+22, sizeof(u_int64_t));
	       if (data64 != 0) dataptr = (void *)data64;
	       else dataptr = Msg + len; /* Assume data after message */
	    } else {
               dataptr = Msg + len; /* Assume data after message */
	    }
        } else {
            u_int32_t data;
            memcpy(&data,Msg+12, sizeof(u_int32_t));
            if (data != 0) dataptr = (void *)data;
            else dataptr = Msg + len; /* Assume data after message */
	}
        memcpy(sndbuf+len, dataptr, datalen);
        len += datalen;
    }

    ret = CapiNoError;
    errno = 0;

    if ((rc = write(fd, sndbuf, len)) != len) {
        switch (errno) {
            case EFAULT:
            case EINVAL:
                ret = CapiIllCmdOrSubcmdOrMsgToSmall;
                break;
            case EBADF:
                ret = CapiIllAppNr;
                break;
            case EIO:
                if (ioctl(fd, CAPI_GET_ERRCODE, &ioctl_data) < 0)
                    ret = CapiMsgOSResourceErr;
                else ret = (unsigned)ioctl_data.errcode;
                break;
          default:
                ret = CapiMsgOSResourceErr;
                break;
       }
    }

    return ret;
}

unsigned
capi20_get_message (unsigned ApplID, unsigned char **Buf)
{
    unsigned ret;
    int rc, fd;

    if (!capi20_isinstalled())
        return CapiRegNotInstalled;

    if (!validapplid(ApplID))
        return CapiIllAppNr;

    fd = applid2fd(ApplID);

    *Buf = rcvbuf;
    if ((rc = read(fd, rcvbuf, sizeof(rcvbuf))) > 0) {
        if (   CAPIMSG_COMMAND(rcvbuf) == CAPI_DATA_B3
	    && CAPIMSG_SUBCOMMAND(rcvbuf) == CAPI_IND) {
           if (sizeof(void *) == 4) {
	       u_int32_t data = (u_int32_t)rcvbuf + CAPIMSG_LEN(rcvbuf);
	       rcvbuf[12] = data & 0xff;
	       rcvbuf[13] = (data >> 8) & 0xff;
	       rcvbuf[14] = (data >> 16) & 0xff;
	       rcvbuf[15] = (data >> 24) & 0xff;
           } else {
	       u_int64_t data;
	       if (CAPIMSG_LEN(rcvbuf) < 30) {
		  /*
		   * grr, 64bit arch, but no data64 included,
	           * seems to be old driver
		   */
	          memmove(rcvbuf+30, rcvbuf+CAPIMSG_LEN(rcvbuf),
		          CAPIMSG_DATALEN(rcvbuf));
	          rcvbuf[0] = 30;
	          rcvbuf[1] = 0;
	       }
	       data = (u_int64_t)rcvbuf + CAPIMSG_LEN(rcvbuf);
	       rcvbuf[12] = rcvbuf[13] = rcvbuf[14] = rcvbuf[15] = 0;
	       rcvbuf[22] = data & 0xff;
	       rcvbuf[23] = (data >> 8) & 0xff;
	       rcvbuf[24] = (data >> 16) & 0xff;
	       rcvbuf[25] = (data >> 24) & 0xff;
	       rcvbuf[26] = (data >> 32) & 0xff;
	       rcvbuf[27] = (data >> 40) & 0xff;
	       rcvbuf[28] = (data >> 48) & 0xff;
	       rcvbuf[29] = (data >> 56) & 0xff;
	   }
	}
        return CapiNoError;
    }

    if (rc == 0)
        return CapiReceiveQueueEmpty;

    switch (errno) {
        case EMSGSIZE:
            ret = CapiIllCmdOrSubcmdOrMsgToSmall;
            break;
        case EAGAIN:
            return CapiReceiveQueueEmpty;
        default:
            ret = CapiMsgOSResourceErr;
            break;
    }

    return ret;
}

unsigned char *
capi20_get_manufacturer(unsigned Ctrl, unsigned char *Buf)
{
    if (!capi20_isinstalled())
       return 0;
    ioctl_data.contr = Ctrl;
    if (ioctl(capi_fd, CAPI_GET_MANUFACTURER, &ioctl_data) < 0)
       return 0;
    strncpy(Buf, ioctl_data.manufacturer, CAPI_MANUFACTURER_LEN);
    return Buf;
}

unsigned char *
capi20_get_version(unsigned Ctrl, unsigned char *Buf)
{
    if (!capi20_isinstalled())
        return 0;
    ioctl_data.contr = Ctrl;
    if (ioctl(capi_fd, CAPI_GET_VERSION, &ioctl_data) < 0)
        return 0;
    memcpy(Buf, &ioctl_data.version, sizeof(capi_version));
    return Buf;
}

unsigned char * 
capi20_get_serial_number(unsigned Ctrl, unsigned char *Buf)
{
    if (!capi20_isinstalled())
        return 0;
    ioctl_data.contr = Ctrl;
    if (ioctl(capi_fd, CAPI_GET_SERIAL, &ioctl_data) < 0)
        return 0;
    memcpy(Buf, &ioctl_data.serial, CAPI_SERIAL_LEN);
    return Buf;
}

unsigned
capi20_get_profile(unsigned Ctrl, unsigned char *Buf)
{
    if (!capi20_isinstalled())
        return CapiMsgNotInstalled;

    ioctl_data.contr = Ctrl;
    if (ioctl(capi_fd, CAPI_GET_PROFILE, &ioctl_data) < 0) {
        if (errno != EIO)
            return CapiMsgOSResourceErr;
        if (ioctl(capi_fd, CAPI_GET_ERRCODE, &ioctl_data) < 0)
            return CapiMsgOSResourceErr;
        return (unsigned)ioctl_data.errcode;
    }
    if (Ctrl)
        memcpy(Buf, &ioctl_data.profile, sizeof(struct capi_profile));
    else
        memcpy(Buf, &ioctl_data.profile.ncontroller,
                       sizeof(ioctl_data.profile.ncontroller));
    return CapiNoError;
}
/*
 * functions added to the CAPI2.0 spec
 */

unsigned
capi20_waitformessage(unsigned ApplID, struct timeval *TimeOut)
{
  int fd;
  fd_set rfds;
  int retval;

  FD_ZERO(&rfds);

  if(!capi20_isinstalled())
    return CapiRegNotInstalled;

  if(!validapplid(ApplID))
    return CapiIllAppNr;
  
  fd = applid2fd(ApplID);

  FD_SET(fd, &rfds);
  
  retval = select(fd + 1, &rfds, NULL, NULL, TimeOut);
  
  return(CapiNoError);
}

int
capi20_fileno(unsigned ApplID)
{
   return applid2fd(ApplID);
}

