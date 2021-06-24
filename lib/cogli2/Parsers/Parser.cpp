/*
 * Parser.cpp
 *
 *  Created on: 20/ott/2013
 *      Author: lorenzo
 */

#include <cstdlib>

#include "Parser.h"
#include "../Utils/Logging.h"
#include "../Utils/StringHelper.h"

using namespace std;
using namespace glm;

Parser::Parser(const queue<string> &input_files) : _input_files(input_files), _conf_index(0) {
	_base_colors["BLACK"] = glm::vec4(0.f, 0.f, 0.f, 1.f);
	_base_colors["WHITE"] = glm::vec4(1.f, 1.f, 1.f, 1.f);
	_base_colors["GREY"] = glm::vec4(0.5f, 0.5f, 0.5f, 1.f);

	_base_colors["RED"] = glm::vec4(1.f, 0.f, 0.f, 1.f);
	_base_colors["GREEN"] = glm::vec4(0.f, 1.f, 0.f, 1.f);
	_base_colors["BLUE"] = glm::vec4(0.f, 0.f, 1.f, 1.f);

	_base_colors["YELLOW"] = glm::vec4(1.f, 1.f, 0.f, 1.f);
	_base_colors["CYAN"] = glm::vec4(0.f, 1.f, 1.f, 1.f);
	_base_colors["MAGENTA"] = glm::vec4(1.f, 0.f, 1.f, 1.f);

	_base_colors["ORANGE"] = glm::vec4(1.f, 0.6f, 0.f, 1.f);

	_base_colors["VIOLET"] = glm::vec4(0.56f, 0.f, 1.f, 1.f);
	_base_colors["BROWN"] = glm::vec4(0.647f, 0.165f, 0.165f, 1.f);
	_base_colors["PINK"] = glm::vec4(0.98f, 0.855f, 0.867f, 1.f);

	_options = NULL;
}

Parser::~Parser() {

}

void Parser::_open_input_if_required() {
	if(!_current_input.is_open()) {
		_current_input_name = _input_files.front();
		_current_input.open(_current_input_name.c_str());
		_conf_index = 0;
		if(!_current_input.good()) {
			Logging::log_critical(_current_input_name + ": file is not readable\n");
			exit(1);
		}
	}
}

// check whether there is at least another configuration to be read from the already open input file
void Parser::_close_current_input_if_eof() {
	if(_current_input.is_open()) {
		int current = _current_input.tellg();
		string buff;
		getline(_current_input, buff);
		// there is one indeed!
		if(buff.size() > 0) {
			_current_input.seekg(current);
			_conf_index++;
		}
		else {
			_current_input.close();
			_input_files.pop();
		}
	}
}

void Parser::skip(int N) {
	for(int i = 0; i < N; i++) {
		if(!done()) {
			Scene *my_scene = next_scene();
			delete my_scene;
		}
	}
}

vec4 Parser::_get_color(string &line) {
	StringHelper::to_upper(line);

	if(line.substr(0, 2) != "C[" || line[line.length()-1] != ']') throw string("Invalid color\n");
	string color_string = line.substr(2, line.length()-3);

	map<string, vec4>::iterator found = _base_colors.find(color_string);
	if(found != _base_colors.end()) return found->second;

	vector<string> spl = StringHelper::split(color_string, ',');
	if(spl.size() < 3 || spl.size() > 4) throw string("Invalid color\n");

	vec4 color;
	if(!StringHelper::from_string<float>(spl[0], color[0])) throw string("Invalid color\n");
	if(!StringHelper::from_string<float>(spl[1], color[1])) throw string("Invalid color\n");
	if(!StringHelper::from_string<float>(spl[2], color[2])) throw string("Invalid color\n");
	if(spl.size() == 4) {
		if(!StringHelper::from_string<float>(spl[3], color[3])) throw string("Invalid color\n");
	}
	else color[3] = 1.f;

	if(color[0] > 1.f || color[1] > 1.f || color[2] > 1.f) {
		color[0] /= 255.f;
		color[1] /= 255.f;
		color[2] /= 255.f;
	}

	return color;
}
