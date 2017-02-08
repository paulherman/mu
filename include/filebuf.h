#ifndef FILE_BUF_H
#define FILE_BUF_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

struct file_buffer {
  size_t length;
  uint8_t *data;
  size_t position;
};

bool file_buffer_read(struct file_buffer *fb, const char *path);
void file_buffer_delete(struct file_buffer *fb);
bool file_buffer_read_bool(struct file_buffer *file, bool *value);
bool file_buffer_read_float(struct file_buffer *file, float *value);
bool file_buffer_read_uint8(struct file_buffer *file, uint8_t *value);
bool file_buffer_read_uint16(struct file_buffer *file, uint16_t *value);
bool file_buffer_read_uint32(struct file_buffer *file, uint32_t *value);
bool file_buffer_read_int16(struct file_buffer *file, int16_t *value);
bool file_buffer_read_int32(struct file_buffer *file, int32_t *value);
bool file_buffer_read_string(struct file_buffer *file, char *str, size_t length);
bool file_buffer_read_array(struct file_buffer *file, void *arr, size_t element_size, size_t length);

#endif
