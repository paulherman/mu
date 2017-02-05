#ifndef mesh_H
#define mesh_H

#include <stdint.h>
#include <stdlib.h>

struct mesh {
  struct vec3f *vertex_positions;
  struct vec3f *vertex_normals;
  struct vec2f *texture_coords;
  uint32_t *vertex_ids;
  size_t num_vertex_defs;
  size_t num_vertices;
  size_t num_triangles;
};

int mesh_load_obj(struct mesh *mesh, const char *file_path);
void mesh_delete(struct mesh *mesh);
void mesh_rectangle(struct mesh *mesh);

#endif
