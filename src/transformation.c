#include "transformation.h"

struct transformation transformation_new(float x, float y, float z, float rot_x, float rot_y, float rot_z, float scale) {
  struct transformation t = {
    .position = {
      .x = x,
      .y = y,
      .z = z
    },
    .rotation = {
      .x = rot_x,
      .y = rot_y,
      .z = rot_z 
    },
    .scale = scale
  };
  return t;
}

void transformation_move(struct transformation *t, float dx, float dy, float dz) {
  t->position.x += dx;
  t->position.y += dy;
  t->position.z += dz;
}

void transformation_rotate(struct transformation *t, float drot_x, float drot_y, float drot_z) {
  t->rotation.x += drot_x;
  t->rotation.y += drot_y;
  t->rotation.z += drot_z;
}

void transformation_scale(struct transformation *t, float scale) {
  t->scale *= scale;
}
