#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "shader.h"
#include "renderable.h"
#include "error.h"

static int shader_load_file(const char *path, GLenum type, GLuint *shader) {
  *shader = 0;
  int result_code = SUCCESS;
  FILE *file = fopen(path, "r");
  if (file == NULL)
    return SHADER_FILE_ERROR;

  const size_t batch_size = 4096;
  size_t num_batches = 0;
  char *buffer = calloc(batch_size + 1, sizeof(char));
  char *contents = NULL;

  while (fread(buffer, 1, batch_size, file) != 0) {
    num_batches++;
    contents = realloc(contents, (batch_size * num_batches + 1) * sizeof(char));
    strcpy(contents + batch_size * (num_batches - 1), buffer);
  }
  if (feof(file) == 0)
    goto file_error;

  *shader = glCreateShader(type);
  if (glGetError() != GL_NO_ERROR)
    goto shader_error;

  glShaderSource(*shader, 1, (const char **)&contents, NULL);
  if (glGetError() != GL_NO_ERROR)
    goto shader_error;

  glCompileShader(*shader);
  GLint compile_error = GL_FALSE;
  glGetShaderiv(*shader, GL_COMPILE_STATUS, &compile_error);
  if (compile_error == GL_FALSE)
    goto shader_error;

  goto cleanup;
file_error:
  result_code = SHADER_FILE_ERROR;
  goto cleanup;
shader_error:
  result_code = SHADER_CREATE_ERROR;
  glDeleteShader(*shader);
  *shader = 0;
  goto cleanup;
cleanup:
  fclose(file);
  free(buffer);
  free(contents);
  return result_code;
}

static int shader_attach(const char *shader_path, GLenum type, GLuint program) {
  int result_code = SUCCESS;
  if (shader_path != NULL) {
    GLuint shader = 0;
    result_code = shader_load_file(shader_path, type, &shader);
    if (result_code != SUCCESS)
      goto ret;

    glAttachShader(program, shader);
    if (glGetError() != GL_NO_ERROR) {
      result_code = SHADER_LINK_ERROR;
      goto ret;
    }
  }
ret:
  return result_code;
}

static int shader_bind_attribute(struct shader *shader, const char *name, int attribute_index) {
  glBindAttribLocation(shader->program, attribute_index, name);
  if (glGetError() != GL_NO_ERROR)
    return SHADER_ATTRIBUTE_ERROR;
  return SUCCESS;
}

int shader_load_with_attributes(struct shader *shader, const char *vertex_shader_path, const char *fragment_shader_path, struct buffer *attributes) {
  int result_code = SUCCESS;

  shader->program = 0;

  shader->program = glCreateProgram();
  if (glGetError() != GL_NO_ERROR)
    goto program_error;
  
  result_code = shader_attach(vertex_shader_path, GL_VERTEX_SHADER, shader->program);
  if (result_code != SUCCESS)
    goto shader_error;

  result_code = shader_attach(fragment_shader_path, GL_FRAGMENT_SHADER, shader->program);
  if (result_code != SUCCESS)
    goto shader_error;

  for (size_t i = 0; i < attributes->size; i++) {
    struct shader_attribute *attribute = buffer_get(attributes, i);
    result_code = shader_bind_attribute(shader, attribute->name, attribute->index);
    if (result_code != SUCCESS)
      goto shader_error;
  }

  glLinkProgram(shader->program);
  glValidateProgram(shader->program);

  GLint link_success = GL_FALSE, validated = GL_TRUE;
  glGetProgramiv(shader->program, GL_LINK_STATUS, &link_success);
  glGetProgramiv(shader->program, GL_VALIDATE_STATUS, &validated);
  if (link_success == GL_FALSE || validated == GL_FALSE)
    goto program_error;

  goto cleanup;
program_error:
  result_code = SHADER_CREATE_ERROR;
shader_error:
  shader_delete(shader); 
cleanup:
  return result_code;
}

int shader_set_uniform(struct shader *shader, struct shader_uniform *uniform) {
  int result_code = SUCCESS;
  GLint location = glGetUniformLocation(shader->program, uniform->name);
  if (glGetError() != GL_NO_ERROR)
    return SHADER_UNIFORM_SET_ERROR;

  switch (uniform->type) {
    case SHADER_UNIFORM_INT:
      glUniform1i(location, uniform->i);
      break;
    case SHADER_UNIFORM_FLOAT:
      glUniform1f(location, uniform->f);
      break;
    case SHADER_UNIFORM_VEC2F:
      glUniform2f(location, uniform->vec2f.x, uniform->vec2f.y);
      break;
    case SHADER_UNIFORM_VEC3F:
      glUniform3f(location, uniform->vec3f.x, uniform->vec3f.y, uniform->vec3f.z);
      break;
    case SHADER_UNIFORM_VEC4F:
      glUniform4f(location, uniform->vec4f.x, uniform->vec4f.y, uniform->vec4f.z, uniform->vec4f.t);
      break;
    case SHADER_UNIFORM_MAT4F:
      glUniformMatrix4fv(location, 1, GL_FALSE, (float *)&uniform->mat4f);
      break;
    case SHADER_UNIFORM_TRANSPOSE_MAT4F:
      glUniformMatrix4fv(location, 1, GL_TRUE, (float *)&uniform->mat4f);
      break;
    default:
      result_code = SHADER_UNIFORM_UNKOWN_ERROR;
      break;
  }
  if (glGetError() != GL_NO_ERROR)
    return SHADER_UNIFORM_SET_ERROR;

  return result_code;
}

int shader_set_uniforms(struct shader *shader, struct buffer *uniforms) {
  for (size_t i = 0; i < uniforms->size; i++) {
    int result_code = shader_set_uniform(shader, (struct shader_uniform *)buffer_get(uniforms, i));
    if (result_code != SUCCESS)
      return result_code;
  }
  return SUCCESS;
}

int shader_load(struct shader *shader, const char *vertex_shader_path, const char *fragment_shader_path) {
  struct buffer attributes = buffer_new(3, sizeof(struct shader_attribute));

  struct shader_attribute position_attribute = {
    .index = VERTEX_BUFFER_POSITION,
    .name = "position"
  };
  struct shader_attribute normal_attribute = {
    .index = VERTEX_BUFFER_NORMAL,
    .name = "normal"
  };
  struct shader_attribute texture_attribute = {
    .index = VERTEX_BUFFER_TEXTURE_COORD,
    .name = "texture_coord"
  };

  *(struct shader_attribute *)buffer_get(&attributes, 0) = position_attribute;
  *(struct shader_attribute *)buffer_get(&attributes, 1) = normal_attribute;
  *(struct shader_attribute *)buffer_get(&attributes, 2) = texture_attribute;

  return shader_load_with_attributes(shader, vertex_shader_path, fragment_shader_path, &attributes);
}

void shader_delete(struct shader *shader) {
  if (shader->program != 0) {
    GLuint attached_shaders[shader_max_attached_files];
    int num_attached_shaders = 0;
    glGetAttachedShaders(shader->program, shader_max_attached_files, &num_attached_shaders, attached_shaders);
    for (int i = 0; i < num_attached_shaders; i++) {
      glDetachShader(shader->program, attached_shaders[i]);      
      glDeleteShader(attached_shaders[i]);
    }
    glDeleteProgram(shader->program);
    
    glGetError(); // Clear errors
  }
}

int shader_use(struct shader *shader, struct buffer *uniforms) {
  int result_code = SUCCESS;
  glUseProgram(shader->program);
  if (glGetError() != GL_NO_ERROR)
    return SHADER_USE_ERROR;

  result_code = shader_set_uniforms(shader, uniforms);
  return result_code;
}

int shader_unuse(struct shader *shader) {
  glUseProgram(0);
  if (glGetError() != GL_NO_ERROR)
    return SHADER_USE_ERROR;
  else
    return SUCCESS;
}
