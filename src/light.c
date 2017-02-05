#include "light.h"

struct light light_new(float x, float y, float z, float r, float g, float b) {
  struct light light = {
    .position = {
      .x = x,
      .y = y,
      .z = z
    },
    .color = {
      .x = r,
      .y = g,
      .z = b
    }
  };
  return light;
}

void light_move(struct light *light, float dx, float dy, float dz) {
  light->position.x += dx;
  light->position.y += dy;
  light->position.z += dz;
}

void light_color(struct light *light, float r, float g, float b) {
  light->color.x = r;
  light->color.y = g;
  light->color.z = b;
}
