#include "readwrite.h"
#include "buffer.h"

char buffer_1_space[BUFFER_OUTSIZE];
static cdb_buffer it = BUFFER_INIT(write,1,buffer_1_space,sizeof buffer_1_space);
cdb_buffer *buffer_1 = &it;
