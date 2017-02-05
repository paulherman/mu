#ifndef BMD_H
#define BMD_H

#include <stdbool.h>
#include <stdint.h>
#include "error.h"
#include "filebuf.h"
#include "vecmath.h"

struct bmd_animation {
  uint16_t num_frames;
  bool offset;
  struct vec3f *offsets;
};

struct bmd_bone {
  bool empty;
  char name[32];
  int16_t parent;
  struct vec3f *translations;
  struct vec3f *rotations;
};

struct bmd_position {
  uint32_t bone;
  struct vec3f position;
};

struct bmd_normal {
  uint32_t bone;
  struct vec3f normal;
  uint32_t unk0;
};

struct bmd_triangle {
  uint16_t unk0;
  uint16_t positions[3];
  uint16_t unk1;
  uint16_t normals[3];
  uint16_t unk2;
  uint16_t texture_coords[3];
  uint16_t unk3[20];
};

struct bmd_object {
  uint16_t num_positions;
  uint16_t num_normals;
  uint16_t num_texture_coords;
  uint16_t num_triangles;
  uint16_t id;
  char texture[32];
  struct bmd_position *positions;
  struct bmd_normal *normals;
  struct vec2f *texture_coords;
  struct bmd_triangle *triangles;
};

struct bmd_mesh {
  char filename[32];
  uint16_t num_objects;
  uint16_t num_bones;
  uint16_t num_animations;
  struct bmd_object *objects;
  struct bmd_animation *animations;
  struct bmd_bone *bones;
};

bool bmd_load(struct bmd_mesh *mesh, const char *path);
void bmd_delete(struct bmd_mesh *mesh);

#endif
