#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <stdlib.h>
#include "error.h"
#include "mesh.h"
#include "buffer.h"
#include "vecmath.h"

int mesh_load_obj(struct mesh *mesh, const char *file_path) {
  int result_code = SUCCESS;
  FILE *obj_file = fopen(file_path, "r");
  if (obj_file == NULL)
    goto file_error;

  struct buffer positions = buffer_new(0, sizeof(struct vec3f));
  struct buffer normals = buffer_new(0, sizeof(struct vec3f));
  struct buffer texture_coords = buffer_new(0, sizeof(struct vec2f));

  for (;;) {
    char type[3];
    if (fscanf(obj_file, " %2s ", type) <= 0) {
      break;
    }

    if (strcmp(type, "v") == 0) {
      struct vec3f position;
      if (fscanf(obj_file, "%f %f %f", &position.x, &position.y, &position.z) <= 0) {
        goto read_error;
      }
      *(struct vec3f *)buffer_push(&positions) = position;
    } else if (strcmp(type, "vt") == 0) {
      struct vec2f texture_coord;
      if (fscanf(obj_file, "%f %f", &texture_coord.x, &texture_coord.y) <= 0) {
        goto read_error;
      }
      texture_coord.y = 1.0 - texture_coord.y;
      *(struct vec2f *)buffer_push(&texture_coords) = texture_coord;
    } else if (strcmp(type, "vn") == 0) {
      struct vec3f normal;
      if (fscanf(obj_file, "%f %f %f", &normal.x, &normal.y, &normal.z) <= 0) {
        goto read_error;
      }
      *(struct vec3f *)buffer_push(&normals) = normal;
    } else if (strcmp(type, "f") == 0) {
      break;
    } else {
      fscanf(obj_file, "%*[^\n]"); 
    }
  }

  size_t num_vertex_defs = positions.size;
  mesh->num_vertex_defs = num_vertex_defs;
  mesh->vertex_positions = calloc(num_vertex_defs, sizeof(struct vec3f));
  mesh->vertex_normals = calloc(num_vertex_defs, sizeof(struct vec3f));
  mesh->texture_coords = calloc(num_vertex_defs, sizeof(struct vec2f));
  mesh->vertex_position_bones = calloc(num_vertex_defs, sizeof(uint32_t));
  mesh->vertex_normal_bones = calloc(num_vertex_defs, sizeof(uint32_t));
  
  struct buffer vertex_ids = buffer_new(0, sizeof(uint32_t));
  for (;;) {
    char type[3];
    if (fscanf(obj_file, " %2s ", type) <= 0) {
      break;
    }
    if (strcmp(type, "f") == 0) {
      for (size_t i = 0; i < 3; i++) {
        uint32_t position_id, normal_id, texture_coord_id;
        if (fscanf(obj_file, "%" SCNd32 "/%" SCNd32 "/%" SCNd32, &position_id, &texture_coord_id, &normal_id) <= 0)
          goto read_error;

        position_id--;
        mesh->vertex_positions[position_id] = *(struct vec3f *)buffer_get(&positions, position_id);

        texture_coord_id--;
        mesh->texture_coords[position_id] = *(struct vec2f *)buffer_get(&texture_coords, texture_coord_id);

        normal_id--;
        mesh->vertex_normals[position_id] = *(struct vec3f *)buffer_get(&normals, normal_id);
        *(uint32_t *)buffer_push(&vertex_ids) = position_id;
      }
    } else {
      fscanf(obj_file, "%*[^\n]"); 
    }
  }

  mesh->num_vertices = vertex_ids.size;
  mesh->num_triangles = vertex_ids.size / 3;
  mesh->vertex_ids = calloc(vertex_ids.size, sizeof(int64_t));
  for (size_t i = 0; i < vertex_ids.size; i++) {
    mesh->vertex_ids[i] = *(int64_t *)buffer_get(&vertex_ids, i);
  }

  goto success;
file_error:
  result_code = MESH_LOAD_FILE_NOT_FOUND;
  goto ret;
read_error:
  free(mesh->vertex_positions);
  free(mesh->vertex_normals);
  free(mesh->texture_coords);
  result_code = MESH_LOAD_FILE_READ;
success:
  buffer_delete(&positions);
  buffer_delete(&normals);
  buffer_delete(&texture_coords);
  buffer_delete(&vertex_ids);
  fclose(obj_file);
ret:
  return result_code;
}

void mesh_rectangle(struct mesh *mesh, float height_top_left, float height_bottom_left, float height_top_right, float height_bottom_right) {
  mesh->num_vertex_defs = 4;
  mesh->num_vertices = 6;
  mesh->num_triangles = 2;

  mesh->vertex_positions = calloc(4, sizeof(struct vec3f));
  mesh->vertex_normals = calloc(4, sizeof(struct vec3f));
  mesh->texture_coords = calloc(4, sizeof(struct vec2f));
  mesh->vertex_ids = calloc(6, sizeof(uint32_t));

  // top left
  mesh->vertex_positions[0].x = -1.0;
  mesh->vertex_positions[0].y = 1.0;
  mesh->vertex_positions[0].z = height_top_left;

  // bottom left
  mesh->vertex_positions[1].x = -1.0;
  mesh->vertex_positions[1].y = -1.0;
  mesh->vertex_positions[1].z = height_bottom_left;

  // top right
  mesh->vertex_positions[2].x = 1.0;
  mesh->vertex_positions[2].y = -1.0;
  mesh->vertex_positions[2].z = height_top_right;

  // bottom right
  mesh->vertex_positions[3].x = 1.0;
  mesh->vertex_positions[3].y = 1.0;
  mesh->vertex_positions[3].z = height_bottom_right;

  mesh->vertex_normals[0] = vec3f_cross(mesh->vertex_positions[2], mesh->vertex_positions[1]);
  mesh->vertex_normals[1] = vec3f_cross(mesh->vertex_positions[0], mesh->vertex_positions[3]);
  mesh->vertex_normals[2] = vec3f_cross(mesh->vertex_positions[0], mesh->vertex_positions[3]);
  mesh->vertex_normals[3] = vec3f_cross(mesh->vertex_positions[1], mesh->vertex_positions[2]);

  mesh->texture_coords[3].x = 1.0;
  mesh->texture_coords[3].y = 0.0;
  mesh->texture_coords[2].x = 1.0;
  mesh->texture_coords[2].y = 1.0;
  mesh->texture_coords[0].x = 0.0;
  mesh->texture_coords[0].y = 0.0;
  mesh->texture_coords[1].x = 0.0;
  mesh->texture_coords[1].y = 1.0;

  mesh->vertex_ids[0] = 0;
  mesh->vertex_ids[1] = 1;
  mesh->vertex_ids[2] = 3;
  mesh->vertex_ids[3] = 3;
  mesh->vertex_ids[4] = 1;
  mesh->vertex_ids[5] = 2;
}

void mesh_delete(struct mesh *mesh) {
  free(mesh->vertex_positions);
  free(mesh->vertex_normals);
  free(mesh->texture_coords);
  free(mesh->vertex_ids);
  free(mesh->vertex_position_bones);
  free(mesh->vertex_normal_bones);
}
