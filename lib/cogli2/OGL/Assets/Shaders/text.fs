#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D text;
uniform vec3 text_color;
uniform bool is_background;

void main() {
	if(is_background) {
		color = vec4(text_color, 1.f);
	}
	else {
    	vec4 sampled = vec4(1.f, 1.f, 1.f, texture(text, TexCoords).r);
    	color = vec4(text_color, 1.f) * sampled;
    }
}
