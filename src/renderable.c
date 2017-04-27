#include "renderable.h"
#include "vecmath.h"
#include "log.h"
#include "error.h"

int renderable_load_mesh(struct renderable *renderable, struct mesh *mesh) {
  int result_code = SUCCESS;

  glGenVertexArrays(1, &renderable->vertex_array_object);
  if (glGetError() != GL_NO_ERROR)
    goto create_error;

  glBindVertexArray(renderable->vertex_array_object);
  if (glGetError() != GL_NO_ERROR)
    goto create_error;

  glGenBuffers(NUM_VERTEX_BUFFERS, renderable->vertex_buffers);
  if (glGetError() != GL_NO_ERROR)
    goto create_error;

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderable->vertex_buffers[VERTEX_BUFFER_INDEX]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->num_vertices * sizeof(uint32_t), mesh->vertex_ids, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  if (glGetError() != GL_NO_ERROR)
    goto create_error;

  glBindBuffer(GL_ARRAY_BUFFER, renderable->vertex_buffers[VERTEX_BUFFER_POSITION]);
  glBufferData(GL_ARRAY_BUFFER, 3 * mesh->num_vertex_defs * sizeof(float), mesh->vertex_positions, GL_STATIC_DRAW);
  glVertexAttribPointer(VERTEX_BUFFER_POSITION, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  if (glGetError() != GL_NO_ERROR)
    goto create_error;

  glBindBuffer(GL_ARRAY_BUFFER, renderable->vertex_buffers[VERTEX_BUFFER_NORMAL]);
  glBufferData(GL_ARRAY_BUFFER, 3 * mesh->num_vertex_defs * sizeof(float), mesh->vertex_normals, GL_STATIC_DRAW);
  glVertexAttribPointer(VERTEX_BUFFER_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  if (glGetError() != GL_NO_ERROR)
    goto create_error;

  glBindBuffer(GL_ARRAY_BUFFER, renderable->vertex_buffers[VERTEX_BUFFER_TEXTURE_COORD]);
  glBufferData(GL_ARRAY_BUFFER, 2 * mesh->num_vertex_defs * sizeof(float), mesh->texture_coords, GL_STATIC_DRAW);
  glVertexAttribPointer(VERTEX_BUFFER_TEXTURE_COORD, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  if (glGetError() != GL_NO_ERROR)
    goto create_error;

  glBindBuffer(GL_ARRAY_BUFFER, renderable->vertex_buffers[VERTEX_BUFFER_POSITION_BONE]);
  glBufferData(GL_ARRAY_BUFFER, mesh->num_vertex_defs * sizeof(uint32_t), mesh->vertex_position_bones, GL_STATIC_DRAW);
  glVertexAttribIPointer(VERTEX_BUFFER_POSITION_BONE, 1, GL_INT, 0, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  if (glGetError() != GL_NO_ERROR)
    goto create_error;

  glBindBuffer(GL_ARRAY_BUFFER, renderable->vertex_buffers[VERTEX_BUFFER_NORMAL_BONE]);
  glBufferData(GL_ARRAY_BUFFER, mesh->num_vertex_defs * sizeof(uint32_t), mesh->vertex_normal_bones, GL_STATIC_DRAW);
  glVertexAttribIPointer(VERTEX_BUFFER_NORMAL_BONE, 1, GL_INT, 0, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  if (glGetError() != GL_NO_ERROR)
    goto create_error;

  glBindVertexArray(0);
  renderable->num_vertices = mesh->num_vertices;

  goto cleanup;
create_error:
  result_code = RENDERABLE_BUFFER_ERROR;
  glBindVertexArray(0);
  glDeleteVertexArrays(1, &renderable->vertex_array_object);
  glDeleteBuffers(NUM_VERTEX_BUFFERS, renderable->vertex_buffers);
cleanup:
  return result_code;
}

void renderable_delete(struct renderable *renderable) {
  glDeleteVertexArrays(1, &renderable->vertex_array_object);
  glDeleteBuffers(NUM_VERTEX_BUFFERS, renderable->vertex_buffers);
}

static float aspect_ratio() {
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);
  float width = viewport[2];
  float height = viewport[3];
  return width / height;
}

int renderable_render(struct renderable *renderable, struct shader *shader, struct texture *texture, struct camera *camera, struct transformation *transformation, struct buffer *lights, struct buffer *bones) {
  int error_code = SUCCESS;

  const struct mat4f transformation_matrix = mat4f_transformation(transformation->position, transformation->rotation, transformation->scale);
  const struct shader_uniform transformation_uniform = {
    .type = SHADER_UNIFORM_TRANSPOSE_MAT4F,
    .mat4f = transformation_matrix,
    .name = "transformation"
  };

  const struct mat4f projection_matrix = mat4f_projection(aspect_ratio(), camera->fov, camera->clip_near, camera->clip_far);
  const struct shader_uniform projection_uniform = {
    .type = SHADER_UNIFORM_MAT4F,
    .mat4f = projection_matrix,
    .name = "projection"
  };

  const struct mat4f view_matrix = mat4f_transformation(camera->transformation.position, camera->transformation.rotation, camera->transformation.scale);
  const struct shader_uniform view_uniform = {
    .type = SHADER_UNIFORM_TRANSPOSE_MAT4F,
    .mat4f = view_matrix,
    .name = "view"
  };

  const struct shader_uniform bones_uniform = {
    .type = SHADER_UNIFORM_TRANSPOSE_MAT4F_ARRAY,
    .mat4f_array = bones,
    .name = "bone_transformations"
  };

  struct buffer uniforms = buffer_new(4, sizeof(struct shader_uniform));
  *(struct shader_uniform *)buffer_get(&uniforms, 0) = transformation_uniform;
  *(struct shader_uniform *)buffer_get(&uniforms, 1) = projection_uniform;
  *(struct shader_uniform *)buffer_get(&uniforms, 2) = view_uniform;
  *(struct shader_uniform *)buffer_get(&uniforms, 3) = bones_uniform;

  error_code = shader_use(shader, &uniforms);
  if (error_code != SUCCESS)
    goto err;

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture->id);

  glBindVertexArray(renderable->vertex_array_object);

  glEnableVertexAttribArray(VERTEX_BUFFER_POSITION);
  glEnableVertexAttribArray(VERTEX_BUFFER_NORMAL);
  glEnableVertexAttribArray(VERTEX_BUFFER_TEXTURE_COORD);

  glDrawElements(GL_TRIANGLES, renderable->num_vertices, GL_UNSIGNED_INT, 0);

  glDisableVertexAttribArray(VERTEX_BUFFER_TEXTURE_COORD);
  glDisableVertexAttribArray(VERTEX_BUFFER_NORMAL);
  glDisableVertexAttribArray(VERTEX_BUFFER_POSITION);

  glBindVertexArray(0);

  if (glGetError() != GL_NO_ERROR)
    goto err;

  goto ret;
err:
  error_code = RENDERABLE_RENDER_ERROR;
ret:
  error_code = shader_unuse(shader);
  return error_code;
}
