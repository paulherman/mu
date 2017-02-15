#ifndef MAP_H
#define MAP_H

#include "item.h"
#include "const.h"
#include "vecmath.h"

struct map_item {
  struct item item;
};

struct map_attributes {
  uint8_t attributes[MAP_SIZE * MAP_SIZE];
};

struct map_object_def {
  int16_t id;
  struct vec3f position;
  struct vec3f rotation;
  float scale;
};

struct map_objects {
  uint16_t map_id;
  uint16_t num_objects;
  struct map_object_def *objects;
};

struct map_height {
  uint8_t heights[MAP_SIZE * MAP_SIZE];
};

struct map_tiles {
  uint8_t tiles[MAP_SIZE * MAP_SIZE];
};

struct map_client {
  size_t id;
  char name[32];
  struct map_attributes attributes;
  struct map_objects objects;
  struct map_height heights;
};

bool map_height_load(struct map_height *map, const char *path);
bool map_attributes_load(struct map_attributes *map, const char *path);
bool map_objects_load(struct map_objects *map, const char *path);
bool map_tiles_load(struct map_tiles *map, const char *path);
void map_objects_delete(struct map_objects *map_objects);
bool map_client_load(struct map_client *map, size_t id);
void map_client_delete(struct map_client *map);

#endif
