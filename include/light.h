#ifndef LIGHT_H
#define LIGHT_H

#include "vecmath.h"

struct light {
  struct vec3f position;
  struct vec3f color;
};

struct light light_new(float x, float y, float z, float r, float g, float b);
void light_move(struct light *light, float dx, float dy, float dz);
void light_color(struct light *light, float r, float g, float b);

#endif
