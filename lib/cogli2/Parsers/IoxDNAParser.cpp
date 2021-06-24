/*
 * IoxDNAParser.cpp
 *
 *  Created on: 20/ott/2013
 *      Author: lorenzo
 */

#include "IoxDNAParser.h"
#include "../Utils/StringHelper.h"
#include "../Utils/Logging.h"
#include "../Drawables/Sphere.h"
#include "../Drawables/Cylinder.h"
#include "../cl_arguments.h"

using std::string;
using std::vector;
using glm::vec3;
using glm::vec4;

IoxDNAParser::IoxDNAParser() {
	_time = -1;
}

IoxDNAParser::~IoxDNAParser() {

}

void IoxDNAParser::_parse_headers(std::ifstream &_current_input, Scene *my_scene) {
	string buff;
	// parse the time
	getline(_current_input, buff);
	vector<string> spl = StringHelper::split(buff);
	StringHelper::from_string<long long int>(spl[2], _time);

	if(StringHelper::trim(buff).size() == 0) {
		Logging::log_critical("Malformed configuration file\n");
		exit(1);
	}

	// get the box from the second line
	getline(_current_input, buff);
	vec3 box = _get_box(buff);
	my_scene->set_box(box);

	// ignore line with energies
	getline(_current_input, buff);
}

glm::vec3 IoxDNAParser::_get_box(string &line) {
	vector<string> spl = StringHelper::split(line);

	if(spl.size() != 5) {
		Logging::log_critical("Malformed or empty configuration file, the second header line should contain 5 tokens\n");
		exit(1);
	}

	vec3 box;

	StringHelper::from_string<float>(spl[2 + 0], box[0]);
	StringHelper::from_string<float>(spl[2 + 1], box[1]);
	StringHelper::from_string<float>(spl[2 + 2], box[2]);

	return box;
}
