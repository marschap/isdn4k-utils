#include "readwrite.h"
#include "buffer.h"

char buffer_1small_space[256];
static cdb_buffer it = BUFFER_INIT(write,1,buffer_1small_space,sizeof buffer_1small_space);
cdb_buffer *buffer_1small = &it;
