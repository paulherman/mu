#ifndef FILE_BUF_H
#define FILE_BUF_H

#include <stdbool.h>
#include <stdint.h>

struct file_buffer {
  size_t length;
  uint8_t *data;
  size_t position;
};

bool file_buffer_read(struct file_buffer *fb, const char *path);
void file_buffer_delete(struct file_buffer *fb);
#endif
