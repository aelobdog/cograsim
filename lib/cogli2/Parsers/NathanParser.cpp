/*
 * NathanParser.cpp
 *
 *  Created on: 20/ott/2013
 *      Author: lorenzo
 */

#include "NathanParser.h"
#include "../Utils/StringHelper.h"
#include "../Utils/Logging.h"
#include "../Drawables/PatchyParticle.h"
#include "../Drawables/Sphere.h"
#include "../Drawables/Cylinder.h"

using std::string;
using std::vector;
using glm::vec3;
using glm::vec4;

NathanParser::NathanParser(string topology, const std::queue<string> &input_files, float size_ratio) : Parser(input_files), _pol_r(0.5f / size_ratio), _N(-1), _N_patchy(-1), _N_per_chain(-1), _N_chains(-1) {
	_read_topology(topology);
}

NathanParser::~NathanParser() {

}

Scene *NathanParser::next_scene() {
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

				if(index < _N_patchy) {
					PatchyParticle *p = new PatchyParticle(pos, 0.5f, glm::vec4(1.f, 0.f, 0.f, 1.f));
					glm::vec3 patch_pos = (0.5f + NATHAN_PATCH_DELTA) * v3;
					p->add_patch(patch_pos, NATHAN_PATCH_WIDTH, glm::vec4(0.f, 0.f, 1.f, 1.f));
					p->add_patch(-patch_pos, NATHAN_PATCH_WIDTH, glm::vec4(0.f, 0.f, 1.f, 1.f));
					p->set_id(index);
					my_scene->add_shape(p);
				}
				else {
					Shape *p = new Sphere(pos, _pol_r, glm::vec4(0.5f, 0.5f, 0.5f, 0.5f));
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

void NathanParser::_read_topology (std::string filename) {
		_current_input.open(filename.c_str());

		string buff;
		getline(_current_input, buff);
		std::vector<std::string> spl = StringHelper::split(buff);
		if(spl.size() != 3) {
			Logging::log_critical("Invalid topology found in '%s'\n", filename.c_str());
			exit(1);
		}
		StringHelper::from_string<int> (spl[0], _N_patchy);
		StringHelper::from_string<int> (spl[1], _N_chains);
		StringHelper::from_string<int> (spl[2], _N_per_chain);

		_N = _N_patchy + _N_chains*_N_per_chain;

		_current_input.close();
}
