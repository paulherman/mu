#ifndef MAP_H
#define MAP_H

#include "item.h"

#define MAP_MAX_WIDTH 256
#define MAP_MAX_HEIGHT 256

struct map_item {
  struct item item;
};

struct map {
  int32_t width;
  int32_t height;
  uint8_t attributes[MAP_MAX_WIDTH * MAP_MAX_HEIGHT];
};

bool map_load(struct map *map, const char *path);

#endif
