#include "freecdbmake.h"

void cdbmake_pack(buf,num)
unsigned char *buf;
uint32_t num;
{
#ifdef WORDS_BIGENDIAN
  buf[3] = num; num >>= 8;
  buf[2] = num; num >>= 8;
  buf[1] = num; num >>= 8;
  buf[0] = num;
#else
  *buf++ = num; num >>= 8;
  *buf++ = num; num >>= 8;
  *buf++ = num; num >>= 8;
  *buf = num;
#endif  
}
