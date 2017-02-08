#include "map.h"
#include "filebuf.h"

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

  return true;
error:
  file_buffer_delete(&file);
  return false;
}
