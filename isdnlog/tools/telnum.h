/* telnum.h
  (c) 1999 by Leopold Toetsch <lt@toetsch.at>
*/  
#ifndef _TELNUM_H_
#define _TELNUM_H_
#include "isdnlog.h"
#include "tools/zone.h"

/* print_msg */
#define PRT_V 17 /* verbose */
#define PRT_A 18 /* always on stderr */

/* string lens */

#define TN_MAX_VBN_LEN 4
#define TN_MAX_PROVIDER_LEN 8
#define TN_MAX_COUNTRY_LEN 8
#define TN_MAX_AREA_LEN 10
#define TN_MAX_SAREA_LEN 40
#define TN_MAX_MSN_LEN 10
#define TN_MAX_NUM_LEN (TN_MAX_PROVIDER_LEN+TN_MAX_COUNTRY_LEN+TN_MAX_AREA_LEN+TN_MAX_MSN_LEN+4)

typedef struct {
  char vbn[TN_MAX_VBN_LEN];
  char provider[TN_MAX_PROVIDER_LEN];
  int nprovider;
  COUNTRY *country;
  int ncountry;
  char area[TN_MAX_AREA_LEN];
  int narea;
  char sarea[TN_MAX_SAREA_LEN];
  char msn[TN_MAX_MSN_LEN];
} TELNUM;

/* flags */
#define TN_PROVIDER 1
#define TN_COUNTRY 	2
#define TN_AREA		4
#define TN_MSN 		8
#define TN_ALL 		15
#define TN_NO_PROVIDER 14

/* functions */
void initTelNum(void);
int normalizeNumber(char *target, TELNUM *num, int flag);
char * formatNumber(char* format, TELNUM* num);
char *prefix2provider(int prefix, char*prov, TELNUM *num);
int provider2prefix(char *p, int *prefix, TELNUM *num); 
void clearNum(TELNUM *num);
void initNum(TELNUM *num);

#endif
