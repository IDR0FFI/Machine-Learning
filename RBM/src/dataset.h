#ifndef __DATASET_H
#define __DATASET_H

#include <stdint.h>
#include <stdio.h>

#include "util.h"

typedef struct {
  uint32_t magic_number;
  uint32_t count;
} IDX_Header, Label_Header;

typedef struct {
  IDX_Header id;
  uint32_t row;
  uint32_t column;
} Image_Header;

int read_be32(FILE *fptr, uint32_t *result);

int stat_dataset(const char* path, void* header);
int load_idx_format(const char* path, Data data[], const void* header);

#endif // __DATASET_H
