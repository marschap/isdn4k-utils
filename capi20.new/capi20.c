/*
 * CAPI 2.0 library
 * 
 * 2002-03-27 - Added remote capi features.
 *              Armin Schindler <armin@melware.de>
 *
 * This program is free software and may be modified and 
 * distributed under the terms of the GNU Public License.
 *
 */

#include <sys/types.h>
#include <sys/ioctl.h>
#include <dirent.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include <stdarg.h>
#define _LINUX_LIST_H
#include <linux/capi.h>
 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
 
#include "capi20.h"
#include "capi_mod.h"

#ifndef CAPI_GET_FLAGS
#define CAPI_GET_FLAGS		_IOR('C',0x23, unsigned)
#endif
#ifndef CAPI_SET_FLAGS
#define CAPI_SET_FLAGS		_IOR('C',0x24, unsigned)
#endif
#ifndef CAPI_CLR_FLAGS
#define CAPI_CLR_FLAGS		_IOR('C',0x25, unsigned)
#endif
#ifndef CAPI_NCCI_OPENCOUNT
#define CAPI_NCCI_OPENCOUNT	_IOR('C',0x26, unsigned)
#endif
#ifndef CAPI_NCCI_GETUNIT
#define CAPI_NCCI_GETUNIT	_IOR('C',0x27, unsigned)
#endif

static int                  capi_fd = -1;

static char *globalconfigfilename = "/etc/capi20.conf";
static char *userconfigfilename = ".capi20rc";

static short port = -1;
static char driver[1024] = "";
static char hostname[1024] = "";

static int tracelevel;
static char *tracefile;

/** debug level, for debugging purpose */
static int nDebugLevel = 0;

/**
 * \brief CapiDebug output functions
 * \param nLevel debug level of following message
 * \param pnFormat formatted string
 */
void CapiDebug( int nLevel, const char *pnFormat, ... ) {
	if ( nLevel <= nDebugLevel ) {
		char anOutput[ 512 ];
		va_list pArgs;
		time_t sTime = time( NULL );
		struct tm *psNow = localtime( &sTime );

		va_start( pArgs, pnFormat );
		vsnprintf( anOutput, sizeof( anOutput ), pnFormat, pArgs );

		printf( "[%s] %02d.%02d.%d %02d:%02d:%02d - %s\n", "libcapi", psNow -> tm_mday, psNow -> tm_mon + 1, psNow -> tm_year + 1900, psNow -> tm_hour, psNow -> tm_min, psNow -> tm_sec, anOutput );
		va_end( pArgs );
	}
}

/**
 * \brief Get byte from data buffer and increase buffer pointer
 * \param ppnPtr data buffer pointer
 * \return byte from buffer
 */
unsigned short get_byte( unsigned char **ppnPtr ) {
	*ppnPtr += 1;

	return ( ( unsigned char ) *( *ppnPtr - 1 ) );
}

/**
 * \brief Get word from data buffer and increase buffer pointer
 * \param ppnPtr data buffer pointer
 * \return word from buffer
 */
unsigned short get_word( unsigned char **ppnPtr ) {
	return ( ( get_byte( ppnPtr ) ) | get_byte( ppnPtr ) << 8 );
}

/**
 * \brief Get netword (lo/hi switch) from data buffer and increase
 * \param ppnPtr data buffer pointer
 * \brief buffer pointer
 * \return networdword from buffer
 */
unsigned short get_netword( unsigned char **ppnPtr ) {
	return ( ( get_byte( ppnPtr ) << 8 ) | get_byte( ppnPtr ) );
}

/**
 * \brief Put byte to data buffer and increase buffer pointer
 * \param ppnPtr data buffer pointer
 * \param nVal byte number
 * \return byte from buffer
 */
unsigned char *put_byte( unsigned char **ppnPtr, _cbyte nVal ) {
	**ppnPtr = nVal;
	*ppnPtr += 1;

	return *ppnPtr;
}

/**
 * \brief Put word to data buffer and increase buffer pointer
 * \param ppnPtr data buffer pointer
 * \param nVal word number
 * \return word from buffer
 */
unsigned char *put_word( unsigned char **ppnPtr, _cword nVal ) {
	put_byte( ppnPtr, nVal & 0xFF );
	put_byte( ppnPtr, ( nVal & 0xFF00 ) >> 8 );

	return *ppnPtr;
}

/**
 * \brief Put dword to data buffer and increase buffer pointer
 * \param ppnPtr data buffer pointer
 * \param nVal dword number
 * \return dword from buffer
 */
unsigned char *put_dword( unsigned char **ppnPtr, _cdword nVal ) {
	put_byte( ppnPtr, nVal & 0xFF );
	put_byte( ppnPtr, ( nVal & 0xFF00 ) >> 8 );
	put_byte( ppnPtr, ( nVal & 0xFF0000 ) >> 16 );
	put_byte( ppnPtr, ( nVal & 0xFF000000 ) >> 24 );

	return *ppnPtr;
}

/**
 * \brief Put netword to data buffer and increase buffer pointer
 * \param ppnPtr data buffer pointer
 * \param nVal netword number
 * \return netword from buffer
 */
unsigned char *put_netword( unsigned char **ppnPtr, _cword nVal ) {
	put_byte( ppnPtr, ( nVal & 0xFF00 ) >> 8 );
	put_byte( ppnPtr, nVal & 0xFF );

	return *ppnPtr;
}

static char *skip_whitespace( char *s ) {
	while ( *s && isspace( *s ) ) {
		s++;
	}

	return s;
}

static char *skip_nonwhitespace(char *s)
{
	while (*s && !isspace(*s)) s++;
		return s;
} 

/*
 * read config file
 */

static int read_config(void)
{
	FILE *fp = NULL;
	char *s, *t;
	char buf[1024];

	if ((s = getenv("HOME")) != NULL) {
		strcpy(buf, s);
		strcat(buf, "/");
		strcat(buf, userconfigfilename);
		fp = fopen(buf, "r");
	}
	if ((!fp) && ((fp = fopen(globalconfigfilename, "r")) == NULL))
			return(0);

	while(fgets(buf, sizeof(buf), fp)) {
		buf[strlen(buf)-1] = 0;
		s = skip_whitespace(buf);
		if (*s == 0 || *s == '#')
					continue;
		if (!(strncmp(s, "REMOTE", 6))) {
			s = skip_nonwhitespace(s);
			
			t = skip_whitespace(s);
			s = skip_nonwhitespace(t);
			if (*s) *s++ = 0;
			strncpy(driver, t, (sizeof(driver) - 1));

			t = skip_whitespace(s);
			s = skip_nonwhitespace(t);
			if (*s) *s++ = 0;
			strncpy(hostname, t, (sizeof(hostname) - 1));

			t = skip_whitespace(s);
			s = skip_nonwhitespace(t);
			if (*s) *s++ = 0;
			port = strtol(t, NULL, 10);
			if (!port)
					port = 2662;
			continue;
		} else if (!(strncmp(s, "TRACELEVEL", 10))) {
			t = skip_nonwhitespace(s);
			s = skip_whitespace(t);
			tracelevel = (int)strtol(s, NULL, 10);
			continue;
		} else if (!(strncmp(s, "TRACEFILE", 9))) {
			t = skip_nonwhitespace(s);
			s = skip_whitespace(t);
			t = skip_nonwhitespace(s);
			if (*t) *t++ = 0;
			tracefile = strdup(s);
			continue;
		}
	}
	fclose(fp);
	return(1);
}

int getPort( void ) {
	return port;
}

void setPort( int nPortNumber ) {
	port = nPortNumber;
}

char *getHostName( void ) {
	return hostname;
}

void setHostName( char *pnHostName ) {
	snprintf( hostname, sizeof( hostname ), "%s", pnHostName );
}

/*
 * managment of application ids
 */

#define MAX_APPL 1024

static int applidmap[MAX_APPL];

static inline int remember_applid(unsigned applid, int fd)
{
	if (applid >= MAX_APPL)
		return -1;

	applidmap[applid] = fd;
	return 0;
}

static inline unsigned alloc_applid(int fd)
{
	unsigned applid;

	for (applid = 1; applid < MAX_APPL; applid++) {
		if (applidmap[applid] < 0) {
			applidmap[applid] = fd;
			return applid;
		}
	}
	return 0;
}

static inline void freeapplid(unsigned applid)
{
	if (applid < MAX_APPL)
		applidmap[applid] = -1;
}

static inline int validapplid(unsigned applid)
{
	return ((applid > 0) && (applid < MAX_APPL) && (applidmap[applid] >= 0));
}

inline int applid2fd(unsigned applid)
{
	if (applid < MAX_APPL)
		return applidmap[applid];

	return -1;
}

/*
 * buffer management
 */

struct recvbuffer {
	struct recvbuffer *next;
	unsigned int  datahandle;
	unsigned int  used;
	unsigned int  ncci;
	unsigned char *buf; /* 128 + MaxSizeB3 */
};

struct applinfo {
	unsigned  maxbufs;
	unsigned  nbufs;
	size_t    recvbuffersize;
	struct recvbuffer *buffers;
	struct recvbuffer *firstfree;
	struct recvbuffer *lastfree;
	unsigned char *bufferstart;
};

static struct applinfo *alloc_buffers(
	unsigned MaxB3Connection,
	unsigned MaxB3Blks,
	unsigned MaxSizeB3)
{
	struct applinfo *ap;
	unsigned nbufs = 2 + MaxB3Connection * (MaxB3Blks + 1);
	size_t recvbuffersize = 128 + MaxSizeB3;
	unsigned i;
	size_t size;

	if (recvbuffersize < 2048)
		recvbuffersize = 2048;

	size = sizeof(struct applinfo);
	size += sizeof(struct recvbuffer) * nbufs;
	size += recvbuffersize * nbufs;

	ap = (struct applinfo *)malloc(size);
	if (ap == 0)
		return 0;

	memset(ap, 0, size);
	ap->maxbufs = nbufs;
	ap->recvbuffersize = recvbuffersize;
	ap->buffers = (struct recvbuffer *)(ap+1);
	ap->firstfree = ap->buffers;
	ap->bufferstart = (unsigned char *)(ap->buffers+nbufs);
	for (i = 0; i < ap->maxbufs; i++) {
		ap->buffers[i].next = &ap->buffers[i+1];
		ap->buffers[i].used = 0;
		ap->buffers[i].ncci = 0;
		ap->buffers[i].buf = ap->bufferstart+(recvbuffersize*i);
	}
	ap->lastfree = &ap->buffers[ap->maxbufs-1];
	ap->lastfree->next = 0;
	return ap;
}

static void free_buffers(struct applinfo *ap)
{
	free(ap);
}

static struct applinfo *applinfo[MAX_APPL];

unsigned char *get_buffer(unsigned applid, size_t *sizep, unsigned *handle)
{
	struct applinfo *ap;
	struct recvbuffer *buf;

	assert(validapplid(applid));
	ap = applinfo[applid];
	if ((buf = ap->firstfree) == 0)
		return 0;

	ap->firstfree = buf->next;
	buf->next = 0;
	buf->used = 1;
	ap->nbufs++;
	*sizep = ap->recvbuffersize;
	*handle  = (buf->buf-ap->bufferstart)/ap->recvbuffersize;

	return buf->buf;
}

void save_datahandle(
	unsigned char applid,
	unsigned offset,
	unsigned datahandle,
	unsigned ncci)
{
	struct applinfo *ap;
	struct recvbuffer *buf;

	assert(validapplid(applid));
	ap = applinfo[applid];
	assert(offset < ap->maxbufs);
	buf = ap->buffers+offset;
	buf->datahandle = datahandle;
	buf->ncci = ncci;
}

unsigned return_buffer(unsigned char applid, unsigned offset)
{
	struct applinfo *ap;
	struct recvbuffer *buf;

	assert(validapplid(applid));
	ap = applinfo[applid];
	assert(offset < ap->maxbufs);
	buf = ap->buffers+offset;
	assert(buf->used == 1);
	assert(buf->next == 0);

	if (ap->lastfree) {
		ap->lastfree->next = buf;
		ap->lastfree = buf;
	} else {
		ap->firstfree = ap->lastfree = buf;
	}
	buf->used = 0;
	buf->ncci = 0;
	assert(ap->nbufs-- > 0);

	return buf->datahandle;
}

void cleanup_buffers_for_ncci(unsigned char applid, unsigned ncci)
{
	struct applinfo *ap;
	unsigned i;
	
	assert(validapplid(applid));
	ap = applinfo[applid];

	for (i = 0; i < ap->maxbufs; i++) {
		if (ap->buffers[i].used) {
			assert(ap->buffers[i].ncci != 0);
			if (ap->buffers[i].ncci == ncci) {
				return_buffer(applid, i);
			}
		}
	}
}

void cleanup_buffers_for_plci(unsigned char applid, unsigned plci)
{
	struct applinfo *ap;
	unsigned i;
	
	assert(validapplid(applid));
	ap = applinfo[applid];

	for (i = 0; i < ap->maxbufs; i++) {
		if (ap->buffers[i].used) {
			assert(ap->buffers[i].ncci != 0);
			if ((ap->buffers[i].ncci & 0xffff) == plci) {
				return_buffer(applid, i);
			}
		}
	}
}

/* 
 * CAPI2.0 functions
 */

/** Simple single-linked list for modules */
struct sList {
	struct sModule *psMod;
	struct sList *psNext;
};

/* Active module pointer */
static struct sModule *psModule = NULL;
/* Global loaded module list */
static struct sList *psModuleList = NULL;

/**
 * \brief Register Module - add it to psModuleList
 * \param psMod new module pointer
 * \return error code. 0 on success, else error occurred
 */
static int RegisterModule( struct sModule *psMod ) {
	struct sList *psList;

	if ( psMod == NULL || psMod -> pnName == NULL || psMod -> nVersion != MODULE_LOADER_VERSION || psMod -> psOperations == NULL ) {
		return -1;
	}

	if ( psModuleList == NULL ) {
		psModuleList = malloc( sizeof( struct sList ) );
		if ( psModuleList != NULL ) {
			psModuleList -> psMod = psMod;
			psModuleList -> psNext = NULL;
		}

		return psModuleList != NULL ? 0 : -1;
	}

	psList = psModuleList;
	while ( psList -> psNext ) {
		psList = psList -> psNext;
	}

	psList -> psNext = malloc( sizeof( struct sList ) );
	if ( psList -> psNext != NULL ) {
		psList -> psNext -> psMod = psMod;
		psList -> psNext -> psNext = NULL;
		return 0;
	}

	return -2;
}

/**
 * \brief Try to load the module and register it
 * \param pnName full path name to module
 * \return error code, see RegisterModule
 */
static int LoadModule( char *pnName ) {
	struct sModule *psMod;
	void *pHandle;
	typedef int ( *InitModule )( struct sModule *psModule );
	InitModule initModule;

	/* Try to open module */
	pHandle = dlopen( pnName, RTLD_GLOBAL | RTLD_LAZY );
	if ( pHandle == NULL ) {
		CapiDebug( 1, "Could not open module!\n" );
		return -1;
	}

	/* Find InitModule function within module */
	initModule = dlsym( pHandle, "InitModule" );
	if ( initModule == NULL ) {
		CapiDebug( 1, "Module has no InitModule!\n" );
		dlclose( pHandle );
		return -2;
	}

	/* Create new module structure */
	psMod = malloc( sizeof( struct sModule ) );
	if ( psMod == NULL ) {
		CapiDebug( 1, "Could not alloc memory for module structure!\n" );
		dlclose( pHandle );
		return -2;
	}

	/* Initialize module */
	if ( initModule( psMod ) != 0 ) {
		CapiDebug( 1, "Could not init!\n" );
		free( psMod );
		psMod = NULL;
		dlclose( pHandle );
		return -3;
	}

	/* Success, add handle to module structure */
	psMod -> pHandle = pHandle;

	/* register module */
	return RegisterModule( psMod );
}

/**
 * \brief Initialize module
 * \param pnModuleDir directory where we can find the modules
 */
static void InitModules( char *pnModuleDir ) {
	DIR *psDir;
	struct dirent *psEntry;
	char *pnFullName;
	int nLen;

	/* try to open module directory */
	psDir = opendir( pnModuleDir );
	if ( psDir != NULL ) {
		/* read entry by entry */
		while ( ( psEntry = readdir( psDir ) ) != NULL )  {
			/* skip ".", ".." and files which do not end with "so" */
			nLen = strlen( psEntry -> d_name );

			switch ( nLen ) {
				case 1:
					if ( psEntry -> d_name[ 0 ] == '.' ) {
						continue;
					}
					break;
				case 2:
					if ( psEntry -> d_name[ 0 ] == '.' && psEntry -> d_name[ 1 ] == '.' ) {
						continue;
					}
					break;
				default:
					if ( strncmp( psEntry -> d_name + nLen - 3, ".so", nLen ) ) {
						continue;
					}
					break;
			}

			/* len of complete path + filename */
			nLen = strlen( pnModuleDir ) + strlen( psEntry -> d_name ) + 2;
			pnFullName = malloc( nLen );
			if ( pnFullName != NULL ) {
				/* create full name */
				snprintf( pnFullName, nLen, "%s/%s", pnModuleDir, psEntry -> d_name );
				/* load module */
				LoadModule( pnFullName );

				/* free full name */
				free( pnFullName );
				pnFullName = NULL;
			}
		}

		/* close directory */
		closedir( psDir );
	}
}

unsigned capi20_isinstalled( void ) {
	struct sList *psList;

	/* check if we are already opened */
	if ( capi_fd >= 0 ) {
		return CapiNoError;
	}

	/* Load and initialize modules */
	InitModules( LIBDIR );

	if ( psModuleList == NULL ) {
		/* if no modules are loaded, psModuleList is NULL, abort */
		return CapiRegNotInstalled;
	}

	/* read configuration file */
	read_config();

	/* no special driver requested, auto-detect */
	if ( strlen( driver ) <= 0 ) {
		/* backwards-compatible: check standard interface fist */
		psList = psModuleList;
		while ( psList != NULL ) {
			
			if ( !strcasecmp( psList -> psMod -> pnName, "standard" ) ) {
				psModule = psList -> psMod;
				capi_fd = psModule -> psOperations -> IsInstalled();
				if ( capi_fd >= 0 ) {
					/* no error */
					return CapiNoError;
				}
			}
			psList = psList -> psNext;
		}

		/* no standard device detect, try the other modules */
		psList = psModuleList;
		while ( psList != NULL ) {
			if ( strcasecmp( psList -> psMod -> pnName, "standard" ) ) {
				psModule = psList -> psMod;
				capi_fd = psModule -> psOperations -> IsInstalled();
				if ( capi_fd >= 0 ) {
					/* no error */
					return CapiNoError;
				}
			}
			psList = psList -> psNext;
		}
	} else {
		/* Find requested driver in list */
		psList = psModuleList;
		while ( psList != NULL ) {
			if ( !strcasecmp( psList -> psMod -> pnName, driver ) ) {
				psModule = psList -> psMod;
				capi_fd = psModule -> psOperations -> IsInstalled();
				if ( capi_fd >= 0 ) {
					/* no error */
					return CapiNoError;
				}
				break;
			}
			psList = psList -> psNext;
		}
	}

	/* uhh, not installed */
	return CapiRegNotInstalled;
}

unsigned capi20_register( unsigned MaxB3Connection, unsigned MaxB3Blks, unsigned MaxSizeB3, unsigned *ApplID ) {
	int applid = 0;
	int fd = -1;

    *ApplID = 0;

    if (capi20_isinstalled() != CapiNoError)
       return CapiRegNotInstalled;

	fd = psModule -> psOperations -> Register( MaxB3Connection, MaxB3Blks, MaxSizeB3, &applid );
	if ( fd < 0 ) {
		return CapiRegOSResourceErr;
	}

	if (remember_applid(applid, fd) < 0) {
		close(fd);
		return CapiRegOSResourceErr;
	}
	applinfo[applid] = alloc_buffers(MaxB3Connection, MaxB3Blks, MaxSizeB3);
	if (applinfo[applid] == 0) {
		close(fd);
		return CapiRegOSResourceErr;
	}
	*ApplID = applid;
	return CapiNoError;
}

unsigned
capi20_release (unsigned ApplID)
{
	if (capi20_isinstalled() != CapiNoError)
		return CapiRegNotInstalled;

	if (!validapplid(ApplID))
		return CapiIllAppNr;

	if ( psModule -> psOperations -> Release != NULL ) {
		psModule -> psOperations -> Release( applid2fd( ApplID ), ApplID );
	}

	(void)close(applid2fd(ApplID));
	freeapplid(ApplID);
	free_buffers(applinfo[ApplID]);
	applinfo[ApplID] = 0;

	return CapiNoError;
}

/**
 * \brief Process data message (should be moved up to capi20.c as it is general)
 * \param pnMsg message data pointer
 * \param nApplId application id
 * \param nCommand major packet command
 * \param nSubCommand minor packet command
 * \param nLen len of message
 * \return length of full packet
 */
int processMessage( unsigned char *pnMsg, int nApplId, int nCommand, int nSubCommand, int nLen ) {
	/* DATA_B3_REQ specific:
	 * we have to copy  the buffer and patch the buffer address!
	 */
	if ( nCommand == CAPI_DATA_B3 ) {
		if ( nSubCommand == CAPI_REQ ) {
			int nDataLen = CAPIMSG_DATALEN( pnMsg );
			void *pDataPtr;

			if ( sizeof( void * ) != 4 ) {
				if ( nLen >= 30 ) {
					/* 64Bit CAPI-extension */
					u_int64_t nData64;

					memcpy( &nData64, pnMsg + 22, sizeof( u_int64_t ) );
					if ( nData64 != 0 ) {
						pDataPtr = ( void * )( unsigned long ) nData64;
					} else {
						/* Assume data after message */
						pDataPtr = pnMsg + nLen;
					}
				} else {
					/* Assume data after message */
					pDataPtr = pnMsg + nLen;
				}
			} else {
				u_int32_t nData;

				memcpy( &nData, pnMsg + 12, sizeof( u_int32_t ) );
				if ( nData != 0 ) {
					pDataPtr = ( void * )( ( unsigned long ) nData );
				} else {
					pDataPtr = pnMsg + nLen;
				}
			}

			if ( nLen + nDataLen > SEND_BUFSIZ ) {
				return CapiMsgOSResourceErr;
			}

			memcpy( pnMsg + nLen, pDataPtr, nDataLen );
			nLen += nDataLen;
		} else if ( nSubCommand == CAPI_RESP ) {
			capimsg_setu16( pnMsg, 12, return_buffer( nApplId, CAPIMSG_U16( pnMsg, 12 ) ) );
		}
	}

	if ( nCommand == CAPI_DISCONNECT_B3 && nSubCommand == CAPI_RESP ) {
		cleanup_buffers_for_ncci( nApplId, CAPIMSG_U32( pnMsg, 8 ) );
	}

	return nLen;
}

unsigned capi20_put_message( unsigned ApplID, unsigned char *Msg ) {
	if (capi20_isinstalled() != CapiNoError) {
		return CapiRegNotInstalled;
	}

	if (!validapplid(ApplID)) {
		return CapiIllAppNr;
	}

	return psModule -> psOperations -> PutMessage( applid2fd(ApplID), ApplID, Msg );
}

unsigned capi20_get_message( unsigned ApplID, unsigned char **Buf ) {
	if (capi20_isinstalled() != CapiNoError)
		return CapiRegNotInstalled;

	if (!validapplid(ApplID))
		return CapiIllAppNr;

	return psModule -> psOperations -> GetMessage( applid2fd(ApplID), ApplID, Buf );
}

unsigned char *capi20_get_manufacturer( unsigned Ctrl, unsigned char *Buf ) {
	if (capi20_isinstalled() != CapiNoError)
		return 0;

	return psModule -> psOperations -> GetManufactor( capi_fd, Ctrl, Buf );
}

unsigned char *
capi20_get_version(unsigned Ctrl, unsigned char *Buf)
{
	if (capi20_isinstalled() != CapiNoError)
		return 0;

	return psModule -> psOperations -> GetVersion( capi_fd, Ctrl, Buf );
}

unsigned char * 
capi20_get_serial_number(unsigned Ctrl, unsigned char *Buf)
{
	if (capi20_isinstalled() != CapiNoError)
		return 0;

	return psModule -> psOperations -> GetSerialNumber( capi_fd, Ctrl, Buf );
}

unsigned
capi20_get_profile(unsigned Ctrl, unsigned char *Buf)
{
	if (capi20_isinstalled() != CapiNoError)
		return CapiMsgNotInstalled;

	return psModule -> psOperations -> GetProfile( capi_fd, Ctrl, Buf );
}
/*
 * functions added to the CAPI2.0 spec
 */

unsigned
capi20_waitformessage(unsigned ApplID, struct timeval *TimeOut)
{
	int fd;
	fd_set rfds;

	FD_ZERO(&rfds);

	if (capi20_isinstalled() != CapiNoError)
		return CapiRegNotInstalled;

	if (!validapplid(ApplID))
		return CapiIllAppNr;
  
	fd = applid2fd(ApplID);

	FD_SET(fd, &rfds);

	if (select(fd + 1, &rfds, NULL, NULL, TimeOut) < 1)
		return CapiReceiveQueueEmpty;
  
	return CapiNoError;
}

int
capi20_fileno(unsigned ApplID)
{
	return applid2fd(ApplID);
}

/*
 * Extensions for middleware
 */

int
capi20ext_get_flags(unsigned ApplID, unsigned *flagsptr)
{
	if ( psModule && psModule -> psOperations -> GetFlags ) {
		return psModule -> psOperations -> GetFlags( ApplID, flagsptr );
	}

	return CapiMsgOSResourceErr;
}

int
capi20ext_set_flags(unsigned ApplID, unsigned flags)
{
	if ( psModule && psModule -> psOperations -> SetFlags ) {
		return psModule -> psOperations -> SetFlags( ApplID, flags );
	}

	return CapiMsgOSResourceErr;
}

int
capi20ext_clr_flags(unsigned ApplID, unsigned flags)
{
	if ( psModule && psModule -> psOperations -> ClearFlags ) {
		return psModule -> psOperations -> ClearFlags( ApplID, flags );
	}

	return CapiMsgOSResourceErr;
}

char *
capi20ext_get_tty_devname(unsigned applid, unsigned ncci, char *buf, size_t size)
{
	if ( psModule && psModule -> psOperations -> GetTtyDeviceName ) {
		return psModule -> psOperations -> GetTtyDeviceName( applid, ncci, buf, size );
	}

	return NULL;
}

char *capi20ext_get_raw_devname( unsigned applid, unsigned ncci, char *buf, size_t size ) {
	if ( psModule && psModule -> psOperations -> GetRawDeviceName ) {
		return psModule -> psOperations -> GetRawDeviceName( applid, ncci, buf, size );
	}

	return NULL;
}

int capi20ext_ncci_opencount( unsigned applid, unsigned ncci ) {
	if ( psModule && psModule -> psOperations -> GetNcciOpenCount ) {
		return psModule -> psOperations -> GetNcciOpenCount( applid, ncci );
	}

	return CapiMsgOSResourceErr;
}

static void initlib( void ) __attribute__( ( constructor ) );
static void exitlib( void ) __attribute__( ( destructor ) );

static void initlib( void ) {
	int i;

	for ( i = 0; i < MAX_APPL; i++ ) {
		applidmap[ i ] = -1;
	}
}

static void exitlib( void ) {
	if ( capi_fd >= 0 ) {
		close( capi_fd );
		capi_fd = -1;
	}
}
