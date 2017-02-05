#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include "buffer.h"
#include "vecmath.h"

struct shader {
  GLuint program;
};

struct shader_attribute {
  GLuint index;
  const char *name;
};

enum shader_uniform_type {
  SHADER_UNIFORM_INT,
  SHADER_UNIFORM_FLOAT,
  SHADER_UNIFORM_VEC2F,
  SHADER_UNIFORM_VEC3F,
  SHADER_UNIFORM_VEC4F,
  SHADER_UNIFORM_MAT4F,
  SHADER_UNIFORM_TRANSPOSE_MAT4F,
};

struct shader_uniform {
  enum shader_uniform_type type;
  union {
    struct vec2f vec2f;
    struct vec3f vec3f;
    struct vec4f vec4f;
    struct mat4f mat4f;
    float f;
    int32_t i;
  };
  const char *name;
};

static const size_t shader_max_attached_files = 32;

int shader_load_with_attributes(struct shader *shader, const char *vertex_shader_path, const char *fragment_shader_path, struct buffer *attributes);
int shader_load(struct shader *shader, const char *vertex_shader_path, const char *fragment_shader_path);
void shader_delete(struct shader *shader);
int shader_set_uniform(struct shader *shader, struct shader_uniform *uniform);
int shader_set_uniforms(struct shader *shader, struct buffer *uniforms);
int shader_use_no_uniforms(struct shader *shader);
int shader_use(struct shader *shader, struct buffer *uniforms);
int shader_unuse(struct shader *shader);

#endif
