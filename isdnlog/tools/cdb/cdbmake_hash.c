#include "freecdbmake.h"

uint32_t cdbmake_hashadd(h,c)
uint32_t h;
unsigned int c;
{
  h += (h << 5);
  h ^= (uint32_t) (unsigned char) c;
  return h;
}
