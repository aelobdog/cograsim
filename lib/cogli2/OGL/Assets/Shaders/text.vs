#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
out vec2 TexCoords;

uniform bool is_background;
uniform mat4 projection;

void main() {
	if(is_background) {
		gl_Position = projection * vec4(vertex.xy, -0.1f, 1.f);
	}
	else {
    	gl_Position = projection * vec4(vertex.xy, 0.f, 1.f);
    }
    TexCoords = vertex.zw;
}
