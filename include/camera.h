#ifndef CAMERA_H
#define CAMERA_H

#include "vecmath.h"
#include "transformation.h"

struct camera {
  struct transformation transformation;
  float fov;
  float clip_near;
  float clip_far;
};

struct camera camera_new(float fov, float clip_near, float clip_far, float x, float y, float z, float rx, float ry, float rz);
void camera_move(struct camera *camera, float dx, float dy, float dz);
void camera_rotate(struct camera *camera, float drx, float dry, float drz);

#endif
