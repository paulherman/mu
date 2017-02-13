#include "map.h"
#include "filebuf.h"
#include "xor_decrypt.h"

bool map_load(struct map *map, const char *path) {
  struct file_buffer file;
  if (!file_buffer_read(&file, path))
    return false;

  uint8_t header;
  if (!file_buffer_read_uint8(&file, &header))
    goto error;

  if (!file_buffer_read_int32(&file, &map->width))
    goto error;

  if (!file_buffer_read_int32(&file, &map->height))
    goto error;

  if (map->width < 0 || map->width > MAP_MAX_WIDTH || map->height < 0 || map->height > MAP_MAX_HEIGHT)
    goto error;

  if (!file_buffer_read_array(&file, map->attributes, sizeof(uint8_t), MAP_MAX_WIDTH * MAP_MAX_HEIGHT))
    goto error;

  file_buffer_delete(&file);
  return true;
error:
  file_buffer_delete(&file);
  return false;
}

bool map_object_defs_load(struct map_object_def *object_defs, const size_t max_num_objects, const char *path) {
  struct file_buffer file;
  if (!file_buffer_read(&file, path))
    return false;

  xor_decrypt(&file, 0);

  uint16_t map_id, num_objects;
  if (!file_buffer_read_uint16(&file, &map_id))
    goto error;
  if (!file_buffer_read_uint16(&file, &num_objects))
    goto error;

  if (num_objects > max_num_objects)
    goto error;

  if (!file_buffer_read_array(&file, object_defs, sizeof(struct map_object_def), num_objects))
    goto error;

  file_buffer_delete(&file);
  return true;
error:
  file_buffer_delete(&file);
  return false;
}
