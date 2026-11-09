#ifndef __UTIL_H
#define __UTIL_H

#include <stdint.h>
#include <stddef.h>

typedef struct {
  size_t size;
  float *values;
} f_array; /* Really bad at naming */

typedef struct {
  size_t row;
  size_t col;
  float *values;
} f_matrix;

typedef struct {
  size_t   size;
  uint8_t *block;
} Data;

int free_Data(Data*, size_t);

char give_base(uint8_t);
void base64(uint8_t*);
void touint(f_array*, Data*);

float randf();

#endif // __UTIL_H
