#ifndef mesh_H
#define mesh_H

#include <stdint.h>
#include <stdlib.h>
#include "list.h"
#include "vecmath.h"

struct bone {
  uint32_t id;
  struct mat4f local_transform;
  size_t num_children;
  struct bone *children;
};

struct animation {
  size_t num_frames;
  struct transformation *transformations;
};

struct mesh {
  struct vec3f *vertex_positions;
  struct vec3f *vertex_normals;
  struct vec2f *texture_coords;
  uint32_t *vertex_position_bones;
  uint32_t *vertex_normal_bones;
  uint32_t *vertex_ids;
  size_t num_vertex_defs;
  size_t num_vertices;
  size_t num_triangles;
};

struct animated_mesh {
  struct mesh mesh;
  struct bone *bones; // bones[0] is the root bone
  size_t num_bones;
  struct animation *animations;
  size_t num_animations;
};

int mesh_load_obj(struct mesh *mesh, const char *file_path);
void mesh_delete(struct mesh *mesh);
void mesh_rectangle(struct mesh *mesh, float heights_top_left, float height_bottom_left, float height_top_right, float height_bottom_right);

#endif
