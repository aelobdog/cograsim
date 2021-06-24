#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in mat4 model;
layout (location = 6) in mat3 normal_matrix;
layout (location = 9) in vec4 color;

out vec4 vec_color;
out vec3 world_pos;
out vec3 normal;

uniform mat4 projection;
uniform mat4 view;

void main() {
	vec_color = color;
	gl_Position = projection * view * model * vec4(pos, 1.0f);
	world_pos = vec3(model * vec4(pos, 1.0f));
	normal = normal_matrix * in_normal;
}
