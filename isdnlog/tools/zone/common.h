/* mkzonedb.c, zone.c needs this */
#define READ 0
#define WRITE 1
#if HAVE_LIBGDBM
	#include <gdbm.h>
	static char dbv[]="GDBM";	/* don't change */
	#define OPEN(name,wr) gdbm_open(name,0,wr?GDBM_NEWDB:GDBM_READER,0644,0)
	#define CLOSE(db) gdbm_close(db)
	#define GET_ERR gdbm_strerror(gdbm_errno)
	#define STORE(db, key, value) gdbm_store(db, key, value, GDBM_INSERT)
	#define FETCH(db, key) gdbm_fetch(db, key)
	#define _DB GDBM_FILE
#elif HAVE_LIBDBM
	#include <db1/ndbm.h>
	#include <fcntl.h>
	static char dbv[]="DBM";
	#define OPEN(name,wr) dbm_open(name,wr?O_RDWR|O_CREAT:O_RDONLY,0644)
	#define CLOSE(db) dbm_close(db)
	#define GET_ERR "unknown"
	#define STORE(db, key, value) dbm_store(db, key, value, DBM_INSERT)
	#define FETCH(db, key) dbm_fetch(db, key)
	#define _DB DBM *
#elif HAVE_LIBDB
#if 0
 /* there are too mny libdb out there */
	#define DB_DBM_HSEARCH
	#include <db.h>
#else
	#include <db1/ndbm.h>
#endif
	#include <fcntl.h>
	static char dbv[]="DB";
	#define OPEN(name,wr) dbm_open(name,wr?O_RDWR|O_CREAT:O_RDONLY,0644)
	#define CLOSE(db) dbm_close(db)
	#define GET_ERR "unknown"
	#define STORE(db, key, value) dbm_store(db, key, value, DBM_INSERT)
	#define FETCH(db, key) dbm_fetch(db, key)
	#define _DB DB *
#else
	Sorry, no database found in configure
#endif

/* if the following doesn't - how can we find datatypes with len 1,2,4 ? */
#if SIZEOF_CHAR != 1
	Something is wrong sizeof(char) != 1
#else
	typedef unsigned char  UC;
#endif
#if SIZEOF_SHORT != 2
	Something is wrong sizeof(short) != 2
#else
	typedef unsigned short US; /* len 2 */
#endif
#if SIZEOF_LONG != 4
	#if SIZEOF_INT != 4
		Something is wrong sizeof(long/int) != 4
	#else
		typedef unsigned int  UL; /* len 4 */
	#endif
#else
	typedef unsigned long  UL; /* len 4 */
#endif

typedef enum {false,true} bool;
