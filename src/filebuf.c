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

bool file_buffer_read_float(struct file_buffer *file, float *value) {
  if (file->position + sizeof(float) >= file->length)
    return false;
  *value = *(float *)&file->data[file->position];
  file->position += sizeof(float);
  return true;
}

bool file_buffer_read_uint8(struct file_buffer *file, uint8_t *value) {
  if (file->position + sizeof(uint8_t) >= file->length)
    return false;
  memcpy(value, file->data + file->position, sizeof(uint8_t));
  file->position += sizeof(uint8_t);
  return true;
}

bool file_buffer_read_uint16(struct file_buffer *file, uint16_t *value) {
  if (file->position + sizeof(uint16_t) >= file->length)
    return false;
  memcpy(value, file->data + file->position, sizeof(uint16_t));
  file->position += sizeof(uint16_t);
  return true;
}

bool file_buffer_read_int32(struct file_buffer *file, int32_t *value) {
  if (file->position + sizeof(int32_t) >= file->length)
    return false;
  memcpy(value, file->data + file->position, sizeof(int32_t));
  file->position += sizeof(int32_t);
  return true;
}

bool file_buffer_read_int16(struct file_buffer *file, int16_t *value) {
  if (file->position + sizeof(int16_t) >= file->length)
    return false;
  memcpy(value, file->data + file->position, sizeof(int16_t));
  file->position += sizeof(int16_t);
  return true;
}

bool file_buffer_read_uint32(struct file_buffer *file, uint32_t *value) {
  if (file->position + sizeof(uint32_t) >= file->length)
    return false;
  *value = *(uint32_t *)&file->data[file->position];
  file->position += sizeof(uint32_t);
  return true;
}

bool file_buffer_read_array(struct file_buffer *file, void *arr, size_t element_size, size_t length) {
  size_t read_size = length * element_size;
  if (file->position + read_size >= file->length)
    return false;
  memcpy(arr, file->data + file->position, read_size);
  file->position += read_size;
  return true;
}

bool file_buffer_read_string(struct file_buffer *file, char *str, size_t length) {
  return file_buffer_read_array(file, str, sizeof(char), length);
}

bool file_buffer_read_bool(struct file_buffer *file, bool *value) {
  if (file->position + sizeof(bool) >= file->length)
    return false;
  *value = *(bool *)&file->data[file->position];
  file->position += sizeof(bool);
  return true;
}
