#ifndef MAP_H
#define MAP_H

#include "item.h"
#include "const.h"
#include "vecmath.h"

struct map_item {
  struct item item;
};

struct map {
  int32_t width;
  int32_t height;
  uint8_t attributes[MAP_MAX_WIDTH * MAP_MAX_HEIGHT];
};

struct map_object_def {
  uint16_t id;
  struct vec3f position;
  struct vec3f rotation;
  float scale;
};

bool map_load(struct map *map, const char *path);
bool map_object_defs_load(struct map_object_def *object_defs, const size_t max_num_objects, const char *path);

#endif
