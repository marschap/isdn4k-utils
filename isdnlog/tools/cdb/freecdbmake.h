#ifndef CDBMAKE_H
#define CDBMAKE_H

#include <stdio.h>
#ifndef CDB_H
#include "config.h"
#if SIZEOF_INT==4
typedef unsigned int uint32_t ;
#elif SIZEOF_LONG==4
typedef unsigned long uint32_t ;
#else
#error "No uint32"
#endif
#endif

#define CDBMAKE_HPLIST 1000

struct cdbmake_hp { uint32_t h; uint32_t p; } ;

struct cdbmake_hplist {
  struct cdbmake_hp hp[CDBMAKE_HPLIST];
  struct cdbmake_hplist *next;
  int num;
} ;

struct cdbmake {
  char final[2048];
  uint32_t count[256];
  uint32_t start[256];
  struct cdbmake_hplist *head;
  struct cdbmake_hp *split; /* includes space for hash */
  struct cdbmake_hp *hash;
  uint32_t numentries;
} ;

extern void cdbmake_pack();
#define CDBMAKE_HASHSTART ((uint32_t) 5381)
extern uint32_t cdbmake_hashadd();

extern void cdbmake_init();
extern int cdbmake_add();
extern int cdbmake_split();
extern uint32_t cdbmake_throw();

#endif
