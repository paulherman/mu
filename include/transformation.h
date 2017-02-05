#ifndef TRANSFORMATION_H
#define TRANSFORMATION_H

#include "vecmath.h"

struct transformation {
  struct vec3f position;
  struct vec3f rotation;
  float scale;
};

struct transformation transformation_new(float x, float y, float z, float rot_x, float rot_y, float rot_z, float scale);
void transformation_move(struct transformation *t, float dx, float dy, float dz);
void transformation_rotate(struct transformation *t, float drot_x, float drot_y, float drot_z);
void transformation_scale(struct transformation *t, float scale);

#endif
