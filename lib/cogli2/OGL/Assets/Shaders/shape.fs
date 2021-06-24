#version 330 core

struct PointLight {
	bool enabled;

	vec3 direction;
	
	vec3 diffuse;
	vec3 specular;
};

#define NR_POINT_LIGHTS 8

out vec4 frag_color;

uniform vec3 ambient_light;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform vec3 camera_position;
uniform bool disable_lighting;

in vec4 vec_color;
in vec3 normal;
in vec3 world_pos;

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 camera_dir);

void main() {
	if(disable_lighting) {
		frag_color = vec_color;
		return;
	}

    vec3 norm = normalize(normal);
    vec3 camera_dir = normalize(camera_position - world_pos);

	vec3 result = ambient_light;
	for(int i = 0; i < NR_POINT_LIGHTS; i++) {
		if(pointLights[i].enabled) {
    		result += CalcPointLight(pointLights[i], norm, world_pos, camera_dir);
    	}
    }

	frag_color = vec_color * vec4(result, 1.f);
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 camera_dir) {
    vec3 lightDir = normalize(light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(camera_dir, reflectDir), 0.0), 70.f);
    
    // combine results
    vec3 diffuse = light.diffuse * diff;
	vec3 specular = light.specular * spec;
    
    return diffuse + specular;
}
