/*
 * Lighting.h
 *
 *  Created on: 06/nov/2013
 *      Author: lorenzo
 */

#ifndef LIGHTING_H_
#define LIGHTING_H_

#include <vector>
#include <fstream>
#include <glm/glm.hpp>

#define LIGHT_DELTA_AMBIENT 0.02f
#define LIGHT_DELTA_DIFFUSE 0.05f

struct Light {
	bool enabled;

	glm::vec3 direction;

	glm::vec3 diffuse;
	glm::vec3 specular;

	Light();
	virtual ~Light() { };
};

class LightingState {
protected:
	std::vector<Light> _lights;
	bool _dirty = true;

public:
	glm::vec3 ambient;

	LightingState();
	virtual ~LightingState();

	void change_ambient(float delta);
	void change_diffuse(float delta);
	void toggle_light(int ind);
	std::vector<Light> &get_lights() { return _lights; }
	Light &get_light(int i) { return _lights[i]; }

	bool dirty();
	void set_dirty(bool);

	std::string get_state();
	void load_state_from_file(std::ifstream &in);
};

#endif /* LIGHTING_H_ */
