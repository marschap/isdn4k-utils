/*
 * $Id: capi20.h,v 1.8 1999/12/06 17:08:30 calle Exp $
 *
 * $Log: capi20.h,v $
 * Revision 1.8  1999/12/06 17:08:30  calle
 * - Splitted capi20.h into capi20.h and capiutils.h.
 *   - capi20.h: the functions from the CAPI-2.0 Spec
 *   - capiutils.h: the "CAPI-ADK" functions
 * - bug in 64Bit-Support fixed.
 *
 */
#ifndef __CAPI20_H__
#define __CAPI20_H__

#include <sys/time.h>

#ifdef __cplusplus
extern "C" {
#endif

/* standard CAPI2.0 functions */

unsigned capi20_register (unsigned MaxLogicalConnection,
			  unsigned MaxBDataBlocks,
			  unsigned MaxBDataLen,
			  unsigned *ApplIDp);

unsigned capi20_release (unsigned ApplID);

unsigned capi20_put_message (unsigned ApplID, unsigned char *Msg);

unsigned capi20_get_message (unsigned ApplID, unsigned char **Buf);

unsigned capi20_waitformessage(unsigned ApplID, struct timeval *TimeOut);

unsigned char *capi20_get_manufacturer (unsigned Ctrl, unsigned char *Buf);

unsigned char *capi20_get_version (unsigned Ctrl, unsigned char *Buf);

unsigned char *capi20_get_serial_number (unsigned Ctrl, unsigned char *Buf);

unsigned capi20_get_profile (unsigned Controller, unsigned char *Buf);

unsigned capi20_isinstalled (void);

int capi20_fileno(unsigned ApplID);

/* end standard CAPI2.0 functions */

#ifdef __cplusplus
}
#endif

#ifndef __NO_CAPIUTILS__
#include <capiutils.h>
#endif

#endif /* __CAPI20_H */
