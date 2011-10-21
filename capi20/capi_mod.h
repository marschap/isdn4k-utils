#ifndef CAPI_MOD_H
#define CAPI_MOD_H

struct sModuleOperations {
	unsigned ( *IsInstalled )( void );
	unsigned ( *Register )( unsigned nMaxB3Connection, unsigned nMaxB3Blks, unsigned nMaxSizeB3, unsigned *pnApplId );
	unsigned ( *Release )( int nHandle, int nApplId );
	unsigned ( *PutMessage )( int nHandle, unsigned nApplId, unsigned char *pnMessage );
	unsigned ( *GetMessage )( int nHandle, unsigned nApplId, unsigned char **ppnBuffer );
	unsigned char *( *GetManufactor )( int nHandle, unsigned nController, unsigned char *pnBuffer );
	unsigned char *( *GetVersion )( int nHandle, unsigned nController, unsigned char *pnBuffer );
	unsigned char *( *GetSerialNumber )( int nHandle, unsigned nController, unsigned char *pnBuffer );
	unsigned ( *GetProfile )( int nHandle, unsigned nController, unsigned char *pnBuffer );

	/* The following parts are only used on the standard capi device */
	int ( *GetFlags )( unsigned nApplId, unsigned *pnFlagsPtr );
	int ( *SetFlags )( unsigned nApplId, unsigned nFlags );
	int ( *ClearFlags )( unsigned nApplId, unsigned nFlags );
	char *( *GetTtyDeviceName )( unsigned nApplId, unsigned nNcci, char *pnBuffer, size_t nSize );
	char *( *GetRawDeviceName )( unsigned nApplId, unsigned nNcci, char *pnBuffer, size_t nSize );
	int ( *GetNcciOpenCount )( unsigned nApplId, unsigned nNcci );
} __attribute__ ((packed));

struct sModule {
	char *pnName;
	int nVersion;
	void *pHandle;
	struct sModuleOperations *psOperations;
} __attribute__ ((packed));

char *getHostName( void );
int getPort( void );
char *getTraceFile( void );
int getTraceLevel( void );
unsigned char *get_buffer( unsigned applid, size_t *sizep, unsigned *handle );

#define MODULE_LOADER_VERSION		0x01

#define MODULE_INIT( NAME, OPS )\
	int InitModule( struct sModule *psModule ) {\
		psModule -> pnName = strdup( NAME );\
		psModule -> nVersion = MODULE_LOADER_VERSION;\
		psModule -> psOperations = OPS;\
		return 0;\
	}

#endif
