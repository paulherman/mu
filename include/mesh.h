#ifndef mesh_H
#define mesh_H

#include <stdint.h>
#include <stdlib.h>
#include "list.h"
#include "vecmath.h"

struct bone {
  uint32_t id;
  struct mat4f local_transform;
  struct list_node siblings;
  struct bone *children;
};

struct vertex_bone {
  uint32_t id;
  float weight;
};

struct mesh {
  struct vec3f *vertex_positions;
  struct vec3f *vertex_normals;
  struct vec2f *texture_coords;
  struct buffer *vertex_bones;
  struct bone *root_bone;
  uint32_t *vertex_ids;
  size_t num_vertex_defs;
  size_t num_vertices;
  size_t num_triangles;
  size_t num_bones;
};

int mesh_load_obj(struct mesh *mesh, const char *file_path);
void mesh_delete(struct mesh *mesh);
void mesh_rectangle(struct mesh *mesh);

#endif
