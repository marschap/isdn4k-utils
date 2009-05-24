/*****************************************************************
 * CAPI 2.0 Library                                              *
 * All parts are distributed under the terms of GPL. See COPYING *
 *****************************************************************/

/**
 * \file capi_mod_fritzbox.c
 * \brief Capi Over TCP for FRITZ!Box or compatible routers
 * \author Jan-Michael Brummer, based upon the work by Marco Zissen
 */

#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <linux/capi.h>
#include <errno.h>
#include "capi20.h"
#include "capi_mod.h"
#include "capi_mod_fritzbox.h"

/**
 * \brief Add standard fritzbox capi over tcp header to buffer pointer
 * \param ppnPtr data buffer pointer
 * \param nLen length of message
 * \param nApplId application id
 * \param nCommand packet command
 */
static void fritzboxSetHeader( unsigned char **ppnPtr, short nLen, short nApplId, short nCommand ) {
	/* CAPI-Over-TCP Header (3 Bytes): */
	/* TYPE: Message */
	put_byte( ppnPtr, 0x80 );
	/* Length of whole message */
	put_word( ppnPtr, nLen );

	/* CAPI-Message Header: */
	/* Length */
	put_word( ppnPtr, nLen );
	/* Application ID */
	put_word( ppnPtr, nApplId );
	/* Command */
	put_netword( ppnPtr, nCommand );
	/* Message-Number 0x01 */
	put_word( ppnPtr, 0x01 );
	/* Default */
	put_byte( ppnPtr, 0x02 );
	put_byte( ppnPtr, 0x00 );
}

/**
 * \brief Send message to socket and wait for response
 * \param nHandle socket handle
 * \param pnBuffer data buffer pointer
 * \param nWrite number of bytes to write from pnBuffer
 * \return number of bytes read
 */
static int fritzboxRemoteCommand( int nHandle, unsigned char *pnBuffer, int nWrite ) {
	unsigned char anTemp[ 2 ];
	int nNum, nType, nLen;

	/* write message to socket */
	nNum = write( nHandle, pnBuffer, nWrite );
	if ( nNum != nWrite ) {
		return 0;
	}

	/* try to read header */
	nNum = read( nHandle, anTemp, 2 );
	if ( nNum == 2 ) {
		nType = anTemp[ 0 ];
		nLen = anTemp[ 1 ];

		/* ok, we have a header... now the rest */
		nNum = read( nHandle, pnBuffer, nLen );
		if ( nNum != nLen ) {
			CapiDebug( 1, "[%s]: read got %x, want %x", __FUNCTION__, nNum, nLen );
			return 0;
		}

		return nNum;
	} else {
		CapiDebug( 1, "[%s]: read got %x, want 2", __FUNCTION__, nNum );
	}

	/* uhm, error. return 0 */
	return 0;
}

/**
 * \brief Create a socket to hostname:port
 * \return socket number
 */
static int fritzboxOpenSocket( void ) {
	int nHandle;
	struct hostent *psHostInfo;
	struct sockaddr_in sAddr;

	/* Create new socket */
	nHandle = socket( PF_INET, SOCK_STREAM, 0 );
	if ( nHandle == -1 ) {
		CapiDebug( 1, "[%s]: Could not create socket! (%d)", __FUNCTION__, nHandle );
		return nHandle;
	}

	sAddr.sin_family = PF_INET;
	sAddr.sin_port = htons( getPort() );
	/* Retrieve hostname information */
	psHostInfo = gethostbyname( getHostName() );
	if ( psHostInfo != NULL ) {
		sAddr.sin_addr = *( struct in_addr * ) psHostInfo -> h_addr;

		/* Connect socket to address */
		if ( !connect( nHandle, ( struct sockaddr * ) &sAddr, sizeof( sAddr ) ) ) {
			/* no errors, return handle */
			return nHandle;
		}
		CapiDebug( 1, "[%s]: Could not connect to port %d on '%s'. CapiOverTCP enabled??", __FUNCTION__, getPort(), getHostName() );
	} else {
		CapiDebug( 1, "[%s]: Could not resolve host '%s'", __FUNCTION__, getHostName() );
	}

	/* error occured, either we could not connect to port or we couldn't find hostname
	 * close socket and return -1 as error
	 */
	close( nHandle );
	return -1;
}

/**
 * \brief Check if fritzbox interface is available
 * \return file descriptor of socket (see fritzboxOpenSocket for more informations)
 */
static unsigned fritzboxIsInstalled( void ) {
	unsigned nHandle;
	char *pnHostName;

	/* Check if we have valid data for hostname and port */
	pnHostName = getHostName();
	if ( strlen( pnHostName ) <= 0 ) {
		setHostName( "fritz.box" );
	}
	if ( getPort() == -1 ) {
		setPort( 5031 );
	}

	/* we check if we can open a new socket to hostname:port */
	nHandle = fritzboxOpenSocket();

	/* return new handle or error code */
	return nHandle;
}

/**
 * \brief Register at fritzbox
 * \param nMaxB3Connection maximum b3 connection
 * \param nMaxB3Blks maximum b3 blocks
 * \param nMaxSizeB3 maximum b3 size
 * \param pnApplId pointer where we store the new application id
 * \return new socket handle
 */
static unsigned fritzboxRegister( unsigned nMaxB3Connection, unsigned nMaxB3Blks, unsigned nMaxSizeB3, unsigned *pnApplId ) {
	unsigned char anMessage[ 256 ];
	unsigned char *pnPtr = anMessage;
	int nSock, nError;

	/* Safety: Set application id to invalid (-1) */
	*pnApplId = -1;

	/* open a new socket for communication */
	nSock = fritzboxOpenSocket();
	if ( nSock < 0 ) {
		return nSock;
	}

	/* Setup registration message packet */
	fritzboxSetHeader( &pnPtr, 20, 0x0000, CAPICMD( 0x20, CAPI_REQ ) );

	/* Parameter length */
	put_byte( &pnPtr, 0x09 );

	/* The Parameters: */

	/* Buffer pointer (currently not used), 2bytes*/
	put_byte( &pnPtr, 0x00 );
	put_byte( &pnPtr, 0x00 );
	/* Message buffer size */
	put_byte( &pnPtr, 0x06 );
	/* Maximum number of logical connections */
	put_word( &pnPtr, nMaxB3Connection );
	/* Number of data blocks available simultaneously */
	put_word( &pnPtr, nMaxB3Blks );
	/* Maximum size of a data block */
	put_word( &pnPtr, nMaxSizeB3 );
	/* Capi version?? */

	/* Send message to socket and wait for answer */
	if ( !( fritzboxRemoteCommand( nSock, anMessage, 23 ) ) ) {
		/* Error occured, close socket and return error code -2 */
		CapiDebug( 1, "Error: Unable to register CAPI! (ApplId: %d, MaxB3Con: %d, MaxB3Blks: %d, MaxB3Size: %d)", *pnApplId, nMaxB3Connection, nMaxB3Blks, nMaxSizeB3 );
		close( nSock );
		return -2;
	}

	nError = CAPIMSG_U16( anMessage, 16 );
	if ( nError != 0 ) {
		CapiDebug( 1, "Error: Unable to register CAPI! Error %x\n", nError );
		close( nSock );
		return -3;
	}

	/* Fine, we have a new application id, set it to pnApplId and return socket handle */
	*pnApplId = CAPIMSG_APPID( anMessage );
	CapiDebug( 1, "Successfully registered CAPI (ApplId: %d, MaxB3Con: %d, MaxB3Blks: %d, MaxB3Size: %d)", *pnApplId, nMaxB3Connection, nMaxB3Blks, nMaxSizeB3 );

	return nSock;
}

/**
 * \brief Release fritzbox connection
 * \param nSock socket handle
 * \param nApplId registered application id
 * \return error code (CapiNoError or CapiRegNotInstalled on error)
 */
static unsigned fritzboxRelease( int nSock, int nApplId ) {
	unsigned char anMessage[ 256 ];
	unsigned char *pnPtr = anMessage;

	/* Setup release message packet */
	fritzboxSetHeader( &pnPtr, 14, nApplId, CAPICMD( 0x20, CAPI_REQ ) );

	/* Parameter length */
	put_byte( &pnPtr, 0x03 );

	/* Parameters */
	put_byte( &pnPtr, 0x01 );
	put_byte( &pnPtr, 0x00 );
	put_byte( &pnPtr, 0x00 );

	/* write message to socket */
	if( write( nSock, anMessage, 17 ) != 17 ) {
		return 0;
	}

	/* try to read header */
	read( nSock, anMessage, sizeof( anMessage ) );

	/* well done, return no error */
	CapiDebug( 3, "Successfully released CAPI (ApplId: %d)", nApplId );

	return CapiNoError;
}

/**
 * \brief Put capi message to fritzbox
 * \param nSock socket handle
 * \param nApplId application id
 * \param pnMsg message pointer
 * \return error code (CapiNoError or CapiMsgOSResourceErr on error)
 */
static unsigned fritzboxPutMessage( int nSock, unsigned nApplId, unsigned char *pnMsg ) {    
	unsigned char anSendBuffer[ SEND_BUFSIZ ];
	int nNum;
	int nLen = CAPIMSG_LEN( pnMsg );
	int nCommand = CAPIMSG_COMMAND( pnMsg );
	int nSubCommand = CAPIMSG_SUBCOMMAND( pnMsg );

	nLen = processMessage( pnMsg, nApplId, nCommand, nSubCommand, nLen );

	/* Create capi over tcp header (0x80, 2 bytes length) */
	anSendBuffer[ 0 ] = 0x80;
	anSendBuffer[ 1 ] = nLen & 0xFF;
	anSendBuffer[ 2 ] = ( nLen >> 8 ) & 0xFF;

	if ( nCommand == CAPI_DATA_B3 && nSubCommand == CAPI_REQ ) {
		/* Special case: Set buffer address to zero */
		pnMsg[ 12 ] = 0x00;
		pnMsg[ 13 ] = 0x00;
		pnMsg[ 14 ] = 0x00;
		pnMsg[ 15 ] = 0x00;
	}

	/* attach capi message after header */
	memcpy( anSendBuffer + 3, pnMsg, sizeof( anSendBuffer ) - 3 );
	nLen += 3;

	/* write data to socket */    
	nNum = write( nSock, anSendBuffer, nLen );
	if ( nNum != nLen ) {
		CapiDebug( 3, "Error: Unable send CAPI_PUT_MESSAGE (nApplId: %d, Ctrl: %d, Cmd: %d, SubCmd: %d)", nApplId, anSendBuffer[ 11 ] & 0x7F, CAPIMSG_COMMAND( pnMsg ), CAPIMSG_SUBCOMMAND( pnMsg ) );
		return CapiMsgOSResourceErr;
	}

	CapiDebug( 3, "CAPI_PUT_MESSAGE (nApplId: %d, Ctrl: %d, Cmd: %d, SubCmd: %d)", nApplId, anSendBuffer[ 11 ] & 0x7F, CAPIMSG_COMMAND( pnMsg ), CAPIMSG_SUBCOMMAND( pnMsg ) );
	return CapiNoError;
}

/**
 * \brief Read data from socket
 * \param nSock socket handle
 * \param pnBuffer buffer pointer
 * \param nLen length of buffer
 * \return read len
 */
static int readSocket( int nSock, unsigned char *pnBuffer, int nLen ) {
	unsigned char anTemp[ 4096 ], *pnPtr;
	int nTotLen, nReadLen, nActLen, nOrigLen, nType;
	time_t nTime;

	/* try to read the CAPI over TCP Header, 3 bytes */
	if ( read( nSock, anTemp, 3 ) == 3 ) {
		pnPtr = anTemp;

		/* Get message type and the total length of message */
		nType = CAPIMSG_U8( anTemp, 0 );
		nTotLen = nOrigLen = CAPIMSG_U16( anTemp, 1 );
		nTime = time( NULL );
		nReadLen = 0;

		/* read message */
		while ( ( ( nActLen = read( nSock, &anTemp[ nReadLen ], nTotLen ) ) < nTotLen ) && ( time( NULL ) < ( nTime + 5 ) ) ) {
			if ( nActLen > 0 ) {
				nTotLen -= nActLen;
				nReadLen += nActLen;
			}
	
			nActLen = 0;
		}
	
		if ( nActLen > 0 ) {
			nReadLen += nActLen;
		}
	
		if ( nReadLen != nOrigLen ) {
			return 0;
		}
	
		if ( nLen == 0 ) {
			nLen = nOrigLen;
		}

		nOrigLen = ( nLen < nReadLen ) ? nLen : nReadLen;

		/* copy informations to buffer, cut to nLen if needed */
		memcpy( pnBuffer, anTemp, nOrigLen );
		return nOrigLen;
	}

	/* could not read header, return 0 */
	return 0;
}

/**
 * \brief Get message from fritzbox
 * \param nSock socket handle
 * \param nApplId application id
 * \param ppnBuffer pointer to data buffer pointer (where we store the data)
 * \return error code
 */
static unsigned fritzboxGetMessage( int nSock, unsigned nApplId, unsigned char **ppnBuffer ) {
	int nRet;
	unsigned char *pnBuffer;
	unsigned nOffset;
	size_t nBufSize;

	/* try to get a new buffer from queue */
	if ( ( *ppnBuffer = pnBuffer = ( unsigned char * ) get_buffer( nApplId, &nBufSize, &nOffset ) ) == 0 ) {
		CapiDebug( 1, "[%s]: could not get buffer\n", __FUNCTION__ );
		return CapiMsgOSResourceErr;
	}

	/* Get CAPI-over-TCP Header */
	nRet = readSocket( nSock, pnBuffer, nBufSize );
	if ( nRet > 0 ) {
		/* workaround for old driver */
		CAPIMSG_SETAPPID( pnBuffer, nApplId );

		/* DATA_B3? Then set buffer address */
		if ( CAPIMSG_COMMAND( pnBuffer ) == CAPI_DATA_B3 && CAPIMSG_SUBCOMMAND( pnBuffer ) == CAPI_IND ) {
			save_datahandle( nApplId, nOffset, CAPIMSG_U16( pnBuffer, 18 ), CAPIMSG_U32( pnBuffer, 8 ) );
			/* patch datahandle */
			capimsg_setu16( pnBuffer, 18, nOffset );

			if ( sizeof( void * ) == 4 ) {
				/* 32bit case */
				u_int32_t nData = ( ( ( u_int32_t ) pnBuffer ) + CAPIMSG_LEN( pnBuffer ) );

				pnBuffer[ 12 ] = ( ( unsigned char ) nData & 0xFF );
				pnBuffer[ 13 ] = ( ( unsigned char )( nData >> 8 ) & 0xFF );
				pnBuffer[ 14 ] = ( ( unsigned char )( nData >> 16 ) & 0xFF );
				pnBuffer[ 15 ] = ( ( unsigned char )( nData >> 24 ) & 0xFF );
			} else {
				/* 64bit case */
				u_int64_t nData;

				if ( CAPIMSG_LEN( pnBuffer ) < 30 ) {
					memmove( pnBuffer + 30, pnBuffer + CAPIMSG_LEN( pnBuffer ), CAPIMSG_DATALEN( pnBuffer ) );
					pnBuffer[ 0 ] = 30;
					pnBuffer[ 1 ] = 0;
				}

				nData = ( ( ( ulong ) pnBuffer ) + CAPIMSG_LEN( pnBuffer ) );
				pnBuffer[ 12 ] = pnBuffer[ 13 ] = pnBuffer[ 14 ] = pnBuffer[ 15 ] = 0;
				pnBuffer[ 22 ] = nData & 0xFF;
				pnBuffer[ 23 ] = ( nData >>  8 ) & 0xFF;
				pnBuffer[ 24 ] = ( nData >> 16 ) & 0xFF;
				pnBuffer[ 25 ] = ( nData >> 24 ) & 0xFF;
				pnBuffer[ 26 ] = ( nData >> 32 ) & 0xFF;
				pnBuffer[ 27 ] = ( nData >> 40 ) & 0xFF;
				pnBuffer[ 28 ] = ( nData >> 48 ) & 0xFF;
				pnBuffer[ 29 ] = ( nData >> 56 ) & 0xFF;
			}

			CapiDebug( 3, "CAPI_GET_MESSAGE (nApplId: %d, Ctrl: %d)", nApplId, pnBuffer[ 8 ] & 0x7F ); 

			/* keep buffer */
			return CapiNoError;
		}

		/* buffer is not needed, return it */
		return_buffer( nApplId, nOffset );

		if ( ( CAPIMSG_COMMAND( pnBuffer ) == CAPI_DISCONNECT ) && ( CAPIMSG_SUBCOMMAND( pnBuffer ) == CAPI_IND ) ) {
			/* we got a disconnect, cleanup buffers */
			cleanup_buffers_for_plci( nApplId, CAPIMSG_U32( pnBuffer, 8 ) );
		}

		return CapiNoError;
	}

	/* uh, error occured while reading capi message, return buffer and check for errors */
	return_buffer( nApplId, nOffset );
	if ( nRet == 0 ) {
		return CapiReceiveQueueEmpty;
	}

	switch ( errno ) {
		case EMSGSIZE:
			nRet = CapiIllCmdOrSubcmdOrMsgToSmall;
			break;
		case EAGAIN:
			nRet = CapiReceiveQueueEmpty;
			break;
		default:
			nRet = CapiMsgOSResourceErr;
			break;
	}

	return nRet;
}

/**
 * \brief Get manufactor informations
 * \param nHandle socket handle
 * \param nController controller id
 * \param pnBuffer buffer pointer we write our informations to
 * \return pnBuffer
 */
static unsigned char *fritzboxGetManufactor( int nHandle, unsigned nController, unsigned char *pnBuffer ) {
	/* set manufactor informations into request buffer */
	memcpy( pnBuffer, CAPI20_FB_MANUF, strlen( CAPI20_FB_MANUF ) );
	pnBuffer[ strlen( CAPI20_FB_MANUF ) ] = '\0';

	return pnBuffer;
}

/**
 * \brief Get version informations
 * \param nHandle socket handle
 * \param nController controller id
 * \param pnBuffer buffer pointer we write our informations to
 * \return pnBuffer
 */
static unsigned char *fritzboxGetVersion( int nHandle, unsigned nController, unsigned char *pnBuffer ) {
	capi_version cversion;

	/* set version information */
	cversion.majorversion = 2;
	cversion.minorversion = 0;
	cversion.majormanuversion = CAPI20_FB_VERSION_MAJOR;
	cversion.minormanuversion = CAPI20_FB_VERSION_MINOR;

	memcpy( pnBuffer, &cversion, sizeof( capi_version ) );

	return pnBuffer;
}

/**
 * \brief Get serial number informations
 * \param nHandle socket handle
 * \param nController controller id
 * \param pnBuffer buffer pointer we write our informations to
 * \return pnBuffer
 */
static unsigned char *fritzboxGetSerialNumber( int nHandle, unsigned nController, unsigned char *pnBuffer ) {
	/* set serial number */
	memcpy( pnBuffer, CAPI20_FB_SERIAL, strlen( CAPI20_FB_SERIAL ) );
	pnBuffer[ strlen( CAPI20_FB_SERIAL ) ] = 0;

	return pnBuffer;
}

/**
 * \brief Get profile from fritzbox
 * \param nHandle socket handle
 * \param nControllerId controller
 * \param pnBuf buffer
 * \return error code
 */
static unsigned fritzboxGetProfile( int nHandle, unsigned nController, unsigned char *pnBuf ) {
	unsigned char anMessage[ 256 ];
	unsigned char *pnPtr = anMessage;

	memset( pnPtr, 0, sizeof( anMessage ) );

	/* we want a get profile message */
	fritzboxSetHeader( &pnPtr, 0x12, 0x0000, CAPICMD( 0x20, CAPI_REQ ) );

	/* Parameter length */
	put_byte( &pnPtr, 0x05 );

	/* Default (0x02, 0x00) */
	put_byte( &pnPtr, 0x02 );
	put_byte( &pnPtr, 0x00 );
	/* Param size */
	put_byte( &pnPtr, 0x02 );
	/* Controller */
	put_word( &pnPtr, nController );

	/* send message packet and read answer */
	if ( !( fritzboxRemoteCommand( nHandle, anMessage, 21 ) ) ) {
		/* wuuh, an error occurred, return error */
		return CapiMsgOSResourceErr;
	}

	/* copy capi profile information to buffer */
	memcpy( pnBuf, anMessage + 18, sizeof( struct capi_profile ) );

	return CapiNoError;
}

/** Module operations structure */
static struct sModuleOperations sFritzBox = {
	fritzboxIsInstalled,
	fritzboxRegister,
	fritzboxRelease,
	fritzboxPutMessage,
	fritzboxGetMessage,
	fritzboxGetManufactor,
	fritzboxGetVersion,
	fritzboxGetSerialNumber,
	fritzboxGetProfile,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};

MODULE_INIT( "fritzbox", &sFritzBox );
