/*
 * $Id: capi20.c,v 1.4 1998/11/18 17:05:44 paul Exp $
 * 
 * $Log: capi20.c,v $
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
        return 0;

    return ioctl(capi_fd, CAPI_INSTALLED, 0) == 0;
}

/*
 * managment of application ids
 */

static struct capi_applidmap {
    int used;
    int fd;
} capi_applidmap[CAPI_MAXAPPL] = {{0,0}};

static inline _cword allocapplid(int fd)
{
   _cword i;
   for (i=0; i < CAPI_MAXAPPL; i++) {
      if (capi_applidmap[i].used == 0) {
         capi_applidmap[i].used = 1;
         capi_applidmap[i].fd = fd;
         return i+1;
      }
   }
   return 0;
}

static inline void freeapplid(_cword applid)
{
    capi_applidmap[applid-1].used = 0;
    capi_applidmap[applid-1].fd = -1;
}

static inline int validapplid(_cword applid)
{
    return applid > 0 && applid <= CAPI_MAXAPPL
                      && capi_applidmap[applid-1].used;
}

static inline int applid2fd(_cword applid)
{
    if (applid < CAPI_MAXAPPL)
	    return capi_applidmap[applid-1].fd;
    return -1;
}

/* 
 * CAPI2.0 functions
 */

_cword
capi20_register (unsigned MaxB3Connection,
		 unsigned MaxB3Blks,
		 unsigned MaxSizeB3,
		 CAPI_REGISTER_ERROR *ErrorCode)
{
    _cword applid;
    char buf[PATH_MAX];
    int i, fd = -1;

    if (!capi20_isinstalled()) {
       *ErrorCode = CapiRegNotInstalled;
       return 0;
    }

    *ErrorCode = CapiRegOSResourceErr;

    for (i=0; fd < 0; i++) {
        /*----- open pseudo-clone device -----*/
        sprintf(buf, "/dev/capi20.%02d", i);
        if ((fd = open(buf, O_RDWR|O_NONBLOCK, 0666)) < 0) {
            switch (errno) {
            case EEXIST:
                break;
            default:
                return 0;
            }
        }
    }

    if ((applid = allocapplid(fd)) == 0)
        return 0;

    ioctl_data.rparams.level3cnt = MaxB3Connection;
    ioctl_data.rparams.datablkcnt = MaxB3Blks;
    ioctl_data.rparams.datablklen = MaxSizeB3;

    if (ioctl(fd, CAPI_REGISTER, &ioctl_data) < 0) {
        if (errno == EIO) {
            if (ioctl(fd, CAPI_GET_ERRCODE, &ioctl_data) < 0)
                return 0;
            *ErrorCode = ioctl_data.errcode;
        }
        return 0;
    }
    return applid;
}

MESSAGE_EXCHANGE_ERROR
capi20_release (unsigned Appl_Id)
{
    if (!capi20_isinstalled())
        return CapiRegNotInstalled;
    if (!validapplid(Appl_Id))
        return CapiIllAppNr;
    (void)close(applid2fd(Appl_Id));
    freeapplid(Appl_Id);
    return CapiNoError;
}

MESSAGE_EXCHANGE_ERROR
capi20_put_message (CAPI_MESSAGE Msg, unsigned Appl_Id)
{
    MESSAGE_EXCHANGE_ERROR ret;
    int len = (Msg[0] | (Msg[1] << 8));
    int cmd = Msg[4];
    int subcmd = Msg[5];
    int rc;
    int fd;

    if (!capi20_isinstalled())
        return CapiRegNotInstalled;

    if (!validapplid(Appl_Id))
        return CapiIllAppNr;

    fd = applid2fd(Appl_Id);

    memcpy(sndbuf, Msg, len);

    if (cmd == CAPI_DATA_B3 && subcmd == CAPI_REQ) {
        int datalen = (Msg[16] | (Msg[17] << 8));
        void *dataptr = (void *)(Msg[12]|(Msg[13]<<8)|(Msg[14]<<16)|(Msg[15]<<24));
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
                else ret = (MESSAGE_EXCHANGE_ERROR)ioctl_data.errcode;
                break;
          default:
                ret = CapiMsgOSResourceErr;
                break;
       }
    }

    return ret;
}

MESSAGE_EXCHANGE_ERROR
capi20_get_message (unsigned Appl_Id, CAPI_MESSAGE  *ReturnMessage)
{
    MESSAGE_EXCHANGE_ERROR ret;
    int rc, fd;

    if (!capi20_isinstalled())
        return CapiRegNotInstalled;

    if (!validapplid(Appl_Id))
        return CapiIllAppNr;

    fd = applid2fd(Appl_Id);

    *ReturnMessage = rcvbuf;
    if ((rc = read(fd, rcvbuf, sizeof(rcvbuf))) > 0) {
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

CAPI_MESSAGE
capi20_get_manufacturer(unsigned contr, CAPI_MESSAGE LpBuffer)
{
    if (!capi20_isinstalled())
       return 0;
    ioctl_data.contr = contr;
    if (ioctl(capi_fd, CAPI_GET_MANUFACTURER, &ioctl_data) < 0)
       return 0;
    strncpy(LpBuffer, ioctl_data.manufacturer, CAPI_MANUFACTURER_LEN);
    return LpBuffer;
}

CAPI_MESSAGE
capi20_get_version(unsigned contr, CAPI_MESSAGE version)
{
    if (!capi20_isinstalled())
        return 0;
    ioctl_data.contr = contr;
    if (ioctl(capi_fd, CAPI_GET_VERSION, &ioctl_data) < 0)
        return 0;
    memcpy(version, &ioctl_data.version, sizeof(capi_version));
    return version;
}

CAPI_MESSAGE 
capi20_get_serial_number(unsigned contr, CAPI_MESSAGE LpBuffer)
{
    if (!capi20_isinstalled())
        return 0;
    ioctl_data.contr = contr;
    if (ioctl(capi_fd, CAPI_GET_SERIAL, &ioctl_data) < 0)
        return 0;
    memcpy(LpBuffer, &ioctl_data.serial, CAPI_SERIAL_LEN);
    return LpBuffer;
}

MESSAGE_EXCHANGE_ERROR
capi20_get_profile(unsigned Controller, CAPI_MESSAGE LpBuffer)
{
    if (!capi20_isinstalled())
        return CapiMsgNotInstalled;

    ioctl_data.contr = Controller;
    if (ioctl(capi_fd, CAPI_GET_PROFILE, &ioctl_data) < 0) {
        if (errno != EIO)
            return CapiMsgOSResourceErr;
        if (ioctl(capi_fd, CAPI_GET_ERRCODE, &ioctl_data) < 0)
            return CapiMsgOSResourceErr;
        return (MESSAGE_EXCHANGE_ERROR)ioctl_data.errcode;
    }
    if (Controller)
        memcpy(LpBuffer, &ioctl_data.profile, CAPI_SERIAL_LEN);
    else
        memcpy(LpBuffer, &ioctl_data.profile.ncontroller,
                       sizeof(ioctl_data.profile.ncontroller));
    return CapiNoError;
}
/*
 * functions added to the CAPI2.0 spec
 */

MESSAGE_EXCHANGE_ERROR
capi20_waitformessage(_cword Appl_Id, struct timeval *tvp)
{
  int fd;
  fd_set rfds;
  int retval;

  FD_ZERO(&rfds);

  if(!capi20_isinstalled())
    return CapiRegNotInstalled;

  if(!validapplid(Appl_Id))
    return CapiIllAppNr;
  
  fd = applid2fd(Appl_Id);

  FD_SET(fd, &rfds);
  
  fcntl(fd, F_SETFL, !O_NONBLOCK);
  retval = select(fd + 1, &rfds, NULL, NULL, tvp);
  fcntl(fd, F_SETFL, O_NONBLOCK);
  
  return(CapiNoError);
}

int
capi20_fileno(_cword applid)
{
   return applid2fd(applid);
}

