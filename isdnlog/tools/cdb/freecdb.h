#ifndef CDB_H
#define CDB_H

#include <stdio.h>
#include "config.h"
#if SIZEOF_INT==4
typedef unsigned int uint32_t ;
#elif SIZEOF_LONG==4
typedef unsigned long uint32_t ;
#else
#error "No uint32"
#endif
extern uint32_t cdb_hash();
extern uint32_t cdb_unpack();

extern int cdb_bread();
extern int cdb_seek();

#endif
