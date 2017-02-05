#include <stdlib.h>
#include <string.h>
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

bool bmd_load(struct bmd_mesh *mesh, const char *path) {
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

void bmd_delete(struct bmd_mesh *mesh) {
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

