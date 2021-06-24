/*
 * Lighting.cpp
 *
 *  Created on: 06/nov/2013
 *      Author: lorenzo
 */

#include "Lighting.h"
#include "Utils/StringHelper.h"

#define LIGHT_INITIAL_DIFFUSE 0.95f
#define LIGHT_INITIAL_AMBIENT 0.05f

using std::endl;

Light::Light() {
	enabled = false;

	direction = glm::vec3(0.f, 0.f, 0.f);
	// openGL defaults
	diffuse = glm::vec3(LIGHT_INITIAL_DIFFUSE, LIGHT_INITIAL_DIFFUSE, LIGHT_INITIAL_DIFFUSE);
	specular = glm::vec3(0.1f, 0.1f, 0.1f);
}

LightingState::LightingState() :
				_lights(8, Light()) {
	ambient = glm::vec3(LIGHT_INITIAL_AMBIENT, LIGHT_INITIAL_AMBIENT, LIGHT_INITIAL_AMBIENT);
	// we place a directional light on each vertex of a cube
	int counter = 0;
	for(int i = -1; i <= 1; i += 2) {
		for(int j = -1; j <= 1; j += 2) {
			for(int k = -1; k <= 1; k += 2) {
				Light &light = _lights[counter];
				// the order i, k, j is intentional
				light.direction = glm::vec3(i, k, j);
				glm::normalize(light.direction);
				counter++;
			}
		}
	}
}

LightingState::~LightingState() {

}

void LightingState::change_ambient(float delta) {
	set_dirty(true);

	glm::vec3 dv(delta, delta, delta);
	ambient += dv;
	if(ambient[0] < 0.) {
		ambient = glm::vec3(0., 0., 0.);
	}
}

void LightingState::change_diffuse(float delta) {
	set_dirty(true);

	glm::vec3 dv(delta, delta, delta);
	for(int i = 0; i < 8; i++) {
		_lights[i].diffuse += dv;
		if(_lights[i].diffuse[0] < 0.) {
			_lights[i].diffuse = glm::vec3(0., 0., 0.);
		}
	}
}

void LightingState::toggle_light(int ind) {
	set_dirty(true);

	_lights[ind].enabled = !_lights[ind].enabled;
}

bool LightingState::dirty() {
	return _dirty;
}

void LightingState::set_dirty(bool d) {
	_dirty = d;
}

std::string LightingState::get_state() {
	std::stringstream out;

	out << "diffuse " << _lights[0].diffuse[0] << endl;
	out << "specular " << _lights[0].specular[0] << endl;
	out << "ambient " << ambient[0] << endl;
	out << "llist";
	for(int i = 0; i < 8; i++) {
		int val = (_lights[i].enabled);
		out << " " << val;
	}
	out << endl;

	return out.str();
}

void LightingState::load_state_from_file(std::ifstream &in) {
	in.clear();
	in.seekg(0);
	while(in.good()) {
		std::string buff;
		getline(in, buff);

		std::vector<std::string> spl = StringHelper::split(buff);
		if(spl.size() > 0 && buff[0] != '#') {
			if(spl[0] == "diffuse") {
				float new_diffuse;
				StringHelper::from_string<float>(spl[1], new_diffuse);
				glm::vec3 diffuse(new_diffuse, new_diffuse, new_diffuse);
				for(int i = 0; i < 8; i++) {
					_lights[i].diffuse = diffuse;
				}
			}
			if(spl[0] == "specular") {
				float new_specular;
				StringHelper::from_string<float>(spl[1], new_specular);
				glm::vec3 specular(new_specular, new_specular, new_specular);
				for(int i = 0; i < 8; i++) {
					_lights[i].specular = specular;
				}
			}
			else if(spl[0] == "ambient") {
				float new_ambient;
				StringHelper::from_string<float>(spl[1], new_ambient);
				ambient[0] = ambient[1] = ambient[2] = new_ambient;
			}
			else if(spl[0] == "llist") {
				if(spl.size() != 9) {
					throw std::string("Wrong number of lights. Ignoring llist");
				}

				for(int i = 0; i < 8; i++) {
					StringHelper::from_string<bool>(spl[i + 1], _lights[i].enabled);
				}
			}
		}
	}
}
