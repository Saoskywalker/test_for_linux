#ifndef _MTF_FILE_H
#define _MTF_FILE_H

#include "stdio.h"

long MTF_fsize(const char *filename);
int MTF_load_file(unsigned char **out, size_t *outsize, const char *filename);

#endif
