/*
 * IcoParser.cpp
 *
 *  Created on: 25/lug/2018
 *      Author: Flavio
 */

#include "IcoParser.h"
#include "../Utils/StringHelper.h"
#include "../Utils/Logging.h"
#include "../Drawables/Icosahedron.h"
#include "../Drawables/Sphere.h"

using std::string;
using std::vector;
using glm::vec3;
using glm::vec4;

IcoParser::IcoParser(string topology, const std::queue<string> &input_files) :
				Parser(input_files),
				_N(-1) {
	_mycolors.resize(0);
	_pcol.resize(0);
	_ppos.resize(0);
	_mycolors.push_back(glm::vec4(1.0, 0.0, 0.0, 1.0));
	_mycolors.push_back(glm::vec4(0.0, 1.0, 0.0, 1.0));
	_mycolors.push_back(glm::vec4(0.0, 0.0, 1.0, 1.0));
	_mycolors.push_back(glm::vec4(1.0, 1.0, 0.0, 1.0));
	_mycolors.push_back(glm::vec4(0.0, 1.0, 1.0, 1.0));
	_mycolors.push_back(glm::vec4(1.0, 0.0, 1.0, 1.0));

	_read_topology(topology);
}

IcoParser::~IcoParser() {

}

Scene* IcoParser::next_scene() {
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
			else if(spl.size() != 0) {
				spl = StringHelper::split(buff);
				vec3 pos, v1, v3, v2;

				StringHelper::get_vec3(spl, 0, pos);
				StringHelper::get_vec3(spl, 3, v1);
				StringHelper::get_vec3(spl, 6, v3);
				v2 = glm::cross(v3, v1);

				glm::mat3x3 orientation;
				orientation[0] = v1;
				orientation[1] = v2;
				orientation[2] = v3;

				Icosahedron *p = new Icosahedron(pos, v1, v2, 0.5f, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

				// here we add the patches:
				for(unsigned int i = 0; i < _ppos.size(); i++) {
					glm::vec3 mypos = _ppos[i];
					glm::vec4 mycol = _pcol[i];
					mypos = orientation * mypos;
					p->add_patch(mypos, 0.07, mycol);
				}

				p->set_id(index);
				my_scene->add_shape(p);

				index++;
			}
			curr_line++;
			if(_current_input.eof())
				done = true;
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

void IcoParser::_read_topology(std::string filename) {
	_current_input.open(filename.c_str());

	string buff;
	getline(_current_input, buff);
	std::vector<std::string> spl = StringHelper::split(buff);
	if(spl.size() != 1) {
		Logging::log_critical("Invalid topology found in '%s'\n", filename.c_str());
		exit(1);
	}
	StringHelper::from_string<int>(spl[0], _N);

	_current_input.close();

	// here is where we read the patches
	_current_input.open("patches.txt");
	if(!_current_input.good()) {
		Logging::log_critical("Icosahedron patch file 'patches.txt' not found\n");
		exit(1);
	}
	getline(_current_input, buff);
	spl = StringHelper::split(buff);
	_pcol.resize(0);
	_ppos.resize(0);
	bool done = false;
	int pcntr = -1;
	while(done == false) {
		if(spl.size() > 0) {
			if(spl[0].compare(0, 5, "patch") == 0) {
				//Logging::log ("patch found %d\n", pcntr);
				pcntr++;
			}
			if(spl[0].compare("position") == 0) {
				glm::vec3 mypos;
				StringHelper::get_vec3(spl, 2, mypos);
				_ppos.push_back(mypos);
				//Logging::log("found position for patch %d %g %g %g\n", pcntr, mypos[0], mypos[1], mypos[2]);
			}
			if(spl[0].compare("color") == 0) {
				glm::vec3 mypos;
				int c;
				StringHelper::from_string<int>(spl[2], c);
				//Logging::log("found color for patch %d %d\n", pcntr, c);
				if(c >= 0 && c < 6) {
					_pcol.push_back(_mycolors[c]);
				}
				else {
					_pcol.push_back(_mycolors[0]);
				}
			}
		}
		if(_current_input.eof()) {
			done = true;
		}
		getline(_current_input, buff);
		spl = StringHelper::split(buff);
	}
	_current_input.close();
}
