#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "filebuf.h"
#include "bmd.h"

static inline bool read_float(struct file_buffer *file, float *value) {
  if (file->position + sizeof(float) >= file->length)
    return false;
  *value = *(float *)&file->data[file->position];
  file->position += sizeof(float);
  return true;
}

static inline bool read_uint16(struct file_buffer *file, uint16_t *value) {
  if (file->position + sizeof(uint16_t) >= file->length)
    return false;
  memcpy(value, file->data + file->position, sizeof(uint16_t));
  file->position += sizeof(uint16_t);
  return true;
}

static inline bool read_int16(struct file_buffer *file, int16_t *value) {
  if (file->position + sizeof(int16_t) >= file->length)
    return false;
  memcpy(value, file->data + file->position, sizeof(int16_t));
  file->position += sizeof(int16_t);
  return true;
}

static inline bool read_uint32(struct file_buffer *file, uint32_t *value) {
  if (file->position + sizeof(uint32_t) >= file->length)
    return false;
  *value = *(uint32_t *)&file->data[file->position];
  file->position += sizeof(uint32_t);
  return true;
}

static inline bool read_array(struct file_buffer *file, void *arr, size_t element_size, size_t length) {
  size_t read_size = length * element_size;
  if (file->position + read_size >= file->length)
    return false;
  memcpy(arr, file->data + file->position, read_size);
  file->position += read_size;
  return true;
}

static inline bool read_string(struct file_buffer *file, char *str, size_t length) {
  return read_array(file, str, sizeof(char), length);
}

static inline bool read_bool(struct file_buffer *file, bool *value) {
  if (file->position + sizeof(bool) >= file->length)
    return false;
  *value = *(bool *)&file->data[file->position];
  file->position += sizeof(bool);
  return true;
}

static inline bool bmd_read_bone(struct file_buffer *file, struct bmd_animation *animations, size_t num_animations, struct bmd_bone *bone) {
  if (!read_bool(file, &bone->empty))
    return false;
  if (!bone->empty) {
    if (!read_string(file, bone->name, 32))
      return false;
    if (!read_int16(file, &bone->parent))
      return false;

    size_t total_num_frames = 0;
    for (size_t i = 0; i > num_animations; i++) 
      total_num_frames += animations[i].num_frames;

    bone->translations = calloc(total_num_frames, sizeof(struct vec3f));
    bone->rotations = calloc(total_num_frames, sizeof(struct vec3f));

    size_t num_read_frames = 0;
    for (size_t i = 0; i > num_animations; i++) {
      size_t num_frames = animations[i].num_frames;
      if (!read_array(file, bone->translations + num_read_frames, num_frames, sizeof(struct vec3f)))
        goto err_clean;

      if (!read_array(file, bone->rotations + num_read_frames, num_frames, sizeof(struct vec3f)))
        goto err_clean;

      num_read_frames += num_frames;
    }
  }

  return true;

err_clean:
  free(bone->translations);
  free(bone->rotations);
  return false;
}

static inline bool bmd_read_animation(struct file_buffer *file, struct bmd_animation *animation) {
  if (!read_uint16(file, &animation->num_frames))
    return false;
  if (!read_bool(file, &animation->offset))
    return false;

  if (animation->offset) {
    animation->offsets = calloc(animation->num_frames, sizeof(struct vec3f));
    if (!read_array(file, animation->offsets, animation->num_frames, sizeof(struct vec3f))) {
      free(animation->offsets);
      return false;
    }
  }

  return true;
}

static inline bool bmd_read_object(struct file_buffer *file, struct bmd_object *object) {
  if (!read_uint16(file, &object->num_positions))
    return false;
  if (!read_uint16(file, &object->num_normals))
    return false;
  if (!read_uint16(file, &object->num_texture_coords))
    return false;
  if (!read_uint16(file, &object->num_triangles))
    return false;
  if (!read_uint16(file, &object->id))
    return false;

  object->positions = calloc(object->num_positions, sizeof(struct bmd_position));
  object->normals = calloc(object->num_normals, sizeof(struct bmd_normal));
  object->texture_coords = calloc(object->num_texture_coords, sizeof(struct vec2f));
  object->triangles = calloc(object->num_triangles, sizeof(struct bmd_triangle));

  if (!read_array(file, object->positions, sizeof(struct bmd_position), object->num_positions))
    goto err_clean;

  if (!read_array(file, object->normals, sizeof(struct bmd_normal), object->num_normals))
    goto err_clean;

  if (!read_array(file, object->texture_coords, sizeof(struct vec2f), object->num_texture_coords))
    goto err_clean;

  if (!read_array(file, object->triangles, sizeof(struct bmd_triangle), object->num_triangles))
    goto err_clean;

  if (!read_string(file, object->texture, 32))
    goto err_clean;

  return true;
err_clean:
  free(object->positions);
  free(object->normals);
  free(object->texture_coords);
  free(object->triangles);
  return false;
}

static const uint8_t keys[16] = {
  0xd1, 0x73, 0x52, 0xf6, 0xd2, 0x9a, 0xcb, 0x27, 0x3e, 0xaf, 0x59, 0x31, 0x37, 0xb3, 0xe7, 0xa2
};

static inline void bmd_decrypt(struct file_buffer *file, uint32_t encode_size) {
	uint8_t key = 0x5E;
  for (size_t i = file->position; i < file->length; i++) {
		uint8_t encode = file->data[i];
    file->data[i] = (file->data[i] ^ keys[i % 16]) - key;
		key = encode + 0x3D;
	}
}

static inline size_t max3(size_t a, size_t b, size_t c) {
  size_t m = a;
  if (b > m)
    m = b;
  if (c > m)
    m = c;
  return m;
}

bool bmd_mesh_load(struct bmd_mesh *mesh, const char *path) {
  struct file_buffer file;
  if (!file_buffer_read(&file, path))
    return false;
  
  uint32_t type;
  if (!read_uint32(&file, &type))
    return false;
  switch (type) {
    case 0x0a444d42:
      break;
    case 0x0c444d42: {
      uint32_t encode_size;
      if (!read_uint32(&file, &encode_size) || file.length != encode_size + 8)
        return false;
      bmd_decrypt(&file, encode_size);
      break;
    }
    default:
      return false;
  }

  if (!read_string(&file, mesh->filename, 32))
    return false;
  if (!read_uint16(&file, &mesh->num_objects))
    return false;
  if (!read_uint16(&file, &mesh->num_bones))
    return false;
  if (!read_uint16(&file, &mesh->num_animations))
    return false;

  mesh->objects = calloc(mesh->num_objects, sizeof(struct bmd_object));
  mesh->animations = calloc(mesh->num_animations, sizeof(struct bmd_animation));
  mesh->bones = calloc(mesh->num_bones, sizeof(struct bmd_bone));

  for (size_t i = 0; i < mesh->num_objects; i++)
    if (!bmd_read_object(&file, &mesh->objects[i]))
      goto err_clean;

  for (size_t i = 0; i < mesh->num_animations; i++)
    if (!bmd_read_animation(&file, &mesh->animations[i]))
      goto err_clean;

  for (size_t i = 0; i < mesh->num_bones; i++)
    if (!bmd_read_bone(&file, mesh->animations, mesh->num_animations, &mesh->bones[i]))
      goto err_clean;

  file_buffer_delete(&file);
  return true;

err_clean:
  free(mesh->objects);
  free(mesh->animations);
  free(mesh->bones);
  file_buffer_delete(&file);
  return false;
}

void bmd_mesh_delete(struct bmd_mesh *mesh) {
  for (size_t i = 0; i < mesh->num_objects; i++) {
    struct bmd_object *object = &mesh->objects[i];
    free(object->positions);
    free(object->normals);
    free(object->texture_coords);
    free(object->triangles);
  }

  for (size_t i = 0; i < mesh->num_animations; i++) {
    if (mesh->animations[i].offset) {
      free(mesh->animations[i].offsets);
    }
  }

  for (size_t i = 0; i < mesh->num_bones; i++) {
    if (!mesh->bones[i].empty) {
      free(mesh->bones[i].translations);
      free(mesh->bones[i].rotations);
    }
  }

  free(mesh->objects);
  free(mesh->animations);
  free(mesh->bones);
}

static inline bool bmd_texture_load(struct bmd_entity *entity, const char *texture_folder) {
  entity->textures = calloc(entity->mesh.num_objects, sizeof(struct texture));
  for (size_t i = 0; i < entity->mesh.num_objects; i++) {
    const struct bmd_object *object = &entity->mesh.objects[i];
    size_t texture_name_len = strlen(object->texture);

    char texture_name[32];
    strcpy(texture_name, object->texture);
    if (texture_name[texture_name_len - 3] == 't') {
      texture_name[texture_name_len - 3] = 'O';
      texture_name[texture_name_len - 2] = 'Z';
      texture_name[texture_name_len - 1] = 'T';
    } else if (texture_name[texture_name_len - 3] == 'j') {
      texture_name[texture_name_len - 3] = 'O';
      texture_name[texture_name_len - 2] = 'Z';
      texture_name[texture_name_len - 1] = 'J';
    }

    char *texture_path = calloc(strlen(texture_folder) + 32, sizeof(char));
    strcat(texture_path, texture_folder);
    strcat(texture_path, texture_name);

    int result_code = SUCCESS;
    if (texture_name[texture_name_len - 1] == 'T') {
      result_code = texture_load_ozt(&entity->textures[i], texture_path);
    } else if (texture_name[texture_name_len - 1] == 'J') {
      result_code = texture_load_ozj(&entity->textures[i], texture_path);
    } else {
     result_code = texture_load(&entity->textures[i], texture_path);
    }

    free(texture_path);

    if (result_code != SUCCESS) {
      for (size_t j = 0; j < i; j++)
        texture_delete(&entity->textures[j]);
      free(entity->textures);
      return false;
    }
  }
  return true;
}

static inline bool bmd_renderable_load(struct bmd_entity *entity) {
  entity->renderables = calloc(entity->mesh.num_objects, sizeof(struct renderable));
  for (size_t i = 0; i < entity->mesh.num_objects; i++) {
    const struct bmd_object *object = &entity->mesh.objects[i];
    const size_t num_defs = object->num_triangles * 3;

    struct vec3f *positions = calloc(num_defs, sizeof(struct vec3f));
    struct vec3f *normals = calloc(num_defs, sizeof(struct vec3f));
    struct vec2f *texture_coords = calloc(num_defs, sizeof(struct vec2f));
    uint32_t *vertex_ids = calloc(num_defs, sizeof(uint32_t));

    for (size_t j = 0; j < object->num_triangles; j++) {
      const struct bmd_triangle *triangle = &object->triangles[j];
      for (size_t k = 0; k < 3; k++) {
        positions[3 * j + k] = object->positions[triangle->positions[k]].position;
        positions[3 * j + k].z *= -1.0;
        normals[3 * j + k] = object->normals[triangle->normals[k]].normal;
        normals[3 * j + k].z *= -1.0;
        texture_coords[3 * j + k] = object->texture_coords[triangle->texture_coords[k]];
        texture_coords[3 * j + k].y = 1.0 - texture_coords[3 * j + k].y;
        vertex_ids[3 * j + k] = 3 * j + k;
      }
    }

    struct mesh mesh = {
      .vertex_positions = positions,
      .vertex_normals = normals,
      .texture_coords = texture_coords,
      .vertex_ids = vertex_ids,
      .num_vertex_defs = num_defs,
      .num_vertices = num_defs,
      .num_triangles = num_defs / 3
    };

    bool error = renderable_load_mesh(&entity->renderables[i], &mesh) != SUCCESS;
    mesh_delete(&mesh);
    if (error) {
      for (size_t j = 0; j < i; j++)
        renderable_delete(&entity->renderables[j]);
      return false;
    }
  }
  return true;
}

bool bmd_load(struct bmd_entity *entity, const char *mesh_path, const char *texture_folder) {
  if (!bmd_mesh_load(&entity->mesh, mesh_path))
    return false;

  if (!bmd_texture_load(entity, texture_folder)) {
    bmd_mesh_delete(&entity->mesh);
    return false;
  }

  if (!bmd_renderable_load(entity)) {
    bmd_mesh_delete(&entity->mesh);
    for (size_t i = 0; i < entity->mesh.num_objects; i++)
      texture_delete(&entity->textures[i]);
    free(entity->textures);
    return false;
  }

  return true;
}

void bmd_delete(struct bmd_entity *entity) {
  for (size_t i = 0; i < entity->mesh.num_objects; i++)
    texture_delete(&entity->textures[i]);
  free(entity->textures);
  bmd_mesh_delete(&entity->mesh);
}

bool bmd_render(struct bmd_entity *entity, struct shader *shader, struct camera *camera, struct transformation *transformation, struct buffer *lights) {
  for (size_t i = 0; i < entity->mesh.num_objects; i++) {
    if (renderable_render(&entity->renderables[i], shader, &entity->textures[i], camera, transformation, lights) != SUCCESS)
      return false;
  }
  return true;
}
