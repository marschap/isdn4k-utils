#include <stdio.h>
#include <malloc.h>
#include <sys/time.h>
#include <linux/capi.h>
#include <capi20.h>

#include "init.h"
#include "contr.h"
#include "capi.h"

/*
 * defines needed by InitISDN
 */
unsigned short Appl_Id = 0;

#define MaxNumBChan        2    /* max. number of B-channels */
#define MaxNumB3DataBlocks 7    /* max. number of unconfirmed B3-datablocks */
			        /* 7 is the maximal number supported by CAPI */
#define MaxB3DataBlockSize 2048 /* max. B3-Datablocksize */
				/* 2048 is the maximum supported by CAPI */

static CAPI_MESSAGE CAPI_BUFFER = NULL;

/*
 * RegisterCAPI: Check for CAPI, allocate memory for CAPI-buffer and
 * register application. This function has to be called before using any
 * other CAPI functions.
 */
unsigned RegisterCAPI (void) {
	CAPI_REGISTER_ERROR ErrorCode;
	int numController;

	if (!CAPI20_ISINSTALLED()) {
		fprintf(stderr, "No CAPI support on this system.\n");
		return 0;
	}
	if (!(numController = GetNumController())) {
		fprintf(stderr, "RegisterCAPI: No ISDN-controller installed\n");
		return 0;
	}
	Appl_Id = CAPI20_REGISTER(MaxNumBChan, MaxNumB3DataBlocks,
				  MaxB3DataBlockSize, &ErrorCode);
	if (!Appl_Id) {
		fprintf(stderr, "RegisterCAPI: error: %04x\n", ErrorCode);
		return 0;
	}
	return numController;
}

/*
 * ReleaseCAPI: deregister application
 */
void ReleaseCAPI (void) {
	MESSAGE_EXCHANGE_ERROR ErrorCode;

	ErrorCode = CAPI20_RELEASE(Appl_Id);
	if (ErrorCode != 0)
		fprintf(stderr, "ReleaseCAPI: error: 0x%04X\n", ErrorCode);
	if (CAPI_BUFFER) {
		free(CAPI_BUFFER);
		CAPI_BUFFER = NULL;
	}
}
