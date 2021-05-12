#include "MTF_file.h"
#include <limits.h> /* LONG_MAX */
#include "malloc.h"

static void *MTF_malloc(size_t size)
{
  return malloc(size);
}

static void MTF_free(void *ptr)
{
  free(ptr);
}

long MTF_fsize(const char *filename)
{
  FILE *file;
  long size;
  file = fopen(filename, "rb");
  if (!file)
    return -1;

  if (fseek(file, 0, SEEK_END) != 0)
  {
    fclose(file);
    return -1;
  }

  size = ftell(file);
  /* It may give LONG_MAX as directory size, this is invalid for us. */
  if (size == LONG_MAX)
    size = -1;

  fclose(file);
  return size;
}

/* load file into buffer that already has the correct allocated size. Returns error code.*/
static int _buffer_file(unsigned char *out, size_t size, const char *filename)
{
  FILE *file;
  size_t readsize;
  file = fopen(filename, "rb");
  if (!file)
    return 78;

  readsize = fread(out, 1, size, file);
  fclose(file);

  if (readsize != size)
    return 78;
  return 0;
}

int MTF_load_file(unsigned char **out, size_t *outsize, const char *filename)
{
  long size = MTF_fsize(filename);
  if (size < 0)
    return 78;
  *outsize = (size_t)size;

  *out = (unsigned char *)MTF_malloc((size_t)size);
  if (!(*out) && size > 0)
    return 83; /*the above malloc failed*/

  return _buffer_file(*out, (size_t)size, filename);
}
