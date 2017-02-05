#version 330

in vec3 frag_position;
in vec3 frag_normal;
in vec2 frag_texture_coord;

out vec4 color;

uniform sampler2D texture_sampler;

void main() {
  color = texture(texture_sampler, frag_texture_coord);
}
