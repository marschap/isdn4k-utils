#include "readwrite.h"
#include "buffer.h"

char buffer_2_space[256];
static cdb_buffer it = BUFFER_INIT(write,2,buffer_2_space,sizeof buffer_2_space);
cdb_buffer *buffer_2 = &it;
