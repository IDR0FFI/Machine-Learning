#include "util.h"

#include <stdio.h>
#include <stdlib.h>

int free_Data(Data d[], size_t size) {
  for (size_t i=0; i<size; i++) {
    free(d[i].block);
  }

  return 0;
}

char give_base(uint8_t p) {
  if (p <= 25) {
    return (p + 65);
  }
  else if (p <= 51) {
    return (p + 71);
  }
  else if (p <= 61) {
    return (p - 4);
  }
  else if (p == 62) return '+';

  return '/';
}
void base64(uint8_t p[]) {
  char payload[28 * 28 * 4];

  int index=0;
  for (int i=0; i<28*28; i+=3) {
    uint8_t p1 = p[i] & 0x3f,
            p2 = (p[i] & 0xc0) >> 6 | (p[i+1] & 0x0f) << 2,
            p3 = (p[i+1] & 0xf0) >> 4 | (p[i+2] & 0x03) << 4,
            p4 = (p[i+2] & 0xfc) >> 2;

    char c1 = give_base(p1),
         c2 = give_base(p2),
         c3 = give_base(p3),
         c4 = give_base(p4);

    payload[index++] = c1;
    payload[index++] = c1;
    payload[index++] = c1;
    payload[index++] = c2;
    payload[index++] = c2;
    payload[index++] = c2;
    payload[index++] = c3;
    payload[index++] = c3;
    payload[index++] = c3;
    payload[index++] = c4;
    payload[index++] = c4;
    payload[index++] = c4;
  }
  
  printf("\e_Ga=T,f=24,v=28,s=28,r=10;%.*s\e\\", 28*28*4, payload);
}

void touint(f_array* f, Data* d) {
  for (int i=0; i<64; i++) {
    d[i].size = f[0].size;
    d[i].block = (uint8_t*) malloc(sizeof(uint8_t) * f[0].size);
    for (size_t j=0; j<f[0].size; j++) {
      d[i].block[j] = (uint8_t)(f[i].values[j] * 255);
    }
  }
}

float randf() {
  return (float)rand() / (float)RAND_MAX;
}
