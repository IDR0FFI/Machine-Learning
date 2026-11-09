#include "dataset.h"

#include <stdlib.h>

int read_be32(FILE *fptr, uint32_t *result) {
  uint8_t byte[4];

  if (fread(byte, 1, 4, fptr) != 4)
    return 0;

  *result = ((uint32_t)byte[0] << 24)
            | ((uint32_t)byte[1] << 16)
            | ((uint32_t)byte[2] <<  8)
            |  (uint32_t)byte[3];

  return 1;
}

int stat_dataset(const char* path, void* header) {
  FILE *fptr = fopen(path, "rb");
  if (!fptr) {
    perror("fopen");
    return 1;
  }

  uint32_t magic_number;
  if (!read_be32(fptr, &magic_number)) {
    fprintf(stderr, "Invalid magic number id\n");
    fclose(fptr);
    return 1;
  }

  if (magic_number == 0x0803) {
    // Image_Header
    uint32_t count, row, column;
    if (!read_be32(fptr, &count) ||
        !read_be32(fptr, &row) ||
        !read_be32(fptr, &column)) {
      fprintf(stderr, "Invalid or truncated image header\n");
      fclose(fptr);
      return 1;
    }

    Image_Header *h = (Image_Header*)header;
    h->id.magic_number = magic_number;
    h->id.count = count;
    h->row = row;
    h->column = column;
  }
  else if (magic_number == 0x0801) {
    // Label Header
    uint32_t count;
    if (!read_be32(fptr, &count)) {
      fprintf(stderr, "Invalid or truncated label header\n");
      fclose(fptr);
      return 1;
    }

    Label_Header *h = (Label_Header*)header;
    h->magic_number = magic_number;
    h->count = count;
  }
  else {
    fprintf(stderr, "Not supported header\n");
    fclose(fptr);
    return 1;
  }

  fclose(fptr);
  return 0;
}

int load_idx_format(const char* path, Data* data, const void* header) {
  FILE *fptr = fopen(path, "rb");
  if (!fptr) {
    perror("fopen");
    return 1;
  }
  
  uint32_t magic_number = *(uint32_t*)header;
  if (magic_number == 0x0803) {
    Image_Header *h = (Image_Header*)header;

    fseek(fptr, 16, SEEK_SET);
    size_t size = h->row * h->column;
    int index=0, count = h->id.count;

    for (int i=0; i<count; i++) {
      uint8_t *image = (uint8_t*)malloc(size);
      if (image == NULL) {
        fprintf(stderr, "Could not allocate images\n");
        fclose(fptr);
        return 1;
      }

      if (fread(image, sizeof(uint8_t), size, fptr) != size) {
        fprintf(stderr, "Invalid or truncated image\n");
        fclose(fptr);
        return 1;
      }

      data[index].size = size;
      data[index].block = image;
      index++;
    }
  }
  else if (magic_number == 0x0801) {
    Label_Header *h = (Label_Header*)header;

    fseek(fptr, 8, SEEK_SET);

    uint8_t *label = (uint8_t*)malloc(h->count);
    if (fread(label, sizeof(uint8_t), h->count, fptr) != h->count) {
      fprintf(stderr, "Invalid or truncated lable\n");
      fclose(fptr);
      return 1;
    }

    data->size = h->count;
    data->block = label;
  }
  else {
    fprintf(stderr, "Unsupported header cannot load file\n");
    fclose(fptr);
    return 1;
  }

  fclose(fptr);
  return 0;
}
