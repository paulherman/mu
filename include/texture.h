#ifndef TEXTURE_H
#define TEXTURE_H

#include <SDL2/SDL.h>

struct texture {
  GLuint id;
};

int texture_load(struct texture *texture, const char *path);
int texture_load_ozj(struct texture *texture, const char *path);
int texture_load_ozt(struct texture *texture, const char *path);
void texture_delete(struct texture *texture);

#endif
