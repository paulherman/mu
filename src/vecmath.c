#include <math.h>
#include <stdlib.h>
#include "vecmath.h"

float vec2f_dot(struct vec2f a, struct vec2f b) {
  return a.x * b.x + a.y * b.y;
}

struct vec2f vec2f_scale(struct vec2f a, float s) {
  struct vec2f res = {
    .x = a.x * s,
    .y = a.y * s
  };
  return res;
}

struct vec2f vec2f_add(struct vec2f a, struct vec2f b) {
  struct vec2f res = {
    .x = a.x + b.x,
    .y = a.y + b.y
  };
  return res;
}

struct vec2f vec2f_normalize(struct vec2f a) {
  return vec2f_scale(a, sqrt(vec2f_dot(a, a)));
}

struct vec3f vec2f_to_vec3f(struct vec2f a, float z) {
  struct vec3f res = {
    .x = a.x,
    .y = a.y,
    .z = z
  };
  return res;
}

struct vec4f vec2f_to_vec4f(struct vec3f a, float z, float t) {
  struct vec4f res = {
    .x = a.x,
    .y = a.y,
    .z = z,
    .t = t
  };
  return res;
}

float vec3f_dot(struct vec3f a, struct vec3f b) {
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

struct vec3f vec3f_add(struct vec3f a, struct vec3f b) {
  struct vec3f res = {
    .x = a.x + b.x,
    .y = a.y + b.y,
    .z = a.z + b.z
  };
  return res;
}

struct vec3f vec3f_scale(struct vec3f a, float s) {
  struct vec3f res = {
    .x = a.x * s,
    .y = a.y * s,
    .z = a.z * s
  };
  return res;
}

struct vec3f vec3f_normalize(struct vec3f a) {
  return vec3f_scale(a, sqrt(vec3f_dot(a, a)));
}

struct vec4f vec3f_to_vec4f(struct vec3f a, float t) {
  struct vec4f res = {
    .x = a.x,
    .y = a.y,
    .z = a.z,
    .t = t
  };
  return res;
}

float vec4f_dot(struct vec4f a, struct vec4f b) {
  return a.x * b.x + a.y * b.y + a.z * b.z + a.t * b.t;
}

struct vec4f vec4f_add(struct vec4f a, struct vec4f b) {
  struct vec4f res = {
    .x = a.x + b.x,
    .y = a.y + b.y,
    .z = a.z + b.z,
    .t = a.t + b.t
  };
  return res;
}
struct vec4f vec4f_scale(struct vec4f a, float s) {
  struct vec4f res = {
    .x = a.x * s,
    .y = a.y * s,
    .z = a.z * s,
    .t = a.t * s
  };
  return res;
}

struct vec4f vec4f_normalize(struct vec4f a) {
  return vec4f_scale(a, sqrt(vec4f_dot(a, a)));
}

struct vec3f vec4f_to_vec3f(struct vec4f a) {
  struct vec3f res = {
    .x = a.x,
    .y = a.y,
    .z = a.z
  };
  return res;
}

struct mat4f mat4f_multiply(struct mat4f a, struct mat4f b) {
  struct mat4f res = { 0 };
  for (size_t row = 0; row < 4; row++) {
    for (size_t column = 0; column < 4; column++) {
      res.elems[row][column] = 0.0f;
      for (size_t k = 0; k < 4; k++) {
        res.elems[row][column] += a.elems[row][k] * b.elems[k][column]; 
      }
    }
  }
  return res;
}

struct vec4f mat4f_dot(struct mat4f a, struct vec4f b) {
  struct vec4f res = { 0 };
  for (size_t row = 0; row < 4; row++) {
    res.raw[row] = 0.0f;
    for (size_t i = 0; i < 4; i++) {
      res.raw[row] += a.elems[row][i] * b.raw[i];
    }
  }
  return res;
}

struct mat4f mat4f_transformation(struct vec3f translation, struct vec3f rotation, float scale) {
  const struct mat4f translation_matrix = {
    .elems = {
      { 1.0, 0.0, 0.0, translation.x },
      { 0.0, 1.0, 0.0, translation.y },
      { 0.0, 0.0, 1.0, translation.z },
      { 0.0, 0.0, 0.0, 1.0 }
    }
  };

  const struct mat4f scaling_matrix = {
    .elems = {
      { scale, 0.0, 0.0, 0.0 },
      { 0.0, scale, 0.0, 0.0 },
      { 0.0, 0.0, scale, 0.0 },
      { 0.0, 0.0, 0.0, 1.0 }
    }
  };

  float rx = rotation.x;
  const struct mat4f rot_x_matrix = {
    .elems = {
      { 1.0, 0.0, 0.0, 0.0 },
      { 0.0, cos(rx), -sin(rx), 0.0 },
      { 0.0, sin(rx), cos(rx), 0.0 },
      { 0.0, 0.0, 0.0, 1.0 }
    }
  };

  float ry = rotation.y;
  const struct mat4f rot_y_matrix = {
    .elems  = {
      { cos(ry), 0.0, sin(ry), 0.0 },
      { 0.0, 1.0, 0.0, 0.0 },
      { -sin(ry), 0.0, cos(ry), 0.0 },
      { 0.0, 0.0, 0.0, 1.0 }
    }
  };

  float rz = rotation.z;
  const struct mat4f rot_z_matrix = {
    .elems = {
      { cos(rz), -sin(rz), 0.0, 0.0 },
      { sin(rz), cos(rz), 0.0, 0.0 },
      { 0.0, 0.0, 1.0, 0.0 },
      { 0.0, 0.0, 0.0, 1.0 }
    }
  };
  
  const struct mat4f rotation_matrix = mat4f_multiply(rot_x_matrix, mat4f_multiply(rot_y_matrix, rot_z_matrix));

  struct mat4f result = mat4f_multiply(translation_matrix, mat4f_multiply(rotation_matrix, scaling_matrix));
  return result;
}

struct mat4f mat4f_projection(float aspect_ratio, float fov, float clip_near, float clip_far) {
  const float y_scale = aspect_ratio / tanf(fov / 2.0);
  const float x_scale = y_scale / aspect_ratio;
  const float length = clip_far - clip_near;

  struct mat4f projection = {
    .elems = {
      { x_scale, 0.0, 0.0, 0.0  },
      { 0.0, y_scale, 0.0, 0.0 },
      { 0.0, 0.0, -(clip_near + clip_far) / length, -1 },
      { 0.0, 0.0, -2.0 * clip_near * clip_far / length, 0.0 }
    }
  };
  return projection;
}

void mat4f_dump(FILE *f, struct mat4f matrix) {
  for (size_t i = 0; i < 4; i++) {
    for (size_t j = 0; j < 4; j++)
      printf("%f ", matrix.elems[i][j]);
    printf("\n");
  }
}

static const float pi = acos(-1.0);

float deg(float rad) {
  return rad / pi * 180.0;
}

float rad(float deg) {
  return deg / 180.0 * pi;
}
