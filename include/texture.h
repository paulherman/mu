#ifndef TEXTURE_H
#define TEXTURE_H

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

struct texture {
  GLuint id;
};

int texture_load(struct texture *texture, const char *path);
int texture_load_ozj(struct texture *texture, const char *path);
int texture_load_ozt(struct texture *texture, const char *path);
void texture_delete(struct texture *texture);

#endif
