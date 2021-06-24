/*
 * ManfredoParser.cpp
 *
 *  Created on: 20/ott/2013
 *      Author: lorenzo
 */

#include "ManfredoParser.h"
#include "../Utils/StringHelper.h"
#include "../Utils/Logging.h"
#include "../Drawables/Sphere.h"
#include "../Drawables/Cylinder.h"
#include "../Drawables/DNA.h"

using std::string;
using std::vector;
using glm::vec3;
using glm::vec4;

ManfredoParser::ManfredoParser(string topology, const std::queue<string> &input_files) : Parser(input_files), _N(-1), _N_tetra(-1), _N_per_tetra(29) {
	_read_topology(topology);
}

ManfredoParser::~ManfredoParser() {

}

Scene *ManfredoParser::next_scene() {
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
		DNAStrand *curr_strand = NULL;
		glm::vec3 centre_pos(0.f, 0.f, 0.f);
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

				int rel_ind = index % _N_per_tetra;
				int centre_id = (index/_N_per_tetra) * 9;
				// centre
				if(rel_ind == 0) {
					centre_pos = pos;
					Shape *p = new Sphere(pos, 3.f, glm::vec4(0.f, 0.f, 1.f, 1.f));
					p->set_id(centre_id);
					my_scene->add_shape(p);
				}
				// arm
				else {
					int arm_ind = (rel_ind-1) % 7;
					// if it's the first, then we have to create a new strand
					if(arm_ind == 0) {
						int arm_id = centre_id + 2*((rel_ind-1) / 7);

						Shape *centre_arm = new Cylinder(centre_pos, pos-centre_pos, 0.5, glm::vec4(0.f, 1.f, 1.f, 0.5f));
						my_scene->add_shape(centre_arm);

						curr_strand = new DNAStrand();
						curr_strand->set_id(arm_id+1);
					}
					Nucleotide *nn = new Nucleotide(pos, v1, v3, glm::vec4(1.f, 0.f, 0.f, 1.f));
					curr_strand->add_nucleotide(nn, arm_ind);
					if(arm_ind == 6) {
						curr_strand->finalise();

						glm::vec4 s_color;
						int rel_arm = (rel_ind-1)/7;
						switch(rel_arm) {
						case 0:
							s_color = glm::vec4(1, 0, 0, 1);
							break;
						case 1:
							s_color = glm::vec4(0, 1, 0, 1);
							break;
						case 2:
							s_color = glm::vec4(0, 0, 1, 1);
							break;
						case 3:
							s_color = glm::vec4(1, 1, 0, 1);
							break;
						}
						curr_strand->set_color(s_color);

						my_scene->add_shape(curr_strand);
					}
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

void ManfredoParser::_read_topology (std::string filename) {
		_current_input.open(filename.c_str());

		string buff;
		getline(_current_input, buff);
		std::vector<std::string> spl = StringHelper::split(buff);
		if(spl.size() != 2) {
			Logging::log_critical("Invalid topology found in '%s'\n", filename.c_str());
			exit(1);
		}
		StringHelper::from_string<int> (spl[0], _N);
		StringHelper::from_string<int> (spl[1], _N_tetra);

		_current_input.close();
}
