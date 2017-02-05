#include <SDL2/SDL_image.h>
#include <SDL2/SDL_opengl.h>
#include "texture.h"
#include "error.h"
#include "filebuf.h"

static inline int texture_surface(struct texture *texture, SDL_Surface *surface) {
  int result_code = SUCCESS;
  if (surface == NULL)
    goto err;

  int mode = 0;
  if (surface->format->BytesPerPixel == 3) {
    mode = GL_RGB;
  } else if (surface->format->BytesPerPixel == 4) {
    mode = GL_RGBA;
  } else {
    goto err;
  }

  glGenTextures(1, &texture->id);
  if (glGetError() != GL_NO_ERROR)
    goto err;

  glBindTexture(GL_TEXTURE_2D, texture->id);
  if (glGetError() != GL_NO_ERROR)
    goto err;

  glTexImage2D(GL_TEXTURE_2D, 0, mode, surface->w, surface->h, 0, mode, GL_UNSIGNED_BYTE, surface->pixels);
  if (glGetError() != GL_NO_ERROR)
    goto err;

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  if (glGetError() != GL_NO_ERROR)
    goto err;

  goto ret;
err:
  result_code = TEXTURE_LOAD_ERROR;
ret:
  SDL_FreeSurface(surface);
  return result_code;
}

int texture_load(struct texture *texture, const char *path) {
  SDL_Surface *surface = IMG_Load(path);
  return texture_surface(texture, surface);
}

static int texture_load_skip_bytes(struct texture *texture, const char *path, const size_t skip_bytes, const char *type) {
  int result_code = SUCCESS;
  struct file_buffer file;
  FILE *converted_file = tmpfile();

  if (!file_buffer_read(&file, path))
    goto err_close;

  if (file.length < skip_bytes)
    goto err_close;

  if (fwrite(file.data + skip_bytes, sizeof(uint8_t), file.length - skip_bytes, converted_file) != file.length - skip_bytes)
    goto err_close;

  fflush(converted_file);
  rewind(converted_file);

  SDL_RWops *sdl_converted_file = SDL_RWFromFP(converted_file, SDL_FALSE);
  if (sdl_converted_file == NULL)
    goto err_close;

  SDL_Surface *surface = IMG_LoadTyped_RW(sdl_converted_file, 0, type);
  result_code = texture_surface(texture, surface);

  goto ret;
err_close:
  result_code = TEXTURE_LOAD_ERROR;
ret:
  SDL_RWclose(sdl_converted_file);
  file_buffer_delete(&file);
  fclose(converted_file);
  return result_code;
}

int texture_load_ozj(struct texture *texture, const char *path) {
  return texture_load_skip_bytes(texture, path, 24, "JPG");
}

int texture_load_ozt(struct texture *texture, const char *path) {
  return texture_load_skip_bytes(texture, path, 4, "TGA");
}

void texture_delete(struct texture *texture) {
  glDeleteTextures(1, &texture->id);
}
