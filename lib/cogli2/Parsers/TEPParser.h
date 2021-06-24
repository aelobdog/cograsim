/*
 * TEPParser.h
 *
 *  Created on: 11/Nov/2013
 *      Author: Flavio
 */

#ifndef TEPPARSER_H_
#define TEPPARSER_H_

#include <map>
#include <vector>

#include "Parser.h"
#include "IoxDNAParser.h"

class TEPParser: public Parser, public IoxDNAParser {
protected:
	int _N;
	float _twist_threshold, _twist_b;

	void _read_topology(std::string);
	void _set_color_from_HSV(glm::vec4 &color, float h, float s, float v);
public:
	TEPParser(std::string topology, const std::queue<std::string> &input_files);
	virtual ~TEPParser();

	virtual void set_options(option::Option *options);
	virtual Scene *next_scene();
};

#endif /* TEPPARSER_H_ */

