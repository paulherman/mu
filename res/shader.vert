#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texture_coord;
layout(location = 3) in int position_bone_id;
layout(location = 4) in int normal_bone_id;

out vec3 frag_position;
out vec3 frag_normal;
out vec2 frag_texture_coord;

uniform mat4 transformation;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 bone_transformations[32];

void main() {
  gl_Position = projection * view * transformation * bone_transformations[position_bone_id] * vec4(position, 1.0);
  frag_normal = normal;
  frag_texture_coord = texture_coord;
}
