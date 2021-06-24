/*
 * Parser.h
 *
 *  Created on: 20/ott/2013
 *      Author: lorenzo
 */

#ifndef PARSER_H_
#define PARSER_H_

#include <vector>
#include <string>
#include <fstream>
#include <queue>
#include <map>

#include "../Drawables/Scene.h"
#include <glm/glm.hpp>
#include "../optionparser.h"

class Parser {
protected:
	std::queue<std::string> _input_files;
	std::ifstream _current_input;
	std::string _current_input_name;
	int _conf_index;
	option::Option *_options;

	std::map<std::string, glm::vec4> _base_colors;

	void _open_input_if_required();
	void _close_current_input_if_eof();
	virtual glm::vec4 _get_color(std::string &line);

public:
	Parser(const std::queue<std::string> &_input_files);
	virtual ~Parser();

	virtual void set_options(option::Option *options) { _options = options; }

	virtual bool done() { return _input_files.empty(); }
	virtual void skip(int N);
	virtual Scene *next_scene() = 0;
};

#endif /* PARSER_H_ */
