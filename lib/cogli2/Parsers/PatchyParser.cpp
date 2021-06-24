/*
 * PatchyParser.cpp
 *
 *  Created on: 20/ott/2013
 *      Author: lorenzo
 */

#include <glm/gtc/matrix_access.hpp>

#include "PatchyParser.h"
#include "../Utils/StringHelper.h"
#include "../Utils/Logging.h"
#include "../Drawables/PatchyParticle.h"
#include "../Drawables/Sphere.h"
#include "../Drawables/Cylinder.h"

using std::string;
using std::vector;
using glm::vec3;
using glm::vec4;

PatchyParser::PatchyParser(string topology, const std::queue<string> &input_files, string opts) : Parser(input_files) {
	_N = _N_A = _N_B = -1;
	_read_topology(topology);

	vector<string> spl = StringHelper::split(opts, ',');
	if(spl.size() < 3) {
		Logging::log_critical("The -p/--patchy option (%s) requires a comma-separated value containing at least three fields (delta, cosmax and N_A).\n", opts.c_str());
		exit(1);
	}
	StringHelper::from_string<float>(spl[0], _delta);
	StringHelper::from_string<float>(spl[1], _cosmax);
	_theta = acos(_cosmax);
	StringHelper::from_string<int>(spl[2], _N_patches_A);
	if(spl.size() > 3) StringHelper::from_string<int>(spl[3], _N_patches_B);
	else {
		_N_patches_B = _N_patches_A;
		if(_N_B > 0) Logging::log_warning("Unspecified number of patches for species B. Assuming %d\n", _N_patches_A);
	}

	_base_patches_A = _get_base_patches(_N_patches_A);
	_base_patches_B = _get_base_patches(_N_patches_B);
}

PatchyParser::~PatchyParser() {

}

Scene *PatchyParser::next_scene() {
	Scene *my_scene = new Scene();

	try {
		_open_input_if_required();

		_parse_headers(_current_input, my_scene);

		// set scene title
		std::stringstream title;
		title << _current_input_name << ": " << _conf_index << ", time: " << _time;
		my_scene->set_base_title(title.str());

		int curr_line = 3;
		bool done = false;
		int index = 0;
		while(!done) {
			string buff;
			int current = _current_input.tellg();
			getline(_current_input, buff);
			// if the first character is a 't' and the string contains a has 3
			// entries we have reached the end of the configuration. If this is
			// the case, we have to be sure that the next time we read from this
			// file, we get the first line.
			vector<string> spl = StringHelper::split(buff);
			if(buff[0] == 't' && spl.size() == 3) {
				_current_input.seekg(current);
				done = true;
			}
			else if(spl.size() != 0){
				spl = StringHelper::split(buff);
				vec3 pos, v1, v3;

				StringHelper::get_vec3(spl, 0, pos);
				StringHelper::get_vec3(spl, 3, v1);
				StringHelper::get_vec3(spl, 6, v3);
				vec3 v2 = glm::cross(v3, v1);

				glm::mat3x3 orientation;
				orientation[0] = v1;
				orientation[1] = v2;
				orientation[2] = v3;

				if(index < _N_A) {
					PatchyParticle *p = new PatchyParticle(pos, 0.5f, glm::vec4(1.f, 0.f, 0.f, 1.f));
					for(int i = 0; i < _N_patches_A; i++) {
						glm::vec3 patch_pos = (0.5f + _delta)*(orientation*_base_patches_A[i]);
						p->add_patch(patch_pos, _theta, glm::vec4(0.5f, 0.5f, 0.5f, 1.f));
					}
					p->set_id(index);
					my_scene->add_shape(p);
				}
				else {
					PatchyParticle *p = new PatchyParticle(pos, 0.5f, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
					for(int i = 0; i < _N_patches_B; i++) {
						glm::vec3 patch_pos = (0.5f + _delta)*(orientation*_base_patches_B[i]);
						p->add_patch(patch_pos, _theta, glm::vec4(0.5f, 0.5f, 0.5f, 1.f));
					}
					p->set_id(index);
					my_scene->add_shape(p);
				}
				index++;
			}
			curr_line++;
			if(_current_input.eof()) done = true;
		}

		if(_N != index) {
			Logging::log_critical("The number of nucleotides found in the trajectory (%d) is different from the one found in the topology (%d)\n", index, _N);
			exit(1);
		}

		_close_current_input_if_eof();
	}
	catch (string &error) {
		Logging::log_critical(error);
		exit(1);
	}

	return my_scene;
}

vector<vec3> PatchyParser::_get_base_patches(int N_patches) {
	vector<vec3> v(N_patches);

	switch(N_patches) {
	case 1: {
		v[0] = vec3(1.f, 0.f, 0.f);
		break;
	}
	case 2: {
		v[0] = vec3(0.f, 1.f, 0.f);
		v[1] = vec3(0.f, -1.f, 0.f);
		break;
	}
	case 3: {
		float cos30 = cos(M_PI/6.);
		float sin30 = sin(M_PI/6.);

		v[0] = vec3(0.f, 1.f, 0.f);
		v[1] = vec3(cos30, -sin30, 0.f);
		v[2] = vec3(-cos30, -sin30, 0.f);
		break;
	}
	case 4: {
		float half_isqrt3 =  0.5f/sqrt(3.f);

		v[0] = vec3(-half_isqrt3, -half_isqrt3,  half_isqrt3);
		v[1] = vec3( half_isqrt3, -half_isqrt3, -half_isqrt3);
		v[2] = vec3( half_isqrt3,  half_isqrt3,  half_isqrt3);
		v[3] = vec3(-half_isqrt3,  half_isqrt3, -half_isqrt3);
		break;
	}
	default:
		Logging::log_critical("Unsupported number of patches %d\n", N_patches);
		exit(1);
	}

	for(int i = 0; i < N_patches; i++) v[i] = glm::normalize(v[i]);

	return v;
}

void PatchyParser::_read_topology (std::string filename) {
		_current_input.open(filename.c_str());

		string buff;
		getline(_current_input, buff);
		std::vector<std::string> spl = StringHelper::split(buff);
		if(spl.size() != 2) {
			Logging::log_critical("Invalid topology found in '%s'\n", filename.c_str());
			exit(1);
		}
		StringHelper::from_string<int> (spl[0], _N);
		StringHelper::from_string<int> (spl[1], _N_B);
		_N_A = _N - _N_B;

		_current_input.close();
}
