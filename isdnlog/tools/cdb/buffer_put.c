#include "buffer.h"
#include "str.h"
#include "byte.h"
#include "error.h"

static int allwrite(int (*op)(),int fd,char *buf,unsigned int len)
{
  int w;

  while (len) {
    w = op(fd,buf,len);
    if (w == -1) {
      if (errno == error_intr) continue;
      return -1; /* note that some data may have been written */
    }
    if (w == 0) ; /* luser's fault */
    buf += w;
    len -= w;
  }
  return 0;
}

int buffer_flush(cdb_buffer *s)
{
  int p;
 
  p = s->p;
  if (!p) return 0;
  s->p = 0;
  return allwrite(s->op,s->fd,s->x,p);
}

int buffer_putalign(cdb_buffer *s,char *buf,unsigned int len)
{
  unsigned int n;
 
  while (len > (n = s->n - s->p)) {
    byte_copy(s->x + s->p,n,buf); s->p += n; buf += n; len -= n;
    if (buffer_flush(s) == -1) return -1;
  }
  /* now len <= s->n - s->p */
  byte_copy(s->x + s->p,len,buf);
  s->p += len;
  return 0;
}

int buffer_put(cdb_buffer *s,char *buf,unsigned int len)
{
  unsigned int n;
 
  n = s->n;
  if (len > n - s->p) {
    if (buffer_flush(s) == -1) return -1;
    /* now s->p == 0 */
    if (n < BUFFER_OUTSIZE) n = BUFFER_OUTSIZE;
    while (len > s->n) {
      if (n > len) n = len;
      if (allwrite(s->op,s->fd,buf,n) == -1) return -1;
      buf += n;
      len -= n;
    }
  }
  /* now len <= s->n - s->p */
  byte_copy(s->x + s->p,len,buf);
  s->p += len;
  return 0;
}

int buffer_putflush(cdb_buffer *s,char *buf,unsigned int len)
{
  if (buffer_flush(s) == -1) return -1;
  return allwrite(s->op,s->fd,buf,len);
}

int buffer_putsalign(cdb_buffer *s,char *buf)
{
  return buffer_putalign(s,buf,str_len(buf));
}

int buffer_puts(cdb_buffer *s,char *buf)
{
  return buffer_put(s,buf,str_len(buf));
}

int buffer_putsflush(cdb_buffer *s,char *buf)
{
  return buffer_putflush(s,buf,str_len(buf));
}
