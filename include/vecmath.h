#ifndef VECMATH_H
#define VECMATH_H

#include <stdio.h>

struct vec2f {
  union {
    struct {
      float x, y;
    };
    float raw[2];
  };
};

struct vec3f {
  union {
    struct {
      float x, y, z;
    };
    float raw[3];
  };
};

struct vec4f {
  union {
    struct {
      float x, y, z, t;
    };
    float raw[4];
  };
};

struct mat4f {
  float elems[4][4];
};

float vec2f_dot(struct vec2f a, struct vec2f b);
struct vec2f vec2f_scale(struct vec2f a, float s);
struct vec2f vec2f_add(struct vec2f a, struct vec2f b);
struct vec2f vec2f_normalize(struct vec2f a);
struct vec3f vec2f_to_vec3f(struct vec2f a, float z);
struct vec4f vec2f_to_vec4f(struct vec3f a, float z, float t);

struct vec3f vec3f_new(float x, float y, float z);
float vec3f_dot(struct vec3f a, struct vec3f b);
struct vec3f vec3f_add(struct vec3f a, struct vec3f b);
struct vec3f vec3f_scale(struct vec3f a, float s);
struct vec3f vec3f_normalize(struct vec3f a);
struct vec4f vec3f_to_vec4f(struct vec3f a, float z);
struct vec3f vec3f_cross(struct vec3f a, struct vec3f b);

float vec4f_dot(struct vec4f a, struct vec4f b);
struct vec4f vec4f_add(struct vec4f a, struct vec4f b);
struct vec4f vec4f_scale(struct vec4f a, float s);
struct vec4f vec4f_normalize(struct vec4f a);
struct vec3f vec4f_to_vec3f(struct vec4f a);

static const struct mat4f mat4f_identity = {
  .elems = {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}
};

struct mat4f mat4f_multiply(struct mat4f a, struct mat4f b);
struct vec4f mat4f_dot(struct mat4f a, struct vec4f b);
struct mat4f mat4f_transformation(struct vec3f translation, struct vec3f rotation, float scale);
struct mat4f mat4f_get_rotation(struct mat4f a);
struct mat4f mat4f_rotation(struct vec3f rotation);
struct mat4f mat4f_projection(float aspect_ratio, float fov, float clip_near, float clip_far);

float deg(float rad);
float rad(float deg);

#endif
