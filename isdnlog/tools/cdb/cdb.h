#ifndef _CDB_H_
#define _CDB_H_

#include "freecdb.h"

typedef uint32_t uint32;

struct cdb {
    int fd;
    uint32_t dpos;
    uint32_t dlen;
    } ;
    
void cdb_init(struct cdb *cdb, int fd);
void cdb_free(struct cdb *cdb);
int cdb_read(struct cdb *cdb, char *buf,unsigned int len, uint32_t pos);
void cdb_findstart(struct cdb *cdb);
int cdb_findnext(struct cdb *cdb ,char *key, unsigned int len);

#endif
