#include "freecdb.h"

uint32_t cdb_unpack(buf)
unsigned char *buf;
{
  uint32_t num;
#ifdef WORDS_BIGENDIAN
  num = buf[0]; num <<= 8;
  num += buf[1]; num <<= 8;
  num += buf[2]; num <<= 8;
  num += buf[3];
#else  
  num = buf[3]; num <<= 8;
  num += buf[2]; num <<= 8;
  num += buf[1]; num <<= 8;
  num += buf[0];
#endif  
  return num;
}