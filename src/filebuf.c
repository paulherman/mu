#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "filebuf.h"

bool file_buffer_read(struct file_buffer *fb, const char *path) {
  FILE *file = fopen(path, "rb");
  if (file == NULL)
    false;

  uint8_t buffer[4096];
  uint8_t *contents = NULL;
  size_t num_batches = 0;

  while (fread(buffer, 1, 4096, file) != 0) {
    num_batches++;
    contents = realloc(contents, (4096 * num_batches + 1) * sizeof(uint8_t));
    memcpy(contents + 4096 * (num_batches - 1), buffer, sizeof(uint8_t) * 4096);
    memset(buffer, 0, sizeof(buffer));
  }

  if (ferror(file) != 0) {
    free(contents);
    fclose(file);
    return false;
  } else {
    fb->position = 0;
    fb->data = contents;
    fb->length = num_batches * 4096;
    fclose(file);
    return true;
  }
}

void file_buffer_delete(struct file_buffer *fb) {
  free(fb->data);
}
