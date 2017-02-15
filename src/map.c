#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "SDL2/SDL_image.h"
#include "map.h"
#include "light.h"
#include "filebuf.h"
#include "log.h"
#include "xor_decrypt.h"

bool map_tiles_load(struct map_tiles *map, const char *path) {
  struct file_buffer file;
  if (!file_buffer_read(&file, path))
    return false;

  xor_decrypt(&file, 0);

  return false;
}

bool map_attributes_load(struct map_attributes *map, const char *path) {
  struct file_buffer file;
  if (!file_buffer_read(&file, path))
    return false;

  bool result = true;
  switch (file.length) {
    case MAP_ATT_OLD_FILE_SIZE:
      xor_decrypt(&file, 0);
      xor3_decrypt(&file, 0);
      memcpy(map->attributes, file.data + 4, MAP_SIZE * MAP_SIZE * sizeof(uint8_t));
      break;

    case MAP_ATT_NEW_FILE_SIZE:
      xor_decrypt(&file, 0);
      xor3_decrypt(&file, 0);
      for (size_t y = 0; y < MAP_SIZE; y++) {
        for (size_t x = 0; x < MAP_SIZE; x++) {
          size_t index = y * MAP_SIZE + x;
          map->attributes[index] = file.data[4 + index * 3];
        }
      }
      break;

    case MAP_ATT_SERVER_FILE_SIZE:
      memcpy(map->attributes, file.data + 3, MAP_SIZE * MAP_SIZE * sizeof(uint8_t));
      break;

    default:
      log_error("Unknown map attribute file %s of size %zuB", path, file.length);
      result = false;
  }
  file_buffer_delete(&file);
  return result;
}

bool map_height_load(struct map_height *map, const char *path) {
  struct file_buffer file;
  if (!file_buffer_read(&file, path))
    return false;

  bool result_code = true;
  SDL_RWops *sdl_converted_file = NULL;

  FILE *converted_file = tmpfile();

  if (file.length < 4)
    goto err_close;

  if (fwrite(file.data + 4, sizeof(uint8_t), file.length - 4, converted_file) != file.length - 4)
    goto err_close;

  fflush(converted_file);
  rewind(converted_file);

  sdl_converted_file = SDL_RWFromFP(converted_file, SDL_FALSE);
  if (sdl_converted_file == NULL)
    goto err_close;

  SDL_Surface *surface = IMG_LoadTyped_RW(sdl_converted_file, 0, "BMP");

  memcpy(map->heights, surface->pixels, surface->h * surface->w);

  goto ret;
err_close:
  result_code = false;
ret:
  SDL_FreeSurface(surface);
  SDL_RWclose(sdl_converted_file);
  file_buffer_delete(&file);
  fclose(converted_file);
  return result_code;
}

bool map_object_load(struct map_objects *map_objects, const char *path) {
  struct file_buffer file;
  if (!file_buffer_read(&file, path))
    return false;

  xor_decrypt(&file, 0);

  uint16_t map_id; 
  if (!file_buffer_read_uint16(&file, &map_id)) {
    file_buffer_delete(&file);
    return false;
  }

  if (!file_buffer_read_uint16(&file, &map_objects->num_objects)) {
    file_buffer_delete(&file);
    return false;
  }

  if (map_objects->num_objects < 0) {
    file_buffer_delete(&file);
    return false;
  }

  if (file.length < 4 + 30 * map_objects->num_objects) {
    file_buffer_delete(&file);
    return false;
  }

  map_objects->objects = calloc(map_objects->num_objects, sizeof(struct map_object_def));
  for (size_t i = 0; i < map_objects->num_objects; i++) {
    struct map_object_def *object = &map_objects->objects[i];
    if (!file_buffer_read_int16(&file, &object->id) || object->id < 0) {
      free(map_objects->objects);
      file_buffer_delete(&file);
      return false;
    }

    struct vec3f swapped_position;
    if (!file_buffer_read_array(&file, &swapped_position, sizeof(float), 3)) {
      free(map_objects->objects);
      file_buffer_delete(&file);
      return false;
    }

    swapped_position = vec3f_scale(swapped_position, 0.01);
    object->transformation.position.x = swapped_position.x;
    object->transformation.position.y = swapped_position.z;
    object->transformation.position.z = -swapped_position.y;

    struct vec3f swapped_rotation;
    if (!file_buffer_read_array(&file, &swapped_rotation, sizeof(float), 3)) {
      free(map_objects->objects);
      file_buffer_delete(&file);
      return false;
    }
    swapped_rotation = vec3f_scale(swapped_rotation, pi() / 180.0);
    object->transformation.rotation.x = fmod(swapped_rotation.x, pi());
    object->transformation.rotation.y = fmod(swapped_rotation.z, pi());
    object->transformation.rotation.z = fmod(-swapped_rotation.y, pi());

    if (!file_buffer_read_float(&file, &object->transformation.scale)) {
      free(map_objects->objects);
      file_buffer_delete(&file);
      return false;
    }
    object->transformation.scale *= 0.01;
  }

  file_buffer_delete(&file);
  return true;
}

void map_objects_delete(struct map_objects *map_objects) {
  free(map_objects->objects);
}

bool map_object_entities_load(struct map_client *map, const char *entity_folder_path) {
  size_t max_entity_id = 0;
  for (size_t i = 0; i < map->objects.num_objects; i++) {
    if (map->objects.objects[i].id > max_entity_id)
      max_entity_id = map->objects.objects[i].id;
  }
  max_entity_id++;
  map->num_object_entities = max_entity_id;

  bool used_entity_ids[max_entity_id];
  for (size_t i = 0; i < max_entity_id; i++)
    used_entity_ids[i] = false;

  for (size_t i = 0; i < map->objects.num_objects; i++) {
    used_entity_ids[map->objects.objects[i].id] = true;
  }

  map->object_entities = calloc(max_entity_id, sizeof(struct bmd_entity));
  for (size_t i = 0; i < max_entity_id; i++) {
    if (used_entity_ids[i]) {
      char entity_path[MAX_MAP_PATH_LENGTH];
      sprintf(entity_path, "%s/Object%02zu.bmd", entity_folder_path, i + 1);

      struct bmd_entity *entity = &map->object_entities[i];
      if (!bmd_load(entity, entity_path, entity_folder_path)) {
        log_error("Unable to load entity %zu from %s", i + 1, entity_folder_path);
        free(map->object_entities);
        return false;
      }
    }
  }

  return true;
}

bool map_client_load(struct map_client *map, size_t id) {
  char attributes_path[MAX_MAP_PATH_LENGTH];
  sprintf(attributes_path, "./res/World%zu/EncTerrain%zu.att", id, id);

  char heights_path[MAX_MAP_PATH_LENGTH];
  sprintf(heights_path, "./res/World%zu/TerrainHeight.OZB", id);

  char objects_path[MAX_MAP_PATH_LENGTH];
  sprintf(objects_path, "./res/World%zu/EncTerrain%zu.obj", id, id);

  char object_entities_path[MAX_MAP_PATH_LENGTH];
  sprintf(object_entities_path, "./res/Object%zu", id);

  if (!map_height_load(&map->heights, heights_path)) {
    log_error("Unable to load height map %zu from %s", id, heights_path);
    return false;
  }
  log_info("Loaded height map %zu from %s", id, heights_path);

  if (!map_object_load(&map->objects, objects_path)) {
    log_error("Unable to load objects map %zu from %s", id, objects_path);
    return false;
  }
  log_info("Loaded %"PRIu16" objects on map %zu from %s", map->objects.num_objects, id, objects_path);

  if (!map_attributes_load(&map->attributes, attributes_path)) {
    map_objects_delete(&map->objects);
    log_error("Unable to load attributes map %zu from %s", id, attributes_path);
    return false;
  }
  log_info("Loaded attributes map %zu from %s", id, attributes_path);

  if (!map_object_entities_load(map, object_entities_path)) {
    map_objects_delete(&map->objects);
    return false;
  }
  log_info("Loaded %zu object entities for map %zu", map->num_object_entities, id);

  return true;
}

void map_client_delete(struct map_client *map) {
  for (size_t i = 0; i < map->num_object_entities; i++) {
    bmd_delete(&map->object_entities[i]);
  }
  free(map->object_entities);
  map_objects_delete(&map->objects);
}

bool map_client_render(struct map_client *map, struct shader *shader, struct camera *camera) {
  for (size_t i = 0; i < map->objects.num_objects; i++) {
    size_t bmd_id = map->objects.objects[i].id;
    if (!bmd_render(&map->object_entities[bmd_id], shader, camera, &map->objects.objects[i].transformation, NULL))
      return false;
  }
  return true;
}
