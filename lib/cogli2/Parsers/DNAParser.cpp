/*
 * DNAParser.cpp
 *
 *  Created on: 20/ott/2013
 *      Author: lorenzo
 */

#include "DNAParser.h"
#include "../Utils/StringHelper.h"
#include "../Utils/Logging.h"
#include "../Drawables/Sphere.h"
#include "../Drawables/Cylinder.h"
#include "../Drawables/DNA.h"
#include "../cl_arguments.h"

using std::string;
using std::vector;
using glm::vec3;
using glm::vec4;

DNAParser::DNAParser(string topology, const std::queue<string> &input_files) :
				Parser(input_files) {
	_read_topology(topology);
}

DNAParser::~DNAParser() {

}

Scene *DNAParser::next_scene() {
	Scene *my_scene = new Scene();

	try {
		_open_input_if_required();

		_parse_headers(_current_input, my_scene);

		// set scene title
		std::stringstream title;
		title << _current_input_name << ": " << _conf_index << ", time: " << _time;
		my_scene->set_base_title(title.str());

		vector<Nucleotide *> nucleotides;

		int curr_line = 3;
		bool done = false;
		int i_nuc = 0;
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

				std::string cacas("C[red]");
				vec4 color = _get_color(cacas);

				nucleotides.push_back(new Nucleotide(pos, v1, v3, color, _base_id[i_nuc]));
			}
			curr_line++;
			i_nuc++;
			if(_current_input.eof()) done = true;
		}

		if(_N_nuc != (int) nucleotides.size()) {
			Logging::log_critical("The number of nucleotides found in the trajectory (%d) is different from the one found in the topology (%d)\n", nucleotides.size(), _N_nuc);
			exit(1);
		}

		std::map<int, DNAStrand *> strands;

		std::vector<Nucleotide *>::iterator it;
		int nid = 0;
		for(it = nucleotides.begin(); it != nucleotides.end(); it++) {
			int strandid = _n2s_map[nid] - 1;
			if(strands.find(strandid) == strands.end()) strands[strandid] = new DNAStrand();
			strands[strandid]->add_nucleotide(*it, nid);
			nid += 1;
		}

		std::map<int, DNAStrand *>::iterator sit = strands.begin();
		while(sit != strands.end()) {
			sit->second->finalise();
			if(_strand_circularity[sit->first]) sit->second->set_as_circular();
			sit->second->set_id(sit->first);
			my_scene->add_shape(sit->second);
			sit->second->set_automatic_color();
			sit++;
		}

		_close_current_input_if_eof();

		// we set the colors from the file, if it is present
		if(_options[COLORS_FROM]) {
			std::fstream colors_file;
			colors_file.open(_options[COLORS_FROM].arg);
			if(!colors_file.good()) {
				Logging::log_critical("Could not open color file `%s'\n", _options[COLORS_FROM].arg);
				exit(1);
			}

			string buff;
			getline(colors_file, buff);
			while(colors_file.good()) {
				vector<string> spl = StringHelper::split(buff);

				if(spl.size() != 2) {
					Logging::log_info("Ignoring malformed line\n\t%s\nin the colors_from file\n", buff.c_str());
				}

				string colorstring = "C[" + spl[1] + "]";
				vec4 color = _get_color(colorstring);

				auto ids = StringHelper::ids_from_range(spl[0]);
				for(auto id : ids) {
					if(id < 0 || id >= (int) nucleotides.size()) {
						Logging::log_info("Ignoring invalid nucleotide id %d in colors_from file\n", id);
					}
					else {
						nucleotides[id]->set_color(color);
					}
				}

				getline(colors_file, buff);
			}

			colors_file.close();
		}

	}
	catch(string &error) {
		Logging::log_critical(error);
		exit(1);
	}

	return my_scene;
}

void DNAParser::_read_topology(std::string filename) {
	_current_input.open(filename.c_str());
	// we read N, nstrands

	string buff;
	getline(_current_input, buff);
	std::vector<std::string> spl = StringHelper::split(buff);
	if(spl.size() != 2) {
		Logging::log_critical("Invalid topology found in '%s'\n", filename.c_str());
		exit(1);
	}
	StringHelper::from_string<int>(spl[0], _N_nuc);
	StringHelper::from_string<int>(spl[1], _N_str);
	for(int i = 0; i < _N_nuc; i++) {
		getline(_current_input, buff);

		if(!_current_input.good()) {
			Logging::log_critical("The topology file '%s' contains info about %d nucleotides only (should be %d)\n", filename.c_str(), i, _N_nuc);
			exit(1);
		}

		spl = StringHelper::split(buff);

		if(spl.size() != 4) {
			Logging::log_critical("Malformed topology line '%s'", buff);
			exit(1);
		}

		int p, q, k;
		StringHelper::from_string<int>(spl[0], k);
		_base_id.push_back(spl[1]);
		StringHelper::from_string<int>(spl[2], p);
		StringHelper::from_string<int>(spl[3], q);
		_n2s_map[i] = k;
		if(k > (int) _strand_circularity.size()) {
			_strand_circularity.resize(k, true);
		}
		if(p == -1 || q == -1) {
			_strand_circularity[k - 1] = false;
		}
	}

	_current_input.close();
}
