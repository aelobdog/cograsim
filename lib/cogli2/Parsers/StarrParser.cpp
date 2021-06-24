/*
 * StarrParser.cpp
 *
 *  Created on: 20/ott/2013
 *      Author: lorenzo
 */

#include "StarrParser.h"
#include "../Utils/StringHelper.h"
#include "../Utils/Logging.h"
#include "../Drawables/Sphere.h"
#include "../Drawables/Cylinder.h"

using std::string;
using std::vector;
using glm::vec3;
using glm::vec4;

StarrParser::StarrParser(string topology, const std::queue<string> &input_files, const char *opts) :
				Parser(input_files),
				_N(-1),
				_N_dimer_spacers(2) {
	_read_topology(topology);

	if(opts != NULL) {
		_N_dimer_spacers = atoi(opts);
		if((_N_dimer_spacers % 2) != 0) {
			Logging::log_critical("The number of dimer spacers should be an even integer\n");
			exit(1);
		}
	}
}

StarrParser::~StarrParser() {

}

Scene *StarrParser::next_scene() {
	Scene *my_scene = new Scene();

	try {
		_open_input_if_required();

		_parse_headers(_current_input, my_scene);

		// set scene title
		std::stringstream title;
		title << _current_input_name << ": " << _conf_index << ", time: " << _time;
		my_scene->set_base_title(title.str());

		int N_per_tetramer = 4 + 8 * _seqs[0].size();
		int N_per_dimer = 0;
		if(_mode == VITRIMERS) N_per_dimer = _N_dimer_spacers + 4 * _seqs[1].size();
		int N_in_tetramers = N_per_tetramer * _N_tetramers;
		int N_in_dimers = N_per_dimer * _N_dimers;
		if(_mode != STRANDS && _N != (N_in_tetramers + N_in_dimers)) {
			Logging::log_critical("The number of particles in tetramers (%d) plus the number of particles in dimers (%d) do not add up to the total number of particles specified in the topology (%d)\n", N_in_tetramers, N_in_dimers, _N);
			exit(1);
		}

		int curr_line = 3;
		bool done = false;
		int index = 0;
		glm::vec3 centre_pos(0.f, 0.f, 0.f);
		Shape *last = NULL;
		int strand_id = 0;
		int idx_in_strand = 0;
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
			else if(spl.size() != 0) {
				spl = StringHelper::split(buff);
				vec3 pos, v1, v3;

				StringHelper::get_vec3(spl, 0, pos);
				StringHelper::get_vec3(spl, 3, v1);
				StringHelper::get_vec3(spl, 6, v3);

				int base_idx, N_per_strand, N_per_construct, N_spacers_per_strand;
				if(index < N_in_tetramers) {
					base_idx = index;
					N_per_strand = N_per_tetramer / 4;
					N_per_construct = N_per_tetramer;
					N_spacers_per_strand = 1;
				}
				else {
					base_idx = index - N_in_tetramers;
					N_per_strand = N_per_dimer / 2;
					N_per_construct = N_per_dimer;
					N_spacers_per_strand = _N_dimer_spacers / 2;
				}

				int idx_in_construct = (_mode == STRANDS) ? 0 : base_idx % N_per_construct;
				int idx_in_arm = (_mode == STRANDS) ? index : idx_in_construct % N_per_strand;

				if(idx_in_arm < N_spacers_per_strand && _mode != STRANDS) {
					centre_pos = pos;
					glm::vec4 color = (index < N_in_tetramers) ? glm::vec4(1.f, 0.f, 0.f, 1.f) : _base_colors["YELLOW"];
					Shape *p = new Sphere(pos, 0.5f, color);
					p->set_id(index);
					my_scene->add_shape(p);

					last = p;
				}
				else {
					if(last != NULL) {
						glm::vec3 axis = last->position() - pos;
						Shape *cyl = new Cylinder(pos, axis, 0.1, glm::vec4(0.f, 0.f, 0.f, 1.f));
						cyl->set_id(index);
						my_scene->add_shape(cyl);
					}

					Shape *sphere;
					bool is_base = ((_mode == STRANDS && (idx_in_arm % 2) == 1) || (_mode != STRANDS && ((idx_in_arm - N_spacers_per_strand) % 2)));
					if(is_base) {
						sphere = new Sphere(pos, 0.175f, glm::vec4(0.f, 0.f, 1.f, 1.f));
					}
					else {
						sphere = new Sphere(pos, 0.2f, glm::vec4(0.f, 1.f, 0.f, 1.f));
						last = sphere;
						if(_mode == STRANDS) {
							idx_in_strand++;
							if(strand_id >= (int) _seqs.size()) {
								Logging::log_critical("Not enough strand sequences specified in the topology file (%u)\n", _seqs.size());
								exit(1);
							}
							if(idx_in_strand == (int) _seqs[strand_id].size()) {
								idx_in_strand = 0;
								strand_id++;
								last = NULL;
							}
						}
					}
					sphere->set_id(index);
					my_scene->add_shape(sphere);
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
	catch(string &error) {
		Logging::log_critical(error);
		exit(1);
	}

	return my_scene;
}

void StarrParser::_read_topology(std::string filename) {
	_current_input.open(filename.c_str());

	string buff;
	getline(_current_input, buff);
	std::vector<std::string> spl = StringHelper::split(buff);
	if(spl.size() > 0) StringHelper::from_string<int>(spl[0], _N);
	_N_tetramers = _N_dimers = 0;

	while(!_current_input.eof()) {
		getline(_current_input, buff);
		if(buff.size() > 0) _seqs.push_back(buff);
	}

	switch(spl.size()) {
	case 1:
		_mode = TETRAMERS;
		_N_tetramers = _N / 68;
		_seqs.push_back(string("ACGTACGT"));
		break;
	case 2:
		_mode = STRANDS;
		StringHelper::from_string<int>(spl[1], _N_strands);
		break;
	case 3:
		_mode = VITRIMERS;
		StringHelper::from_string<int>(spl[1], _N_tetramers);
		StringHelper::from_string<int>(spl[2], _N_dimers);
		Logging::log_info("Vitrimer mode, %d tetramer(s) and %d dimer(s)\n", _N_tetramers, _N_dimers);
		break;
	case 0:
	default:
		Logging::log_critical("Invalid topology found in '%s'\n", filename.c_str());
		exit(1);
	}

	_current_input.close();
}
