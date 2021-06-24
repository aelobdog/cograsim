/*
 * LevyParser.cpp
 *
 *  Created on: 20/ott/2013
 *      Author: lorenzo
 */

#include "LevyParser.h"
#include "../Utils/StringHelper.h"
#include "../Utils/Logging.h"
#include "../Drawables/Group.h"
#include "../Drawables/Sphere.h"
#include "../Drawables/Cylinder.h"

using std::string;
using std::vector;
using glm::vec3;
using glm::vec4;

LevyParser::LevyParser(string topology, const std::queue<string> &input_files) : Parser(input_files), _N(-1) {
	_read_topology(topology);
}

LevyParser::~LevyParser() {

}

Scene *LevyParser::next_scene() {
	Scene *my_scene = new Scene();

	try {
		_open_input_if_required();

		_parse_headers(_current_input, my_scene);

		// set scene title
		std::stringstream title;
		title << _current_input_name << ": " << _conf_index << ", time: " << _time;
		my_scene->set_base_title(title.str());

		int N_per_tetramer = 5;
		int N_per_dimer = 7;
		int N_in_tetramers = N_per_tetramer*_N_tetramers;

		int curr_line = 3;
		bool done = false;
		int index = 0;
		glm::vec3 centre_pos(0.f, 0.f, 0.f);
		Shape *last = NULL;
		Group *curr_group = NULL;
		int curr_id = 0;
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

				int base_idx, N_per_construct;
				if(index < N_in_tetramers) {
					base_idx = index;
					N_per_construct = N_per_tetramer;
					int idx_in_construct = base_idx % N_per_construct;
					if(idx_in_construct == 0) {
						curr_group = new Group();
						curr_group->set_id(curr_id);
						curr_id++;
						my_scene->add_shape(curr_group);

						Shape *p = new Sphere(pos, 0.5f, glm::vec4(1.f, 0.f, 0.f, 1.f));
						curr_group->add_shape(p);
						last = p;
					}
					else {
						Shape *p = new Sphere(pos, 0.5f, glm::vec4(0.f, 1.f, 0.f, 1.f));
						curr_group->add_shape(p);
						// patch
						glm::vec3 patch_pos = pos + 0.5f*v1;
						Shape *pp = new Sphere(patch_pos, 0.12f, _base_colors["ORANGE"]);
						curr_group->add_shape(pp);

						glm::vec3 axis = last->position() - pos;
						Shape *cyl = new Cylinder(pos, axis, 0.1, _base_colors["MAGENTA"]);
						curr_group->add_shape(cyl);
					}

				}
				else {
					base_idx = index - N_in_tetramers;
					N_per_construct = N_per_dimer;
					int idx_in_construct = base_idx % N_per_construct;
					Shape *p;
					if(idx_in_construct == 0 || idx_in_construct == (N_per_dimer - 1)) {
						if(idx_in_construct == 0) {
							curr_group = new Group();
							curr_group->set_id(curr_id);
							curr_id++;
							my_scene->add_shape(curr_group);
						}
						p = new Sphere(pos, 0.5f, glm::vec4(0.f, 0.f, 1.f, 1.f));
						// patch
						glm::vec3 patch_pos = pos + 0.5f*v1;
						Shape *pp = new Sphere(patch_pos, 0.12f, _base_colors["ORANGE"]);
						curr_group->add_shape(pp);
					}
					else p = new Sphere(pos, 0.525f, glm::vec4(0.5f, 0.5f, 0.5f, 1.f));

					curr_group->add_shape(p);

					if(idx_in_construct != 0) {
						glm::vec3 axis = last->position() - pos;
						Shape *cyl = new Cylinder(pos, axis, 0.1, _base_colors["MAGENTA"]);
						curr_group->add_shape(cyl);
					}
					last = p;
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

void LevyParser::_read_topology(std::string filename) {
		_current_input.open(filename.c_str());

		string buff;
		getline(_current_input, buff);
		std::vector<std::string> spl = StringHelper::split(buff);
		if(spl.size() != 3) {
			Logging::log_critical("Invalid topology found in '%s'\n", filename.c_str());
			exit(1);
		}

		StringHelper::from_string<int>(spl[0], _N);
		_N_tetramers = _N_dimers = 0;
		StringHelper::from_string<int>(spl[1], _N_tetramers);
		StringHelper::from_string<int>(spl[2], _N_dimers);

		_current_input.close();
}
