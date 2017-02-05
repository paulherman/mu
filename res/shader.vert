#version 330

in vec3 position;
in vec3 normal;
in vec2 texture_coord;

out vec3 frag_position;
out vec3 frag_normal;
out vec2 frag_texture_coord;

uniform mat4 transformation;
uniform mat4 projection;
uniform mat4 view;

void main() {
  gl_Position = projection * view * transformation * vec4(position, 1.0);
  frag_position = position;
  frag_normal = normal;
  frag_texture_coord = texture_coord;
}
