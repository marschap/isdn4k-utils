#include <assert.h>
#include <sys/time.h>
#include <linux/capi.h>
#include <capi20.h>

#include "contr.h"
#include "id.h"

/*
 * GetNumController: Returns the number of controllers detected by CAPI
 */
unsigned GetNumController(void) {
	unsigned short Buffer;

	/* retrieve the number of installed controllers */
	CAPI20_GET_PROFILE (0, (unsigned char *)&Buffer);
	return Buffer;
}

/*
 * GetNumOfSupportedBChannels: Returns the number of supported B-channels
 * for the specified controller
 */
unsigned GetNumOfSupportedBChannels (long Controller) {
	unsigned short Buffer[64 / sizeof (unsigned short)];

	assert (Controller != INVAL_CONTROLLER);
	/* retrieve controller specific information */
	CAPI20_GET_PROFILE((unsigned)Controller, (unsigned char *)Buffer);
	return (unsigned)Buffer[1];
}

/*
 * GetFreeController: Returns the number of the first controller that has
 * one free B-channel, or INVAL_CONTROLLER if none found.
 */
long GetFreeController(void) {
	long Controller;
	int numController;

	numController = GetNumController ();
	for (Controller = 1; Controller <= numController; Controller++) {
		if (GetNumOfSupportedBChannels (Controller) > GetNumberOfConnections (Controller)) {
			return Controller;
		}
	}
	return INVAL_CONTROLLER;
}
