#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 in_normal;

out vec4 vec_color;
out vec3 normal;
out vec3 world_pos;

uniform vec4 color;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat3 normal_matrix;
uniform vec2 translation_offset;

void main() {
	vec_color = color;
	gl_Position = projection * view * model * vec4(pos, 1.0f);
	gl_Position.xy += translation_offset * gl_Position.w;
	world_pos = vec3(model * vec4(pos, 1.0f));
	normal = normal_matrix * in_normal;
}
